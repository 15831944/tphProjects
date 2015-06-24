#pragma  once

#pragma once

#include "AirsideEvent.h"
#include "AirsideFlightInSim.h"

class ReleaseResourceLockEvent : public AirsideEvent
{
public:
	int Process()
	{
		ASSERT(m_pResource);
		if(m_pResource)
			m_pResource->ReleaseLock(m_pFlight,getTime());
		return TRUE;
	}

	ReleaseResourceLockEvent(AirsideFlightInSim * pFlight, AirsideResource * pRes){ m_pFlight = pFlight; m_pResource = pRes; }

	AirsideFlightInSim * getFlight() const { return m_pFlight ; } 

	virtual const char *getTypeName (void) const { return "FlightGetClearance";}

	virtual int getEventType (void) const {return FlightGetClearnce;}

public:
	AirsideResource * m_pResource;
	AirsideFlightInSim * m_pFlight;
};