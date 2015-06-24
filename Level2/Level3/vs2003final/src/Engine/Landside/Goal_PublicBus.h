#pragma once
#include "Goal_LandsideVehicle.h"


class Goal_ParkingBusStation: public LandsideVehicleSequenceGoal
{
public:
	Goal_ParkingBusStation(LandsideVehicleInSim* pEnt,LandsideBusStationInSim *pStation):LandsideVehicleSequenceGoal(pEnt){
		m_sDesc = "Goal_ParkingBusStation";
		m_pBusStation=pStation;
		mpSpot = NULL;
	};

	void PostProcessSubgoals(CARCportEngine* pEngine);
	void Activate(CARCportEngine* pEngine);
protected:
	LandsideBusStationInSim *m_pBusStation;
	LandsideLaneNodeList mPath;
	LaneParkingSpot* mpSpot;
private:
};
class Goal_LeaveBusStation : public LandsideVehicleSequenceGoal
{
public:
	Goal_LeaveBusStation(LandsideVehicleInSim* pEnt,LandsideBusStationInSim* pStation);
	~Goal_LeaveBusStation();
	//void doProcess(CARCportEngine* pEngine);

	void Activate(CARCportEngine* pEngine);
	virtual void PostProcessSubgoals(CARCportEngine* pEngine);

protected:
	LandsideBusStationInSim* m_pStation;
	LandsideLaneNodeList mPath;	
};


class Goal_WaitForSometime:public LandsideVehicleGoal
{
public:
	Goal_WaitForSometime(LandsideVehicleInSim* pEnt,const ElapsedTime& t):LandsideVehicleGoal(pEnt){
		m_sDesc = "Goal_WaitAtBusStation";
		m_waitT = t;
	}
	void Process(CARCportEngine* pEngine);
protected:
	ElapsedTime m_waitT;
private:
};


