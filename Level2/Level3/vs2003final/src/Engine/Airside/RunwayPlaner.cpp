#include "StdAfx.h"

#include ".\runwayplaner.h"
#include "./../../Common/ARCUnit.h"
#include "FlightInAirsideSimulation.h"
#include "./AirspaceResourceManager.h"
#include "./RunwayResourceManager.h"
#include "./TaxiwayResourceManager.h"

#include "FlightTypeRunwayAssignmentStrategyItemInSim.h"


RunwayPlaner::RunwayPlaner(void)
{
}

RunwayPlaner::~RunwayPlaner(void)
{
}

AirsideResource* RunwayPlaner::ResourceAssignment(FlightInAirsideSimulation *pFlight)
{
	if(pFlight->getFlightState() == OnApproach ){
		RouteApproach * pRouteApproach = (RouteApproach*) pFlight->GetCurrentResource();
		
		AirsideResource * pNextRes = pRouteApproach->prunway;
		if(pNextRes){
			pFlight->setFlightState(LandingOnRuwway);pFlight->SetBackUp(false);
			return pNextRes;
		}
	}

	if( pFlight->getFlightState() == LandingOnRuwway ){
		RunwayInSim  * pRunway =(RunwayInSim*) pFlight->GetCurrentResource();
		AirsideResource * pNextRes = GetOutExit(pRunway,pFlight);
		if(pNextRes){
			pFlight->setFlightState(ExitRunway);pFlight->SetBackUp(false);
			return pNextRes;
		}
	}

	
	if(pFlight->getFlightState() == ExitTaxiway){	

		RunwayExitInSim * pExit  = (RunwayExitInSim*) pFlight->GetCurrentResource();
		
		pFlight->setFlightState(PreTakeOff);pFlight->SetBackUp(false);
		return pExit->pRunway;
		
	}

	if(pFlight->getFlightState() == PreTakeOff ){
		pFlight->setFlightState(TakeoffOnRunway);pFlight->SetBackUp(false);
		return pFlight->GetCurrentResource();	
	}

	

	return NULL;
}

bool RunwayPlaner::Init( int nPrjID, int nAirportID, RunwayResourceManager * pRunwayRes,CAirportDatabase *pAirportDatabase  )
{
	m_RunwayStategies.Init(nPrjID);
	m_pAirportDatabase = pAirportDatabase;
	m_pRunwayRes = pRunwayRes;
	return true;
}

RunwayExitInSim * RunwayPlaner::GetOutExit( RunwayInSim * pRunway, FlightInAirsideSimulation* pFlight )
{
		
	return pRunway->TaxiInExit(pFlight);
}

RunwayExitInSim * RunwayPlaner::GetInExit( FlightInAirsideSimulation *pFlight )
{
	TaxiwaySegInSim * pTaxiSeg = (TaxiwaySegInSim* )pFlight->GetCurrentResource();
	

	return NULL;
}

