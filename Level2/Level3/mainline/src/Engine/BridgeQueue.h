#pragma once
#include "fixedq.h"
class BridgeQueue :
	public FixedQueue
{
public:
	BridgeQueue (const Point& tailPos, const Path& path);

	virtual void getTailPoint (Person *aPerson) const;

	virtual void addToQueue(Person *aPerson);

	virtual void approachQueue (Person *aPerson, ElapsedTime approachTime);
	
	virtual void setPersonCurrentPosition(Person *aPerson,const ElapsedTime& arriveTime);

    virtual void arriveAtQueue (Person *aPerson, ElapsedTime arriveTime);

	Point getLocation (Person *aPerson, DistanceUnit inStep) const;

	void updateApproaching (ElapsedTime updateTime);


	// Remove current head of queue and inform next Person of advancement.
	virtual void leaveQueue (const Person *aPerson, ElapsedTime p_time);

	// A person arriving at next location notifies next guy and try to continue. 
	// It notifies next Person of advancement and test for continued movement.
	virtual void arriveAtWaitPoint (Person *p_person, ElapsedTime p_time);

	//It notifies Person waiting at index + 1 that they can advance.
	virtual void relayAdvance (Person *p_person, ElapsedTime p_time);

	// It determines aPerson's next pathSegment and waiting position.
	virtual void getQueuePosition (Person *aPerson) const;

	//This function is used to set segment of qCorner list of Person at index.
	//It is called previous to getLocation() to determine direction of queueing.
	virtual int getSegment (Person *aPerson, DistanceUnit inStep) const;


protected:
	Point m_TailPos;
};

