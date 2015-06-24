#pragma once
#include "../EngineDll.h"

#include "ResourcePlaner.h"
#include "../../InputAirside/AirRoute.h"
#include "../../InputAirside/FlightPlan.h"
#include "SID_STARAssignmentInSim.h"
#include "AirspaceResource.h"

class InputTerminal;
class AirsideResourceManager;
class AirspaceResourceManager;
class AirsideResource;
class RunwayInSim;
class AirWayPointInSim;
class AirWayPointInSimList;
class CAirRoute;

#include "FlightInAirsideSimulation.h"
//static resource assignment like a quire system 
class ENGINE_TRANSFER AirspacePlaner : public ResourcePlaner
{
public:

	//AirsideResource* ResourceAssignment( FlightInAirsideSimulation* pFlight);	
	
	bool Init(int nPrjID, InputTerminal *pInterm, AirspaceResourceManager * pAirsideRes, CAirportDatabase* pAirportDatabase);	


	double GetPlanSpeed(FlightInAirsideSimulation *pFlight, AirWayPointInSim * pWaypoint );
	double GetPlanAltitude(FlightInAirsideSimulation *pFlight,AirWayPointInSim  * pWaypoint);
	
	AirspaceResourceManager * GetAirspaceResource(){ return m_pAirspaceRes; }

	// return approach ,terminal , enroute, climb out mode
	AirsideMobileElementMode GetFlightModeInResource(FlightInAirsideSimulation *pFlight, AirsideResource * resource)const;

	//CAirRoute* GetFlightSIDByRunwayPort(RunwayPortInSim* pRunway, FlightInAirsideSimulation* pFlight);
	//CAirRoute* GetFlightSTARByRunwayPort(RunwayPortInSim* pRunway, FlightInAirsideSimulation* pFlight);
		
protected :

	AirsideResourceManager * m_pAirsideRes;
	AirspaceResourceManager * m_pAirspaceRes;
	CAirportDatabase* m_pAirportDatabase;
	//ns_FlightPlan::FlightPlans m_FltPlans;
	//InputTerminal * m_pInTerm;
	
	//map for the flight -> CFlightPlan
	std::map<FlightInAirsideSimulation*, ns_FlightPlan::FlightPlan* > m_FltPlanMap;

	ns_FlightPlan::FlightPlan * findCompatiblePlan(FlightInAirsideSimulation * pFlight);
	ns_FlightPlan::FlightPlan * getCompatiblePlan(FlightInAirsideSimulation * pFlight);

	CSID_STARAssignmentInSim m_SID_STARAssignmentInSim;
};
