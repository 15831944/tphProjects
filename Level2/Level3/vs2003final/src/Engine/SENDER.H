#ifndef SENDER_H
#define SENDER_H

#include "common\elaptime.h"
#include "common\containr.h"
#include "common\template.h"
#include "engine\visitor.h"

class Sender : public Visitor
{
protected:
    ElapsedTime visitTime;

public:
    Sender (MobLogEntry& p_entry, Terminal* _pTerm);
    virtual ~Sender () {};

    // Attaches visitor to a passenger
    virtual void initPassenger (Passenger *p_pax);

    //It is a method called by Passenger to update visitor's location, state.
    virtual int update (ElapsedTime p_time);

    // Dumps all tracks to the log and updates ElementDescription record.
    // It is called at death of passenger and must repeat once for each member of group.
    virtual void flushLog (ElapsedTime p_time, bool bmissflight = false);
};

#endif
