#include "StdAfx.h"
#include ".\flightexitstandevent.h"
#include "../../inputs/flight.h"
#include "../../Common/elaptime.h"
#include "../terminal.h"
#include "../BusBoardingCall.h"
#include "AirTrafficController.h"
#include "../BoardingCallPolicy.h"
#include "AirsideFlightInSim.h"
#include "../../Common/ARCTracker.h"
#include "../BridgeConnector.h"
#include "StandInSim.h"
#include <CommonData/BaseProcess.h>
#include "../ARCportEngine.h"
#include "../OnboardSimulation.h"


FlightExitStandEvent::FlightExitStandEvent(AirsideFlightInSim* AirsideFlt)
:CAirsideMobileElementEvent(NULL),
m_pFlight(NULL),m_pAirsideFlt(AirsideFlt)
{
}

FlightExitStandEvent::~FlightExitStandEvent(void)
{
}
// if flight is is turnaround, ReflectOnFlightArriveStand will be invoked twice
int FlightExitStandEvent::process(CARCportEngine* _pEngine)
{
	PLACE_METHOD_TRACK_STRING();

	if(m_pAirsideFlt)
		m_pAirsideFlt->DisConnectBridges(_pEngine,getTime());

	return 0;
}

void FlightExitStandEvent::SetExitFlight(Flight* pFlight)
{
	ASSERT(pFlight != NULL);

	m_pFlight = pFlight;
	m_pMobileElement = (CAirsideMobileElement *)pFlight;
}

void FlightExitStandEvent::SetActualDepartureTime(const ElapsedTime& dtTime)
{
	m_dtActualDepTime = dtTime;
}
