#pragma once
#include "landsidestretchinsim.h"
#include "LaneParkingSpot.h"
#include "LandsidePaxSericeInSim.h"

/*class LandsideLaneNodeList;*/
class LaneSegInSim;
class LandsideResourceManager;
class LandsideBusStation;
class PaxLandsideBehavior;
class LandsideQueueSystemProcess;
class LandsideBaseBehavior;
class LandsideSimulation;
class ResidentRelatedVehicleTypePlan;

//a line can enter to bus station
class LandsideBusStationEntry : public LandsideLaneExit
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
	LandsideBusStationInSim* getBusStation(){ return mpToRes?mpToRes->toBusSation():0; }
};
//a line can exit curbside to lane
class LandsideBusStationExit : public LandsideLaneEntry
{
public:
	void SetDistRangeInlane(DistanceUnit distF, DistanceUnit distT){ 
		distFromInLane = distF; distToInLane = distT;
	}
	DistanceUnit distFromInLane;
	DistanceUnit distToInLane;
	LandsideBusStationInSim* getCurb(){ return mpFromRes?mpFromRes->toBusSation():0; }	
};


class LandsideBusStationInSim :
	public LandsideResourceInSim, public LandsideLayoutObjectInSim, public LandsidePaxSericeInSim
{
public:
	LandsideBusStationInSim(LandsideBusStation *pStation,bool bLeftDrive);
	~LandsideBusStationInSim(void);
	virtual void InitRelateWithOtherObject(LandsideResourceManager* pRes);
	virtual void InitGraphNodes(LandsideRouteGraph* pGraph)const{}
	virtual void InitGraphLink(LandsideRouteGraph* pGraph)const{}
	virtual LandsideLayoutObjectInSim* getLayoutObject()const;

	LandsideBusStationInSim* toBusSation(){return this;}
	LandsideResourceInSim* IsResource(){ return this; }

	virtual CString print()const;
	LandsideBusStation *getBusStation();
	
	LandsideParkingLotInSim * getEmbeddedParkingLotInSim() const;
	
	LandsideLaneNodeInSim* GetExitNode(LandsideLaneInSim* pLane);
   // LandsideVehicleInSim* GetWantBus(int nDetailID);
	//void AddWaitingPax(PaxLandsideBehavior* pPax);
	//void DelWaitingPax(PaxLandsideBehavior* pPax);
	void AddWaitingBus(LandsideVehicleInSim* pBus);
	void DelWaitingBus(LandsideVehicleInSim* pBus);
	void NoticeWaitingPax(const ElapsedTime &time, LandsideVehicleInSim* pBus, LandsideSimulation* pSimulation);
	//bool HavePaxWaitingForThisBus(LandsideVehicleInSim *pBus);
	LaneParkingSpotsGroup& getParkingSpot();

	virtual CPath2008 GetQueue();

	virtual void PassengerMoveInto(PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime);
	virtual void PassengerMoveOut(PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime);

	bool PaxTakeOnBus(PaxLandsideBehavior *pBehavior, LandsideSimulation *pSimulation, const ElapsedTime& eTime);
	
	virtual CPoint2008 GetPaxWaitPos()const;

	void PaxLeaveQueue(const ElapsedTime& eTime);

	void SetLastServiceTime(ResidentRelatedVehicleTypePlan* pPlan , const ElapsedTime& t);
	bool GetLastServiceTime(ResidentRelatedVehicleTypePlan* pPlan , ElapsedTime& t)const;

	std::map<ResidentRelatedVehicleTypePlan*, ElapsedTime> m_ServiceTimeMap;
private:
	LandsideBusStation *m_pBusStation;
	LandsideParkingLotInSim *m_pEmbeddedParkingLotInSim ;//embedded parking lot in simulation

	//std::vector<LaneSegInSim*> m_vLaneOccupy; 
	std::vector<LandsideLaneEntry*> m_vLaneEntries;
	std::vector<LandsideLaneExit*> m_vLaneExits;

	VehicleList m_vWaitingBus;
	LaneParkingSpotsGroup mParkingSpots;
	LandsideQueueSystemProcess* m_queueProcessSys;


	CPath2008 m_waitingPath;
};
