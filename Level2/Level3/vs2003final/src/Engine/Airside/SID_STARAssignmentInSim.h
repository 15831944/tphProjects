#pragma once
#include "../EngineDll.h"
#include "FlightPlanInSim.h"

class FlightSIDAssignmentList;
class FlightStarAssignmentList;
class AirsideFlightInSim;
class CAirportDatabase;
class CAirRoute;
class AirRouteNetworkInSim;
class LogicRunwayInSim;
class FlightRouteInSim;
class AirRouteSegInSim;

class ENGINE_TRANSFER CSID_STARAssignmentInSim
{
public:
	CSID_STARAssignmentInSim(void);
	~CSID_STARAssignmentInSim(void);

	void Init( int nPrjID, CAirportDatabase* pAirportDatabase);
	bool GetSTARRoute(AirsideFlightInSim* pFlight, AirsideResourceManager* pResManger,FlightRouteInSim*& pnewRoute);
	bool GetSIDRoute(AirsideFlightInSim* pFlight, AirsideResourceManager* pResManger,FlightRouteInSim*& pnewRoute);
	bool GetLandingCircuitRoute(AirsideFlightInSim* pFlight, AirsideResourceManager* pResManger,FlightRouteInSim*& pnewRoute);
	bool GetTakeoffCircuitRoute(AirsideFlightInSim* pFlight, AirsideResourceManager* pResManger,FlightRouteInSim*& pnewRoute);

//protected:
//	CLandingRunwayStarAssignmentList* m_pSTARAssignmentList;
//	CLandingRunwaySIDAssignmentList* m_pSIDAssignmentList;
private:
	FlightSIDAssignmentList* m_pFlightSIDAssignmentList;
	FlightStarAssignmentList* m_pFlightSTARAssignmentList;
	CAirportDatabase* m_pAirportDB;
};
