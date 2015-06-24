#include "stdafx.h"
#include "Event.h"

NAMESPACE_AIRSIDEENGINE_BEGINE

SimEvent::~SimEvent(){}
FlightEvent::~FlightEvent(){}


FlightEvent * FlightEventList::getBirthEvent()
{
	return getEventInType(FlightEvent::FlightBirth);
}

FlightEvent * FlightEventList::getLastEvent()
{
	if(getCount()<1)
		return NULL;

	return getItem(getCount()-1);
}

FlightEvent * FlightEventList::getEventInType(FlightEvent::EventType eventType)
{
	for(int i=0;i<getCount();i++)
	{
		if( getItem(i)->GetEventType() == eventType )
			return getItem(i);
	}

	return NULL;
}

void FlightEventList::Delay(FlightEvent& firstEvent, SimClock::TimeSpanType delayTime)
{
	int nFirstIndex = findItem(&firstEvent);
	if (nFirstIndex == INT_MAX)
		return;

	int nCount = getCount();
	for (int i = nFirstIndex; i < nCount; i++)
	{
		FlightEvent* pEvent = getItem(i);
		pEvent->SetComputeTime(pEvent->GetComputeTime() + delayTime);
		pEvent->setEventTime(pEvent->getEventTime() + delayTime);
	}
}


NAMESPACE_AIRSIDEENGINE_END