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

	Terminal* pTerminal = _pEngine->getTerminal();
	//check the flight has bus service or not
	if(pTerminal && m_pAirsideFlt)
	{
		//get stand
		//if(m_pAirsideFlt->GetResource() && m_pAirsideFlt->GetResource()->GetType() == AirsideResource::ResType_StandLeadInLine)
		{
			//StandInSim *pStandInSim = (StandInSim *)m_pAirsideFlt->GetResource();
			//if(pStandInSim)
			//{
			//	ALTObjectID standName;
			//	pStandInSim->GetStandInput()->getObjName(standName);

				ProcessorList *pProcList =  pTerminal->GetTerminalProcessorList();
				std::vector<BaseProcessor*> vBridgeProcessor;
				if(pProcList)
					pProcList->GetProcessorsByType(vBridgeProcessor,BridgeConnectorProc);

				std::vector<BaseProcessor *>::iterator iterBridge = vBridgeProcessor.begin();
				for (; iterBridge != vBridgeProcessor.end(); ++ iterBridge)
				{
					BridgeConnector *pBridgeConnector = (BridgeConnector *)*iterBridge;
					if(pBridgeConnector && pBridgeConnector->IsBridgeConnectToFlight(m_pAirsideFlt->GetFlightInput()->getFlightIndex()))
					{
						if(_pEngine->IsOnboardSel())
						{
							bool bArrival = m_pAirsideFlt->IsArrivingOperation();
							OnboardFlightInSim* pOnboardFlightInSim = _pEngine->GetOnboardSimulation()->GetOnboardFlightInSim(m_pAirsideFlt,bArrival);
							if (pOnboardFlightInSim)
							{
								pBridgeConnector->DisOnboardConnect(getTime());
								//return 0;
							}	
						}
				
						pBridgeConnector->DisAirsideConnect(getTime());
						//break;
					}
				}
			//}
		}
	}
	m_pAirsideFlt->CloseDoors();

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
