#include "StdAfx.h"

#include "AirsideFlightInSim.h"
#include "./FlightMovementEvent.h"


int FlightMovementEvent::Process()
{
	m_pFlight->PerformClearanceItem(m_clearanceitem);
	return TRUE;
}



FlightMovementEvent::FlightMovementEvent( AirsideFlightInSim * pFlight ,const ClearanceItem& item )
:CAirsideMobileElementEvent(pFlight),
m_clearanceitem(item)
{
	m_pFlight = pFlight;	
}