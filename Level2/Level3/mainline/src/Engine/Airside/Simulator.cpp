#include "StdAfx.h"
#include "AirportModel.h"
#include "Simulator.h"
#include "Flight.h"
#include "Event.h"


NAMESPACE_AIRSIDEENGINE_BEGINE
Simulator::Simulator(){
	m_pAirport = NULL;
	m_pflights =NULL;
}

//determinate the event can happend at this time
bool Simulator::Determinate(FlightEvent* theEvent){
	return true;
}
//do simulat
bool Simulator::doSim(){
	m_minNextTime += SimClock::TimeSpanType (long(100)); 

	for(int i=0;i<int(m_pflights->size());i++){
		 FlightEventList* theFEL = m_pflights->at(i)->getFEL();
		 FlightEventList* thePEL = m_pflights->at(i)->getPEL();

		for(int ii=0;ii<theFEL->getCount();){
			FlightEvent * theEvent = theFEL->getItem(0);
			if(theEvent->GetComputeTime() <= m_pSimClock->getCurrentTime() ){
				
				if( Determinate(theEvent) ){
					theFEL->removeItem(theEvent);
					theEvent->setEventTime( m_pSimClock->getCurrentTime() );
					thePEL->addItem(theEvent);
					continue;
				}else{
					
				}	
				
			}else{ // record the min next simtime;
				if(theEvent->GetComputeTime() < m_minNextTime ) m_minNextTime = theEvent->GetComputeTime();
				break;
			}
		}
	}
	
	
	return true;
}


NAMESPACE_AIRSIDEENGINE_END