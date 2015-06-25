#include "StdAfx.h"
#include ".\bagcartsbeginserviceparkingspotevent.h"
#include "AirsideBaggageTrainInSim.h"
#include "ARCportEngine.h"
#include "BagCartsServiceParkingSpotEvent.h"
#include "Airside\AirsideFlightInSim.h"

CBagCartsBeginServiceParkingSpotEvent::CBagCartsBeginServiceParkingSpotEvent(AirsideBaggageTrainInSim* pBagTrain,ElapsedTime _time,CARCportEngine *_pEngine)
:CAirsideMobileElementEvent(pBagTrain)
,m_pBaggageTrain(pBagTrain)
,m_pEngine(_pEngine)
{
	time = _time;
}

CBagCartsBeginServiceParkingSpotEvent::~CBagCartsBeginServiceParkingSpotEvent(void)
{
}

int CBagCartsBeginServiceParkingSpotEvent::Process()
{
	if(m_pBaggageTrain == NULL)
		return 0;
	if(!m_pEngine->IsTerminal())
	{

		m_pBaggageTrain->SetMode(OnService);
		CBagCartsServiceParkingSpotEvent* event = new CBagCartsServiceParkingSpotEvent(m_pBaggageTrain);
		m_pBaggageTrain->SetTime(time) ;
		event->setTime(time) ;
		event->addEvent() ;
		return 1;
	}
	if(m_pBaggageTrain->getFlightOperation() == ARRIVAL_OPERATION)
	{
		NotifyBagTakeOffBusToLoaderProcessor() ;
	}
	else
	{
		NotifyBagTakeOnBaggageCarts() ;
	}

	return true;
}

void CBagCartsBeginServiceParkingSpotEvent::NotifyBagTakeOffBusToLoaderProcessor()
{
	m_pBaggageTrain->UnloadBaggageToLoader(time);
}

void CBagCartsBeginServiceParkingSpotEvent::NotifyBagTakeOnBaggageCarts( )
{
	AirsideFlightInSim* flight = m_pBaggageTrain->GetServiceFlight() ;
	if(flight == NULL)
		return ;
	if(m_pEngine && m_pEngine->IsAirsideSel())
	{	
		//flight->PassengerBusArrive(false,time,m_pBaggageTrain);
		m_pBaggageTrain->LoadBaggageFromPusher(time);
	}
}
