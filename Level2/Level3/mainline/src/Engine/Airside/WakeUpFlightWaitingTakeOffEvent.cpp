#include "StdAfx.h"
#include ".\wakeupflightwaitingtakeoffevent.h"
#include "AirsideFlightInSim.h"

CWakeUpFlightWaitingTakeOffEvent::CWakeUpFlightWaitingTakeOffEvent(AirsideFlightInSim* pWakeUpFlt)
:CAirsideMobileElementEvent(pWakeUpFlt)
{

}

CWakeUpFlightWaitingTakeOffEvent::~CWakeUpFlightWaitingTakeOffEvent(void)
{
}

int CWakeUpFlightWaitingTakeOffEvent::Process()
{
	AirsideFlightInSim* pFlight = (AirsideFlightInSim *)m_pMobileElement;
	if (pFlight)
	{
		pFlight->WakeUp(time);
	}
	return 1;
}