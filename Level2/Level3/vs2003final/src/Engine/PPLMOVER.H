/*****
*
*   Unloads passengers in inverse (LIFO) order
*   Not yet recording
*
*****/

#ifndef PEOPLE_MOVER_H
#define PEOPLE_MOVER_H

#include "common\template.h"
#include "common\point.h"
#include "inputs\movrdata.h"
#include "engine\mobile.h"
//#include "arctools\general\containr.h"

/*
Unloads passengers in inverse (LIFO) order.
Not yet recording.
*/

/*
VehicleDescStruct
{
    long id;
    long system;

    long startTime;
    long endTime;
    long startPos;              // eventFile offset of 1st event
    long endPos;                // eventFile offset of last event
}
*/

class PeopleMover
{
protected:
    Station *currentStation;
    Dispatcher *dispatcher;

    //mover id.
    long id;

    //mover capacity.
    long capacity;

    double rateOfAccel;
    double rateOfDecel;

    //mover speed.
    double speed;

    //mover location.
    Point location;

    //Flag of shut down.
    int shuttingDown;

    //occupants in mover.
    MobileElementList occupants;

public:
    PeopleMover (Dispatcher *p_dispatch);
    virtual ~PeopleMover () {};

    void initialize (const PeopleMoverData *p_data);

    //Returns mover id.
    int getID (void) const { return id; }

    //Checks if mover is shut down.
    int isShuttingDown (void) const { return shuttingDown; }

    //Set flag shuttingDown true.
    void shutDown (void) { shuttingDown = 1; }

    int addOccupant (Person *p_person, ElapsedTime p_time);

    void unloadOccupants (ElapsedTime p_time);

    void setStation (Station *p_station);

    ElapsedTime getMoveTime (Point p_dest) const;

    int operator == (const PeopleMover &p_mover) const { return this == &p_mover; }

    void clearPassengers (ElapsedTime p_time);
};

#endif
