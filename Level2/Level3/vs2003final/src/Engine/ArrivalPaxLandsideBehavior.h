#pragma once
#include "paxlandsidebehavior.h"

class ArrivalPaxLandsideBehavior :
	public PaxLandsideBehavior
{
public:
	ArrivalPaxLandsideBehavior(Person* p);
	~ArrivalPaxLandsideBehavior(void);
	
	virtual int performanceMove(ElapsedTime p_time,bool bNoLog);

	virtual void InitializeBehavior();

	virtual ArrivalPaxLandsideBehavior* toArrBehavior(){ return this; }

	virtual bool OnVehicleParkAtPlace(LandsideVehicleInSim* pVehicle, LandsideResourceInSim* pRes,const ElapsedTime&t);
	virtual bool CanBoardingOrBoarding(LandsideResourceInSim* pRes);

	virtual bool CanPaxTakeThisBus(LandsideResidentVehicleInSim  *pResidentVehicle, LandsideSimulation *pSimulation);
	virtual bool CanPaxTkeOffThisBusAtStation(LandsideBusStationInSim* pBusStation, LandsideSimulation *pSimulation);
protected:
	void processEntryLandisde(const ElapsedTime& t);
	void processGoToVehicleParking(const ElapsedTime& t);
	void processWaitForVehicle_old(const ElapsedTime& t);
	void processPutBag(const ElapsedTime& t);
	void processGetOnVehicle_old(const ElapsedTime& t);
	void processOnVehicle(const ElapsedTime& t);
	void ProcessLeaveQueue(const ElapsedTime& t);


	void ProcessWaitForVehicle(const ElapsedTime& eEventTime);
	void ProcessMoveToVehicle(const ElapsedTime& eEventTime);
	void ProcessPutCarryonToVehicle(const ElapsedTime& eEventTime);
	void ProcessGetOnVehicle(const ElapsedTime& eEventTime);
	void ProcessStayOnVehicle(const ElapsedTime& eEventTime);
	void ProcessGetOffVehicle(const ElapsedTime& eEventTime);
	void ProcessPickCarryOnFromVehicle(const ElapsedTime& eEventTime);

	void ProcessMoveToFacility(const ElapsedTime& eEventTime);
	void ProcessArriveAtFacility(const ElapsedTime& eEventTime);
// 	void processGoToBusStation(const ElapsedTime& t);



};
