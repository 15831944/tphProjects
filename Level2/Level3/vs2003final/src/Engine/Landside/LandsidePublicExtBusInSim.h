#pragma once
#include "LandsidePaxVehicleInSim.h"
#include "State_LandsideVehicle.h"



class LandsideLayoutObjectInSim;
class LandsideFacilityObject;
class LandsideSimulation;
class LandsideVehicleServiceEndEvent;
//////////////////////////////////////////////////////////////////////////
class LandsidePublicExtBusInSim : public LandsidePaxVehicleInSim
{
public:
	LandsidePublicExtBusInSim(const PaxVehicleEntryInfo& entryInfo);
	virtual LandsidePublicExtBusInSim *toPublicExBus(){ return this; }

	virtual bool InitBirth(CARCportEngine *_pEngine);
	virtual bool ProceedToNextFcObject(CARCportEngine* pEngine);

	virtual void ArrivalLayoutObject(LandsideLayoutObjectInSim* pObj,LandsideResourceInSim* pDetailRes, CARCportEngine* pEngine );
	virtual void SuccessParkInBusStation(LandsideBusStationInSim* pStation,LaneParkingSpot* spot);

	virtual void NotifyPaxMoveInto(CARCportEngine*pEngine, LandsideBaseBehavior* pPassenger, const ElapsedTime& eTime);
	bool CanLoadPax(PaxLandsideBehavior* pPax);

	ElapsedTime	 _UnLoadPax(LandsideBusStationInSim* pBusStation,const ElapsedTime& t,CARCportEngine* pEngine);
	bool IsReadyToLeave(LandsideBusStationInSim* pBustation)const;
protected:
	//
	bool IsFull()const;
	bool IsAllPassengersReady()const;
};


//////////////////////////////////////////////////////////////////////////
class State_ProcessAtBusStationExtBus : public State_LandsideVehicle<LandsidePublicExtBusInSim>
{
public:
	State_ProcessAtBusStationExtBus(LandsidePublicExtBusInSim* pBus , LandsideBusStationInSim* pStation,LaneParkingSpot* spot)
		:State_LandsideVehicle<LandsidePublicExtBusInSim>(pBus){ m_pBusStation = pStation; m_spot= spot; }

	void Entry(CARCportEngine* pEngine);
	void Execute(CARCportEngine* pEngine);

	virtual const char* getDesc()const { return "Ext Bus Process at BusStation"; }
protected:
	LandsideBusStationInSim* m_pBusStation;
	State_ServiceTimer m_waitTimer;
	LaneParkingSpot* m_spot;
};
