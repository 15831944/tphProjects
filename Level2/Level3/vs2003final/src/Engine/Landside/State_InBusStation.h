#pragma once
#include "State_MoveInRoad.h"

class State_ParkingBusStation : public State_MoveInRoad
{
public:
	State_ParkingBusStation(LandsideVehicleInSim* pV, LandsideBusStationInSim* pBusStation);
	virtual void Execute(CARCportEngine* pEngine);
	virtual const char* getDesc()const{ return _T("Try Parking to Bus Station"); }

protected:
	LandsideBusStationInSim *m_pBusStation;
	LandsideLaneNodeList mPath;
	LaneParkingSpot* m_spot;
	void OnMoveOutRoute( CARCportEngine* _pEngine );

};

class State_LeaveBusStation: public State_LandsideVehicle<LandsideVehicleInSim>
{
public:
	State_LeaveBusStation(LandsideVehicleInSim* pV, LandsideBusStationInSim* pBusStation,LaneParkingSpot* spot)
		:State_LandsideVehicle<LandsideVehicleInSim>(pV)
	{
		m_pBusStation = pBusStation;  
		m_bFindLeavePath = false;
		m_spot = spot;
	}
	void Execute( CARCportEngine* pEngine );
	virtual const char* getDesc()const{ return _T("Leave Bus Station"); }
	virtual void Entry(CARCportEngine* pEngine);
protected:
	void OnMoveOutRoute( CARCportEngine* _pEngine );
	LandsideBusStationInSim *m_pBusStation;
	LaneParkingSpot* m_spot;
	bool m_bFindLeavePath;
};