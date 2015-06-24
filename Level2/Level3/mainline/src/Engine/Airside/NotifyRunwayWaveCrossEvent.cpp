#include "StdAfx.h"
#include ".\notifyrunwaywavecrossevent.h"
#include "AirsideFlightInSim.h"
CNotifyRunwayWaveCrossEvent::CNotifyRunwayWaveCrossEvent(RunwayInSim *pNotifyRunway,AirsideFlightInSim* pFlt)
{
	m_pRunway = pNotifyRunway;
	m_pFlight = pFlt;
}

CNotifyRunwayWaveCrossEvent::~CNotifyRunwayWaveCrossEvent(void)
{
}

int CNotifyRunwayWaveCrossEvent::Process()
{

//	m_pRunway->NotifyWaveCross()






	return 1;
}