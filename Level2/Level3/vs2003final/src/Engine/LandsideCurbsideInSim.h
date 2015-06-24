#pragma once
#include "./LandsideStretchInSim.h"
#include "LaneParkingSpot.h"
#include "LandsidePaxSericeInSim.h"
class LandsideLaneNodeList;
//////////////////////////////////////////////////////////////////////////
class LandsideCurbSide;



class LaneSegInSim;

//a line can enter to curbside or leave curbside
class LandsideCursideSideEntry : public LandsideLaneExit
{
public:
	void SetDistRangeInlane(DistanceUnit distF, DistanceUnit distT){ 
		distFromInLane = distF; distToInLane = distT;
	}

	virtual bool IsLaneDirectToCurb()const
	{
		return false;
	};
	DistanceUnit distFromInLane;
	DistanceUnit distToInLane;
	LandsideCurbSideInSim* getCurb(){ return mpToRes?mpToRes->toCurbSide():0; }
	
};
//a line can exit curbside to lane
class LandsideCurbsideSideExit : public LandsideLaneEntry
{
public:
	void SetDistRangeInlane(DistanceUnit distF, DistanceUnit distT){ 
		distFromInLane = distF; distToInLane = distT;
	}
	DistanceUnit distFromInLane;
	DistanceUnit distToInLane;
	LandsideCurbSideInSim* getCurb(){ return mpFromRes?mpFromRes->toCurbSide():0; }	
};




class LandsideCurbSideInSim : public LandsideResourceInSim ,public LandsideLayoutObjectInSim, public LandsidePaxSericeInSim
{
public: 
	LandsideCurbSideInSim(LandsideCurbSide* pCurb,bool bLeftDrive);
	virtual LandsideLayoutObjectInSim* getLayoutObject()const;
	virtual CString print()const;
	LandsideCurbSideInSim* toCurbSide(){ return this; } 
	LandsideResourceInSim* IsResource(){ return this; }

	virtual void InitRelateWithOtherObject(LandsideResourceManager* pRes);
	virtual void InitGraphNodes(LandsideRouteGraph* pGraph)const{}
	virtual void InitGraphLink(LandsideRouteGraph* pGraph)const{};


	bool IsLinkToRoad()const;

	CPoint2008 GetPaxWaitPos()const;	

	virtual CPath2008 GetQueue();

	LandsideVehicleInSim* GetAheadVehicle(LandsideVehicleInSim* mpVehicle, LandsideLaneInSim* pLane,
		double& dDistTo )const;

	LandsideLaneNodeInSim* GetExitNode(LandsideLaneNodeInSim* pFromNode)const;
	LandsideLaneNodeInSim* GetExitNode(LandsideLaneInSim* pLane)const;
	LaneSegInSim* GetLaneSeg(LandsideLaneInSim* plane )const;
	

	LandsideLaneInSim* GetNotAtlane(LandsideLaneInSim* pLane)const; //get the lane next to the lane and the curbside not at

	CLandsideWalkTrafficObjectInSim* getBestTrafficObjectInSim(const CPoint2008& pt,LandsideResourceManager* allRes);

	LandsideCurbSide* getCurbInput()const;
	
	LaneParkingSpotsGroup& getParkingSpot(){ return mParkingSpots; }

	//use seg occupy
	bool FindParkingPos(LandsideVehicleInSim* pVehicle, LandsideLaneNodeList& followPath);
	bool FindLeavePath(LandsideVehicleInSim* pVehicle, LandsideLaneNodeList& followPath);

	void RemoveVehicleParkPos(LandsideVehicleInSim* pVehicle);

	//passenger operation
	virtual void PassengerMoveInto(PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime);
	virtual void PassengerMoveOut(PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime);


	virtual bool IsHaveCapacityAndFull(LandsideVehicleInSim*pVeh)const;

	ElapsedTime GetMaxStopTime();

private:
	void InitTrafficObjectOverlap( LandsideResourceManager* allRes );
protected:
	
	std::vector<LaneSegInSim*> m_vLaneOccupy; //replace with 
	LaneParkingSpotsGroup mParkingSpots;
	
	
	std::vector<LandsideLaneEntry*> m_vLaneEntries;
	std::vector<LandsideLaneExit*> m_vLaneExits;
	bool m_bInit;
	double m_dHeight;
};