#pragma once
#include "airsidemobileelementevent.h"

class AirsideFlightInSim;
class CRemoveAirsideMobileElementEventEvent :
	public CAirsideMobileElementEvent
{
public:
	CRemoveAirsideMobileElementEventEvent(AirsideFlightInSim *pFlight);
	~CRemoveAirsideMobileElementEventEvent(void);

public:
	virtual int Process();

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "RemoveAirsideMobileElementEventEvent";}

	virtual int getEventType (void) const {return RemoveAirsideMobileElementEventEvent;}
};
