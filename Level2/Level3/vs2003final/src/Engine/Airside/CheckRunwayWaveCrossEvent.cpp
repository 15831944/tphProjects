#include "StdAfx.h"
#include ".\checkrunwaywavecrossevent.h"
#include "AirsideFlightInSim.h"
#include "RunwayInSim.h"


CCheckRunwayWaveCrossEvent::CCheckRunwayWaveCrossEvent(RunwayInSim *pCheckRunway,AirsideFlightInSim *pFlight)
{
	m_pRunway = pCheckRunway;
	m_pMobileElement = pFlight;
	
}

CCheckRunwayWaveCrossEvent::~CCheckRunwayWaveCrossEvent(void)
{
}

int CCheckRunwayWaveCrossEvent::Process()
{
	if(m_pRunway)
		m_pRunway->CheckWaveCross(m_pMobileElement,time);
	
	return 1;
}