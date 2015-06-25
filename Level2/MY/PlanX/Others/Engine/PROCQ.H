/*****
*
*   Class       ProcessorQueue
*   Author      Lincoln Fellingham, Aviation Research Corporation
*   Date        December 2, 1992
*   Revision    July 14, 1993  - expanded upwards for subclassing non
*                                fixed queues
*                              - improved flexibility of waiting positions
*                              - developed rules for handling overflow
*
*               April 5-6 1994 - expanded upwards for subclassing lineq
*                              - now contains only functions common to both
*
*   Purpose     Member of Processor that handles queue functions
*
*               approachList is a list of all Persons that are currently
*               moving towards the queue
*
*               waitList is the queue of Persons waiting for service
*
*               m_qCorners is the list of Points that define the shape of
*               the queue
*
*****/

#ifndef PROCESSOR_QUEUE_H
#define PROCESSOR_QUEUE_H

#include "mobile.h"
#include <CommonData/ProcessorQueueBase.h>

#include <string>

#define APPROACHING         -1
#define DELAY               0.6
 
/*
Member of Processor that handles queue functions.

The attribute "waitList" of its subclasses is the list of Persons waiting for service.
The waitList is always contiguous and has no bearing to a Person's physical location in the queue.
A Person's index in the waitList actually indicates the number of Persons in front of them before the Person is even aware of any changes.

A Person's "pathSegment" of its subclassed stores the index into the m_qCorners list of the point that they have most recently passed.
If they have not yet made it to the physical tail of the queue (as defined in the Processor's layout) this value will equal OVERFLOW.
*/
class ProcessorQueue : public ProcessorQueueBase
{
protected:

    // A list of all Persons that are currently moving towards the queue.
    MobileElementList m_approachList;

	// flag to decide trace queue info or not, just for debug complex queue logic purpose
	bool m_bMustTraceQueueInfo;

public:
    // Base constructor
    ProcessorQueue() { m_bMustTraceQueueInfo = false ;};

    //Copy constructor : only its parent's copy constructor is invoked.
    ProcessorQueue (const Path& path) : ProcessorQueueBase(path) {};

	ProcessorQueue ( const ProcessorQueue& _anothrProcQueue ); 
	ProcessorQueue& operator = ( const ProcessorQueue& _anothrProcQueue ); 
    virtual ~ProcessorQueue () {};

    //Pure operation. Returns TRUE if some Person is waiting at head of queue for server; FALSE otherwise.
    virtual int hasWait (void) const = 0;

    // Pure member function :  Returns count of groups of Persons currently in the Queue.
    virtual int getQueueLength (void) const = 0;

    // Returns total count of Persons currently in or approaching the Queue.
    virtual int getTotalQueueLength (void) const;

    // Pure member function : Returns count of groups currently in the Queue.
    virtual int getGroupCount (void) const = 0;

    // Pure member function : returns total count of groups currently in or approaching the Queue.
    virtual int getTotalGroupCount (void) const = 0;

    // Pure member function :  prints all Persons in queue from head to tail withou include approaching.
    virtual void printContents (void) const = 0;

    // Prints ID of all Persons approaching the queue.
//    virtual void printApproaching (void) const;

    // Pure member function :  Removes and kills all Persons in queue.
    virtual int wipe (ElapsedTime curTime) = 0;

    // Removes person from all containers.
    virtual void removePerson (const Person *aPerson);
 
    // Pure member function : prepare the Person waiting at the head of the queue for departure.
    virtual void releaseNext (ElapsedTime releaseTime) = 0;

    // Pure member function : remove current head of queue and inform next Person of advancement.
    virtual void leaveQueue (const Person *aPerson, ElapsedTime departTime) = 0;

    // Pure member function : add new Person to list of approaching Persons.
    virtual void addToQueue (Person *aPerson) = 0;

    // Pure member function : add Person to the tail of the actual queue.
    virtual void arriveAtQueue (Person *aPerson, ElapsedTime arriveTime) = 0;

    // Pure member function : get the current location of the tail.
    virtual void getTailPoint (Person *aPerson) const = 0;

    // Person arriving at next location notifies next guy and try to continue.
    // It notifies next Person of advancement and test for continued movement.
    virtual void arriveAtWaitPoint (Person *p_person, ElapsedTime p_time) {};

    // Pure member function : determines aPerson's next pathSegment and waiting position.
    virtual void getQueuePosition (Person *aPerson) const = 0;

    //This method indicates to other member of receiver's group that this processor is available.
    virtual void balanceQueue (Processor *changedProc, ElapsedTime time) {};



	virtual MobileElementList* GetWaitList()=0;
	virtual void ClearAllTempData()=0;

	//
	void SetTraceFlag( bool _bFlag ){ m_bMustTraceQueueInfo = _bFlag;	}
	bool GetTraceFlag()const { return m_bMustTraceQueueInfo;	}

    virtual void TraceSinglePerson( Person* _pPerson, std::string _strOtherInfo );
	virtual void TraceAllPersonInQueue( std::string _strOtherInfo );

};

#endif
