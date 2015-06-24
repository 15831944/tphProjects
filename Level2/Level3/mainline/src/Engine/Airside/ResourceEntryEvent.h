#pragma once
#include "AirsideEvent.h"

#include "../../Common/point.h"

class AirsideFlightInSim;
class AirsideResource;
class Point;

class ENGINE_TRANSFER ResourceEntryEvent : public AirsideEvent
{
public:
	ResourceEntryEvent(void);
	~ResourceEntryEvent(void);

	virtual int Process();

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "FlightEnterResource";}


	virtual int getEventType (void) const {return FlightEnterResource;}

	void setEventPos (const Point& _pos)
	{
		m_EventPos = _pos;
	}

	Point getEventPos()const
	{
		return m_EventPos;
	}

private:
	Point m_EventPos;
};

class ENGINE_TRANSFER ResourceExitEvent : public AirsideEvent
{
public:
	ResourceExitEvent(AirsideResource* pRes,AirsideFlightInSim* pFlight);
	~ResourceExitEvent(void);

	virtual int Process();

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "FlightExitResource";}


	virtual int getEventType (void) const {return FlightExitResource;}

private:
	AirsideFlightInSim* m_pFlight;
	AirsideResource* m_pResource;
};