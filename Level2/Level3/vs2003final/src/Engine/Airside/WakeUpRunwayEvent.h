#pragma once
#include "airsidemobileelementevent.h"


class RunwayInSim;
class AirsideFlightInSim;
class CWakeUpRunwayEvent :
	public AirsideEvent
{
public:
	CWakeUpRunwayEvent(AirsideFlightInSim *pFlight,RunwayInSim *pWakeUpRunway);
	~CWakeUpRunwayEvent(void);

public:
	virtual int Process();

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "WakeUpRunwayEven";}

	virtual int getEventType (void) const {return WakeUpRunwayEven;}

	void SetInvalid(bool bInvalid);

protected:
	RunwayInSim *m_pWakeUpRunway;
	AirsideFlightInSim* m_pMobileElement;
	bool m_bInvalid;
};
