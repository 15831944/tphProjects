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
	IParkingSpotInSim* m_spot;
	void OnMoveOutRoute( CARCportEngine* _pEngine );

};

class State_LeaveBusStation: public State_LandsideVehicle<LandsideVehicleInSim>
{
public:
	State_LeaveBusStation(LandsideVehicleInSim* pV, LandsideBusStationInSim* pBusStation,IParkingSpotInSim* spot);
	void Execute( CARCportEngine* pEngine );
	virtual const char* getDesc()const{ return _T("Leave Bus Station"); }
	virtual void Entry(CARCportEngine* pEngine);
	virtual void Exit(CARCportEngine* pEngine);
protected:
	LandsideBusStationInSim *m_pBusStation;
	IParkingSpotInSim* m_spot;
	bool m_bOutPath;
};