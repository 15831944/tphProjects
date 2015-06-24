#pragma once

#include "TrafficController.h"
#include "RunwayResourceManager.h"
#include "FlightTypeRunwayAssignmentStrategiesInSim.h"
#include "RunwayExitAssignmentStrategiesInSim.h"
#include "StandResourceManager.h"

class AirportTrafficController;
class FlightInAirsideSimulation;
class ResourceOccupy;
class ElapsedTime;
class AircraftSeparationManager;

//all major task is controller the flight how to land and depart on the runway/assign runway to the flight

class ENGINE_TRANSFER RunwayTrafficController : public TrafficController
{

public:
	explicit RunwayTrafficController(AirportTrafficController * pAirportController);

public:
	bool Init(int prjID, RunwayResourceManager * pRunwayRes, CAirportDatabase* pAirportDatabase,StandResourceManager* pStandRes);

	virtual Clearance* AssignNextClearance(FlightInAirsideSimulation * pFlight);


	//virtual Clearance * GetEnterResourceClearance(FlightInAirsideSimulation* pFlight,const FlightState& fltstate );

	RunwayExitInSim * ChooseRunwayExit(FlightInAirsideSimulation * pFlight, RunwayPortInSim * pRunway);

	void SetSeparationManager(AircraftSeparationManager * pSeparationMan){ m_pSeparationManager = pSeparationMan; }

public:
	
	RunwayPortInSim * GetLandingRunway(FlightInAirsideSimulation * pFlight);
	RunwayPortInSim * GetTakeoffRunway(FlightInAirsideSimulation * pFlight);


	//resolve conflict thought amend clearance
	bool AmendClearance(FlightInAirsideSimulation* pFlight, Clearance * pClearance)	;

	
		// get flight take off position on specified runway
	IntersectionNodeInSim * GetTakeoffPosition(FlightInAirsideSimulation * pFlight, RunwayPortInSim* pRunway);
	
	
protected:
	//EnterSolution GetLandOnRunwaySolution(FlightInAirsideSimulation* pFlight,RunwayPortInSim * pRunway, const ElapsedTime& enterT);
	//EnterSolution GetPreTakeoffRunwaySolution(FlightInAirsideSimulation* pFlight, RunwayPortInSim * pRunway, const ElapsedTime& enterT);
	//EnterSolution GetTakeoffRunwaySolution(FlightInAirsideSimulation* pFlight, RunwayPortInSim * pRunway, const ElapsedTime& enterT);

	RunwayPortInSim* AssignLandRunway(FlightInAirsideSimulation* pFlight);
	//RunwayPortInSim* GetLandRunway(FlightInAirsideSimulation* pFlight);

	RunwayPortInSim* AssignTakeoffRunway(FlightInAirsideSimulation  *pFlight);
	//RunwayPortInSim* GetTakeoffRunway(FlightInAirsideSimulation* pFlight);

	
	ElapsedTime GetSeparationLandingBehindLanding(FlightInAirsideSimulation * leadflight, FlightInAirsideSimulation * trailflight, bool intersectRw = false);
	ElapsedTime GetSeparationLandingBehindTakeoff(FlightInAirsideSimulation * leadflight, FlightInAirsideSimulation * trailflight, bool intersectRw = false);
	ElapsedTime GetSeparationTakeoffBehindLanding(FlightInAirsideSimulation * leadflight, FlightInAirsideSimulation * trailflight, bool intersectRw = false);
	ElapsedTime GetSeparationTakeoffBehindTakeoff(FlightInAirsideSimulation * leadflight, FlightInAirsideSimulation * trailflight, bool intersectRw = false);	


protected:
	RunwayResourceManager * m_pRunwayRes;
	StandResourceManager* m_pStandRes;
	AirportTrafficController * m_pAirportController;
	AircraftSeparationManager * m_pSeparationManager;
	CAirportDatabase* m_pAirportDatabase;

	CFlightTypeRunwayAssignmentStrategiesInSim m_RunwayAssignmentStrategies; 
	CRunwayExitAssignmentStrategiesInSim m_RunwayExitAssignmentStrategies;

};
