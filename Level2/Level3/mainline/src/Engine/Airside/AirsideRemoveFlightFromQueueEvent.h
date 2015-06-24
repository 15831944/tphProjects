#pragma once
#include "airsideevent.h"

class LogicRunwayInSim;
class AirsideFlightInSim;

class CAirsideRemoveFlightFromQueueEvent :
	public AirsideEvent
{
public:
	CAirsideRemoveFlightFromQueueEvent(LogicRunwayInSim *pCheckRunway,AirsideFlightInSim *pFlight);
	~CAirsideRemoveFlightFromQueueEvent(void);


public:
	virtual int Process();

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "AirsideRemoveFlightFromQueueEvent";}

	virtual int getEventType (void) const {return RemoveFlightFromTakeOffQueue;}

protected:
	LogicRunwayInSim *m_pRunway;
	AirsideFlightInSim* m_pMobileElement;


};
