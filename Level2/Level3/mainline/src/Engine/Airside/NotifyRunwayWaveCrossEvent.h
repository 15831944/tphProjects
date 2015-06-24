#pragma once
#include "AirsideMobileElementEvent.h"

class RunwayInSim;
class AirsideFlightInSim;

class CNotifyRunwayWaveCrossEvent : public AirsideEvent
{
public:
	//runway, the runway need to notify
	//flt, the flight which notify the runway
	CNotifyRunwayWaveCrossEvent(RunwayInSim *pNotifyRunway,AirsideFlightInSim* pFlt);
	~CNotifyRunwayWaveCrossEvent(void);

public:
	virtual int Process();

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "NotifyRunwayWaveCross";}

	virtual int getEventType (void) const {return VehicleService;}
	
protected:
	RunwayInSim *m_pRunway;
	AirsideFlightInSim *m_pFlight;
};
