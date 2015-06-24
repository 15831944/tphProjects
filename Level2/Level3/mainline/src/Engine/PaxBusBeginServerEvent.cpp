#include "StdAfx.h"
#include ".\paxbusbeginserverevent.h"
#include "Airside/AirsidePaxBusInSim.h"
#include "MobileElementGenerator.h"
#include "Airside/AirsideFlightInSim.h"
#include "Airside/AirTrafficController.h"
#include "BoardingCallPolicy.h"
#include "Airside/PaxBusServiceGateEvent.h"
#include "terminal.h"
#include "ARCportEngine.h"


CPaxBusBeginServerEvent::CPaxBusBeginServerEvent(CAirsidePaxBusInSim* _bus,ElapsedTime _time,CARCportEngine *_pEngine)
:CAirsideMobileElementEvent(_bus),
m_bus(_bus),
m_pEngine(_pEngine)

{
	setTime(_time) ;
}
int CPaxBusBeginServerEvent::Process()
{
	if(m_bus == NULL)
		return 0;
	if(!m_pEngine->IsTerminal())
	{

			m_bus->SetMode(OnService);
			CPaxBusServiceGateEvent* event = new CPaxBusServiceGateEvent(m_bus);
			m_bus->SetTime(time) ;
			event->setTime(time) ;
			event->addEvent() ;
			return 1;
	}
	if(m_bus->IsServiceArrival())
			NoticePaxTakeOffBusToArrGate() ;
	else
			NoticePaxTakeOnBus(m_bus->GetVehicleCapacity()) ;
	return 1 ;
}
void CPaxBusBeginServerEvent::NoticePaxTakeOffBusToArrGate()
{
	m_bus->TakeOffPassenger(time);
}
void CPaxBusBeginServerEvent::NoticePaxTakeOnBus(int paxNum)
{
	AirsideFlightInSim* flight = m_bus->GetServiceFlight() ;
	if(flight == NULL)
		return ;
   if(m_pEngine && m_pEngine->IsAirsideSel())
   {	//flight->GetAirTrafficController()->GetBoardingCall()->NoticeBusHasArrived(flight->GetFlightInput(),false,time,m_bus,m_pEngine) ;
	   flight->PassengerBusArrive(false,time,m_bus);
   }
}

CPaxBusBeginServerEvent::~CPaxBusBeginServerEvent(void)
 {

 }