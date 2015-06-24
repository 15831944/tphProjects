#include "stdafx.h"
#include "ProcessGenerator.h"

#include "Event.h"
#include "Flight.h"
#include "AirportModel.h"
#include "AirspaceModel.h"
#include "RunwayModel.h"
#include "TaxiwayModel.h"
#include "GateModel.h"

#include "Event.h"

NAMESPACE_AIRSIDEENGINE_BEGINE

const static SimClock::TimeSpanType EvalTimeSpan=long(100);

void ProcessGenerator::GenProcess(SimFlight* the_flight,AirportModel* pAirport)
{
	//birth event
	FlightEvent *birthEvent = new FlightEvent(the_flight);
	birthEvent->SetEventID(0);
	birthEvent->SetEventType(FlightEvent::FlightBirth);
	birthEvent->SetComputeTime(the_flight->getArrivalTime());
	birthEvent->setEventTime(the_flight->getArrivalTime());
	the_flight->getFEL()->addItem(birthEvent);
	//airspace event arrival
	pAirport->getAirspaceModel()->DistributeArrivalProcess(the_flight);
	//landing event	
	pAirport->getRunwayModel()->DistributeLandingProcess(the_flight);
	pAirport->getTaxiwayModel()->DistributeTaxiInProcess(the_flight);
	pAirport->getGateModel()->DistributeInGate(the_flight);
	pAirport->getGateModel()->DistributePushback(the_flight);
	pAirport->getTaxiwayModel()->DistributeTaxiOutProcess(the_flight);

	pAirport->getRunwayModel()->DistributeTakeoffProcess(the_flight);
	pAirport->getAirspaceModel()->DistributeDepatureProcess(the_flight);
	
	// adjust the arrival time
	FlightEventList * pEventList = the_flight->getFEL();
	FlightEvent * pEvent = pEventList->getEventInType(FlightEvent::StopInGate);
	
	if(pEvent)
	{
		SimClock::TimeType ingateTime  = pEvent->getEventTime();
		SimClock::TimeSpanType dT  = the_flight->getArrivalTime() - ingateTime;

		for(int i =0;i<pEventList->getCount();i++)
		{
			FlightEvent * _event = pEventList->getItem(i);
			if(_event->GetEventType() == FlightEvent::Pushback)break;
			SimClock::TimeType realTime = _event->getEventTime() + dT;
			_event->SetComputeTime(realTime);
			_event->setEventTime(realTime);
		}
		for(int i =0;i<pEventList->getCount();i++)
		{
			FlightEvent * _event = pEventList->getItem(i);
		}
	}
}

NAMESPACE_AIRSIDEENGINE_END