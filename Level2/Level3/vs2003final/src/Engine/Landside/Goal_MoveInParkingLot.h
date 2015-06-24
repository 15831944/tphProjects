#pragma once
#include "..\core\SGoal.h"
#include "..\LandsideVehicleInSim.h"
#include "..\LandsideParkingLotGraph.h"
#include "Goal_LandsideVehicle.h"

class Goal_BirthAtParkingLot : public LandsideVehicleGoal
{
public:
	Goal_BirthAtParkingLot(LandsideVehicleInSim* pEnt,LandsideParkingLotInSim* pLot):LandsideVehicleGoal(pEnt){
		m_sDesc = "Goal_BirthAtParkingLot";
		m_pLot = pLot;
	}
	void Process(CARCportEngine* pEngine);
	LandsideParkingLotInSim* m_pLot;
};

class Goal_MoveInParkingLotLane;
class Goal_ParkingToLotSpot : public LandsideVehicleSequenceGoal
{
public:
	Goal_ParkingToLotSpot(LandsideVehicleInSim* pEnt,LandsideParkingLotDoorInSim* pDoor ,LandsideParkingSpotInSim* mpSpot);
	void Activate(CARCportEngine* pEngine);	
protected:	
	LandsideParkingLotDoorInSim* m_pDoor;
	LandsideParkingSpotInSim* m_pSpot;
};

///move in parking lot and find spot to park 
class Goal_ParkingAtParkingLot : public LandsideVehicleGoal
{
public:
	Goal_ParkingAtParkingLot(LandsideVehicleInSim* pEnt,LandsideParkingLotDoorInSim* pDoor)
	:LandsideVehicleGoal(pEnt)
	{
		m_pLot = pDoor->getParkingLot();
		m_pSpot = NULL;
		m_pDoor = pDoor;
		m_MoveinParkinglot = NULL;
	}
	enum State
	{
		_processAtDoor,
		_moveInDriveLane,
		_parkingToSpot,
	};
	virtual void Activate( CARCportEngine* pEngine );
	virtual void doProcess( CARCportEngine* pEngine );

protected:
	LandsideParkingLotDoorInSim* m_pDoor;
	LandsideParkingSpotInSim* m_pSpot;
	LandsideParkingLotInSim* m_pLot;
	Goal_MoveInParkingLotLane * m_MoveinParkinglot;

	State					m_state;

	void ProcessAtLotDoor(CARCportEngine* pEngine);
	void MoveInDriveLane(CARCportEngine* pEngine);
	void ParkingToSpot(CARCportEngine* pEngine);
};

//parking to spot
class Goal_ParkToSpot : public LandsideVehicleGoal{
public:
	Goal_ParkToSpot(LandsideVehicleInSim* pEnt,LandsideParkingSpotInSim* pSpot):LandsideVehicleGoal(pEnt){	mpSpot =pSpot; }
	virtual void Process(CARCportEngine* pEngine);
	LandsideParkingSpotInSim* mpSpot;
};

class Goal_ProcessAtLotDoor : public LandsideVehicleGoal
{	
public:
	Goal_ProcessAtLotDoor(LandsideVehicleInSim* pEnt, LandsideParkingLotDoorInSim* pDoor);
	void Activate(CARCportEngine* pEngine);
	void Process(CARCportEngine* pEngine);
	//bool DefaultHandleSignal(SSignal* pSignal);
protected:
	LandsideParkingLotDoorInSim* mpDoor;
	int nPortIdx;
	BlockTag blockAtDoor;
};


//drive in lot lanes
class ConflictInfo;
class Goal_MoveInParkingLotLane :   public LandsideVehicleGoal
{
public:
	Goal_MoveInParkingLotLane(LandsideVehicleInSim* pEnt, const DrivePathInParkingLot& pDrivePath);	
	void Process(CARCportEngine* pEngine);
protected:
	DrivePathInParkingLot m_path;
	double m_dLastDistInRoute;  //position in path

	void DoMovementsInPath(const ElapsedTime& fromTime , double dS,double dStarSpd, double dEndSpd );
	bool MoveInPath(CARCportEngine* pEngine);
	bool IsOutRoute( double dDist ) const;

	void _getConflictInfo(DistanceUnit distRad ,ConflictInfo& conflictInfo);

};

class Goal_LeaveParkingLot : public LandsideVehicleSequenceGoal
{
public:
	Goal_LeaveParkingLot(LandsideVehicleInSim* pEnt):LandsideVehicleSequenceGoal(pEnt){ 
		m_sDesc = "Goal_LeaveParkingLot";		
	}
	void Activate(CARCportEngine* pEngine);	
protected:
};

class Goal_Dummy : public LandsideVehicleGoal
{
public:
	Goal_Dummy(LandsideVehicleInSim* pEnt):LandsideVehicleGoal(pEnt){
		m_sDesc = _T("Dummy");
	}
	virtual void Process(CARCportEngine* pEngine){ m_iStatus = gs_completed; }
};

