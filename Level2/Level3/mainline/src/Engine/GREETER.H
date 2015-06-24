#ifndef GREETER_H
#define GREETER_H

#include "common\containr.h"
#include "common\elaptime.h"
#include "engine\visitor.h"


class Greeter : public Visitor
{
protected:
    //passenger index that a greeter has.
    long paxIndex;

public:
    Greeter (MobLogEntry& p_entry, Terminal* _pTerm);
    virtual ~Greeter () {};

    //It is overloaded to process greeting events.
    virtual void processServerArrival (ElapsedTime p_time);

    //It attaches visitor to a passenger
    virtual void initPassenger (Passenger *p_pax);

    //This function is a method. It is called by Passenger to update visitor's location, state.
    virtual int update (ElapsedTime p_time);

    // This function dumps all tracks to the log and updates ElementDescription record.
    // It is called at death of mobile element and must repeat once for each member of group.
    virtual void flushLog (ElapsedTime p_time, bool bmissflight = false);
};

#endif
