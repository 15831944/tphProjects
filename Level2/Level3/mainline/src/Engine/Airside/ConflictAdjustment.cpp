#include "StdAfx.h"
#include "ConflictAdjustment.h"
#include "Event.h"
#include "Flight.h"

namespace airside_engine
{

ConflictAdjustment::ConflictAdjustment(FlightList& flights)
 : m_flights(flights)
{
}

void ConflictAdjustment::Run()
{
	FlightEvent* pFirstEvent = m_flights.GetFirstEvent();
	if (pFirstEvent == NULL)
		return;

	pFirstEvent->GetSimFlight();

	for (FlightList::iterator iter1 = m_flights.begin(); iter1 != m_flights.end(); ++iter1)
	{
		for (FlightList::iterator iter2 = iter1 + 1; iter2 != m_flights.end(); ++iter2)
		{
			AdjustConflict(**iter1, **iter2);
		}
	}
}

void ConflictAdjustment::AdjustConflict(SimFlight& flight1, SimFlight& flight2)
{
	FlightEventList* pEventList1 = flight1.getFEL();
	FlightEventList* pEventList2 = flight2.getFEL();
	
	int nEventList1Count = pEventList1->getCount();
	int nEventList2Count = pEventList2->getCount();

	for (int i = 2; i < nEventList1Count; i++)
	{
        FlightEvent* pSegment1StartEvent = pEventList1->getItem(i - 1);
		FlightEvent* pSegment1EndEvent = pEventList1->getItem(i);

		for (int j = 1; j < nEventList2Count; j++)
		{
			FlightEvent* pSegment2StartEvent = pEventList2->getItem(j - 1);
			FlightEvent* pSegment2EndEvent = pEventList2->getItem(j);

			CheckAndResolveConflict(*pSegment1StartEvent, *pSegment1EndEvent, *pSegment2StartEvent, *pSegment2EndEvent);
		}
	}
}

void ConflictAdjustment::CheckAndResolveConflict(FlightEvent& segment1StartEvent, FlightEvent& segment1EndEvent,
												 FlightEvent& segment2StartEvent, FlightEvent& segment2EndEvent)
{
	if (segment1StartEvent.GetComputeTime() > segment2EndEvent.GetComputeTime()
		|| segment1EndEvent.GetComputeTime() > segment2StartEvent.GetComputeTime())
	{
		return;
	}

	if ((segment1StartEvent.GetEventPos().preciseCompare(segment2StartEvent.GetEventPos())
		&& segment1EndEvent.GetEventPos().preciseCompare(segment2EndEvent.GetEventPos()))
		|| (segment1StartEvent.GetEventPos().preciseCompare(segment2EndEvent.GetEventPos())
		&& segment1EndEvent.GetEventPos().preciseCompare(segment2StartEvent.GetEventPos())))
	{
		SimClock::TimeSpanType delayTime = 0L;
		if (segment1StartEvent.GetComputeTime() > segment2StartEvent.GetComputeTime())
		{
			delayTime = segment1EndEvent.GetComputeTime() - segment2StartEvent.GetComputeTime();
			segment1StartEvent.GetSimFlight()->getFEL()->Delay(segment2StartEvent, delayTime);
		}
		else
		{
			delayTime = segment2EndEvent.GetComputeTime() - segment1StartEvent.GetComputeTime();
			segment1StartEvent.GetSimFlight()->getFEL()->Delay(segment1StartEvent, delayTime);
		}
	}
}

}