#pragma once
#include "State_MoveInRoad.h"
#include <memory>
#include "LandsidePaxVehicleInSim.h"
class IParkingSpotInSim;

class State_TryParkingCurbside : public State_LandsideVehicle<LandsideVehicleInSim>
{
public:
	State_TryParkingCurbside(LandsideVehicleInSim* pV, LandsideCurbSideInSim* pCurb);
	virtual const char* getDesc()const{ return _T("Try Parking Curbside"); }

	virtual void Entry(CARCportEngine* pEngine);
	virtual void Execute(CARCportEngine * pEngine);
protected:
	LandsideCurbSideInSim* m_pCurb;
	State_ServiceTimer m_waitTimer;
};

class State_ParkingToCurbside : public State_MoveInRoad
{
public:
	State_ParkingToCurbside(LandsideVehicleInSim* pV, IParkingSpotInSim* pSpot,LandsideCurbSideInSim*pCurb);
	virtual const char* getDesc()const{ return _T("Parking to Curbside"); } 
protected:
	LandsideCurbSideInSim* m_pCurb;
	IParkingSpotInSim* m_spot;
	virtual void OnMoveOutRoute(CARCportEngine* _pEngine );
};

class State_LeaveCurbside : public State_LandsideVehicle<LandsideVehicleInSim>
{
public:
	State_LeaveCurbside(LandsideVehicleInSim* pV, LandsideCurbSideInSim* pCurb,IParkingSpotInSim* spot);
	virtual void Execute(CARCportEngine* pEngine);
	virtual void Exit(CARCportEngine* pEngine);

	virtual const char* getDesc()const{ return _T("Leave Curbside"); }
protected:
	LandsideCurbSideInSim* m_pCurb;
	bool m_bOutPath;
	IParkingSpotInSim* m_spot;
};

//////////////////////////////////////////////////////////////////////////
class LandsidePaxVehicleInSim;
class State_DropPaxAtCurbside : public State_LandsideVehicle<LandsidePaxVehicleInSim>
{
public:
	State_DropPaxAtCurbside(LandsidePaxVehicleInSim* pV,LandsideCurbSideInSim*pCub,IParkingSpotInSim* spot );
	virtual const char* getDesc()const{ return _T("Dep Parking at Curbside"); }
	virtual void Entry(CARCportEngine* pEngine);
	virtual void Execute(CARCportEngine* pEngine);
protected:
	LandsideCurbSideInSim* m_pCurb;
	IParkingSpotInSim* m_spot;
};
