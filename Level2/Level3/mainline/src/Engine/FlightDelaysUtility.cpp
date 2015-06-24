#include "stdafx.h"
#include "FlightDelaysUtility.h"
#include "inputs\schedule.h"
#include "inputs\flight.h"
#include "Results\paxlog.h"
#include "FlightsBoardingCallManager.h"
#include "FlightItemBoardingCallMgr.h"
#include "../Common/ARCTracker.h"

/*
	if Arrival Flights need delay, all flight's passenger's entry time should be delay.
	PaxVisitor does not know flight will be delay and should not be delayed.
	
	-----Arrival at Stand(09:00)----------Departure(09:30)- (Gate Time: 00:30)
	delay: 30 min:
		-----Arrival at Stand(09:30)----------Departure(10:00)-
*/
void FlightDelaysUtility::impactFlightDelaysEffectToMobElement(PaxLog* pAllPaxLog, const FlightSchedule* pSchedule/*, CProgressBar& bar*/)
{
	PLACE_METHOD_TRACK_STRING();
	long nPaxCount = pAllPaxLog->getCount();
	if(nPaxCount == 0)
		return;
	CProgressBar bar("Analysis Flight delays...");
	bar.SetRange(0, nPaxCount);

	bool bNeedResortPaxLog = false;
	for(long i = 0; i < nPaxCount; i++)
	{
		MobLogEntry nextEntry;
		pAllPaxLog->getItem( nextEntry, i );

		// adjust arrival passengers. arrival baggages. except visitors (type == 1)
		if(nextEntry.isArriving() && nextEntry.GetMobileType() != 1)
		{
			ElapsedTime arrDelayTime  = 0L;

			int numFlights = pSchedule->getCount();
			for (int nFltIndex = 0; nFltIndex < numFlights; nFltIndex++)
			{
				Flight* pItem = pSchedule->getItem(nFltIndex);
				if(nFltIndex == nextEntry.getFlightIndex())
				{
					arrDelayTime = pItem->getArrDelay();
                    break;
				}
			}	
			if(arrDelayTime > 0L || arrDelayTime < 0L)
			{
				nextEntry.setEntryTime( max( ElapsedTime(0L), (nextEntry.getEntryTime() + arrDelayTime)) );
				pAllPaxLog->updateItem(nextEntry, i);
			}
		}
		bar.StepIt();
	}

}

/*
	if Departure flight need delay, the time while arrive at stand will be delay, and, departure time will be delay also.
	Flight's boarding call time need delay.

	-----Arrival at Stand(10:30)--------Boarding Call(10:40, 10:45, 10:50,) --Departure(11:00)- (Gate Time: 00:30)
	delay: 30 min:
		(Departure Flight)-----Arrival at Stand(11:00)----------Boarding Call(11:10, 11:15, 11:20,) --Departure(11:30)- 
	  (Turnaround Flight)-----Arrival at Stand(10:00 + arr delay time)----------Boarding Call(11:10, 11:15, 11:20,) --Departure(11:30)- 
*/
void FlightDelaysUtility::impactFlightDelaysEffectToBoardingCall(FlightsBoardingCallManager* _pBoardingcallMgr, const FlightSchedule* pSchedule)
{
	int numFlights = pSchedule->getCount();
	for (int nFltIndex = 0; nFltIndex < numFlights; nFltIndex++)
	{
		Flight* pItem = pSchedule->getItem(nFltIndex);
		if(pItem->isDeparting())// departure or turnaround.
		{
			ElapsedTime depDelayTime = pItem->getDepDelay();
			if(depDelayTime < ElapsedTime(0L) )// departure flight cannot departure earlier(according to schedule).
				continue;

			FlightItemBoardingCallMgr* pFltBoardingMgr = 
			_pBoardingcallMgr->GetFlightItemBoardingCallMgr( pItem );
			//ASSERT(pFltBoardingMgr != NULL );		
			if( NULL != pFltBoardingMgr )
				pFltBoardingMgr->delayEvents( depDelayTime );

			std::map<int,ElapsedTime>&mapLastCalls  = pItem->GetLastCallOfEveryStage();
			std::map<int,ElapsedTime>::iterator iter = mapLastCalls.begin();
			for (; iter != mapLastCalls.end(); ++iter)
			{
				iter->second += depDelayTime;
			}
		}	
	}	

}



