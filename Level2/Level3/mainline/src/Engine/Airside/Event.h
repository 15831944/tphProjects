#pragma once
#include "../EngineDll.h"
#include "../EngineConfig.h"
#include "../SimClock.h"
#include "../../Common/Referenced.h"
#include "../../Common/ref_ptr.h"
#include "../../Common/point.h"
#include "../../Common/containr.h"

NAMESPACE_AIRSIDEENGINE_BEGINE

class SimFlight;

//abstract event 
class ENGINE_TRANSFER SimEvent : public Referenced {
public:
	virtual ~SimEvent();
	SimClock::TimeType csEventTime;
	
	void setEventTime(const SimClock::TimeType& time ){ csEventTime = time; }
	SimClock::TimeType getEventTime()const{ return csEventTime; }
	int operator < (const SimEvent& anEvent) const { return csEventTime < anEvent.csEventTime; };
	int operator == (const SimEvent& anEvent) const { return csEventTime == anEvent.csEventTime; };
};
// flight event



class ENGINE_TRANSFER FlightEvent : public SimEvent
{
public:
	enum EventType
	{ 
		FlightBirth, 
		Cruise,
		Descent,
		Approach,  
		ArriveThreshold,
		Touchdown,
		ExitRunway,
		TaxiIn,
		StopInGate,
		Pushback,
		OutGate,
		TaxiOut,
		ReadyForTakeoff,
		Takeoff,
		ClimbOut,
		FlightTerminate,		
	};

public:
	FlightEvent(SimFlight* pSimFlight)
	 : m_pSimFlight(pSimFlight)
	{}

	virtual ~FlightEvent();
	int operator < (const FlightEvent& anEvent) const { return csEventTime < anEvent.csEventTime; };
	int operator == (const FlightEvent& anEvent) const { return csEventTime == anEvent.csEventTime; };

	//other parameters describe this event
private:
	int nEventID;
	EventType csEventType;
	CString strEntityID;				// Link Name/Hold Name/Runway Name/TaxiwaySegName / Gate Name, 
	Point FlightPos;
	double FlightHorizontalSpeed;		//knot
	double FlightVerticalSpeed;			// ft/min
	SimClock::TimeType csComputeTime;   //eval time
	
	SimFlight* m_pSimFlight;

public:
	void SetEventID(int _id)
	{
		nEventID = _id;
	}

	int GetEventID() const
	{
		return nEventID;
	}

	void SetEventType(EventType _type )
	{
		csEventType = _type;
	}	

	EventType GetEventType() const
	{
		return csEventType;
	}

	void SetComputeTime(SimClock::TimeType _time)
	{
		csComputeTime = _time;
	}

	SimClock::TimeType GetComputeTime() const
	{
		return csComputeTime;
	}

	void SetEventPos(Point _pos)
	{
		FlightPos = _pos;
	}	

	Point GetEventPos() const
	{
		return FlightPos;
	}

	void SetEntityID(CString strID)
	{
		strEntityID = strID;
	}

	CString GetEntityID() const
	{
		return strEntityID; 
	}

	void SetFlightHorizontalSpeed(double _speed)
	{
		FlightHorizontalSpeed = _speed;
	}

	double GetFlightHorizontalSpeed() const
	{
		return FlightHorizontalSpeed;
	}

	void SetFlightVerticalSpeed(double _speed)
	{
		FlightVerticalSpeed = _speed;
	}

	double GetFlightVerticalSpeed() const
	{
		return FlightVerticalSpeed;
	}

	SimFlight* GetSimFlight() const
	{
		return m_pSimFlight;
	}

};

class ENGINE_TRANSFER FlightEventList : public SortedContainer<FlightEvent>
{
public:
	FlightEvent * getBirthEvent();
	FlightEvent * getLastEvent();

	FlightEvent * getEventInType(FlightEvent::EventType eventType);
	void Delay(FlightEvent& firstEvent, SimClock::TimeSpanType delayTime);
};


NAMESPACE_AIRSIDEENGINE_END


















