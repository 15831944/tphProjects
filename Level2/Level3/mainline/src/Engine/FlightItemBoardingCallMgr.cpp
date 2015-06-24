#include "StdAfx.h"
#include "FlightItemBoardingCallMgr.h"
#include "boardcal.h"

FlightItemBoardingCallMgr::FlightItemBoardingCallMgr(int nFltIndex)
:m_nFltIndex(nFltIndex)
,m_bIsStarted(false)
{
	m_vectEvent.clear();
}

FlightItemBoardingCallMgr::~FlightItemBoardingCallMgr(void)
{
	m_vectEvent.clear();
}


int  FlightItemBoardingCallMgr::GetFlightIndex()
{
	return m_nFltIndex;
}

void FlightItemBoardingCallMgr::SetPlannedDepTime(const ElapsedTime& dtPlannedDepTime)
{
	m_dtPlannedDepTime = dtPlannedDepTime;
}

void FlightItemBoardingCallMgr::AddBoardingCallEvent(BoardingCallEvent* pNewItem)
{
	m_vectEvent.push_back(pNewItem);
}

void FlightItemBoardingCallMgr::delayEvents(ElapsedTime delay)
{ 
	std::vector<BoardingCallEvent*>::iterator iter = m_vectEvent.begin();
	for (;iter != m_vectEvent.end(); iter++)
	{
		BoardingCallEvent* pItem = *iter;
		ElapsedTime eventTime = pItem->getTime() + delay;
		pItem->setTime(  max( ElapsedTime(0L), eventTime ) );
	}
}

void FlightItemBoardingCallMgr::ScheduleBoardingCallEvents(const ElapsedTime& currentTime,const ElapsedTime& depTime)
{
 	m_bIsStarted = true;
 
	ElapsedTime dtDepTime = depTime - m_dtPlannedDepTime;
 	// flight's departure time is later than planned time

	
	for (size_t i=0; i<m_vectEvent.size();i++)
	{
		BoardingCallEvent* pItem = m_vectEvent.at(i);
		if (pItem && pItem->getStage() == 1)
		{
			pItem->setTime(max(currentTime,pItem->getTime() + dtDepTime));
		}
		
	}

 	// add to eventList
 	for (size_t i=0; i<m_vectEvent.size();i++)
	{
		BoardingCallEvent* pEvent = m_vectEvent.at(i);
		if (pEvent && pEvent->getStage() == 1)
		{
			pEvent->addEvent();
		}
	}
}

void FlightItemBoardingCallMgr::ScheduleDirectly()
{	
	m_bIsStarted = true;
	for (size_t i=0; i<m_vectEvent.size();i++)
		m_vectEvent.at(i)->addEvent();
}

void FlightItemBoardingCallMgr::ScheduleSecondDirectly()
{
	std::vector<BoardingCallEvent*> vCopy = m_vectEvent;
	m_vectEvent.clear();

	for (size_t i=0; i<vCopy.size();i++)
	{
		BoardingCallEvent* pEvent = vCopy.at(i);
		if (pEvent && pEvent->getStage() > 1)
		{
			pEvent->addEvent();			
		}
		else
		{
			m_vectEvent.push_back(pEvent);
		}
	}
	
}