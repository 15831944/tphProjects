#include "StdAfx.h"
#include ".\airsideremoveflightfromqueueevent.h"

#include "AirsideFlightInSim.h"
#include "RunwayInSim.h"
CAirsideRemoveFlightFromQueueEvent::CAirsideRemoveFlightFromQueueEvent( LogicRunwayInSim *pCheckRunway,AirsideFlightInSim *pFlight )
{
	m_pRunway = pCheckRunway;
	m_pMobileElement = pFlight;
}
CAirsideRemoveFlightFromQueueEvent::~CAirsideRemoveFlightFromQueueEvent(void)
{
}

int CAirsideRemoveFlightFromQueueEvent::Process()
{
	if(m_pRunway && m_pMobileElement)
	{
		m_pRunway->RemoveFlightInQueue(m_pMobileElement);

	}
	return 1;

}