#include "StdAfx.h"
#include ".\flightrunwaytakeoffevent.h"
#include "FlightGetClearanceEvent.h"
#include "AirsideFlightInSim.h"
#include "AirspaceResource.h"
#include "AirTrafficController.h"
#include "AirsideResourceManager.h"
#include "AirspaceResourceManager.h"
#include "AirHoldInInSim.h"


CFlightDelayTakeoffOnRwyEvent::CFlightDelayTakeoffOnRwyEvent(AirsideFlightInSim * pFlight,Clearance& takeoffClearance)
							:CAirsideMobileElementEvent(pFlight)
{
	m_pFlight = pFlight; 
	m_takeOffClearance = takeoffClearance;
}
CFlightDelayTakeoffOnRwyEvent::~CFlightDelayTakeoffOnRwyEvent()
{

}


int CFlightDelayTakeoffOnRwyEvent::Process()
{
	ASSERT(m_pFlight);
	m_pFlight->PerformClearance(m_takeOffClearance);
	if(m_takeOffClearance.GetItemCount())
	{
		
		FlightGetClearanceEvent * pNextEvent = new FlightGetClearanceEvent(m_pFlight);
		pNextEvent->setTime(m_pFlight->GetTime());
		pNextEvent->addEvent();;
	}
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
CFlightTakeOffOnRwyEvent::CFlightTakeOffOnRwyEvent( AirsideFlightInSim * pFlight )
:CAirsideMobileElementEvent(pFlight)
{

}

CFlightTakeOffOnRwyEvent::~CFlightTakeOffOnRwyEvent()
{

}

int CFlightTakeOffOnRwyEvent::Process()
{
	AirsideFlightInSim* pFlight = (AirsideFlightInSim*)m_pMobileElement;
	Clearance newClearence;

	if( !pFlight )
		return TRUE;

// 	ClearanceItem curItem(pFlight->GetResource(),pFlight->GetMode(),pFlight->GetDistInResource());
// 	curItem.SetTime(time);
// 	curItem.SetSpeed(pFlight->GetSpeed());
// 
// 	FlightRouteInSim* pSID = pFlight->GetSID();
// 	AirRouteNetworkInSim* pAirrouteNetwork = pFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetAirspaceResource();
// 	if (pAirrouteNetwork)
// 	{
// 		pAirrouteNetwork->FindClearanceInConcern(pFlight,pSID,curItem,newClearence);
// 
// 		if(newClearence.GetItemCount() >0)
// 		{		
// 			pFlight->WriteLog();
// 			pFlight->PerformClearance(newClearence);	
// 		}
// 	}

	pFlight->WakeUp(pFlight->GetTime());
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
CFlightDelayOnHoldEvent::CFlightDelayOnHoldEvent(AirsideFlightInSim * pFlight,AirEntrySystemHoldInInSim* pEntrySystemHold)
:m_pEntrySystemHold(pEntrySystemHold)
,m_pFlightInSim(pFlight)
{

}

CFlightDelayOnHoldEvent::~CFlightDelayOnHoldEvent()
{

}

int CFlightDelayOnHoldEvent::Process()
{
	if (m_pEntrySystemHold)
	{
		m_pEntrySystemHold->OnFlightExit(m_pFlightInSim,time);
	}

	return 1;
}