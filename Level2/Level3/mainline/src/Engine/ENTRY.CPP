/*****
*
*   Class       TerminalEntryEvent
*   Author      Lincoln Fellingham
*   Date        January 1994
*   Purpose     Handles timing of mobile elements entering the terminal
*
*               Designed to have only one instance in memory at a time
*               To load entire event list some members must become static
*
*****/
#include "stdafx.h"
#include "engine\entry.h"
#include "results\paxlog.h"
#include "engine\pax.h"
#include "engine\GroupLeaderInfo.h"
#include "engine\turnarnd.h"
#include "common\util.h"
#include "common\exeption.h"
#include "common\CodeTimeTest.h"
#include "engine\TurnaroundVisitor.h"
#include "engine\terminal.h"
#include "Airside/AirsideFlightInSim.h"
#include "AirsideMobElementBehavior.h"
#include "Airside/AirsidePaxBusInSim.h"
#include "../Common/ARCTracker.h"
#include "ARCportEngine.h"
#include "SimFlowSync.h"


int TerminalEntryEvent::process(CARCportEngine* pEngine)
{
	PLACE_METHOD_TRACK_STRING();
	Person *newPerson;
	Terminal* pTerm = pEngine->getTerminal();
	if(m_nextEntry.isTurnaround())
	{
		if(m_nextEntry.GetMobileType()>0)
		{
			newPerson = new TurnaroundVisitor(m_nextEntry, pEngine);
		}
		else
		{
			newPerson = new TurnaroundPassenger ( m_nextEntry, pEngine );
		}
	}
	else
	{
		if(m_nextEntry.GetMobileType()>0)
		{
			 newPerson = new PaxVisitor ( m_nextEntry, pEngine );
		}
		else{
			 newPerson = new Passenger (m_nextEntry, pEngine );
		}
	} 

	pEngine->m_simBobileelemList.Register( newPerson );

	if (m_nextEntry.GetMobileType() != 2)
	{
		try
		{
			newPerson->newGroupFollower();	
		}
		catch (ARCSyncUnavailableError* sim_exception)
		{
			pEngine->SendSimFormatMessage( sim_exception->getFormatErrorMsg() );
			delete sim_exception;
		}
		m_nCurrentCount += newPerson->GetAdminGroupSize();
	}
	else
	{
		((CGroupLeaderInfo*)newPerson->m_pGroupInfo)->SetGroupLeader(newPerson);
		m_nCurrentCount++;
	}
    newPerson->generateEvent (time,false);
    scheduleNextEntry();
    return FALSE;
}

//It returns true if there is one more mobile element; false if no more passenger.
int TerminalEntryEvent::scheduleNextEntry ()
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


void TerminalEntryEvent::initList (PaxLog *p_list, long p_max)
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

void TerminalEntryEvent::addEvent()
{
	Event::addEvent();
}



//int CAirsideEntryEvent::GeneraterPerson(InputTerminal* _pInTerm)
//{
//	Person *newPerson;
//	Terminal* pTerm = (Terminal*)_pInTerm;
//	if(m_nextEntry.isTurnaround())
//	{
//		if(m_nextEntry.GetMobileType()>0)
//		{
//			newPerson = new TurnaroundVisitor(m_nextEntry, pTerm);
//		}
//		else
//		{
//			newPerson = new TurnaroundPassenger ( m_nextEntry, pTerm );
//		}
//	}
//	else
//	{
//		if(m_nextEntry.GetMobileType()>0)
//		{
//			newPerson = new PaxVisitor ( m_nextEntry, pTerm );
//		}
//		else{
//			newPerson = new Passenger (m_nextEntry, pTerm );
//		}
//	} 
//	pTerm->m_simBobileelemList.Register( newPerson );
//
//	if (m_nextEntry.GetMobileType() != 2)
//	{
//		newPerson->newGroupFollower();	
//
//		m_nCurrentCount += newPerson->GetAdminGroupSize();
//	}
//	else
//	{
//		((CGroupLeaderInfo*)newPerson->m_pGroupInfo)->SetGroupLeader(newPerson);
//		m_nCurrentCount++;
//	}
//	newPerson->setBehavior( new AirsideMobElementBehavior( newPerson ));
//	newPerson->SetAirsideFlight(m_Flight) ;
//	if(m_hasBus&&m_Bus != NULL)
//	{
//		m_Bus->AddPassenger(newPerson) ;
//		newPerson->SetAirsideBus(m_Bus) ;
//		newPerson->HasBusServer(TRUE) ;
//	}else
//		newPerson->HasBusServer(m_hasBus) ;
//	newPerson->generateEvent (time,false);
//	return FALSE;
//}
//int CAirsideEntryEvent::scheduleNextEntry ()
//{
//	if( m_nCurrentCount >= m_nPaxCount )
//	{
//		if(m_pEntryList!=NULL)
//		delete m_pEntryList ;
//		m_pEntryList = NULL ;
//		return FALSE;
//	}
//	while(  m_nCurrentCount < m_nPaxCount )
//	{
//		m_pEntryList->getItem( m_nextEntry, m_nCurrentCount );
//		if( m_nextEntry.getEntryTime() + m_deyTime < time )
//			throw new StringError ("Mobile Elements not sorted order by entry time");
//
//		if( m_nextEntry.getOwnStart() )
//		{
//			time = m_nextEntry.getEntryTime() + m_deyTime;
//			GeneraterPerson();
//			return TRUE;
//		}
//		else
//		{
//			m_nCurrentCount += m_nextEntry.getGroupSize();
//			if( m_nCurrentCount >= m_nPaxCount )
//			{
//				if(m_pEntryList!=NULL)
//					delete m_pEntryList ;
//				m_pEntryList = NULL ;
//			}
//		}
//	}
//}
//int CAirsideEntryEvent::initList (PaxLog *p_list, long p_max,std::vector<Passenger*>& _paxlist,InputTerminal* _pInTerm)
//{
//	
//	m_pEntryList = p_list;
//	m_nCurrentCount = 0;
//
//	if( m_nCurrentCount >=  m_pEntryList->getCount() )
//	{
//		if(m_pEntryList!=NULL)
//			delete m_pEntryList ;
//		m_pEntryList = NULL ;
//		return FALSE;
//	}
//	while(  m_nCurrentCount < m_pEntryList->getCount() )
//	{
//		m_pEntryList->getItem( m_nextEntry, m_nCurrentCount );
//		if( m_nextEntry.getEntryTime() + m_deyTime < time )
//			throw new StringError ("Mobile Elements not sorted order by entry time");
//
//		if( m_nextEntry.getOwnStart() )
//		{
//			time = m_nextEntry.getEntryTime() + m_deyTime;
//			GeneraterPerson(_pInTerm);
//		}
//		else
//		{
//			m_nCurrentCount += m_nextEntry.getGroupSize();
//			if( m_nCurrentCount >= m_nPaxCount )
//			{
//				if(m_pEntryList!=NULL)
//					delete m_pEntryList ;
//				m_pEntryList = NULL ;
//			}
//		}
//	}
//}