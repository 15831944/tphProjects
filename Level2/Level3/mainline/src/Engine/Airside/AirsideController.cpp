#include "StdAfx.h"
#include ".\airsidecontroller.h"

#include "AirsideResourceManager.h"
#include "FlightInAirsideSimulation.h"
#include "AirsideSimConfig.h"
AirsideTrafficController::AirsideTrafficController(void)
{

	m_pAirspaceController = NULL;
	m_pAirsideRes = NULL;

}


AirsideTrafficController::~AirsideTrafficController(void)
{
	//delete all the controllers;

}

bool AirsideTrafficController::Init(int nPrjID, AirsideResourceManager* pResources,const AirsideSimConfig& simconf, CAirportDatabase *pAirportDatabase)
{
	ASSERT(pAirportDatabase);
	
	m_pAirsideRes = pResources;
	
	if(!simconf.bOnlySimStand)
	{
		m_pAirspaceController = new AirspaceTrafficController(this);
		if(! m_pAirspaceController->Init(nPrjID,pResources->GetAirspaceResource(), pAirportDatabase)) return false;
	}	
	
	for(AirsideResourceManager::AirportResourceList::iterator itr =  pResources->m_vAirportRes.begin();itr!= pResources->m_vAirportRes.end();++itr){
		m_vAirportController[(*itr).first] = new AirportTrafficController(this);
		if(! m_vAirportController[(*itr).first]->Init(nPrjID, &(*itr).second, simconf, pAirportDatabase) ) return false;
	}

	return true;
}

Clearance* AirsideTrafficController::AssignNextClearance( FlightInAirsideSimulation * pFlight )
{
	
	//flight on birth
	if(pFlight->GetFlightMode() == OnBirth)
	{
		Clearance * pClearance = NULL;
		
		if(pFlight->IsArrival()){
			pClearance = GetAirspaceController()? GetAirspaceController()->AssignNextClearance(pFlight) :NULL;
			if(pClearance)return pClearance;
			pClearance = GetAirportController()? GetAirportController()->AssignNextClearance(pFlight) : NULL;
			return pClearance;
		}
		else {
			pClearance = GetAirportController()?GetAirportController()->AssignNextClearance(pFlight): NULL;
			return pClearance;
		}
		
	}

	if(pFlight->GetFlightMode() == OnTerminate) return NULL;
	
	TrafficController * pController = GetAppropriateController(pFlight);
	if(pController)
	{
		return pController->AssignNextClearance(pFlight);
	}

	return NULL;
}

TrafficController* AirsideTrafficController::GetAppropriateController( FlightInAirsideSimulation * pFlight )
{
	
	AirsideResource * pCurrentRes = pFlight->GetCurrentResource();
	

	if(pFlight->GetFlightMode() == OnClimbout 
		|| pFlight->GetFlightMode() == OnTerminal
		|| pFlight->GetFlightMode() == OnCruiseArr
		|| pFlight->GetFlightMode() == OnCruiseDep
		|| pFlight->GetFlightMode() == OnTakeoff
		|| pFlight->GetFlightMode() == OnApproach
		|| pFlight->GetFlightMode() == OnFinalApproach)
	{
		return GetAirspaceController();
	}
	else 
		return GetAirportController();
	
}

AirspaceTrafficController * AirsideTrafficController::GetAirspaceController()
{
	return m_pAirspaceController;
}

AirportTrafficController * AirsideTrafficController::GetAirportController( int nAirportID )
{
	ASSERT(FALSE);
	return NULL;
}

AirportTrafficController * AirsideTrafficController::GetAirportController()
{
	ASSERT(m_vAirportController.size()>0);
	return m_vAirportController.begin()->second;
}

Clearance * AirsideTrafficController::GetEnterResourceClearance(FlightInAirsideSimulation* pFlight,const FlightState& fltstate )
{
	
	const AirsideResource * pRes = fltstate.m_pResource.get();
	ASSERT(pRes);
	if(!pRes)return NULL;
	
	switch(pRes->GetType())
	{
	case AirsideResource::ResType_Stand:
	case AirsideResource::ResType_TaxiwayDirSeg:
	case AirsideResource::ResType_DeiceStation:
	case AirsideResource::ResType_IntersectionNode:
		return GetAirportController()->GetEnterResourceClearance(pFlight,fltstate);
	case AirsideResource::ResType_WayPoint:
	case AirsideResource::ResType_AirRouteSegment:
		return GetAirspaceController()?GetAirspaceController()->GetEnterResourceClearance(pFlight,fltstate):NULL;
	default :
		return NULL;
	}
}

