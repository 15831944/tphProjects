
#pragma once
#include "TrafficController.h"


class AirsideResource;
class AirportTrafficController;
class StandInSim;
class StandResourceManager;
class ResourceOccupy;
class ElapsedTime;
#include "AirsideResource.h"

class ENGINE_TRANSFER StandTrafficController : public TrafficController
{
public:
	explicit StandTrafficController(AirportTrafficController * pAirportController);
	bool Init(int nprjId,StandResourceManager * pStandRes );
	
	virtual Clearance* AssignNextClearance(FlightInAirsideSimulation * pFlight) ;


	//Assign a stand to the flight at the time
	StandInSim * AssignStand(FlightInAirsideSimulation* pFlight);

	//virtual Clearance * GetEnterResourceClearance(FlightInAirsideSimulation* pFlight,const FlightState&fltstate );
	virtual bool AmendClearance(FlightInAirsideSimulation * pFlight, Clearance* pClearance);


	//ResourceOccupy ConflictSolution(const ResourceOccupy& resOcy,FlightInAirsideSimulation * pFlight);
	//EnterSolution GetEnterStandSolution(FlightInAirsideSimulation * pFlight, StandInSim * pStand,const ElapsedTime& enterT);

protected:
	AirportTrafficController * m_pAirportController;
	StandResourceManager * m_pStandRes;


	ElapsedTime GetFlightEnterResourceTime(FlightInAirsideSimulation* pFlight);

	ElapsedTime GetStandBufferTime(FlightInAirsideSimulation* pLeadFlight, FlightInAirsideSimulation* pFollowFlight);

public:
	ElapsedTime GetPushBackTime(FlightInAirsideSimulation* pFlight);
	double GetPushBackSpeed(FlightInAirsideSimulation *pFlight);
	
};