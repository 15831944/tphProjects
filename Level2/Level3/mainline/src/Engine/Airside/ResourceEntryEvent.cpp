#include "StdAfx.h"
#include ".\resourceentryevent.h"
#include "AirsideResource.h"
#include "AirsideFlightInSim.h"
#include "RunwayInSim.h"

ResourceEntryEvent::ResourceEntryEvent(void)
{
}

ResourceEntryEvent::~ResourceEntryEvent(void)
{
}

int ResourceEntryEvent::Process()
{

//write log

	//m_pFlight->AddNewEvent();
	return TRUE;
}

ResourceExitEvent::ResourceExitEvent(AirsideResource* pRes,AirsideFlightInSim* pFlight)
{
	m_pResource = pRes;
	m_pFlight = pFlight;
}

ResourceExitEvent::~ResourceExitEvent(void)
{
}

int ResourceExitEvent::Process()
{
	if (m_pResource && m_pResource->GetType() == AirsideResource::ResType_LogicRunway)
	{
        int nFlightID = m_pFlight->GetUID();
		((LogicRunwayInSim*)m_pResource)->RemoveLeftFlight(m_pFlight, getTime() );
	}
	return TRUE;
}