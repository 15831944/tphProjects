#pragma once
#include "airsidemobileelementevent.h"
#include "../../Common/elaptime.h"

class AirsideFlightInSim;

class CWakeUpFlightWaitingLandingEvent :
	public CAirsideMobileElementEvent
{
public:
	CWakeUpFlightWaitingLandingEvent(AirsideFlightInSim *pWakeUpFlight);
	~CWakeUpFlightWaitingLandingEvent(void);

public:
	virtual int Process();

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "WakeUpFlightWaitingLandingEvent";}

	virtual int getEventType (void) const {return WakeUpFlightWaitingLandingEvent;}

protected:

};
