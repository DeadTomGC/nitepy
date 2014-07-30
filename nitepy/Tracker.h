
#define trackerdll_api __declspec(dllexport)

#include <OpenNI.h>
#include <opencv2/opencv.hpp>
#include "NiTE.h"
#include <fstream>
#include <string>
#include <sstream>

using namespace cv;
using namespace openni;
#define maxUsers 15
class Tracker{
private:
	int* IDs; //this is the array of skeleton ID's 
	int IDCount; //number of ID's that are valid
	bool running;//if it's still running
	Ptr<FaceRecognizer> model; //pointer to the face Recognizing model
	Device device;  //the Device object for the Xtion PRO
	VideoStream color; //our RGB stream
	VideoStream* stream; 
	CascadeClassifier haar_cascade;//Our face detector
	nite::UserTracker userTracker;//our skeleton tracker (from NITE)
	nite::Status niteRc; //a variable for storing errors (used in many places)
	nite::UserTrackerFrameRef userTrackerFrame;//a single skeleton data frame
	const nite::Array<nite::UserData>* users;//all the users skeletons in one array
	nite::UserData* userSnap;// a snapshot of where the skeletons are to be used in face recognition
	int userCount;//number of users at that time
	VideoFrameRef colorFrame;  //a single color frame
	Mat faces_resized[maxUsers]; //the faces that will be recognized by the recognizer after being detected
	int temp[maxUsers]; //an array used for comparing the old and new skeletons and which have been identified
	int tempPeople[maxUsers]; //an array for comparing the old and new
	int itemp; //a temporary counter for temp and tempPeople
	Mat &colorcv; //image from a frame
	//std::ofstream file; //remove
public:
	int* peopleIDs; //this is the array of ID's for peopl who have been recognized with face recognition (non negative numbers are valid ID's)
	Tracker(Mat* c=(new Mat( cv::Size( 640, 480 ), CV_8UC3, NULL ))):colorcv(*c){

		//file.open("img.txt");//remove
		
		IDs=new int[maxUsers];//initialize arrays and associated values...
		peopleIDs=new int[maxUsers];
		IDCount=0;
		nite::NiTE::initialize(); //initialize nite and openni
		OpenNI::initialize();
		
		if ( device.open( openni::ANY_DEVICE ) != 0 ) //open the XtioPRO
		{
			printf( "Kinect not found !\n" ); 
		}

		
		color.create( device, SENSOR_COLOR ); //get the RGB camera
		color.start();

		VideoMode paramvideo;
		paramvideo.setResolution( 640, 480 );//setup the RGB capture settings
		paramvideo.setFps( 30 );
		paramvideo.setPixelFormat( PIXEL_FORMAT_DEPTH_100_UM );
		paramvideo.setPixelFormat( PIXEL_FORMAT_RGB888 );
		color.setVideoMode( paramvideo );

		device.setDepthColorSyncEnabled( false ); //does not sync with depth image (this is not critical and would harm performace)

		stream = &color;

		
		haar_cascade.load("haarcascade_frontalface_alt.xml"); //load face detector xml file
		
		createFaceIdentifier("faces.txt"); //load faces for face recognizers

		niteRc = userTracker.create(); //create skeleton tracker
		if (niteRc != nite::STATUS_OK)
		{
			printf("Couldn't create user tracker\n");
			
		}
		niteRc = userTracker.readFrame(&userTrackerFrame); //read first frame
		if (niteRc != nite::STATUS_OK)
		{
			printf("Get next frame failed\n");
			
		}
		users = &userTrackerFrame.getUsers(); //get user array from frist frame
		
	}
	
