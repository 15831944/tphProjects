#include "StdAfx.h"
#include ".\wakeupflightwaitinglandingevent.h"
#include "AirsideFlightInSim.h"

CWakeUpFlightWaitingLandingEvent::CWakeUpFlightWaitingLandingEvent(AirsideFlightInSim *pWakeUpFlight)
:CAirsideMobileElementEvent(pWakeUpFlight)
{
	//m_eSafeTime = tSafeTime;
}

CWakeUpFlightWaitingLandingEvent::~CWakeUpFlightWaitingLandingEvent(void)
{
}

int CWakeUpFlightWaitingLandingEvent::Process()
{

	AirsideFlightInSim* pFlight = (AirsideFlightInSim *)m_pMobileElement;
	if (pFlight)
	{
		pFlight->WakeUp(time);
	}
	return 1;
}