#include "StdAfx.h"

#include "AirportTrafficController.h"


#include "FlightInAirsideSimulation.h"
#include "TaxiwayController.h"
#include "RunwayController.h"
#include "StandTrafficController.h"
#include "AirportResourceManager.h"

#include "AirsideController.h"
#include "AirsideSimConfig.h"

bool AirportTrafficController::Init( int nPrjID, AirportResourceManager* pResources,const AirsideSimConfig& simconf , CAirportDatabase* pAirportDatabase)
{
	m_pAirportRes = pResources;
	if(!simconf.bOnlySimStand)
	{
		m_pRunwayController = new RunwayTrafficController(this);
		m_pRunwayController->Init(nPrjID,pResources->getRunwayResource(), pAirportDatabase,pResources->getStandResource());
	}

	m_pStandController = new StandTrafficController(this);
	m_pStandController->Init(nPrjID,pResources->getStandResource() );
	
	if(!simconf.bOnlySimStand){
		m_pTaxiController = new TaxiwayTrafficController(this);
		m_pTaxiController->Init(nPrjID,pResources->getTaxiwayResource());
	}

	return true;
}

AirportTrafficController::AirportTrafficController( AirsideTrafficController * pAirsideController )
{
	m_pTaxiController = NULL; //new TaxiwayTrafficController(this);
	m_pStandController = NULL; //new StandTrafficController(this);
	m_pRunwayController = NULL; //new RunwayTrafficController(this);
	
	m_pAirsideController = pAirsideController;
}


TrafficController * AirportTrafficController::GetAppropriateController( AirsideResource * pResource )
{

	AirsideResource * pCurrentRes = pResource;

	ASSERT(pCurrentRes);

	switch(pCurrentRes->GetType()){
		case AirsideResource::ResType_RunwayPort:
			return GetRunwayTrafficController();
		case AirsideResource::ResType_TaxiwayDirSeg:
		case AirsideResource::ResType_IntersectionNode:
			return GetTaxiwayTrafficController();
		case AirsideResource::ResType_Stand:
			return GetStandTrafficController();
		default : return NULL;
	}	
}


Clearance* AirportTrafficController::AssignNextClearance( FlightInAirsideSimulation * pFlight )
{
	//flight birth , assign first clearance
	if(pFlight->GetFlightMode() == OnBirth )
	{
		if(pFlight->IsArrival()){
			Clearance * pNextClearance = NULL;
			if(GetRunwayTrafficController()){
				pNextClearance = GetRunwayTrafficController()->AssignNextClearance(pFlight);
				if(pNextClearance) return pNextClearance;
			}
			if(GetTaxiwayTrafficController()){
				return pNextClearance;
			}
		}
		if(pFlight->IsDeparture())
		{
			Clearance * pNextClearance = NULL;
			if(GetStandTrafficController())
			{
				pNextClearance = GetStandTrafficController()->AssignNextClearance(pFlight);
				if(pNextClearance)return pNextClearance;
			}
			return pNextClearance;
		}
	}

	AirsideResource * pCurrentres = pFlight->GetCurrentResource();
	if(!pCurrentres)
		return NULL;
	//
	return GetAppropriateController(pCurrentres)->AssignNextClearance(pFlight);	
}
//ResourceOccupy AirportTrafficController::ConflictSolution( AirsideResource* pNextResource,FlightInAirsideSimulation * pFlight )
//{
//	ASSERT(pNextResource);
//
//	switch(pNextResource->GetType())
//	{
//	case AirsideResource::ResType_Stand :
//		return GetStandTrafficController()->ConflictSolution(pNextResource,pFlight);
//	case AirsideResource::ResType_TaxiwayDirSeg :
//	case AirsideResource::ResType_IntersectionNode:
//		return GetTaxiwayTrafficController()->ConflictSolution(pNextResource,pFlight);
//	case AirsideResource::ResType_RunwayPort:
//		return GetRunwayTrafficController()->ConflictSolution(pNextResource, pFlight);
//	default :
//		return m_pAirsideController->ConflictSolution(pNextResource,pFlight);
//	}
//}

Clearance * AirportTrafficController::GetEnterResourceClearance( FlightInAirsideSimulation *pFlight, const FlightState& fltstate )
{
	const AirsideResource * pRes = fltstate.m_pResource.get();
	ASSERT(pRes);
	switch(pRes->GetType())
	{
	case AirsideResource::ResType_Stand :
		return GetStandTrafficController()->GetEnterResourceClearance(pFlight,fltstate);
	case AirsideResource::ResType_TaxiwayDirSeg:
	case AirsideResource::ResType_IntersectionNode:
		return GetTaxiwayTrafficController()->GetEnterResourceClearance(pFlight,fltstate);
	case AirsideResource::ResType_RunwayPort:
		return GetRunwayTrafficController()->GetEnterResourceClearance(pFlight,fltstate);
	default:
		return m_pAirsideController->GetEnterResourceClearance(pFlight,fltstate);
	}


}

bool AirportTrafficController::AmendClearance( FlightInAirsideSimulation * pFlight, Clearance* pClearance )
{
	
	if(pClearance && pClearance->m_pResource)
	{
		
		TrafficController * pController = GetAppropriateController(pClearance->m_pResource);
		if(pController)
		{
			return pController->AmendClearance(pFlight,pClearance);
		}
		else 
			return NULL;
		
	}else if(pClearance)
	{
		if(pClearance->m_nMode == OnTaxiToRunway || pClearance->m_nMode == OnTaxiToStand || pClearance->m_nMode == OnTaxiToDeice)
		{
			if(GetTaxiwayTrafficController())
				return GetTaxiwayTrafficController()->AmendClearance(pFlight,pClearance);
		}
	}

	pClearance->m_nMode = OnTerminate;
	return false;
}
