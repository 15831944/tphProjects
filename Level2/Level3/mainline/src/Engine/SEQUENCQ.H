/*****
*
*   Class       SequentialQueue
*   Author      Lincoln Fellingham, Aviation Research Corporation
*   Date        May 6, 1994
*   Purpose     Contains methods from original ProcessorQueue class
*               that handle indexing and ordering
*
*               ProcessorQueue has become its superclass, containing
*               virtual functions that are common to both ordered and
*               random queues
*
*****/

#ifndef SEQUENTIAL_QUEUE_H
#define SEQUENTIAL_QUEUE_H

#include "common\template.h"
#include "engine\procq.h"
#include "engine\accessq.h"
//#include "engine\person.h"
#include "common\containr.h"

#define ARC_OVERFLOW			0xff
#define LAST_SEGMENT			(m_qCorners.getCount()-1)
#define MID_SEGMENT				(m_qCorners.getCount()-2)
//The SequentialQueue contains virtual functions that are common to both ordered and random queues.
class SequentialQueue : public ProcessorQueue
{
protected:
    //The queue of waiting person pointers.
    AccessibleQueue m_waitList;  

    // Pure member function : Recalculates destination for all Persons approaching the tail of the queue.
    // it is called when tail advances or a Person arrives at tail.
    virtual void updateApproaching (ElapsedTime updateTime) = 0;

    // If not yet at the HEAD of the queue and the next Person has already advanced, a person continues moving.
    // This test is required as the queue may have advanced between the time that aPerson was told to advance and the time that its event fired.
    // This is due to variations in speed and instep.
    void continueAdvance (Person *aPerson, ElapsedTime advanceTime);

public:

    // The rudimentary constructor.
    SequentialQueue (const Path& path) : ProcessorQueue (path) {};
    virtual ~SequentialQueue () {};

    // Returns TRUE if Person waiting at head of queue for server; FALSE otherwise.
    virtual int hasWait (void) const;
     
    int getQueueLength (void) const;

    // Returns count of groups currently in the Queue.
    virtual int getGroupCount (void) const;

    // Returns total count of groups currently in or approaching the Queue.
    int getTotalGroupCount (void) const;

    // Prints all Persons in queue from head to tail withou include approaching person.
    void printContents (void) const;

    // Removes and kills all Persons in queue.
    int wipe (ElapsedTime curTime);

    // Removes person from all containers.
    virtual void removePerson (const Person *aPerson);

    Person *peekWait (int index) const;

    // Returns person at index in approach list.
    Person *peekApproaching (int index) const { return (Person *) m_approachList.getItem (index); };

    // Decrements passed index until one greater to than the next Person in the queue.
    int getNextIndex (int index) const;

    // Returns index of tail Person + 1.
    int getTailIndex (void) const;

    // Returns Person waiting at tail of queue.
    Person *getTail (void) const { return (Person *)m_waitList.getTail(); };

    // Returns TRUE if Person at index is at the tail of the queue; FALSE otherwise.
    int isTail (int index) const;


    // Prepare the Person waiting at the head of the queue for departure.
    virtual void releaseNext (ElapsedTime p_time);

    // Remove current head of queue and inform next Person of advancement.
    virtual void leaveQueue (const Person *aPerson, ElapsedTime p_time);

    // A person arriving at next location notifies next guy and try to continue. 
    // It notifies next Person of advancement and test for continued movement.
    virtual void arriveAtWaitPoint (Person *p_person, ElapsedTime p_time);

    //It notifies Person waiting at index + 1 that they can advance.
    virtual void relayAdvance (Person *p_person, ElapsedTime p_time);

    void generateAdvanceEvent (int p_ndx, ElapsedTime p_time, DistanceUnit p_maxSpeed);

    virtual void balanceQueue (Processor *changedProc, ElapsedTime p_time);

    // It determines aPerson's next pathSegment and waiting position.
    virtual void getQueuePosition (Person *aPerson) const;

    //This function is used to set segment of qCorner list of Person at index.
    //It is called previous to getLocation() to determine direction of queueing.
    virtual int getSegment (Person *aPerson, DistanceUnit inStep) const;

    // Calculates coordinate of Person at index in the queue.
    // Person must have correct segment for its location to be correct.
    virtual Point getLocation (Person *aPerson, DistanceUnit inStep) const;

	virtual MobileElementList* GetWaitList(){ return &m_waitList;}

	ElapsedTime GetMaxWaitTime(); 
	
	virtual void TraceAllPersonInQueue( std::string  _strOtherInfo );
	virtual void ClearAllTempData();
	
};


#endif
