/*****
*
*   1 dispatcher per system
*
*****/

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "engine\station.h"
#include "engine\event.h"
#include "inputs\movrdata.h"
#include "engine\pplmover.h"

class Dispatcher;

class DispatcherEvent : public Event
{
protected:
    Dispatcher *dispatcher;
    Station *station;
    PeopleMover *mover;
    int operation;

public:
    DispatcherEvent () {};
    virtual ~DispatcherEvent () {};

    static void removeEvent (Station *p_stn, int p_op, EventList* _pEventList );

    void init (int p_op, Dispatcher *p_disp, Station *p_stn, PeopleMover *p_mover);

    virtual void addEvent ();

    // Main event processing routine for each Event type.
    virtual int process (InputTerminal* _pInTerm);

    //For shut down event.
    virtual int kill (void) { return 1; }

    //It returns event's name
    virtual const char *getTypeName (void) const { return "People Mover"; }

    //It returns event type
    virtual int getEventType (void) const { return PeopleMoverEvent; }

    enum
    {
        UPDATE_DATA,
        ARRIVE_AT_STATION,
        START_LOADING,
        CLOSE_STATION,
        DEPART_STATION
    };
};

//one dispatch per system
class Dispatcher
{
protected:
    PeopleMoverSystem *system;
    ProcessorArray stationList;
    UnsortedContainer<PeopleMover> peopleMovers;	 
    int curData;

public:
    Dispatcher () { curData = -1; }
    virtual ~Dispatcher ();

    void init (PeopleMoverSystem *p_system,InputTerminal* _pInTerm);

    void scheduleUpdate ();

    void updateData (ElapsedTime p_time);

    Station *getNextStation (Station *p_current);

    void clearPassengers (ElapsedTime p_time);

    const PeopleMoverData *getCurrentData (void) const { return system->getItem (curData); }

    int operator == (const Dispatcher &p_disp) const { return system == p_disp.system; }
};

class DispatcherList : UnsortedContainer<Dispatcher>
{
protected:

public:
    DispatcherList () : UnsortedContainer<Dispatcher> (8, 0, 8) {};	 
    virtual ~DispatcherList () {};

    void initialize (PeopleMoverDataSet *p_dataSet, InputTerminal* _pInTerm);

    void clearPassengers (ElapsedTime p_time);
};

#endif
