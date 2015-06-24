#pragma once
#include "State_LandsideVehicle.h"
#include "State_MoveInRoad.h"

class LandsideTaxiInSim;
class LandsideTaxiQueueInSim;

class State_ProcessAtTaxiQ : public State_LandsideVehicle<LandsideTaxiInSim>
{
public:
	State_ProcessAtTaxiQ(LandsideTaxiInSim* pTaxi, LandsideTaxiQueueInSim* pQ,LaneParkingSpot* spot)
		: State_LandsideVehicle<LandsideTaxiInSim>(pTaxi){  m_pTaxiQ = pQ; m_spot = spot; }

	virtual void Entry(CARCportEngine* pEngine);
	virtual void Execute(CARCportEngine* pEngine);
	virtual const char* getDesc()const{ return _T("Process at  Taxi Queue"); }
protected:
	LandsideTaxiQueueInSim* m_pTaxiQ;
	LaneParkingSpot* m_spot;
};

class State_EnterTaxiQ :  public State_MoveInRoad
{
public:
	State_EnterTaxiQ(LandsideTaxiInSim* pTaxi, LandsideTaxiQueueInSim* pQ);
	virtual const char* getDesc()const{ return _T("Entering Taxi Queue"); }

	virtual void Execute(CARCportEngine* pEngine);
protected:
	virtual void OnMoveOutRoute(CARCportEngine* _pEngine );
	LandsideTaxiQueueInSim* m_pTaxiQ;
	LaneParkingSpot* m_spot; 
};


class State_LeaveTaxiQ : public State_LandsideVehicle<LandsideVehicleInSim>
{
public:
	State_LeaveTaxiQ(LandsideTaxiInSim* pTaxi, LandsideTaxiQueueInSim* pQ, LaneParkingSpot* spot);
	virtual const char* getDesc()const{ return _T("Leave Taxi Queue"); }

	virtual void Entry(CARCportEngine* pEngine);
	virtual void Execute(CARCportEngine* pEngine);
protected:
	LandsideTaxiQueueInSim* m_pTaxiQ;
	LaneParkingSpot* m_spot;
	bool m_bMoveOut;
};

//////////////////////////////////////////////////////////////////////////
class State_EnterTaxiPool :  public State_LandsideVehicle<LandsideTaxiInSim>
{
public:
	State_EnterTaxiPool(LandsideTaxiInSim* pTaxi, LandsideTaxiPoolInSim* pPool);
	virtual const char* getDesc()const{ return _T("Entering Taxi Pool"); }
	virtual void Execute(CARCportEngine* pEngine);
	virtual void Entry(CARCportEngine* pEngine);
protected:
	LandsideTaxiPoolInSim* m_pTaxiPool;
	DistanceUnit m_dCurDist;
	LandsidePath m_path;
};


class State_WaitAtTaxiPoolHead  :  public State_LandsideVehicle<LandsideTaxiInSim>
{
public:
	State_WaitAtTaxiPoolHead(LandsideTaxiInSim* pTaxi, LandsideTaxiPoolInSim* pPool)
		: State_LandsideVehicle<LandsideTaxiInSim>(pTaxi){  m_pTaxiPool = pPool; }
	virtual const char* getDesc()const{ return _T("Wait Taxi Pool Head"); }

	virtual void Execute(CARCportEngine* pEngine);
protected:
	LandsideTaxiPoolInSim* m_pTaxiPool;
};