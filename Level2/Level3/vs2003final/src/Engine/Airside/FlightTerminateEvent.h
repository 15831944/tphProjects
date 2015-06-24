
#include "AirsideMobileElementEvent.h"

class AirsideFlightInSim;

class ENGINE_TRANSFER AirsideFlightTerminateEvent : public CAirsideMobileElementEvent
{
public:
	AirsideFlightTerminateEvent(AirsideFlightInSim* pFlight);
	~AirsideFlightTerminateEvent();

	virtual int Process();

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "FlightTerminateEvent";}

	virtual int getEventType (void) const {return FlightTerminateEvent;}

private:
	AirsideFlightInSim* m_pCFlight;

};

