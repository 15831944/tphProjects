#include "StdAfx.h"
#include ".\flightapplyfortowserviceevent.h"
#include "AirsideFlightInSim.h"
#include "AirsideResource.h"
#include "StandLeadInLineInSim.h"
#include "AirsideFlightDeiceRequirment.h"
#include "AirTrafficController.h"

FlightApplyForTowServiceEvent::FlightApplyForTowServiceEvent(AirsideFlightInSim* pFlight)
:AirsideEvent()
,m_pFlight(pFlight)
{
}

FlightApplyForTowServiceEvent::~FlightApplyForTowServiceEvent(void)
{
}

int FlightApplyForTowServiceEvent::Process()
{
	if (m_pFlight->GetResource() == NULL)
		return TRUE;

	FlightDeiceRequirment deiceRequirment;
	m_pFlight->GetAirTrafficController()->UpdateFlightDeiceRequirment(m_pFlight, deiceRequirment);


	//if(deiceRequirment.bNeedDeice())
	//	return TRUE;

	if (m_pFlight->GetResource()->GetType() == AirsideResource::ResType_Stand )
	{
		m_pFlight->ApplyForTowTruckService((StandInSim*)m_pFlight->GetResource(),time);
	}
	else if (m_pFlight->GetResource()->GetType() == AirsideResource::ResType_StandLeadInLine)
	{
		m_pFlight->ApplyForTowTruckService(((StandLeadInLineInSim*)m_pFlight->GetResource())->GetStandInSim(),time);
	}

	return TRUE;

}

FlightApplyForDeiceServiceEvent::FlightApplyForDeiceServiceEvent( AirsideFlightInSim* pFlight )
:AirsideEvent()
,m_pFlight(pFlight)
{

}

FlightApplyForDeiceServiceEvent::~FlightApplyForDeiceServiceEvent( void )
{

}

int FlightApplyForDeiceServiceEvent::Process()
{
	if (m_pFlight->GetResource() == NULL)
		return TRUE;


	m_pFlight->ApplyForDeiceSerive(time);

	return TRUE;
}