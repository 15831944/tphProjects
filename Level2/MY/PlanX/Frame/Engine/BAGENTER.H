/*****
*
*   Class       BaggageEntryEvent
*   Author      Lincoln Fellingham
*   Date        May 1994
*   Purpose     Handles timing of arriving bags arriving at baggage
*               carosels in the terminal
*
*               Designed to have only one instance in memory at a time
*               To load entire event list some members must become static
*
*****/

#ifndef BAGGAGE_ENTRY_EVENT_H
#define BAGGAGE_ENTRY_EVENT_H

#include "common\template.h"
#include "engine\terminalevent.h"
#include "results\baglog.h"

/* 
The class handles timing of arriving bags arriving at baggage carosels in the terminal. 
It is designed to have only one instance in memory at a time.
*/ 
class ENGINE_TRANSFER BaggageEntryEvent : public TerminalEvent
{
protected:
    //a list of Bags waiting to enter the terminal.
    BagLog *entryList;

    //the next Bag scheduled to enter the terminal.
    BagLogEntry nextEntry;

    //the total number of bags being simulated
    long bagCount;

    //the number of bags processed so far
    long currentCount;

    //This operation reads and schedules the next arrival event.
    int scheduleNextEntry ();

public:
    BaggageEntryEvent() {};
    virtual ~BaggageEntryEvent () {};

    //It intializes the bag's entry list and schedules first entry event.
    void initBagList (BagLog *p_list);

    virtual int process (InputTerminal* _pInTerm);

    // For shut down event.
    virtual int kill (void) { return 0; }

    // Returns event's name.
    virtual const char *getTypeName (void) const { return "Bag Entry"; }

    // Returns event type.
    int getEventType (void) const { return BagEntry; }
};


#endif
