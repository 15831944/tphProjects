#include "StdAfx.h"
#include ".\flightmovetomeetingpointevent.h"
#include "TaxiRouteInSim.h"
#include "Clearance.h"
#include "../../Common/elaptime.h"
#include "AirsideFollowMeCarInSim.h"
#include "AirsideFlightInSim.h"
#include "AirsideMeetingPointInSim.h"

const double RadiusOfConcernOnTaxi = 50000;
FlightMoveToMeetingPointEvent::FlightMoveToMeetingPointEvent(CAirsideMobileElement* pMobileElement)
:CAirsideMobileElementEvent(pMobileElement)
{

}

FlightMoveToMeetingPointEvent::~FlightMoveToMeetingPointEvent(void)
{
}

int FlightMoveToMeetingPointEvent::Process()
{
	AirsideFlightInSim* pFlight = (AirsideFlightInSim*)m_pMobileElement;

	if (pFlight == NULL || pFlight->GetRouteToMeetingPoint() == NULL)
		return TRUE;
	
	Clearance newClearance;
	ClearanceItem lastClearanceItem(pFlight->GetResource(),pFlight->GetMode(),pFlight->GetDistInResource());
	lastClearanceItem.SetSpeed(pFlight->GetSpeed());
	lastClearanceItem.SetTime(pFlight->GetTime());
	lastClearanceItem.SetAltitude(0);
	lastClearanceItem.SetPosition(pFlight->GetPosition());

	bool btillEnd = pFlight->GetRouteToMeetingPoint()->FindClearanceInConcern(pFlight,lastClearanceItem,RadiusOfConcernOnTaxi,newClearance);
	if (btillEnd && newClearance.GetItemCount() ==0)	//waiting
		return TRUE;

	if (newClearance.GetItemCount() >0)
	{
		pFlight->WriteLog();
		pFlight->PerformClearance(newClearance);
	}

	if(btillEnd)
	{
		FlightMoveToMeetingPointEvent* pNextEvent = new FlightMoveToMeetingPointEvent(m_pMobileElement);
		pNextEvent->setTime(pFlight->GetTime());
		pNextEvent->addEvent();

		return TRUE;
	}

	if (pFlight->GetTemporaryParking() && pFlight->GetTemporaryParking()->GetParkingType() == TempParkingInSim::TempStand)
		pFlight->SetMode(OnTaxiToTempParking);
	else
		pFlight->SetMode(OnTaxiToStand);

	if (pFlight->GetServiceFollowMeCar() && pFlight->GetServiceFollowMeCar()->IsReadyToService(pFlight))
	{
		pFlight->GetServiceFollowMeCar()->ServicingFlight();
	}

	return TRUE;

}