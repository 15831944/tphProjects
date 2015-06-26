/*****
*
*   Class       GreetingEvent
*   Author      Lincoln Fellingham
*   Date        May 1996
*   Purpose     Processes "greeting" between a visitor and an arrival
*
*****/

#ifndef GREET_EVENT_H
#define GREET_EVENT_H

#include "common\template.h"
#include "engine\terminalevent.h"
#include "common\elaptime.h"
class Passenger;
class PaxVisitor;

class CARCportEngine;
//This class processes "greeting" between a visitor and an arrival.
class GreetEvent : public TerminalEvent
{
protected:
    
    //pointer to a passenger
    Passenger *pax;

    //pointer to a greeter.
    PaxVisitor *greeter;

public:
    GreetEvent() {};

    //copy constructor.
    GreetEvent (Passenger *p_pax, PaxVisitor *p_greeter, ElapsedTime p_time)
        { pax = p_pax; greeter = p_greeter; time = p_time; }

    virtual ~GreetEvent () {};

    // This function is the main event processing routine.
    virtual int process (CARCportEngine *_pEngine );

    //For shut down event.
    virtual int kill (void) { return 0; }

    //Returns event's name.
    virtual const char *getTypeName (void) const { return "Greet"; }

    //Returns event type.
    int getEventType (void) const { return GreetingEvent; }
};

#endif
