#pragma once
#include "AirsideMobileElementEvent.h"

class RunwayInSim;
class AirsideFlightInSim;

class CCheckRunwayWaveCrossEvent: public AirsideEvent
{
public:
	//pCheckRunway, the runway need to be check
	//pFlight, the flight which landing or take off
	CCheckRunwayWaveCrossEvent(RunwayInSim *pCheckRunway,AirsideFlightInSim *pFlight);
	~CCheckRunwayWaveCrossEvent(void);

public:
	virtual int Process();

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "CheckRunwayWaveCrossEvent";}

	virtual int getEventType (void) const {return CheckRunwayWaveCrossEvent;}

protected:
	RunwayInSim *m_pRunway;
	AirsideFlightInSim* m_pMobileElement;
};
