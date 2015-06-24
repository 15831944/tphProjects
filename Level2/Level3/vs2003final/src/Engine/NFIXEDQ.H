/*****
*
*   Class       NonFixedQueue
*   Author      Lincoln Fellingham, Aviation Research Corporation
*   Date        July 12, 1993
*
*   Purpose     Member of Processor that handles non-fixed queue functions
*
*****/

#ifndef NON_FIXED_QUEUE_H
#define NON_FIXED_QUEUE_H

#include "common\template.h"
#include "engine\sequencq.h"
#include "common\path.h"
#include "common\point.h"
#include "common\elaptime.h"

//Member of Processor that handles non-fixed queue functions
class NonFixedQueue : public SequentialQueue
{
protected:

    //The operation recalculates destination for all Persons approaching the tail of the queue.
    //It is called when tail advances or a Person arrives at tail.
    void updateApproaching (ElapsedTime updateTime);

public:

    NonFixedQueue (const Path& path) : SequentialQueue (path) {};
    virtual ~NonFixedQueue () {};

    // Adds aPerson to the list of approaching Persons and calculates first waiting position (tail of the queue).
    void addToQueue (Person *aPerson);

    //Remove aPerson from approachList, add to waitList.
    //Sets pathIndex, resets destination of all other approaching Persons.
    //If next Person in queue has advanced, continue movemnt.
    void arriveAtQueue (Person *aPerson, ElapsedTime arriveTime);

    //Gets the current location of the tail.
    void getTailPoint (Person *aPerson) const;

    //Returns queue type, always "N".
    virtual char isFixed (void) const { return 'N'; };
};



#endif
