/*****
*
*   Class       BoardingCallEvent
*   Author      Lincoln Fellingham
*   Date        August 1994
*   Purpose     Notifies groups of departing passengers when they can
*               board their aircraft
*
*               A number of calls made for each flight, typically a
*               pre-boarding call for passengers that need assistance,
*               followed by passengers in the rearmost rows, and finally
*               forward rows and business or first class
*
*****/

#ifndef BOARDING_CALL_EVENT_H
#define BOARDING_CALL_EVENT_H

#include "common\template.h"
#include "engine\terminalevent.h"
#include "inputs\MobileElemConstraint.h"


/*
Purpose :
    Notifies groups of departing passengers when they can  board their aircraft.
    A number of calls are made for each flight, typically a pre-boarding call for passengers that need assistance.
    The flight is followed by passengers in the rearmost rows, and finally forward rows and business or first class.
*/
class BoardingCallEvent : public TerminalEvent
{
protected:
    // The percent of the departing flight's passengers that will be boarding the aircraft at the scheduled time.
    float percent;

    // A gate at which the departing flight is docked.
    GateProcessor *gate;

    //A constrian that is used to call passengers on the correct flight.
    CMobileElemConstraint flight;

	// which stage 
	int m_iStage;

public:

    BoardingCallEvent () { percent = 0; gate = NULL; m_iStage = 0;};

    //copy constructor
    BoardingCallEvent (const BoardingCallEvent& p_event) 
	{ gate = p_event.gate;  
	  percent = p_event.percent; 
          flight = p_event.flight; };

    virtual ~BoardingCallEvent () {};
     
    static void loadBoardingCalls (const ProcessorList *p_procs, const FlightSchedule *p_flights, const FlightData *p_data, InputTerminal* _pInTerm);

    // Sends boarding call to all holding areas associated with the departure's gate, allowing some passengers to board the aircraft.
    // Always return TRUE.
    // All boarding calls are unique.
    virtual int process (CARCportEngine* _pEngine);

    //For shut down event.
    virtual int kill (void) { return 1; }

	//It adds receiver to the event list
	virtual void addEvent ();

    // Returns event's name
    virtual const char *getTypeName (void) const { return "Departure"; };

    // Returns event type.
    int getEventType (void) const { return BoardingCall; };

	int getStage()const {return m_iStage;}

    // Initializes a boarding calls' individual data elements.
    void init (float p_proportion, GateProcessor *p_proc, const CMobileElemConstraint& p_type, int _iStage) 
	{ 
		  percent = p_proportion;
          gate = p_proc; 
		  flight = p_type; 
		  m_iStage = _iStage;
	}
};

#endif
