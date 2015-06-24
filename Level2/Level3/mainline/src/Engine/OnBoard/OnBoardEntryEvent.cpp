
#include "stdafx.h"
#include "OnBoardEntryEvent.h"
#include "results\paxlog.h"
#include "engine\pax.h"
#include "engine\GroupLeaderInfo.h"
#include "engine\turnarnd.h"
#include "common\util.h"
#include "common\exeption.h"
#include "engine\TurnaroundVisitor.h"
#include "engine\terminal.h"
#include "../../Common/ARCTracker.h"

#include "DeplaneAgent.h"
#include "EnplaneAgent.h"
#include "Services/SeatAssignmentService.h"
#include "Engine/ARCportEngine.h"
/*
MobElement Birth Event.
*/
int OnBoardEntryEvent::process(CARCportEngine* pEngine)
{
	PLACE_METHOD_TRACK_STRING();
	//MobElementsLifeCycleMgr()->getBirthMode();

	ASSERT(pEngine != NULL);
	MobileAgent *newAgent;
	Terminal* pTerm = pEngine->getTerminal();

	// individual objects.

	for (int i = 0 ; i < m_nPaxCount; i++)
	{
		
		m_pEntryList->getItem( m_nextEntry, i );

		if(m_nextEntry.isTurnaround())
		{
			;
		}
		else
		{
			if(m_nextEntry.GetMobileType()>0)
			{
				1;//newPerson = new PaxVisitor ( m_nextEntry, pTerm );
			}
			else{
				//newPerson = new Passenger (m_nextEntry, pTerm );
				if( m_nextEntry.isArriving() )
				{
					m_nextEntry.setEntryTime( time );// birth on carrier seat at same time.
					newAgent = new DeplaneAgent( m_nextEntry, pEngine );
				}
				else if( m_nextEntry.isDeparting() )
				{
					newAgent = new EnplaneAgent ( m_nextEntry, pEngine );
				}
			}
		} 

		pEngine->m_simBobileelemList.Register( newAgent );
		seatAssignService()->registerMobAgent( newAgent );		
		newAgent->generateEvent (m_nextEntry.getEntryTime(),false);
	}

	return FALSE;
}

//It returns true if there is one more mobile element; false if no more passenger.
int OnBoardEntryEvent::scheduleNextEntry ()
{
	if( m_nCurrentCount >= m_nPaxCount )
		return FALSE;

	while( true )
	{
		m_pEntryList->getItem( m_nextEntry, m_nCurrentCount );
		if( m_nextEntry.getEntryTime() < time )
			throw new StringError ("Mobile Elements not sorted order by entry time");

		if( m_nextEntry.getOwnStart() )
		{
			time = m_nextEntry.getEntryTime();
			addEvent();
			return TRUE;
		}
		else
		{
			m_nCurrentCount += m_nextEntry.getGroupSize();
			if( m_nCurrentCount >= m_nPaxCount )
				return FALSE;
		}
	}
}


void OnBoardEntryEvent::initList (PaxLog *p_list, long p_max)
{
	m_pEntryList = p_list;
	m_nCurrentCount = 0;

	time = 0l;

	if( p_max <= 0 )
	{
		m_nPaxCount = p_list->getCount();
	}
	else
	{
		m_nPaxCount = MIN((p_list->getCount()),p_max);
	}
	//    paxCount = p_list->getCount();
	scheduleNextEntry();
}


