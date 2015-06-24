

#pragma once
#include "AirsideMobileElementEvent.h"
#include "AirsideFlightInSim.h"

class FlightGetClearanceEvent : public CAirsideMobileElementEvent
{
public:
	int Process()
	{
		ASSERT(m_pFlight);
		m_pFlight->WakeUp(time);
		return TRUE;
	}

	FlightGetClearanceEvent(AirsideFlightInSim * pFlight)
		:CAirsideMobileElementEvent(pFlight)
	{
		m_pFlight = pFlight; 
	}

	AirsideFlightInSim * getFlight() const { return m_pFlight ; } 

	virtual const char *getTypeName (void) const { return "FlightGetClearance";}

	virtual int getEventType (void) const {return FlightGetClearnce;}

protected:
	AirsideFlightInSim * m_pFlight;

};