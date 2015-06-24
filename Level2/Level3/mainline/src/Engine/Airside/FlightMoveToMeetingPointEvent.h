#pragma once
#include "AirsideMobileElementEvent.h"

class FlightMoveToMeetingPointEvent:public CAirsideMobileElementEvent
{
public:
	FlightMoveToMeetingPointEvent(CAirsideMobileElement* pMobileElement);
	~FlightMoveToMeetingPointEvent(void);

	int Process();

	virtual const char *getTypeName (void) const { return "FlightMoveToMeetingPoint";}
	virtual int getEventType (void) const {return FlightMoveToMeetingPoint;}

};
