#pragma once
#include "event.h"
class ISteppable;
class ENGINE_TRANSFER ScheduleStepEvent : public Event
{
public:
	ISteppable* m_pSteppable;
	ScheduleStepEvent(ISteppable* pSch,const ElapsedTime& time);

	virtual int process (CARCportEngine* pEngine );

	//For shut down event
	virtual int kill (void) ;

	//It returns event's name
	virtual const char *getTypeName (void) const ;

	//It returns event type
	virtual int getEventType (void) const;
};
