#pragma once

#include "TrafficController.h"
#include "AirspacePlaner.h"


class AirspaceResourceManager;

class AirsideTrafficController;
class FlightInAirsideSimulation;
class ResourceOccupy;
class AirsideResource;
class ElapsedTime;


class InputTerminal;
class CAirportDatabase;

class ENGINE_TRANSFER AirspaceTrafficController : public TrafficController
{
public:
	explicit AirspaceTrafficController(AirsideTrafficController * pAirsideController);
	~AirspaceTrafficController();

	bool Init(int nPrjID, AirspaceResourceManager* pAirspaceRes, CAirportDatabase *pAirportDatabase);


	void SetSeparationManager(AircraftSeparationManager * pSeparationMan){ m_pSeparationManager = pSeparationMan; } 

	virtual Clearance* AssignNextClearance(FlightInAirsideSimulation * pFlight) ;
	virtual bool AmendClearance(FlightInAirsideSimulation * pFlight, Clearance* pClearance); 

	
	FlightPlanInSim& GetArrivalFlightPlan(FlightInAirsideSimulation * pFlight);
	FlightPlanInSim& GetDepartureFlightPlan(FlightInAirsideSimulation * pFlight);

protected:
	
	TrafficController * GetNextController(FlightInAirsideSimulation* pFlight);
	//
	ResourceOccupy ConflictSolution(const ResourceOccupy& resOcy,FlightInAirsideSimulation * pFlight);
	

protected:
	AirspaceResourceManager * m_pAirspaceRes;
	AirsideTrafficController  * m_pAirsideController;

	ElapsedTime GetInTrailSeperationTime(FlightInAirsideSimulation* pLeadFlight, FlightInAirsideSimulation* pTrailFlight, AirsideMobileElementMode mode);
	AircraftSeparationManager* m_pSeparationManager;


protected:	

	typedef std::map<FlightInAirsideSimulation* , FlightPlanInSim> FlightPlanMap;
	FlightPlanMap m_vArrivalFlightPlans; 
	FlightPlanMap m_vDepartureFlightPlans;

	ns_FlightPlan::FlightPlans*  m_pflightPlanInput;
	CAirportDatabase *m_pAirportDatabase;
};
