#pragma once
#include "..\LandsideVehicleInSim.h"
#include "landsidePath.h"
#include "State_LandsideVehicle.h"
#include "LandsideNonPaxVehicleInSim.h"
#include "..\LandsideParkingLotGraph.h"
#include "State_MoveInRoad.h"

//////////////////////////////////atom goal , more like behavior tree actions////////////////////////////////////////
//arrival pax Vehicle Logic
class State_EntryLotDoor : public State_LandsideVehicle<LandsideVehicleInSim>
{
public:
	State_EntryLotDoor(LandsideVehicleInSim* pV, LandsideParkingLotDoorInSim* pDoor)
	:State_LandsideVehicle<LandsideVehicleInSim>(pV)
	,m_pDoor(pDoor){}	


	virtual const char* getDesc()const{ return _T("Entry ParkingLot Door"); } 
	virtual void Execute(CARCportEngine* pEngine);
protected:
	LandsideParkingLotDoorInSim* m_pDoor;	
};


class State_ExitLotDoor : public State_LandsideVehicle<LandsideVehicleInSim>
{
public:
	State_ExitLotDoor(LandsideVehicleInSim* pV, LandsideParkingLotDoorInSim* pDoor)
		:State_LandsideVehicle<LandsideVehicleInSim>(pV)
		,m_pDoor(pDoor){}	

	virtual const char* getDesc()const{ return _T("Exit ParkingLot Door"); }
	virtual void Execute(CARCportEngine* pEngine);
protected:
	LandsideParkingLotDoorInSim* m_pDoor;	
};

class State_MoveInParkingLot : public State_LandsideVehicle<LandsideVehicleInSim>
{
public:
	State_MoveInParkingLot(LandsideVehicleInSim* pV);
	virtual void Execute(CARCportEngine* pEngine);
protected:
	DrivePathInParkingLot m_path;
	double m_dLastDistInRoute;  //position in path

	void DoMovementsInPath(const ElapsedTime& fromTime , double dS,double dStarSpd, double dEndSpd );
	bool MoveInPath(CARCportEngine* pEngine);
	bool IsOutRoute() const;

	virtual void OnMoveOutRoute(CARCportEngine* _pEngine ){}
	void _getConflictInfo(DistanceUnit distRad ,ConflictInfo& conflictInfo);
	
};


//finding a spot and parking in it
class State_FindingParkSpot : public State_MoveInParkingLot
{
public:
	State_FindingParkSpot(LandsideVehicleInSim* pV,LandsideParkingLotInSim* pLot)
		:State_MoveInParkingLot(pV)
		,m_pLot(pLot){}

	virtual const char* getDesc()const{ return _T("Finding ParkingSpot"); } 
	virtual void Execute(CARCportEngine* pEngine);
protected:
	LandsideParkingLotInSim* m_pLot;
	ElapsedTime m_maxfindTime;
};

class State_MoveAndParkingToSpot :public State_MoveInParkingLot
{
public:
	State_MoveAndParkingToSpot(LandsideVehicleInSim* pV, LandsideParkingSpotInSim* pSpot);

	virtual const char* getDesc()const{ return _T("Move To Parking Spot"); }
	virtual void Entry(CARCportEngine* pEngine);
protected:
	LandsideParkingSpotInSim* m_pSpot;
	void ParkingToSpot();
	virtual void OnMoveOutRoute(CARCportEngine* _pEngine );
};


class State_LeaveParkingLot : public State_MoveInParkingLot
{
public:
	State_LeaveParkingLot(LandsideVehicleInSim* pV,LandsideParkingLotInSim* pLot)
		:State_MoveInParkingLot(pV)
		,m_pLot(pLot){}

	virtual const char* getDesc()const{ return _T("Leave Parking Lot"); }
	virtual void Entry(CARCportEngine* pEngine);
protected:
	LandsideParkingLotInSim* m_pLot;
	LandsideParkingLotDoorInSim* m_pExitDoor;
	virtual void OnMoveOutRoute(CARCportEngine* _pEngine );
};

//
class State_NonPaxVParkingInLotSpot : public State_LandsideVehicle<LandsideNonPaxVehicleInSim>
{
public:
	State_NonPaxVParkingInLotSpot(LandsideNonPaxVehicleInSim* pV,LandsideParkingSpotInSim* spot);
	
	virtual const char* getDesc()const{ return _T("Parking in LotSpot"); }
	virtual void Entry(CARCportEngine* pEngine);
	virtual void Execute(CARCportEngine* pEngine);
protected:
	LandsideParkingSpotInSim* m_spot;
	State_ServiceTimer m_sExitTimer;
};

