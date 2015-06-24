#pragma once
#include "AirsideMobileElementEvent.h"

class AirsideFlightInSim;
class AirspaceHoldInSim;
class CPoint2008;
class ENGINE_TRANSFER FlightInHoldEvent: public CAirsideMobileElementEvent
{
public:
	explicit FlightInHoldEvent(AirsideFlightInSim* flt, AirspaceHoldInSim* pHold, ElapsedTime tHoldTime);
	~FlightInHoldEvent(void);

	virtual int Process();

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "FlightInHold";}

	virtual int getEventType (void) const {return FlightInHold;}

private:
	void stepIt(const ElapsedTime& eTime,CPoint2008& pt);

private:
	AirsideFlightInSim* m_pCFlight;
	AirspaceHoldInSim* m_pHold;
	ElapsedTime m_tHoldTime;
};

class ENGINE_TRANSFER FlightWakeupEvent: public CAirsideMobileElementEvent
{
public:
	explicit FlightWakeupEvent(AirsideFlightInSim* flt);
	~FlightWakeupEvent(void);

	virtual int Process();

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "FlightGetClearnce";}

	virtual int getEventType (void) const {return FlightGetClearnce;}


private:
	AirsideFlightInSim* m_pCFlight;
};