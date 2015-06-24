#include "StdAfx.h"
#include ".\flightplanedrunwaystrategyinsim.h"
#include "RunwayInSim.h"
#include "RunwayResourceManager.h"
#include "AirsideFlightInSim.h"
#include "AirTrafficController.h"
#include "FlightPlanInSim.h"
#include "AirspaceResource.h"

CFlightPlanedRunwayStrategyInSim::CFlightPlanedRunwayStrategyInSim(void)
{
}

CFlightPlanedRunwayStrategyInSim::~CFlightPlanedRunwayStrategyInSim(void)
{
}

LogicRunwayInSim* CFlightPlanedRunwayStrategyInSim::GetRunway(RunwayResourceManager* pRes)
{

	LogicRunwayInSim* pLogicRunway = NULL;
	FlightRouteInSim* pRoute = NULL;

	if (m_pFlight->GetMode() >= OnHeldAtStand)
	{
		m_pFlight->GetAirTrafficController()->GetFlightPlan()->GetSIDFlightPlan(m_pFlight,pRoute);
		if (pRoute)
			pLogicRunway = pRoute->GetRelateRunway() ;
	}
	else
	{
		m_pFlight->GetAirTrafficController()->GetFlightPlan()->GetSTARFlightPlan(m_pFlight,pRoute);
		if (pRoute)
			pLogicRunway = pRoute->GetRelateRunway() ;
	}
	if (pRoute)
	{
		delete pRoute;
		pRoute = NULL;
	}

	return pLogicRunway;
}