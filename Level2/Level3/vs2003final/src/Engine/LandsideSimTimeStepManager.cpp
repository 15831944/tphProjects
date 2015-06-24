#include "StdAfx.h"
#include "LandsideSimTimeStepManager.h"
#include "LandsideSimulation.h"
#include "..\Landside\LandsideSimSetting.h"
#include "LandsideSimTimeStepEvent.h"


LandsideSimTimeStepManager::LandsideSimTimeStepManager()
:m_pNextEvent(NULL)
{

}

void LandsideSimTimeStepManager::Initialize()
{
	//simulation setting, time step
	LandsideSimSetting lsSimSetting;
	lsSimSetting.ReadData();
	ElapsedTime eDefaultStep;
	lsSimSetting.getDefaultTimeStep(eDefaultStep);
	
	//add the first  one, at the engine start, set the default time step
	LsTimeStepEvent defaultStep(ElapsedTime(0L), eDefaultStep);
	m_vEventList.push_back(defaultStep);


	
	//there will be 2 events 	
	TimeStepRangeList* pSimSetpRange = lsSimSetting.GetTimeStepRangeList();
	int nRangeCount = pSimSetpRange->GetItemCount();
	for(int nRange = 0; nRange < nRangeCount; ++ nRange)
	{
		TimeStepRange* pTimeRange = pSimSetpRange->GetItem(nRange);
		ElapsedTime eTimeStep;
		pTimeRange->GetTimeStep(eTimeStep);

		LsTimeStepEvent stepEventStart(pTimeRange->getTimeRange().GetStartTime(), eTimeStep);
		m_vEventList.push_back(stepEventStart);


		//set back to default
		LsTimeStepEvent stepEventEnd(pTimeRange->getTimeRange().GetEndTime(), eDefaultStep);
		m_vEventList.push_back(stepEventEnd);

	}

	std::sort(m_vEventList.begin(), m_vEventList.end(),LandsideSimTimeStepManager::CompareTimeStep);

	ScheduleNextEvent();

}

bool LandsideSimTimeStepManager::CompareTimeStep( const LsTimeStepEvent& event1, const LsTimeStepEvent& event2 )
{
	if(event1.m_eEventTime > event2.m_eEventTime)
		return false;

	return true;

}

void LandsideSimTimeStepManager::ScheduleNextEvent()
{
	if (m_vEventList.size())
	{
		LsTimeStepEvent& lsEvent =	m_vEventList[0];
		m_pNextEvent = new LandsideSimTimeStepEvent(lsEvent.m_eTimeStep, this);
		m_pNextEvent->setTime(lsEvent.m_eEventTime);
		m_pNextEvent->addEvent();

		m_vEventList.erase(m_vEventList.begin());
	}
	else
	{
		m_pNextEvent = NULL;
	}




}
