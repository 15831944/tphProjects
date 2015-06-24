
#include "stdafx.h"
#include "MobElementEntryEvent.h"
#include "results\paxlog.h"
#include "engine\pax.h"
#include "engine\GroupLeaderInfo.h"
#include "engine\turnarnd.h"
#include "common\util.h"
#include "common\exeption.h"
#include "engine\TurnaroundVisitor.h"
#include "engine\terminal.h"
#include "../Common/ARCTracker.h"

//#include "DeplaneAgent.h"
//#include "EnplaneAgent.h"

#include "MobElementLifecycleManager.h"
#include "ARCportEngine.h"
/*
	MobElement Birth Event.
*/
int MobElementEntryEvent::process(CARCportEngine* _pEngine)
{
	PLACE_METHOD_TRACK_STRING();
	//MobElementsLifeCycleMgr()->getBirthMode();

	Person *newPerson;
	Terminal* pTerm = _pEngine->getTerminal();
	if(m_nextEntry.isTurnaround())
	{
		if(m_nextEntry.GetMobileType()>0)
		{
			newPerson = new TurnaroundVisitor(m_nextEntry, _pEngine);
		}
		else
		{
			newPerson = new TurnaroundPassenger ( m_nextEntry, _pEngine );
			//newPerson = new 
		}
	}
	else
	{
		if(m_nextEntry.GetMobileType()>0)
		{
			newPerson = new PaxVisitor ( m_nextEntry, _pEngine );
		}
		else{
			newPerson = new Passenger (m_nextEntry, _pEngine );
		}
	} 

	_pEngine->m_simBobileelemList.Register( newPerson );

	if (m_nextEntry.GetMobileType() != 2)
	{
		newPerson->newGroupFollower();	

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
int MobElementEntryEvent::scheduleNextEntry ()
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


void MobElementEntryEvent::initList (PaxLog *p_list, long p_max)
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


