#include "StdAfx.h"
#include ".\flight.h"
#include "../../Inputs/schedule.h"


NAMESPACE_AIRSIDEENGINE_BEGINE
SimFlight::SimFlight(){
	m_pflight = NULL;
}
SimFlight::~SimFlight(void)
{
}

SimClock::TimeType SimFlight::getBirthTime(){
	return m_PEL.getBirthEvent()->getEventTime();
}
SimClock::TimeType SimFlight::getLastTime(){
	return m_PEL.getLastEvent()->getEventTime();
}

//ACType* SimFlight::getFlightType(){return m_ACType.get();}
SimClock::TimeType SimFlight::getArrivalTime()const{
	ASSERT(m_pflight);
	return m_pflight->getArrTime();
}

SimClock::TimeType SimFlight::getDepartureTime()const {
	ASSERT(m_pflight);
	return m_pflight->getDepTime();
}
CString SimFlight::getDepartureRoute()const {
	ASSERT(m_pflight);
				
	return "";//m_pflight->GetSID();
}
CString SimFlight::getArrivalRoute()const {
	ASSERT(m_pflight);
	return "";//m_pflight->GetStar();
}

bool FlightList::getTimeRange(SimClock::TimeType&start_time, SimClock::TimeType& end_time)
{
	if (size() < 1)
		return false;

	SimClock::TimeType min_start;
	SimClock::TimeType max_end;
	min_start = at(0)->getArrivalTime();
	max_end  = at(0)->getDepartureTime();
	for(int i =1; i<int( size() );i++){
		if( min_start > at(i)->getArrivalTime() )min_start = at(i)->getArrivalTime();
		if( max_end < at(i)->getDepartureTime() )max_end = at(i)->getDepartureTime();
	}
	start_time  = min_start;
	end_time  = max_end;
	return true;
}

FlightEvent* FlightList::GetFirstEvent()
{
	if (empty())
		return NULL;

	FlightEvent* pFirstEvent = NULL;

	for (iterator iter = begin(); iter != end(); ++iter)
	{
		FlightEventList* pEventList = (*iter)->getFEL();
		if (pEventList == NULL)
			continue;

		FlightEvent* pFlightEvent = pEventList->getBirthEvent();
		if (pFlightEvent == NULL)
			continue;

		if (pFirstEvent == NULL)
		{
			pFirstEvent = pFlightEvent;
		}
		else if (*pFlightEvent < *pFirstEvent)
		{
			pFirstEvent = pFlightEvent;
		}
	}

	return pFirstEvent;
}

FlightEvent* FlightList::GetNextFlightEvent(const FlightEvent& currentEvent)
{
	if (empty())
		return NULL;

	FlightEvent* pNextEvent = NULL;

	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->getDepartureTime() < currentEvent.GetComputeTime())
			continue;
	
		FlightEventList* pEventList = (*iter)->getFEL();
		int nEventCount = pEventList->getCount();
		for (int i = 0; i < nEventCount; ++i)
		{
			FlightEvent* pFlightEvent = pEventList->getItem(i);
			if (currentEvent < *pFlightEvent)
			{
				if (pNextEvent == NULL)
				{
					pNextEvent = pFlightEvent;
				}
				else if (*pFlightEvent < *pNextEvent)
				{
					pNextEvent = pFlightEvent;
				}
			}
		}

	}
	
	return pNextEvent;
}


//Flight Generator
void FlightGenerator::Initialize(FlightSchedule* pflightSch){
	m_pfltSch = pflightSch;	
}

void FlightGenerator::GenerateFlights(FlightList& FltList,const AirportModel& simModel ){
	FltList.clear();
	
	for(int i=0;i<m_pfltSch->getCount();i++){
		Flight * pFlt = m_pfltSch->getItem(i);
		ref_ptr<SimFlight> pnewflight = new SimFlight;
		pnewflight->SetCFlight(pFlt);
		FltList.push_back(pnewflight.get());
	}
	
	/*FltList.clear();
	for(int i=0;i<m_pfltSch->getCount();i++){
		const Flight * pFlt = m_pfltSch->getItem(i);
		ref_ptr<SimFlight> pnewFlt = new SimFlight();	
		char actypename[256];
		pFlt->getACType(actypename); 
		ACType * actype = m_pACTypeList->getACType( _T(actypename) );
		if(! actype )continue; 
		pnewFlt->setACType( actype );
		pnewFlt->setArrivalTime( long(pFlt->getArrTime()) );
		pnewFlt->setDepartureTime( long(pFlt->getDepTime()) );
		
		FltList.push_back(pnewFlt.get());
	}*/
	
}

NAMESPACE_AIRSIDEENGINE_END