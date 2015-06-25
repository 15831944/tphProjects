#pragma once
#include "LandsidePaxVehicleInSim.h"

class LandsideLayoutObjectInSim;
class LandsideFacilityObject;
class LandsideSimulation;

enum eTaxiBehavior
{
	_DEPPAX,
	_NONPAX,
	_ARRPAX,
};

//////////////////////////////////////////////////////////////////////////
class LandsideTaxiInSim : public LandsidePaxVehicleInSim
{
public:
	//taxi that drop departure pax first
	LandsideTaxiInSim(const NonPaxVehicleEntryInfo& nonPaxEntry);
	LandsideTaxiInSim(const PaxVehicleEntryInfo& entryInfo);
	virtual bool InitBirth(CARCportEngine*pEngine);

	virtual LandsideTaxiInSim*	toTaxi(){ return this; }

	//virtual void OnActive(CARCportEngine*pEngine);
	bool CanServeTaxiQ(LandsideTaxiPoolInSim* pool, LandsideTaxiQueueInSim*pTaxiQ);
	bool StartServiceQueue(LandsideTaxiQueueInSim*pTaxiQ);
	bool IsAllPassengersGetOn();


	virtual bool ProceedToNextFcObject(CARCportEngine* pEngine);

	//
	virtual void ArrivalLayoutObject(LandsideLayoutObjectInSim* pObj,LandsideResourceInSim* pDetailRes, CARCportEngine* pEngine );
	virtual void LeaveLayoutObject( LandsideLayoutObjectInSim* pObj,LandsideResourceInSim* pDetailRes,CARCportEngine* pEngine );
	virtual void SuccessParkInCurb(LandsideCurbSideInSim*pCurb,IParkingSpotInSim* spot);

 
	//change route plan to  pick arrival pax when the taxi get into the pool
	void ChangeToPoolPlan(LandsideTaxiPoolInSim* pPool,CARCportEngine* pEngine);
	void ChangeToArrPaxPlan(CARCportEngine* pEngine);
	//once the vehicle get the service queue in the pool, it start move to next object
	LandsideTaxiQueueInSim* getSerivceQueue(){ return m_pServTaxiQueu; } 

	VehicleEntryInfo* getEntryInfo();
protected:
	eTaxiBehavior m_CurBehavior;


	NonPaxVehicleEntryInfo m_NonPaxEntryInfo;
	PaxVehicleEntryInfo m_ArrPaxEntryInfo;

	ElapsedTime	 _UnLoadPax(LandsideResourceInSim* pCurb,const ElapsedTime& t,CARCportEngine* pEngine);
	//is route defined to the taxi q
	bool haveRouteToTaxiQ(LandsideTaxiQueueInSim*pTaxiQ);

	LandsideTaxiQueueInSim* m_pServTaxiQueu; //the taxi q is going to 

};
