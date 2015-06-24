#pragma once
#include "event.h"
class OnboardDoorInSim;
class CARCportEngine;

class OnboardCloseDoorEvent :
	public Event
{
public:
	OnboardCloseDoorEvent(OnboardDoorInSim* pDoorInSim);
	virtual ~OnboardCloseDoorEvent(void);

	virtual int process(CARCportEngine* pEngine );

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "CloseDoorEvent";}

	virtual int getEventType (void) const {return CloseDoorEvent;}

private:
	OnboardDoorInSim*	m_pOnboardDoorInSim;
};
