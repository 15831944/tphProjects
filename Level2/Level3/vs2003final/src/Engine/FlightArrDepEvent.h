// FlightArrDepEvent.h: interface for the FlightArrDepEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLIGHTARRDEPEVENT_H__F12AA06A_2E68_4B55_A680_800E2C54419C__INCLUDED_)
#define AFX_FLIGHTARRDEPEVENT_H__F12AA06A_2E68_4B55_A680_800E2C54419C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\template.h"
#include "engine\terminalevent.h"


//////////////////////////////////////////////////////////////////////////
// the class only to handle flight arrival or departure the airport
//////////////////////////////////////////////////////////////////////////
class ENGINE_TRANSFER FlightArrDepEvent : public TerminalEvent  
{
private:
	int m_iStandGateIndex;
	int m_iFlightState;
public:
	FlightArrDepEvent( int _index, int _state )
	{
		m_iStandGateIndex = _index;
		m_iFlightState = _state;
	}
	// copy constructor
	FlightArrDepEvent( FlightArrDepEvent& _entry )
	{
		m_iStandGateIndex = _entry.m_iStandGateIndex;
		m_iFlightState = _entry.m_iFlightState;
	}
	virtual ~FlightArrDepEvent() {};
	
	// to stand gate, write a FlightArrivalAirport/FlightDepartureAirport Log
    virtual int process (CARCportEngine* pEngine);
	
    //It is for shut down event.
    virtual int kill (void) { return 0; };
	
    //It returns an event's name
    virtual const char *getTypeName (void) const { return "FlightArrDep"; };
	
    //It returns an event type
    int getEventType (void) const { return FlightEvent; };

};

#endif // !defined(AFX_FLIGHTARRDEPEVENT_H__F12AA06A_2E68_4B55_A680_800E2C54419C__INCLUDED_)
