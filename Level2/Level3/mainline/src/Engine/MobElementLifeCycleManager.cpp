#include "StdAfx.h"
#include "MobElementLifeCycleManager.h"
#include "inputs\schedule.h"
#include "inputs\flight.h"
#include "simEngineConfig.h"
#include "Pax.h"
#include "../Common/ARCTracker.h"

MobElementsLifeCycleManager::MobElementsLifeCycleManager(void)
{
	m_pMobEventLog = new EventLog<MobEventStruct>;
	m_pNonArrivalPaxLog  = new PaxLog(m_pMobEventLog);


}

MobElementsLifeCycleManager::~MobElementsLifeCycleManager(void)
{
	if(m_pNonArrivalPaxLog) delete m_pNonArrivalPaxLog;
	if(m_pMobEventLog) delete m_pMobEventLog;

	TRACE("Singleton object: MobElementsLifeCycleMgr() deleted!\n");
}

void MobElementsLifeCycleManager::cleanup()
{
	m_pNonArrivalPaxLog->clearEvents();
	m_pNonArrivalPaxLog->clearLogs();

	for (size_t i=0; i<m_vectNonActiveGroup.size(); i++)
	{
		delete m_vectNonActiveGroup.at(i);
		m_vectNonActiveGroup.at(i) = NULL;
	}
	m_vectNonActiveGroup.clear();

	m_pAllPaxLog = NULL;
}

void MobElementsLifeCycleManager::cleanupActivedMobileElementGroup(MobileElementGroup* pActivedMobGroup)
{
	// do cleanup
	for (MobileElementGroupIter iter = m_vectNonActiveGroup.begin();
		iter != m_vectNonActiveGroup.end(); iter++)
	{
		if( pActivedMobGroup == *iter)
		{
			m_vectNonActiveGroup.erase( iter );
			return;
		}
	}
}


void MobElementsLifeCycleManager::initialize(PaxLog* pAllPaxLog, const FlightSchedule* pSchedule)
{
	PLACE_METHOD_TRACK_STRING();
	////if(simEngineConfig()->isSimTerminalModeAlone())
	////{
	////	terminalEntryEvent.initList(pAllPaxLog, simEngineConfig()->getPaxCount());
	////	return;
	////}

	if(simEngineConfig()->isSimOnBoardAlone())
	{
		onBoardEntryEvent.initList(pAllPaxLog, simEngineConfig()->getMobElementCount());
		return;
	}

	cleanup();

	m_pAllPaxLog = pAllPaxLog;
	m_pFlightSchedule = pSchedule;

	PartitionPaxLog();

	ScheduleGroupEntryEvent();
}

void MobElementsLifeCycleManager::activeMobElementGroup(const Flight* pFlight, const ElapsedTime& actualTime, int nActivePaxMaxCount)
{
	if(!pFlight->isArriving())
		return;

	int nFltIndex = GetFlightIndex(pFlight);
	if(nFltIndex == -1)	return;

	MobileElementGroup* pItem = GetMobileElementGroup(nFltIndex);
	if(pItem == NULL)// been actived. 
		return;

	ElapsedTime dtOffsetTime = pFlight->getArrTime() - actualTime;
	pItem->ScheduleEntryEvent(dtOffsetTime, nActivePaxMaxCount);

	cleanupActivedMobileElementGroup( pItem );
}


//--------------------------------------------------------------------------------------------------------------



//	1.	Add all nonArrival Mobile Element to actively group. these items do not need to update entryTime.
//	2.  Add all arrive Mobile Element to nonActived group except visitor, because visitor's entryTime do not 
//		according to flight actual arrival time.
void MobElementsLifeCycleManager::PartitionPaxLog()
{
	long count = m_pAllPaxLog->getCount();
	if(!count)
		return;

	for(long i=0; i<count; i++)
	{
		MobLogEntry nextEntry;
		m_pAllPaxLog->getItem( nextEntry, i );

		if(nextEntry.isDeparting() && !nextEntry.isTurnaround()) // departing
		{
			m_pNonArrivalPaxLog->addItem( nextEntry );
			continue;
		}

		// arriving or turnaround
		if(nextEntry.GetMobileType() == 1) // visitor
		{
			m_pNonArrivalPaxLog->addItem( nextEntry );
			continue;
		}

		// arrival passenger need wait for active.
		AddMobLogEntry( nextEntry, i);

	}

}

void MobElementsLifeCycleManager::AddMobLogEntry(MobLogEntry& nextEntry, long nOldIndex)
{
	int nFltIndex = nextEntry.getArrFlight();
	MobileElementGroup* pItem = GetMobileElementGroup(nFltIndex);

	if(pItem == NULL)
	{
		pItem = new MobileElementNonActiveGroup;
		pItem->SetFlightIndex(nFltIndex);
		pItem->SetTerminalPaxLogPtr(m_pAllPaxLog);
		m_vectNonActiveGroup.push_back(pItem);
	}
	pItem->AddMobLogEntry(nextEntry, nOldIndex);
}

MobileElementGroup* MobElementsLifeCycleManager::GetMobileElementGroup(const int nFltIndex)
{
	for (size_t i=0; i<m_vectNonActiveGroup.size(); i++)
	{
		MobileElementGroup* pItem = m_vectNonActiveGroup.at(i);
		if(pItem->GetFlightIndex() == nFltIndex)
			return pItem;
	}
	return NULL;
}

int MobElementsLifeCycleManager::GetFlightIndex(const Flight* pFlight)
{
	int nFltIndex = 0;
	int numFlights = m_pFlightSchedule->getCount();

	for (; nFltIndex < numFlights; nFltIndex++)
	{
		Flight* pItem = m_pFlightSchedule->getItem(nFltIndex);
		if(pItem->IsEqual(pFlight))
			break;
	}
	if(nFltIndex<numFlights)
		return nFltIndex;

	return -1;
}



//	1.	execute actively group immediately( those item's entryTime do not need be update.
//	2.  if flight does not has stand, these nonActive groups need be execute immediately.
void MobElementsLifeCycleManager::ScheduleGroupEntryEvent()
{
	// actively group
	m_eventNonArrivalPaxEvent.initList(m_pNonArrivalPaxLog, m_pNonArrivalPaxLog->getCount());

	// flight which does not has stand
	int numFlights = m_pFlightSchedule->getCount();
	for (int nFltIndex = 0; nFltIndex < numFlights; nFltIndex++)
	{
		const Flight* pFlight = m_pFlightSchedule->getItem (nFltIndex);

		if(pFlight->getStandID() == -1)// no stand defined
		{
			MobileElementGroup* pGroup = GetMobileElementGroup(nFltIndex);
			if( pGroup != NULL )
				pGroup->ScheduleEntryEventDirectly();
		}
	}
}