	void loop(){//call to get next frame
		
		niteRc = userTracker.readFrame(&userTrackerFrame);//get next frame
		if (niteRc != nite::STATUS_OK)
		{
			printf("Get next frame failed\n");
			
		}
		
		users = &userTrackerFrame.getUsers();//get current users
		for (int i = 0; i < users->getSize(); ++i)//start tracking them
		{
			const nite::UserData& user = (*users)[i];
			if (user.isNew())
			{
				userTracker.startSkeletonTracking(user.getId());
			}
			
		}
			
	}
	void takeSnapShot(){//take a snapshot so that the faces in it can be processed later
			int changedIndex;
			users=&userTrackerFrame.getUsers();
			userCount = users->getSize();
			userSnap = new nite::UserData[users->getSize()];
			for(int i=0;i<userCount;i++){
				userSnap[i]=(*users)[i]; //copy over skeleton data to snapshot variable
			}
			if( device.isValid() ){
				OpenNI::waitForAnyStream( &stream, 1, &changedIndex );
				color.readFrame( &colorFrame );//grab color frame
			}
		
	}




	void detectPeople(){//use snapshot data to look for faces
		static int img=0;
		//Mat faces_resized[maxUsers];
		itemp=0;
		//std::cerr<<"checkout last recogition\n";
		for(int i=0; i<userCount;i++){//update list of ID's and people
			bool found=false;
			int j=0;
			for(j=0; j<IDCount;j++){
				if(IDs[j]==userSnap[i].getId()){
					found=true;
					break;
				}
			}
			if(found){//if the old user is still around then keep their data unless it's -1
				temp[itemp]=userSnap[i].getId();
				if(peopleIDs[j]>=0){
					tempPeople[itemp]=peopleIDs[j];
				}else{
					tempPeople[itemp]=-2;
				}
				itemp++;
			}else{//we didn't find them
				temp[itemp]=userSnap[i].getId();
				tempPeople[itemp]=-2;
				itemp++;
			}
		}
		//delete IDs;
		//delete peopleIDs;
		IDs=temp;
		peopleIDs=tempPeople;
		IDCount=userCount;
		//Find faces and match them to skeletons
		Mat ROI;
		//std::cerr<<"check faces\n";
		if ( colorFrame.isValid() && userCount>=1){//check to ensure we can go into the face detectio
			colorcv.data = (uchar*) colorFrame.getData();
			cv::cvtColor( colorcv, colorcv, CV_BGR2RGB );
			vector< Rect_<int> > faces;
			//std::cerr<<"entering for\n";
			for(int j=0;j<userCount;j++){//check each skeleton for a face using a ROI around the face
				float x,y;
				unsigned int i=0;
				bool match=false;
				//get teh 2D location from the 3D location of the head
				userTracker.convertJointCoordinatesToDepth(userSnap[j].getSkeleton().getJoint(nite::JOINT_HEAD).getPosition().x,userSnap[j].getSkeleton().getJoint(nite::JOINT_HEAD).getPosition().y,userSnap[j].getSkeleton().getJoint(nite::JOINT_HEAD).getPosition().z,&x,&y);
				x*=640/userTrackerFrame.getDepthFrame().getVideoMode().getResolutionX();
				y*=480/userTrackerFrame.getDepthFrame().getVideoMode().getResolutionY();
				//std::cerr<<"x="<<x<<" y="<<y<<"\n";
				if(x>50 && y>50 && x<590 && y<430){//check to ensure that the 2D coordinates are within a reasonable range

					cv::Rect face(x-50,y-50,100,100);
					ROI=colorcv(face);
					haar_cascade.detectMultiScale(ROI, faces); //look for faces in the ROI

					for(i=0;i<faces.size();i++){ //look through the found faces (if any) for one that surrounds the head point
						//std::cerr<<faces[i].x<<" "<<faces[i].y<<" "<<faces[i].width<<" "<<faces[i].height<<std::endl;

						//std::cerr<<"head at:"<<x<<" "<<y<<std::endl;
						if(50>faces[i].x && 50<faces[i].x+faces[i].width){
							if(50>faces[i].y && 50<faces[i].y+faces[i].height){
								match = true;//face is found for skeleton j
								break;
							}
						}
					}
					//std::cerr<<"MATCH WAS "<<match<<"\n";
					//std::cerr<<"chose face "<<i<<"\n";
					if(match && peopleIDs[j]<0){//if there is a valid face and they haven't already been identified then put face up for recognition
						//std::cerr<<"resizing\n";
						Mat temp = ROI(faces[i]);
						cv::cvtColor(temp, temp, CV_BGR2GRAY);
						face.x=25;
						face.y=50;
						face.height=180;
						face.width=180;  //resize face to standard size
						cv::resize(temp, faces_resized[j], Size(240, 240), 1.0, 1.0, INTER_CUBIC);//works because IDs and faces resized line up with userSnap
						faces_resized[j] = faces_resized[j](face); //crop face to get rid of more background
						//resize to standard size
						cv::resize(faces_resized[j], faces_resized[j], Size(240, 240), 1.0, 1.0, INTER_CUBIC);//works because IDs and faces resized line up with userSnap
						cv::equalizeHist(faces_resized[j],faces_resized[j]);//make colors more defined (just a filter)
						peopleIDs[j]=-1;//try to identify
						//std::cerr<<"face put up for identification\n";
					}

				}
			}
		}
		//std::cerr<<"checking face array\n";
		//identify faces if possible
		for(int i = 0; i < IDCount; i++){ //look through all valid images and try to recognize them
			//std::cerr<<"identifying...\n";
			if(peopleIDs[i] != -1){
				continue; //if the value is not -1, don't check
			}
			//std::cerr<<"identifying face"<<i<<"\n";
			//cv::imshow( "RGB", faces_resized[i] );
			//cv::waitKey( 1 );
			//char * filename=new char[30];//remove / change
			//sprintf(filename,"myTest/chris%d.jpg\0",img); //remove / change
			//file<<filename<<";1\n";//remove / change
			//file.flush();//remove / change
			//imwrite(filename, faces_resized[i]);//remove / change
			//img++;//remove / change
			//system("pause");
			int predictedLabel = -1;
			double confidence = 0.0;
			//does the facial recognition prediction based off of the trained eigenface
			model->predict(faces_resized[i], predictedLabel, confidence); //check the face
			if(predictedLabel!=-1){//it has been recognized
				std::cerr<<predictedLabel<<std::endl;
				std::cerr<<confidence<<std::endl;

			}
			peopleIDs[i] = predictedLabel; //label for person is placed in peopleIDs, -1 if unrecognized
		}

		delete[] userSnap;
	}
	void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
		std::ifstream file(filename.c_str());
		if (!file) {
			string error_message = "No valid input file was given, please check the given filename.";
			CV_Error(CV_StsBadArg, error_message);
		}
		string line, path, classlabel;
		int i = 0;
		while (std::getline(file, line)) {
			std::stringstream liness(line);
			std::getline(liness, path, separator);
			std::getline(liness, classlabel);
			//if both label and image path are received add them to their respective arrays
			if(!path.empty() && !classlabel.empty()) {
				images.push_back(imread(path, CV_LOAD_IMAGE_GRAYSCALE)); //images loaded as grayscale for recognizer
				labels.push_back(atoi(classlabel.c_str()));
			}
		}
	}
	bool createFaceIdentifier(string  csvFilePath){

		// These vectors hold the images and corresponding labels.
		vector<Mat> images;
		vector<int> labels;
		// Read in the data. This can fail if no valid
		// input filename is given.
		try {
			read_csv(csvFilePath, images, labels);
		} catch (cv::Exception& e) {
			std::cerr << "Error opening file \"" << csvFilePath << "\". Reason: " << e.msg << std::endl;
			// nothing more we can do
			return false;
		}
		// returns false if there are too few images
		if(images.size() < 1) {
			//need at least one image in training database
			std::cerr << "Need at least one image in database to train faceRecognizer" << std::endl;
			return false;
		}

		//9500 chosen as threshold for successful recognition
		//0 means that all Eigenfaces should be used
		model = createFisherFaceRecognizer(0, 2000.0);
		//train the faceRecognizer based on the images and labels from the CSV
		model->train(images, labels);

		//face recognizer created and trained
		return true;
	}


	int getUsersCount(){
		return users->getSize();
	}
	bool isUserTracked(int i){
		return ((*users)[i].getSkeleton().getState() == nite::SKELETON_TRACKED);
	}
	int getUserID(int i){
		return (*users)[i].getId();
	}
	float getUserSkeletonHeadConf(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_HEAD).getPositionConfidence();
	}
	float getUserSkeletonHeadX(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_HEAD).getPosition().x;
	}
	float getUserSkeletonHeadY(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_HEAD).getPosition().y;
	}
	float getUserSkeletonHeadZ(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_HEAD).getPosition().z;
	}
	float getUserSkeletonNeckX(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_NECK).getPosition().x;
	}
	float getUserSkeletonNeckY(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_NECK).getPosition().y;
	}
	float getUserSkeletonNeckZ(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_NECK).getPosition().z;
	}
	float getUserSkeletonL_ShX(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_SHOULDER).getPosition().x;
	}
	float getUserSkeletonL_ShY(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_SHOULDER).getPosition().y;
	}
	float getUserSkeletonL_ShZ(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_SHOULDER).getPosition().z;
	}
	float getUserSkeletonR_ShX(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_SHOULDER).getPosition().x;
	}
	float getUserSkeletonR_ShY(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_SHOULDER).getPosition().y;
	}
	float getUserSkeletonR_ShZ(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_SHOULDER).getPosition().z;
	}
	float getUserSkeletonL_ElbowX(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_ELBOW).getPosition().x;
	}
	float getUserSkeletonL_ElbowY(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_ELBOW).getPosition().y;
	}
	float getUserSkeletonL_ElbowZ(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_ELBOW).getPosition().z;
	}
	float getUserSkeletonR_ElbowX(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_ELBOW).getPosition().x;
	}
	float getUserSkeletonR_ElbowY(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_ELBOW).getPosition().y;
	}
	float getUserSkeletonR_ElbowZ(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_ELBOW).getPosition().z;
	}
	float getUserSkeletonL_HandX(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_HAND).getPosition().x;
	}
	float getUserSkeletonL_HandY(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_HAND).getPosition().y;
	}
	float getUserSkeletonL_HandZ(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_HAND).getPosition().z;
	}
	float getUserSkeletonR_HandX(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_HAND).getPosition().x;
	}
	float getUserSkeletonR_HandY(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_HAND).getPosition().y;
	}
	float getUserSkeletonR_HandZ(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_HAND).getPosition().z;
	}
	float getUserSkeletonTorsoX(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_TORSO).getPosition().x;
	}
	float getUserSkeletonTorsoY(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_TORSO).getPosition().y;
	}
	float getUserSkeletonTorsoZ(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_TORSO).getPosition().z;
	}
	float getUserSkeletonL_HipX(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_HIP).getPosition().x;
	}
	float getUserSkeletonL_HipY(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_HIP).getPosition().y;
	}
	float getUserSkeletonL_HipZ(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_HIP).getPosition().z;
	}
	float getUserSkeletonR_HipX(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_HIP).getPosition().x;
	}
	float getUserSkeletonR_HipY(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_HIP).getPosition().y;
	}
	float getUserSkeletonR_HipZ(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_HIP).getPosition().z;
	}
	float getUserSkeletonL_KneeX(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_KNEE).getPosition().x;
	}
	float getUserSkeletonL_KneeY(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_KNEE).getPosition().y;
	}
	float getUserSkeletonL_KneeZ(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_KNEE).getPosition().z;
	}
	float getUserSkeletonR_KneeX(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_KNEE).getPosition().x;
	}
	float getUserSkeletonR_KneeY(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_KNEE).getPosition().y;
	}
	float getUserSkeletonR_KneeZ(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_KNEE).getPosition().z;
	}
	float getUserSkeletonL_FootX(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_FOOT).getPosition().x;
	}
	float getUserSkeletonL_FootY(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_FOOT).getPosition().y;
	}
	float getUserSkeletonL_FootZ(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_LEFT_FOOT).getPosition().z;
	}
	float getUserSkeletonR_FootX(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_FOOT).getPosition().x;
	}
	float getUserSkeletonR_FootY(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_FOOT).getPosition().y;
	}
	float getUserSkeletonR_FootZ(int i){
		return (*users)[i].getSkeleton().getJoint(nite::JOINT_RIGHT_FOOT).getPosition().z;
	}
	void shutdown(void){
		nite::NiTE::shutdown();
	}
};

