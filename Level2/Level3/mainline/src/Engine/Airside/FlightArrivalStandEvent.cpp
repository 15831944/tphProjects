#include "StdAfx.h"
#include "FlightArrivalStandEvent.h"
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
#include "StandLeadInLineInSim.h"
#include <CommonData/BaseProcess.h>
#include "Engine/ARCportEngine.h"
#include "FlightServiceInformation.h"
#include "../OnboardSimulation.h"

FlightArrivalStandEvent::FlightArrivalStandEvent(AirsideFlightInSim* AirsideFlt)
:CAirsideMobileElementEvent(NULL),
m_pFlight(NULL),m_pAirsideFlt(AirsideFlt)
{  
}

FlightArrivalStandEvent::~FlightArrivalStandEvent(void)
{
}

// if flight is is turnaround, ReflectOnFlightArriveStand will be invoked twice
int FlightArrivalStandEvent::process(CARCportEngine* _pEngine)
{
	PLACE_METHOD_TRACK_STRING();

	if(m_pAirsideFlt)
		m_pAirsideFlt->ConnectBridge(getTime());
	
	//Terminal* pTerminal = _pEngine->getTerminal();
	// for adjust arriving flight's mobElement entry time
	if(m_pFlight->isArriving() && _pEngine->IsAirsideSel())
	{
      // m_pAirsideFlt->GetAirTrafficController()->GetPassengerTakeOffFlightPolicy()->NoticeFlightArrived(m_pAirsideFlt->GetFlightInput(),true,time,_pEngine) ;
		m_pAirsideFlt->FlightArriveStand(true,time);
	}
	//for adjust departure flight's boarding call time
	if(m_pFlight->isDeparting()&&!m_pAirsideFlt->IsItinerantFlight())	
	{
		_pEngine->ReflectOnFlightArriveStand(m_pAirsideFlt->GetFlightInput(),time,m_dtActualDepTime);
		//m_pAirsideFlt->GetAirTrafficController()->GetBoardingCall()->NoticeFlightArrived(m_pAirsideFlt->GetFlightInput(),false,m_dtActualDepTime,_pEngine) ;
		m_pAirsideFlt->FlightArriveStand(false,time);
		// Notice the waiting Area that the flight has arrived , the passenger can take on the flight .
	}

	// wakeup serving vehicles
	if(m_pAirsideFlt->GetFlightServiceInformation())
	{
	 	m_pAirsideFlt->GetFlightServiceInformation()->GetFlightServiceRoute()->WakeupTheWaitingVehicleInRingRoad(time);
	}

	return 0;
}

void FlightArrivalStandEvent::SetArrivalFlight(Flight* pFlight)
{
	ASSERT(pFlight != NULL);

	m_pFlight = pFlight;
	m_pMobileElement = (CAirsideMobileElement *)pFlight;
}

void FlightArrivalStandEvent::SetActualDepartureTime(const ElapsedTime& dtTime)
{
	m_dtActualDepTime = dtTime;
}

void FlightArrivalStandEvent::ConnectBridge( CARCportEngine* _pEngine ) const
{
	if(m_pAirsideFlt)
		return;

	BridgeConnector* pBridgeConnector = GetBridgeConnector();
	if (pBridgeConnector == NULL)
		return;

	if(_pEngine->IsOnboardSel())
	{
		bool bArrival = m_pAirsideFlt->IsArrivingOperation();
		OnboardFlightInSim* pOnboardFlightInSim = _pEngine->GetOnboardSimulation()->GetOnboardFlightInSim(m_pAirsideFlt,bArrival);
		if (pOnboardFlightInSim)
		{
			return pBridgeConnector->ConnectOnboardFlight(pOnboardFlightInSim,getTime());
		}
		
	}

	pBridgeConnector->ConnectAirsideFlight(m_pAirsideFlt,getTime());
	
}

BridgeConnector* FlightArrivalStandEvent::GetBridgeConnector() const
{
	Terminal* pTerminal =  m_pFlight->GetTerminal();
	if(pTerminal )
	{
		//get stand
		if(m_pAirsideFlt->GetOperationParkingStand())//m_pAirsideFlt->GetResource() && m_pAirsideFlt->GetResource()->GetType() == AirsideResource::ResType_StandLeadInLine)
		{
			//StandLeadInLineInSim *pStandLeadInSim = (StandLeadInLineInSim *)m_pAirsideFlt->GetResource();
			//ASSERT(pStandLeadInSim);
			StandInSim* pStandInSim = m_pAirsideFlt->GetOperationParkingStand();
			//if(pStandInSim)
			{
				ALTObjectID standName;
				pStandInSim->GetStandInput()->getObjName(standName);

				ProcessorList *pProcList =  pTerminal->GetTerminalProcessorList();
				std::vector<BaseProcessor*> vBridgeProcessor;
				if(pProcList)
					pProcList->GetProcessorsByType(vBridgeProcessor,BridgeConnectorProc);

				std::vector<BaseProcessor *>::iterator iterBridge = vBridgeProcessor.begin();
				for (; iterBridge != vBridgeProcessor.end(); ++ iterBridge)
				{
					BridgeConnector *pBridgeConnector = (BridgeConnector *)*iterBridge;
					if(pBridgeConnector && 
						!pBridgeConnector->IsBridgeConnectToFlight(m_pFlight->getFlightIndex()) &&
						pBridgeConnector->IsBridgeConnectToStand(standName))
					{
						return pBridgeConnector;
					}
				}
			}
		}
	}
	return NULL;
}

void FlightArrivalStandEvent::addEvent()
{
	Event::addEvent();
}