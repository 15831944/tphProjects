#include "StdAfx.h"
#include ".\airsideevent.h"
#include "FlightInAirsideSimulation.h"
#include "../../Common/ARCTracker.h"

AirsideEvent::AirsideEvent(void)
{
	//m_pFlight = NULL;
}

AirsideEvent::~AirsideEvent(void)
{ 

}

int AirsideEvent::process(CARCportEngine* _pEngine)
{
	PLACE_METHOD_TRACK_STRING();
	Process();

	return TRUE;
}

int AirsideEvent::kill()
{
	return TRUE;
}