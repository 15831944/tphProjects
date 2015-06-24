#include "StdAfx.h"
#include ".\removeairsidemobileelementeventevent.h"
#include "AirsideFlightInSim.h"
CRemoveAirsideMobileElementEventEvent::CRemoveAirsideMobileElementEventEvent(AirsideFlightInSim *pFlight)
:CAirsideMobileElementEvent(pFlight)
{
}

CRemoveAirsideMobileElementEventEvent::~CRemoveAirsideMobileElementEventEvent(void)
{
}

int CRemoveAirsideMobileElementEventEvent::Process()
{
	return 1;
}