#pragma once
#include "airsidemobileelementevent.h"

class AirsideFlightInSim;

class CWakeUpFlightWaitingTakeOffEvent :
	public CAirsideMobileElementEvent
{
public:
	CWakeUpFlightWaitingTakeOffEvent(AirsideFlightInSim* pWakeUpFlt);
	~CWakeUpFlightWaitingTakeOffEvent(void);

public:
	virtual int Process();

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "WakeUpFlightWaitingTakeOffEvent";}

	virtual int getEventType (void) const {return WakeUpFlightWaitingTakeOffEvent;}
protected:

};
