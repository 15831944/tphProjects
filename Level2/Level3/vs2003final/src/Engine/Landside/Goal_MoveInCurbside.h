#pragma once
#include "..\core\SGoal.h"
#include "..\LandsideVehicleInSim.h"
//



class Goal_ExitCurbside : public LandsideVehicleSequenceGoal
{
public:
	Goal_ExitCurbside(LandsideVehicleInSim* pEnt,LandsideCurbSideInSim* pCurb);
	~Goal_ExitCurbside();
	void Activate(CARCportEngine* pEngine);
	virtual void PostProcessSubgoals(CARCportEngine* pEngine);
	
protected:
	bool FindOtherLaneOut(LandsideLaneNodeList& path);
	LandsideCurbSideInSim* mpCurb;
	LandsideLaneNodeList mPath;
};

class Goal_SkipParkCurbside : public LandsideVehicleSequenceGoal
{
public:
	Goal_SkipParkCurbside(LandsideVehicleInSim* pEnt,LandsideCurbSideInSim* pCurb);
	~Goal_SkipParkCurbside(){ mPath.DeleteClear(); }
	void Activate(CARCportEngine* pEngine);
protected:
	LandsideCurbSideInSim* mpCurb;
	LandsideLaneNodeList mPath;
	bool FindSkipPath(LandsideLaneNodeList& path);

};


//parking to curbside
class LaneParkingSpot;
class Goal_ParkingCurbside : public LandsideVehicleGoal
{
public:
	Goal_ParkingCurbside(LandsideVehicleInSim* pEnt, LandsideCurbSideInSim* pDest);
	virtual void PostProcessSubgoals(CARCportEngine* pEngine);	
	~Goal_ParkingCurbside();
	void Activate(CARCportEngine* pEngine);
	void  Process(CARCportEngine* pEngine);
protected:	
	LandsideVehicleGoal* mpSubGoal;
	LandsideCurbSideInSim* mpCurb;	
	LandsideLaneNodeList mPath;
	LaneParkingSpot* mpDest;
};

//////////////////////////////////////////////////////////////////////////
class LandsideTaxiQueueInSim;
class Goal_MoveInRoad;
class Goal_ParkingTaxiQueue : public LandsideVehicleSequenceGoal
{
public:
	Goal_ParkingTaxiQueue(LandsideVehicleInSim* pEnt, LandsideTaxiQueueInSim* pDest);
	~Goal_ParkingTaxiQueue(){ mPath.DeleteClear(); }
	void Activate(CARCportEngine* pEngine);
	virtual void PostProcessSubgoals(CARCportEngine* pEngine);	
protected:
	LandsideTaxiQueueInSim* mpCurb;	
	LandsideLaneNodeList mPath;
	LaneParkingSpot* mpDest;
};

class Goal_ExitTaxiQ : public LandsideVehicleSequenceGoal
{
public:
	Goal_ExitTaxiQ(LandsideVehicleInSim* pEnt,LandsideTaxiQueueInSim* pCurb);
	~Goal_ExitTaxiQ(){ mPath.DeleteClear(); }
	void Activate(CARCportEngine* pEngine);	
	virtual void PostProcessSubgoals(CARCportEngine* pEngine);	
protected:
	bool FindOtherLaneOut(LandsideLaneNodeList& path);
	LandsideTaxiQueueInSim* mpCurb;
	LandsideLaneNodeList mPath;
};
//////////////////////////////////////////////////////////////////////////
//other alg
class LaneParkingSpot;
class Goal_ParkingCurbside2 : public LandsideVehicleSequenceGoal
{
public:
	Goal_ParkingCurbside2(LandsideVehicleInSim* pEnt, LandsideCurbSideInSim* pDest)
		:LandsideVehicleSequenceGoal(pEnt){
		mpCurb = pDest;
	}
	virtual void PostProcessSubgoals(CARCportEngine* pEngine);	
	~Goal_ParkingCurbside2();
	void Activate(CARCportEngine* pEngine);

protected:
	LandsideCurbSideInSim* mpCurb;	
	LandsideLaneNodeList mPath;
};