#include "Tracker.h"

 
 
extern "C" { //these are all C wrappers to C++ code so that these methods may be used in python
    trackerdll_api Tracker* Tracker_new(){ return new Tracker(); }
	trackerdll_api int getShirt(Tracker* tracker,int index){ return tracker->getShirt(index);}
	trackerdll_api int getShirtSizeX(Tracker* tracker){ return tracker->getShirtSizeX();}
	trackerdll_api int getShirtSizeY(Tracker* tracker){ return tracker->getShirtSizeY();}
	trackerdll_api int getColor(Tracker* tracker,int x,int y){ return tracker->getColor(x,y);}
    trackerdll_api void loop(Tracker* tracker){ tracker->loop();}
    trackerdll_api int getUsersCount(Tracker* tracker){ return tracker->getUsersCount();}
    trackerdll_api bool isUserTracked(Tracker* tracker,int i){ return tracker->isUserTracked(i);}
	trackerdll_api int getUserID(Tracker* tracker,int i){return tracker->getUserID(i);}
	trackerdll_api void detectPeople(Tracker* tracker){ tracker->detectPeople();}
	trackerdll_api void takeSnapShot(Tracker* tracker){ tracker->takeSnapShot();}
	trackerdll_api int getUserPersonID(Tracker* tracker,int i){return tracker->peopleIDs[i];}
	trackerdll_api float getUserSkeletonHeadConf(Tracker* tracker,int i){ return tracker->getUserSkeletonHeadConf(i);}
	trackerdll_api float getUserSkeletonHeadX(Tracker* tracker,int i){ return tracker->getUserSkeletonHeadX(i);}
	trackerdll_api float getUserSkeletonHeadY(Tracker* tracker,int i){ return tracker->getUserSkeletonHeadY(i);}
	trackerdll_api float getUserSkeletonHeadZ(Tracker* tracker,int i){ return tracker->getUserSkeletonHeadZ(i);}
	trackerdll_api float getUserSkeletonNeckConf(Tracker* tracker,int i){ return tracker->getUserSkeletonNeckConf(i);}
	trackerdll_api float getUserSkeletonNeckX(Tracker* tracker,int i){ return tracker->getUserSkeletonNeckX(i);}
	trackerdll_api float getUserSkeletonNeckY(Tracker* tracker,int i){ return tracker->getUserSkeletonNeckY(i);}
	trackerdll_api float getUserSkeletonNeckZ(Tracker* tracker,int i){ return tracker->getUserSkeletonNeckZ(i);}
	trackerdll_api float getUserSkeletonL_ShConf(Tracker* tracker,int i){ return tracker->getUserSkeletonL_ShConf(i);}
	trackerdll_api float getUserSkeletonL_ShX(Tracker* tracker,int i){ return tracker->getUserSkeletonL_ShX(i);}
	trackerdll_api float getUserSkeletonL_ShY(Tracker* tracker,int i){ return tracker->getUserSkeletonL_ShY(i);}
	trackerdll_api float getUserSkeletonL_ShZ(Tracker* tracker,int i){ return tracker->getUserSkeletonL_ShZ(i);}
	trackerdll_api float getUserSkeletonR_ShConf(Tracker* tracker,int i){ return tracker->getUserSkeletonR_ShConf(i);}
	trackerdll_api float getUserSkeletonR_ShX(Tracker* tracker,int i){ return tracker->getUserSkeletonR_ShX(i);}
	trackerdll_api float getUserSkeletonR_ShY(Tracker* tracker,int i){ return tracker->getUserSkeletonR_ShY(i);}
	trackerdll_api float getUserSkeletonR_ShZ(Tracker* tracker,int i){ return tracker->getUserSkeletonR_ShZ(i);}
	trackerdll_api float getUserSkeletonL_ElbowConf(Tracker* tracker,int i){ return tracker->getUserSkeletonL_ElbowConf(i);}
	trackerdll_api float getUserSkeletonL_ElbowX(Tracker* tracker,int i){ return tracker->getUserSkeletonL_ElbowX(i);}
	trackerdll_api float getUserSkeletonL_ElbowY(Tracker* tracker,int i){ return tracker->getUserSkeletonL_ElbowY(i);}
	trackerdll_api float getUserSkeletonL_ElbowZ(Tracker* tracker,int i){ return tracker->getUserSkeletonL_ElbowZ(i);}
	trackerdll_api float getUserSkeletonR_ElbowConf(Tracker* tracker,int i){ return tracker->getUserSkeletonR_ElbowConf(i);}
	trackerdll_api float getUserSkeletonR_ElbowX(Tracker* tracker,int i){ return tracker->getUserSkeletonR_ElbowX(i);}
	trackerdll_api float getUserSkeletonR_ElbowY(Tracker* tracker,int i){ return tracker->getUserSkeletonR_ElbowY(i);}
	trackerdll_api float getUserSkeletonR_ElbowZ(Tracker* tracker,int i){ return tracker->getUserSkeletonR_ElbowZ(i);}
	trackerdll_api float getUserSkeletonL_HandConf(Tracker* tracker,int i){ return tracker->getUserSkeletonL_HandConf(i);}
	trackerdll_api float getUserSkeletonL_HandX(Tracker* tracker,int i){ return tracker->getUserSkeletonL_HandX(i);}
	trackerdll_api float getUserSkeletonL_HandY(Tracker* tracker,int i){ return tracker->getUserSkeletonL_HandY(i);}
	trackerdll_api float getUserSkeletonL_HandZ(Tracker* tracker,int i){ return tracker->getUserSkeletonL_HandZ(i);}
	trackerdll_api float getUserSkeletonR_HandConf(Tracker* tracker,int i){ return tracker->getUserSkeletonR_HandConf(i);}
	trackerdll_api float getUserSkeletonR_HandX(Tracker* tracker,int i){ return tracker->getUserSkeletonR_HandX(i);}
	trackerdll_api float getUserSkeletonR_HandY(Tracker* tracker,int i){ return tracker->getUserSkeletonR_HandY(i);}
	trackerdll_api float getUserSkeletonR_HandZ(Tracker* tracker,int i){ return tracker->getUserSkeletonR_HandZ(i);}
	trackerdll_api float getUserSkeletonTorsoConf(Tracker* tracker,int i){ return tracker->getUserSkeletonTorsoConf(i);}
	trackerdll_api float getUserSkeletonTorsoX(Tracker* tracker,int i){ return tracker->getUserSkeletonTorsoX(i);}
	trackerdll_api float getUserSkeletonTorsoY(Tracker* tracker,int i){ return tracker->getUserSkeletonTorsoY(i);}
	trackerdll_api float getUserSkeletonTorsoZ(Tracker* tracker,int i){ return tracker->getUserSkeletonTorsoZ(i);}
	trackerdll_api float getUserSkeletonL_HipConf(Tracker* tracker,int i){ return tracker->getUserSkeletonL_HipConf(i);}
	trackerdll_api float getUserSkeletonL_HipX(Tracker* tracker,int i){ return tracker->getUserSkeletonL_HipX(i);}
	trackerdll_api float getUserSkeletonL_HipY(Tracker* tracker,int i){ return tracker->getUserSkeletonL_HipY(i);}
	trackerdll_api float getUserSkeletonL_HipZ(Tracker* tracker,int i){ return tracker->getUserSkeletonL_HipZ(i);}
	trackerdll_api float getUserSkeletonR_HipConf(Tracker* tracker,int i){ return tracker->getUserSkeletonR_HipConf(i);}
	trackerdll_api float getUserSkeletonR_HipX(Tracker* tracker,int i){ return tracker->getUserSkeletonR_HipX(i);}
	trackerdll_api float getUserSkeletonR_HipY(Tracker* tracker,int i){ return tracker->getUserSkeletonR_HipY(i);}
	trackerdll_api float getUserSkeletonR_HipZ(Tracker* tracker,int i){ return tracker->getUserSkeletonR_HipZ(i);}
	trackerdll_api float getUserSkeletonL_KneeConf(Tracker* tracker,int i){ return tracker->getUserSkeletonL_KneeConf(i);}
	trackerdll_api float getUserSkeletonL_KneeX(Tracker* tracker,int i){ return tracker->getUserSkeletonL_KneeX(i);}
	trackerdll_api float getUserSkeletonL_KneeY(Tracker* tracker,int i){ return tracker->getUserSkeletonL_KneeY(i);}
	trackerdll_api float getUserSkeletonL_KneeZ(Tracker* tracker,int i){ return tracker->getUserSkeletonL_KneeZ(i);}
	trackerdll_api float getUserSkeletonR_KneeConf(Tracker* tracker,int i){ return tracker->getUserSkeletonR_KneeConf(i);}
	trackerdll_api float getUserSkeletonR_KneeX(Tracker* tracker,int i){ return tracker->getUserSkeletonR_KneeX(i);}
	trackerdll_api float getUserSkeletonR_KneeY(Tracker* tracker,int i){ return tracker->getUserSkeletonR_KneeY(i);}
	trackerdll_api float getUserSkeletonR_KneeZ(Tracker* tracker,int i){ return tracker->getUserSkeletonR_KneeZ(i);}
	trackerdll_api float getUserSkeletonL_FootConf(Tracker* tracker,int i){ return tracker->getUserSkeletonL_FootConf(i);}
	trackerdll_api float getUserSkeletonL_FootX(Tracker* tracker,int i){ return tracker->getUserSkeletonL_FootX(i);}
	trackerdll_api float getUserSkeletonL_FootY(Tracker* tracker,int i){ return tracker->getUserSkeletonL_FootY(i);}
	trackerdll_api float getUserSkeletonL_FootZ(Tracker* tracker,int i){ return tracker->getUserSkeletonL_FootZ(i);}
	trackerdll_api float getUserSkeletonR_FootConf(Tracker* tracker,int i){ return tracker->getUserSkeletonR_FootConf(i);}
	trackerdll_api float getUserSkeletonR_FootX(Tracker* tracker,int i){ return tracker->getUserSkeletonR_FootX(i);}
	trackerdll_api float getUserSkeletonR_FootY(Tracker* tracker,int i){ return tracker->getUserSkeletonR_FootY(i);}
	trackerdll_api float getUserSkeletonR_FootZ(Tracker* tracker,int i){ return tracker->getUserSkeletonR_FootZ(i);}

	trackerdll_api void shutdown(Tracker* tracker){tracker->shutdown();}
	
}
