#include "StdAfx.h"
#include ".\terminaltrainentryevent.h"
#include ".\Inputs\SingleCarSchedule.h"
#include ".\Results\TrainLogEntry.h"
#include ".\Inputs\AllCarSchedule.h"
#include "MovingTrain.h"
#include "../Common/ARCTracker.h"
#include "terminal.h"
#include "ARCportEngine.h"

TerminalTrainEntryEvent::TerminalTrainEntryEvent(void)
{
}

TerminalTrainEntryEvent::~TerminalTrainEntryEvent(void)
{
}

int TerminalTrainEntryEvent::process(CARCportEngine* pEngine )
{
	PLACE_METHOD_TRACK_STRING();
	MovingTrain* newTrain = NULL;
	Terminal* pTerm = pEngine->getTerminal();
	newTrain = new MovingTrain(m_nextEntry,pTerm);
	pEngine->m_simMovingTrainList.push_back(newTrain);
//	m_nCurrentCount++;
	newTrain->generateEvent(time,false);
	scheduleNextEntry();
	return FALSE;
}

int TerminalTrainEntryEvent::kill()
{
	return 0;
}

const char* TerminalTrainEntryEvent::getTypeName() const
{
	return _T("TrainEntry");
}

int TerminalTrainEntryEvent::getEventType() const
{
	return EntryEvent;
}

// Discarded by Benny
//
// int TerminalTrainEntryEvent::scheduleNextEntry()
// {
// 	TRACK_CLASS_METHOD(TerminalTrainEntryEvent::scheduleNextEntry());
// 	if( m_nCurrentCount >= m_nTrainCount )
// 		return FALSE;
// 
// 	while( true )
// 	{
// 		m_pEntryList->getItem( m_nextEntry, m_nCurrentCount );
// // 		if( m_nextEntry.GetStartTime() < time )
// // 			throw new StringError ("Mobile Elements not sorted order by entry time");
// 
// 		m_nextEntry.SetStartTime(m_nextEntry.GetStartTime() + m_nextEntry.GetHeadWayTime() * m_nCurrentCount);
// 		time = m_nextEntry.GetStartTime();
// 		if (m_nCurrentCount >= m_nTrainCount)
// 		{
// 			return FALSE;
// 		}
// 		else
// 		{	
// 			m_nCurrentCount++;
// 			addEvent();
// 			return TRUE;
// 		}
// 	}
// }

int TerminalTrainEntryEvent::scheduleNextEntry()
{
	PLACE_METHOD_TRACK_STRING();
	if( m_nCurrentCount >= m_nTrainCount )
		return FALSE;

	m_pEntryList->getItem( m_nextEntry, m_nCurrentCount );
	time = m_nextEntry.GetStartTime();

	m_nCurrentCount++;
	addEvent();
	return TRUE;
}

void TerminalTrainEntryEvent::initList(CTrainLog *p_list, long p_max)
{
	PLACE_METHOD_TRACK_STRING();
	m_pEntryList = p_list;
	m_nCurrentCount = 0;

	time = 0l;

	if( p_max <= 0 )
	{
		m_nTrainCount = p_list->getCount();
	}
	else
	{
		m_nTrainCount = MIN((p_list->getCount()),p_max);
	}
	scheduleNextEntry();
}