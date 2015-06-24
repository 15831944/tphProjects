#include "stdafx.h"
#include "engine\pplmover.h"
#include "engine\station.h"
#include "engine\person.h"

static int stnID = 0;

PeopleMover::PeopleMover (Dispatcher *p_dispatch)
{
    id = stnID;
    stnID++;
    dispatcher = p_dispatch;
    shuttingDown = 0;
}

//Initializes attributes.
void PeopleMover::initialize (const PeopleMoverData *p_data)
{
    capacity = p_data->getValue (PeopleMoverData::CARS_PER_TRAIN);
    capacity *= p_data->getValue (PeopleMoverData::CAR_CAPACITY);
    speed = 100.0 * p_data->getValue (PeopleMoverData::SPEED);
    rateOfAccel = p_data->getValue (PeopleMoverData::ACCEL);
    rateOfDecel = p_data->getValue (PeopleMoverData::DECEL);
}

//Returns moving time if mover is located.
ElapsedTime PeopleMover::getMoveTime (Point p_dest) const
{
    if (!location)
        return 0l;
    double dist = location.distance (p_dest);
    return (ElapsedTime) (float) (dist / (double)speed);
}

//Attaches a station to mover.
void PeopleMover::setStation (Station *p_station)
{
    currentStation = p_station;
    if (currentStation)
        location = currentStation->getServiceLocation (NULL);
}

//Adds person to mover if there is at least one more position.
//Returns false if no more space for next passenger.
int PeopleMover::addOccupant (Person *p_person, ElapsedTime p_time)
{
    occupants.addItem (p_person);
    p_person->setState (BoardVehicle);
    p_person->generateEvent (p_time);
    return (occupants.getCount()+1 < capacity);
}

//Releases all passengers that can disembark at this station.
void PeopleMover::unloadOccupants (ElapsedTime p_time)
{
    Person *aPerson;

    for (int i = occupants.getCount() - 1; i >= 0; i--)
    {
        aPerson = (Person *)occupants.getItem (i);
        if (aPerson->getProcessor() == currentStation)
        {
            occupants.removeItem (i);
            aPerson->setState (LeaveVehicle);
            aPerson->generateEvent (p_time);
        }
    }
}

//Removes all passengers from mover.
void PeopleMover::clearPassengers (ElapsedTime p_time)
{
    Person *aPerson;
    for (int i = occupants.getCount() - 1; i >= 0; i--)
    {
        aPerson = (Person *)occupants.getItem (i);
        occupants.removeItem (i);
        aPerson->kill (p_time);
        delete aPerson;
    }
}

