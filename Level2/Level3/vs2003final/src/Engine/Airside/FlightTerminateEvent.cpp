#include "StdAfx.h"
#include "./FlightTerminateEvent.h"
#include "AirsideFlightInSim.h"

AirsideFlightTerminateEvent::AirsideFlightTerminateEvent(AirsideFlightInSim* pFlight)
:CAirsideMobileElementEvent(pFlight)
,m_pCFlight(pFlight)
{

}

AirsideFlightTerminateEvent::~AirsideFlightTerminateEvent()
{

}

int AirsideFlightTerminateEvent::Process()
{
	Clearance theClearance;

	//terminate
	ClearanceItem teminatItem(NULL,OnTerminate, 0);
	teminatItem.SetTime(time);
	theClearance.AddItem(teminatItem);

	m_pCFlight->PerformClearance(theClearance);
	return TRUE;
}

