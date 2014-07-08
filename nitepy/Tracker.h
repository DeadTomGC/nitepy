
#define trackerdll_api __declspec(dllexport)

#include <OpenNI.h>
#include <opencv2/opencv.hpp>
#include "NiTE.h"
#include <fstream>
#include <string>
#include <sstream>

using namespace cv;
using namespace openni;
#define maxUsers 5
class Tracker{
private:
	int* IDs;
	int IDCount;
	bool running;
	Ptr<FaceRecognizer> model;
	Device device;
	VideoStream color;
	VideoStream* stream;
	CascadeClassifier haar_cascade;
	nite::UserTracker userTracker;
	nite::Status niteRc;
	nite::UserTrackerFrameRef userTrackerFrame;
	const nite::Array<nite::UserData>* users;
	const nite::Array<nite::UserData>* userSnap;
	VideoFrameRef colorFrame;
public:
	int* peopleIDs;
	Tracker(void){
		IDs=new int[maxUsers];
		peopleIDs=new int[maxUsers];
		IDCount=0;
		nite::NiTE::initialize();
		OpenNI::initialize();
		
		if ( device.open( openni::ANY_DEVICE ) != 0 )
		{
			printf( "Kinect not found !\n" ); 
		}

		
		color.create( device, SENSOR_COLOR );
		color.start();

		VideoMode paramvideo;
		paramvideo.setResolution( 640, 480 );
		paramvideo.setFps( 30 );
		paramvideo.setPixelFormat( PIXEL_FORMAT_DEPTH_100_UM );
		paramvideo.setPixelFormat( PIXEL_FORMAT_RGB888 );
		color.setVideoMode( paramvideo );

		device.setDepthColorSyncEnabled( false );

		stream = &color;

		
		haar_cascade.load("haarcascade_frontalface_alt.xml");
		
		createFaceIdentifier("faces.txt");

		niteRc = userTracker.create();
		if (niteRc != nite::STATUS_OK)
		{
			printf("Couldn't create user tracker\n");
			
		}
		niteRc = userTracker.readFrame(&userTrackerFrame);
		if (niteRc != nite::STATUS_OK)
		{
			printf("Get next frame failed\n");
			
		}
		users = &userTrackerFrame.getUsers();
		
	}
	
	void loop(){
		
		niteRc = userTracker.readFrame(&userTrackerFrame);
		if (niteRc != nite::STATUS_OK)
		{
			printf("Get next frame failed\n");
			
		}
		
		users = &userTrackerFrame.getUsers();
		for (int i = 0; i < users->getSize(); ++i)
		{
			const nite::UserData& user = (*users)[i];
			if (user.isNew())
			{
				userTracker.startSkeletonTracking(user.getId());
			}
			
		}
			
	}
	void takeSnapShot(){
			int changedIndex;
			userSnap=&userTrackerFrame.getUsers();
			if( device.isValid() ){
				OpenNI::waitForAnyStream( &stream, 1, &changedIndex );
				color.readFrame( &colorFrame );
			}
		
	}




	void detectPeople(){
		static int img=0;
		Mat faces_resized[maxUsers];
		int* temp = new int[maxUsers];
		int* tempPeople = new int[maxUsers];
		int itemp=0;
		Sleep(200);
		for(int i=0; i<userSnap->getSize();i++){//update list of ID's and people
			bool found=false;
			int j=0;
			for(j=0; j<IDCount;j++){
				if(IDs[j]==(*userSnap)[i].getId()){
					found=true;
					break;
				}
			}
			if(found){
				temp[itemp]=(*userSnap)[i].getId();
				if(peopleIDs[j]>=0){
					tempPeople[itemp]=peopleIDs[j];
				}else{
					tempPeople[itemp]=-2;
				}
				itemp++;
			}else{
				temp[itemp]=(*userSnap)[i].getId();
				tempPeople[itemp]=-2;
				itemp++;
			}
		}
		delete IDs;
		delete peopleIDs;
		IDs=temp;
		peopleIDs=tempPeople;
		IDCount=userSnap->getSize();
		//Find faces and match them to skeletons
		
		Mat colorcv( cv::Size( 640, 480 ), CV_8UC3, NULL );
		Sleep(200);
			if ( colorFrame.isValid() ){
				colorcv.data = (uchar*) colorFrame.getData();
				cv::cvtColor( colorcv, colorcv, CV_BGR2RGB );
				vector< Rect_<int> > faces;
				haar_cascade.detectMultiScale(colorcv, faces);
				for(unsigned int i=0;i<faces.size();i++){
					//std::cerr<<faces[i].x<<" "<<faces[i].y<<" "<<faces[i].width<<" "<<faces[i].height<<std::endl;
					int j=0;
					bool match = false;
					float x,y;
					for(j=0;j<userSnap->getSize();j++){//3D->2D
						userTracker.convertJointCoordinatesToDepth((*userSnap)[j].getSkeleton().getJoint(nite::JOINT_HEAD).getPosition().x,(*userSnap)[j].getSkeleton().getJoint(nite::JOINT_HEAD).getPosition().y,(*userSnap)[j].getSkeleton().getJoint(nite::JOINT_HEAD).getPosition().z,&x,&y);
						x*=640/userTrackerFrame.getDepthFrame().getVideoMode().getResolutionX();
						y*=480/userTrackerFrame.getDepthFrame().getVideoMode().getResolutionY();
						//std::cerr<<"head at:"<<x<<" "<<y<<std::endl;
						if(x>faces[i].x && x<faces[i].x+faces[i].width){
							if(y>faces[i].y && y<faces[i].y+faces[i].height){
								match = true;//face is found for skeleton j
								break;
							}
						}
					}
					if(match && peopleIDs[j]<0){//take note of this
						//std::cerr<<"resizing\n";
						Mat temp = colorcv(faces[i]);
						cv::cvtColor(temp, temp, CV_BGR2GRAY);
						cv::resize(temp, faces_resized[j], Size(480, 480), 1.0, 1.0, INTER_CUBIC);//works because IDs and faces resized line up with userSnap
						peopleIDs[j]=-1;//try to identify
						//std::cerr<<"face put up for identification\n";
					}

				}
			}
            Sleep(200);

		

		//identify faces if possible
		for(int i = 0; i < IDCount; i++){
			//std::cerr<<"identifying...\n";
			if(peopleIDs[i] != -1){
				continue; //if the value is not -1, don't check
			}
			
			//cv::imshow( "RGB", faces_resized[i] );
			//cv::waitKey( 1 );
			//char * filename=new char[30];
			//sprintf(filename,"myTest/img%d.jpg\0",img);
			//imwrite(filename, faces_resized[i]);
			//img++;
			//system("pause");
			int predictedLabel = -1;
			double confidence = 0.0;
			//does the facial recognition prediction based off of the trained eigenface
			model->predict(faces_resized[i], predictedLabel, confidence);
			if(predictedLabel!=-1){
				std::cerr<<predictedLabel<<std::endl;
				std::cerr<<confidence<<std::endl;

			}
			peopleIDs[i] = predictedLabel; //label for person is placed in peopleIDs, -1 if unrecognized
		}
		

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
		model = createEigenFaceRecognizer(0, 10500.0);
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

