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

	Point getLocation (Person *aPerson, DistanceUnit inStep) const;
protected:
	Point m_TailPos;
};

