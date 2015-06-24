#include "StdAfx.h"
#include ".\schedulestepevent.h"
//#include "SteppableSchedule.h"
#include "./ISteppable.h"

ScheduleStepEvent::ScheduleStepEvent( ISteppable* pSch,const ElapsedTime& time )
{
	setTime(time);
	m_pSteppable = pSch;
}

int ScheduleStepEvent::process( CARCportEngine* pEngine )
{	
	m_pSteppable->step(*pEngine);
	/*ElapsedTime nextTime;
	if(m_pSchedule->getTime(nextTime))
	{
		(new ScheduleStepEvent(m_pSchedule,nextTime))->addEvent();
		return 1;
	}*/
	return 0;
}

int ScheduleStepEvent::kill( void )
{
	return TRUE;
}

const char * ScheduleStepEvent::getTypeName( void ) const
{
	return "Mobile Step Event";
}

int ScheduleStepEvent::getEventType( void ) const
{
	return ScheduleStep;
}