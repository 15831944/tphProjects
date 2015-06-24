#pragma once
#include "AirsideMobileElementEvent.h"

class AirsideFlightInSim;
class ENGINE_TRANSFER FlightBirthEvent : public CAirsideMobileElementEvent
{
public:
	explicit FlightBirthEvent(AirsideFlightInSim* flt, CARCportEngine * pEngine = NULL);
	~FlightBirthEvent(void);

	virtual int Process();
	
	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "FlightBirth";}

	virtual int getEventType (void) const {return FlightBirth;}

private:
	void PaxGeneration();

	
private:
	AirsideFlightInSim* m_pCFlight;
	bool m_bPaxBirth;
	CARCportEngine *m_pARCPortEngine;	
};
