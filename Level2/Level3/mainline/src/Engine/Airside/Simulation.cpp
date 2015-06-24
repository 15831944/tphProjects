#include "StdAfx.h"
#include "../../Inputs/AirsideInput.h"
#include "../../AirsideInput/AirsideInput.h"
#include "simulation.h"
#include "ConflictAdjustment.h"
#include "Flight.h"

NAMESPACE_AIRSIDEENGINE_BEGINE

static const double pushbackDistance = 1000.0; // 10 meters

Simulation::Simulation(void)
{
}

Simulation::~Simulation(void)
{
}

bool Simulation::IsSthToAnimate(){
	
	for(int i=0;i<(int)m_flights.size();i++){
		SimFlight  * pFlt = m_flights.at(i).get();
		if( pFlt->getPEL()->getCount() > 0) return true;
	}
	return false;
}
bool Simulation::Init( CAirsideInput* _inputC, AirsideInput* _inputD){
	
	//m_ACTypelist.build(_inputD);
	//build model
	m_simModel.Build(_inputC,_inputD);


	//build AcTypes
	
	//generat flights
	m_fltGenerator.Initialize(_inputC->GetFlightSchedule());
	m_fltGenerator.GenerateFlights(m_flights,m_simModel);

	//for each flight ,generate the event list
	for(int i=0;i<(int)m_flights.size();i++){
		m_processGenerator.GenProcess(m_flights[i].get(),&m_simModel);
	}
	
	
	//set simclock
	SimClock::TimeType start_tm, end_tm;
	if(m_flights.getTimeRange(start_tm,end_tm)){
		m_simClock.setStartTime(start_tm);
		m_simClock.setEndTime(end_tm);
		m_simClock.setCurrentTime(start_tm);
	}else{
		m_simClock.setStartTime( long(_inputD->GetSettings()->GetStartTime()) );
		m_simClock.setEndTime( long(_inputD->GetSettings()->GetEndTime()) );
		m_simClock.setCurrentTime( m_simClock.getStartTime());
	}


	return true;
}


bool Simulation::Run()
{
	m_simulator.setSimClock(&m_simClock);
	m_simulator.setAirport(&m_simModel);
	m_simulator.setFlights(&m_flights);
	m_simulator.setNextTime(m_simClock.getEndTime());
	
	/*while(!m_simClock.IsSimEnd() ){
		if( m_simulator.doSim() ) {
			m_simClock.setCurrentTime(m_simulator.getNextTime());
		}else
			return false;
	}*/

	ModifyPushback();

	//ConflictAdjustment conflictAdjustment(m_flights);
	//conflictAdjustment.Run();

	for(int i=0;i<int(m_flights.size());i++)
	{
		FlightEventList* theFEL = m_flights.at(i)->getFEL();
		FlightEventList* thePEL = m_flights.at(i)->getPEL();
		
		for(int j=0; j<theFEL->getCount();)
		{
			FlightEvent * theEvent = theFEL->getItem(0);	
			theFEL->removeItem(theEvent);
			//theEvent->setEventTime( m_pSimClock->getCurrentTime() );
			thePEL->addItem(theEvent);
		}
	}

	return true ;
}

void Simulation::ModifyPushback()
{
	for(size_t i = 0; i < m_flights.size(); i++)
	{
		FlightEventList* theFEL = m_flights.at(i)->getFEL();

		for(int j = 0; j < theFEL->getCount(); j++)
		{
			FlightEvent* theEvent = theFEL->getItem(j);
			if (theEvent->GetEventType() == FlightEvent::Pushback)
			{
				if ((j + 2) >= theFEL->getCount())
					continue;

				Point gatePt = theEvent->GetEventPos();
				FlightEvent* taxiOutEvent = theFEL->getItem(j + 1);
				Point taxiOutPt = taxiOutEvent->GetEventPos();

				FlightEvent* nextEvent = theFEL->getItem(j + 2);
				Point nextPt = nextEvent->GetEventPos();

				double distance = nextPt.distance(taxiOutPt);
				double x = taxiOutPt.getX() - pushbackDistance * (nextPt.getX() - taxiOutPt.getX()) / distance;
				double y = taxiOutPt.getY() - pushbackDistance * (nextPt.getY() - taxiOutPt.getY()) / distance;
				Point newPt(x, y, gatePt.getZ());

				ASSERT(taxiOutEvent->GetFlightHorizontalSpeed() > 0.00000001);
				double delayTime =  (2.0 * pushbackDistance) / taxiOutEvent->GetFlightHorizontalSpeed();

				taxiOutEvent->SetEventType(FlightEvent::Pushback);
				theFEL->Delay(*(theFEL->getItem(j + 2)), long(delayTime * 2.0));

				FlightEvent* newEvent = new FlightEvent(taxiOutEvent->GetSimFlight());
				*newEvent = *taxiOutEvent;
				newEvent->SetComputeTime(newEvent->GetComputeTime() + (long)delayTime);
				newEvent->setEventTime(newEvent->getEventTime() + (long)delayTime);
				newEvent->SetEventType(FlightEvent::TaxiOut);
				newEvent->SetEventPos(newPt);
				
				theFEL->addItem(newEvent);
				j += 2;
			}
		}
	}
}

NAMESPACE_AIRSIDEENGINE_END