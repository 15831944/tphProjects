#include "StdAfx.h"
#include ".\airsidefollowmecarserviceevent.h"
#include "TaxiRouteInSim.h"
#include "../../InputAirside/VehicleDefine.h"
#include "Clearance.h"
#include "TaxiwayResource.h"
#include "../../Common/elaptime.h"
#include "AirsideFollowMeCarInSim.h"
#include "AirsideFlightInSim.h"

AirsideFollowMeCarServiceEvent::AirsideFollowMeCarServiceEvent(CAirsideMobileElement* pMobileElement)
:CAirsideMobileElementEvent(pMobileElement)
{
	m_bWaiting = false;
	m_pServiceVehicle = ((AirsideFlightInSim*)m_pMobileElement)->GetServiceFollowMeCar();
}

AirsideFollowMeCarServiceEvent::~AirsideFollowMeCarServiceEvent(void)
{
}

int AirsideFollowMeCarServiceEvent::Process()
{
	if (((AirsideFlightInSim*)m_pMobileElement)->GetTime() < time)
	{
		((AirsideFlightInSim*)m_pMobileElement)->SetTime(time);
	}
	if (m_pServiceVehicle->GetTime() < time)
	{
		m_pServiceVehicle->SetTime(time);
	}

	bool bEnd = LeadingToDestination();

	if (m_bWaiting)
		return TRUE;

	if (bEnd)
	{
		ServiceFinish();
		return TRUE;
	}

	AirsideFollowMeCarServiceEvent* pNextEvent = new AirsideFollowMeCarServiceEvent(m_pMobileElement);
	pNextEvent->setTime(((AirsideFlightInSim*)m_pMobileElement)->GetTime());
	pNextEvent->addEvent();

	return TRUE;
}

bool AirsideFollowMeCarServiceEvent::LeadingToDestination()
{
	AirsideFlightInSim* pFlight = ((AirsideFlightInSim*)m_pMobileElement);

	ClearanceItem lastItem(pFlight->GetResource(),pFlight->GetMode(),pFlight->GetDistInResource());
	lastItem.SetSpeed(pFlight->GetSpeed());
	lastItem.SetTime(pFlight->GetTime());
	lastItem.SetPosition(pFlight->GetPosition());

	TaxiRouteInSim* pRouteToAbandonPoint = pFlight->GetRouteToAbandonPoint();
	if (pRouteToAbandonPoint)
	{
		Clearance newClearance;

		bool bEnd = pRouteToAbandonPoint->FindClearanceInConcern(pFlight,lastItem,50000,newClearance);

		if (bEnd && newClearance.GetItemCount() ==0)
		{			
			m_bWaiting = true;
			return false;
		}

		if (newClearance.GetItemCount() > 0)
		{
			pFlight->WriteLog();
			m_pServiceVehicle->WirteLog(m_pServiceVehicle->GetPosition(),m_pServiceVehicle->GetSpeed(),m_pServiceVehicle->GetTime());
			pFlight->PerformClearance(newClearance);
		}

		if (bEnd)
			return false;
	}

	return true;
}

void AirsideFollowMeCarServiceEvent::ServiceFinish()
{
	AirsideFlightInSim* pFlight = (AirsideFlightInSim*)m_pMobileElement;

	pFlight->SetFollowMeCarServiceCompleted();
	pFlight->WakeUp(pFlight->GetTime());

	m_pServiceVehicle->GetCompleteReturnRouteToPool();
	m_pServiceVehicle->WakeUp(m_pServiceVehicle->GetTime());
}