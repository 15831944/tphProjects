#include "StdAfx.h"
#include "MobileElementGroup.h"
#include <algorithm>

//----------------------------------------------------------------------------------
//MobileElementGroup
MobileElementGroup::MobileElementGroup()
:m_nFltIndex(-1)
,m_bIsStarted(false)
,m_pTerminalPaxLog(NULL)
{
	m_pMobEventLog = new EventLog<MobEventStruct>;
	m_pPaxsLog  = new PaxLog(m_pMobEventLog); 
}

MobileElementGroup::~MobileElementGroup(void)
{
	delete m_pPaxsLog;
	delete m_pMobEventLog;
}

void MobileElementGroup::ScheduleEntryEventDirectly()
{
	m_bIsStarted = true;
	m_entryEvent.initList(m_pPaxsLog, m_pPaxsLog->getCount());
}

void MobileElementGroup::SetTerminalPaxLogPtr(PaxLog* pAllPaxLog)
{
	ASSERT(pAllPaxLog != NULL);
	m_pTerminalPaxLog = pAllPaxLog;
}

//----------------------------------------------------------------------------------
//MobileElementActiveGroup
MobileElementActiveGroup::MobileElementActiveGroup()
{
}

MobileElementActiveGroup::~MobileElementActiveGroup(void)
{
}

//----------------------------------------------------------------------------------
//MobileElementNonActiveGroup
MobileElementNonActiveGroup::MobileElementNonActiveGroup()
{
}

MobileElementNonActiveGroup::~MobileElementNonActiveGroup(void)
{
}

void MobileElementNonActiveGroup::AddMobLogEntry(MobLogEntry& mobLogEntry, long nOldIndex)
{
	ASSERT(mobLogEntry.getArrFlight() == m_nFltIndex );
	m_pPaxsLog->addItem(mobLogEntry);

	//
	int nNewIndex = m_pPaxsLog->getCount() - 1;
	m_mapNewIdxToOldIdx.insert(std::make_pair(nNewIndex, nOldIndex));
}

void MobileElementNonActiveGroup::AdjustMobileElementEntryTime(const ElapsedTime& offsetTime)
{
	int nCount = m_pPaxsLog->getCount();
	for (int i=0; i<nCount; i++)
	{
		MobLogEntry item;
		m_pPaxsLog->getItem(item, i);

		////char temptime1[32];
		////item.getEntryTime().printTime( temptime1 );

		ElapsedTime newEntryTime = item.getEntryTime() - offsetTime;
		item.setEntryTime( newEntryTime );
		m_pPaxsLog->updateItem(item, i);


		////char temptime2[32];
		////item.getEntryTime().printTime( temptime2 );
		////TRACE("\n Old Arrival Time: %s New Arrival Time: %s" ,temptime1, temptime2);


		// update Terminal's Paxlog
		std::map<long, long>::iterator iter = 
			m_mapNewIdxToOldIdx.find(i);
		ASSERT(iter != m_mapNewIdxToOldIdx.end());

		MobLogEntry itemEx;
		m_pTerminalPaxLog->getItem(itemEx, iter->second);
		itemEx.setEntryTime(newEntryTime);
		m_pTerminalPaxLog->updateItem(itemEx, iter->second);
	}

}

void MobileElementNonActiveGroup::ScheduleEntryEvent(const ElapsedTime& offsetTime, int nActivePaxMaxCount)
{
	if(m_bIsStarted == true)
		return;//AirsideSimEngine send 'FlightArriveStandEvent' more than once

	m_bIsStarted = true;
	AdjustMobileElementEntryTime(offsetTime);

	m_entryEvent.initList(m_pPaxsLog, m_pPaxsLog->getCount());
}















