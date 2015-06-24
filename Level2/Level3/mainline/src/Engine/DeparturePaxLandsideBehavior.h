#pragma once
#include "paxlandsidebehavior.h"
#include "Common\ALTObjectID.h"


class ALTObjectID;
class Processor;
class LandsideLayoutObjectInSim;

class DeparturePaxLandsideBehavior :
	public PaxLandsideBehavior
{

public:
	DeparturePaxLandsideBehavior(Person* p);
	~DeparturePaxLandsideBehavior(void);

	DeparturePaxLandsideBehavior* toDepBehavior(){ return this; }

	virtual int performanceMove(ElapsedTime p_time,bool bNoLog);

	virtual void InitializeBehavior();
	virtual bool IsGetOff(LandsideResourceInSim* pres);
	virtual bool OnVehicleParkAtPlace(LandsideVehicleInSim* pVehicle , LandsideResourceInSim* pRes,const ElapsedTime& t);

	virtual bool CanPaxTakeThisBus(LandsideResidentVehicleInSim  *pResidentVehicle, LandsideSimulation *pSimulation);
	virtual bool CanPaxTkeOffThisBusAtStation(LandsideBusStationInSim* pBusStation, LandsideSimulation *pSimulation);

	//check if this resource linked to first terminal processor, pRource is bus station or curbside
	virtual bool CanPaxTakeOffThisResource(LandsideResourceInSim* pResource,LandsideSimulation* pSimulation)const;
	//decide takeoff at this resource  to entry terminal after 
	virtual void ChooseTakeOffResource(LandsideLayoutObjectInSim* pLayout,LandsideSimulation* pSimulation);

protected:
	void processEntryLandside(const ElapsedTime& t);

	void processMoveToVehicle(const ElapsedTime& eEventTime);
	void processOnVehicle(const ElapsedTime& t);
	void processGetOffVehicle(const ElapsedTime& t);
	void processPickCarryonFromVehicle(const ElapsedTime& t);
	void processMoveToFacility(const ElapsedTime& t);
	void processArriveAtFacility(const ElapsedTime& t);

	void processArrivalAtCurbside(const ElapsedTime& t);
	void processArrivalAtTerminalPosition(const ElapsedTime& t);
	void processEntryTerminal( ElapsedTime p_time, double ifloor);
	void ProcessPutCarryonToVehicle( const ElapsedTime& eEventTime );
	void ProcessGetOnVehicle(const ElapsedTime& eEventTime);
	void ProcessLeaveQueue(const ElapsedTime& t);
protected:
	//link to the destination floor of terminal processor
	double			m_nLinkTerminalFloor;
	std::vector<ALTObjectID> m_vLinkTerminalProc;//processor list linked

	std::vector<LandsideBusStationInSim *> m_vDestBusStation;

};

