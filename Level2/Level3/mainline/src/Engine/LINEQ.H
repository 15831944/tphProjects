#ifndef LINEAR_QUEUE_H
#define LINEAR_QUEUE_H

#include "common\template.h"
#include "engine\procq.h"
#include "engine\accessq.h"
//#include "arctools\general\containr.h"


class LinearQueue : public ProcessorQueue
{
protected:
    //Pointer to a list of passengers.
    MobileElementList m_waitList;

    //Path of line queue.
    Path m_actualCorners;

    //Number of conners of Path.
    int m_queuePoints;

    //The occupany flag.
    char* m_occupied;

public:

    LinearQueue (const Path& path);
    virtual ~LinearQueue () {};

    //Returns TRUE if Person is waiting at head of queue for server.
    virtual int hasWait (void) const;

    //Returns count of groups of Persons currently in the Queue.
    virtual int getQueueLength (void) const;

    //Returns count of groups currently in the Queue.
    virtual int getGroupCount (void) const;

    //Returns total count of groups currently in or approaching the Queue.
    virtual int getTotalGroupCount (void) const;

    // This function prints all Persons in queue from head to tail withou including person approaching.
    virtual void printContents (void) const;

    //Removes and kills all Persons in queue.
    virtual int wipe (ElapsedTime curTime);
	
    //Returns point at a corner of the original path.
    virtual Point corner (int index) const { return m_actualCorners.getPoint(index); };
	virtual Path* corner() { return &m_actualCorners; }

    //Returns number of conners of path.
    virtual int cornerCount (void) const { return m_actualCorners.getCount(); };

    //Removes a person from all containers.
    virtual void removePerson (const Person *aPerson);

    //Prepares the Person waiting at the head of the queue for departure.
    virtual void releaseNext (ElapsedTime releaseTime) {};

    //Prepares the Person waiting at the head of the queue for departure.
    virtual void release (Point availPoint, ElapsedTime releaseTime);

    //Removes person from the queue (only from waitList) to prevent duplicate departures.
    void removePerson (Person *aPerson);

    //Removes current head of queue and informs next Person of advancement.
    virtual void leaveQueue (const Person *aPerson, ElapsedTime departTime);

    //It adds a new Person to the list of approaching Persons.
    virtual void addToQueue (Person *aPerson);

    //Starts p_person moving and relays advance event to next pax in line.
    virtual void moveThroughQueue (Person *p_person, ElapsedTime p_time){ return; }

    //Add Person to the tail of the actual queue.
    virtual void arriveAtQueue (Person *aPerson, ElapsedTime arriveTime);

    //Gets the current location of the tail.
    virtual void getTailPoint (Person *aPerson) const{ getQueuePosition (aPerson); };
     
    //it determines aPerson's next pathSegment and waiting position
    virtual void getQueuePosition (Person *aPerson) const;

    virtual int getClosestIndex (const Point& aPoint) const;
	
	virtual MobileElementList* GetWaitList(){ return &m_waitList;}

	// Do offset to all position related data,
	virtual void DoOffset(DistanceUnit _xOffset, DistanceUnit _yOffset, DistanceUnit _zOffset);
	virtual void Rotate( DistanceUnit _degree );
	virtual void ClearAllTempData();

};


#endif
