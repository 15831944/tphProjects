#include "StdAfx.h"
#include ".\wakeuprunwayevent.h"
#include "AirsideFlightInSim.h"
#include "RunwayInSim.h"
CWakeUpRunwayEvent::CWakeUpRunwayEvent(AirsideFlightInSim *pFlight,RunwayInSim *pWakeUpRunway)
{
	m_pWakeUpRunway = pWakeUpRunway;
	m_pMobileElement = pFlight;
	m_bInvalid = true;
}

CWakeUpRunwayEvent::~CWakeUpRunwayEvent(void)
{
}

int CWakeUpRunwayEvent::Process()
{
	if(m_bInvalid == false)
		return 1;

	if(m_pWakeUpRunway)
	{
		m_pWakeUpRunway->WakeUpRunwayLanding(time);
		m_pWakeUpRunway->SetWakeupRunwayEvent(NULL);
	}

	return 1;
}

void CWakeUpRunwayEvent::SetInvalid( bool bInvalid )
{
	m_bInvalid = bInvalid;
}