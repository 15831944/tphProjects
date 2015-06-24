#pragma once
#include "airsideevent.h"

class AirsideFlightInSim;

class ENGINE_TRANSFER FlightApplyForTowServiceEvent: public AirsideEvent
{
public:
	FlightApplyForTowServiceEvent(AirsideFlightInSim* pFlight);
	~FlightApplyForTowServiceEvent(void);

	int Process();

	virtual const char *getTypeName (void) const { return "ApplyForTowServiceEvent";}
	virtual int getEventType (void) const {return ApplyForTowServiceEvent;}

private:
	AirsideFlightInSim* m_pFlight;
};


class ENGINE_TRANSFER FlightApplyForDeiceServiceEvent: public AirsideEvent
{
public:
	FlightApplyForDeiceServiceEvent(AirsideFlightInSim* pFlight);
	~FlightApplyForDeiceServiceEvent(void);

	int Process();

	virtual const char *getTypeName (void) const { return "ApplyForDeiceServiceEvent";}
	virtual int getEventType (void) const {return ApplyForDeiceServiceEvent;}

private:
	AirsideFlightInSim* m_pFlight;
};
