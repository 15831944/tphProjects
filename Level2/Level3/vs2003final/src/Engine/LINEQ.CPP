#include "stdafx.h"
#include "engine\lineq.h"
#include "engine\accessq.h"
#include "engine\person.h"
#include "engine\process.h"
#include "common\states.h"
#include "common\exeption.h"
#include "engine\TracePredef.h"
#include "TerminalMobElementBehavior.h"

//Initializes attributes.
LinearQueue::LinearQueue (const Path& path)
{
    int pointCount = path.getCount();
    Point *pointList = path.getPointList();
    if (pointCount <= 1)
        throw new StringError ("Line Processors must have 2+ service locations");

    m_actualCorners.init (path);

    Path points;
    points.distribute (pointCount, pointList, 80l);
    m_qCorners.init (points);

    m_queuePoints = m_qCorners.getCount();
    m_occupied = new char[m_queuePoints];
    memset ((void *)m_occupied, 0, m_queuePoints);
}


int LinearQueue::hasWait (void) const
{
    Person *aPerson;
    for (int i = 0; i < m_waitList.getCount(); i++)
    {
        aPerson = (Person *)m_waitList.getItem (i);
		TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
        if (spTerminalBehavior&&spTerminalBehavior->isWait())
            return TRUE;
    }
    return FALSE;
}

//The waiting list is searched. 
int LinearQueue::getQueueLength (void) const
{
    Person *aPerson;
    int groupCount = m_waitList.getCount();
    int queueLength = 0;

    for (int i = 0; i < groupCount; i++)
    {
        aPerson = (Person *)m_waitList.getItem(i);
        queueLength += aPerson->GetActiveGroupSize();
    }
    return queueLength;
}

 
int LinearQueue::getGroupCount (void) const
{
    return m_waitList.getCount();
}

int LinearQueue::getTotalGroupCount (void) const
{
    return m_waitList.getCount() + m_approachList.getCount();
}


void LinearQueue::printContents (void) const
{
//    int queueLength = m_waitList.getCount();
//    for (int i = 0; i < queueLength; i++)
//        cout << '\n' << (Person *)m_waitList.getItem (i);
}

//Returns the totoal number of passenger in queue.
int LinearQueue::wipe (ElapsedTime curTime)
{
    int count = getQueueLength();

    Person *aPerson;
    while (m_waitList.getCount())
    {
        aPerson = (Person *)m_waitList.getItem(0);
        m_waitList.removeItem (0);
        aPerson->writeLogEntry (curTime, false);
        aPerson->flushLog (curTime);
//        delete aPerson;
    }
    return count;
}

//It calls its base class' removePerson(). 
void LinearQueue::removePerson (const Person *aPerson)
{
    int index = m_waitList.findItem (aPerson);
    if (index != INT_MAX)
        m_waitList.removeItem (index);
    ProcessorQueue::removePerson (aPerson);
}

// find a person closest to the availPoint, get it out
void LinearQueue::release (Point availPoint, ElapsedTime releaseTime)
{
    static int tester = -1;

    Person *best, *aPerson;
    best = aPerson = (Person *)m_waitList.getItem(0);
	Point locationPt;
	aPerson->getTerminalPoint(locationPt);
    double distance, closest = availPoint.distance (locationPt);

    for (int i = 1; i < m_waitList.getCount(); i++)
    {
        aPerson = (Person *)m_waitList.getItem(i);
		Point waitPt;
		aPerson->getTerminalPoint(waitPt);
        distance = availPoint.distance (waitPt);
        if (distance < closest)
        {
            closest = distance;
            best = aPerson;
        }
    }
//    if (best->getID() == tester)
  //      cout << ' ';

    removePerson (best);
    best->setState (LeaveQueue);
    best->setTerminalDestination(locationPt);
    best->generateEvent (releaseTime,false);
}

 
void LinearQueue::removePerson (Person *aPerson)
{
    static int tester = -1;
//    if (aPerson->getID() == tester)
//        cout << ' ';

    int index = m_waitList.findItem (aPerson);
    if (index != INT_MAX)
        m_waitList.removeItem (index);
}

//simply, the occuupany flag is assigned zero.
void LinearQueue::leaveQueue (const Person *aPerson, ElapsedTime)
{
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	if (spTerminalBehavior)
	{
		m_occupied[spTerminalBehavior->getCurrentIndex()] = 0;
	}
   
}

//Initializes the pax index, destination and occupany flag.
void LinearQueue::addToQueue (Person *aPerson)
{
    static int tester = -1;
//    if (aPerson->getID() == tester)
//        cout << ' ';
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
    m_approachList.addItem (aPerson);
    aPerson->setState (WaitInQueue);
	if (spTerminalBehavior)
	{
		// determine closest available queue position
		Point locationPt;
		aPerson->getTerminalPoint(locationPt);
		spTerminalBehavior->setNextIndex (getClosestIndex (locationPt));
		spTerminalBehavior->updateIndex();
		aPerson->setTerminalDestination(m_qCorners.getPoint (spTerminalBehavior->getCurrentIndex()));
		m_occupied[spTerminalBehavior->getCurrentIndex()] = 1;
	}
    
}

 
void LinearQueue::arriveAtQueue (Person *aPerson, ElapsedTime arriveTime)
{
#ifdef WANT_TO_TRACE_QUUEU_INFO
	if( m_bMustTraceQueueInfo )
	{
		char tempChar[32];
		arriveTime.printTime( tempChar );
		TraceSinglePerson( aPerson, std::string(" Person arrive at queue at ") + tempChar );
		TraceAllPersonInQueue( " arrive at queue. other person info : " );
	}
#endif
	 static int tester = -1;
//	 if (aPerson->getID() == tester)
//		  cout << ' ';

	 int index = m_approachList.findItem (aPerson);
	 m_approachList.removeItem (index);
	 m_waitList.addItem (aPerson);

	 TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	 if (spTerminalBehavior)
	 {
		spTerminalBehavior->wait (TRUE);
	 }
	
}


void LinearQueue::getQueuePosition (Person *aPerson) const
{
	Point locatePt;
	aPerson->getTerminalPoint(locatePt);
    Point dest = m_qCorners.getPoint (getClosestIndex (locatePt));
    aPerson->setTerminalDestination(dest);
}


//Returns index of closest position if found; otherwise, a random number.
int LinearQueue::getClosestIndex (const Point& aPoint) const
{
    double closest = 999999999.9;
    double distance;
    int ndx = -1;

    for (int i = 0; i < m_queuePoints; i++)
    {
        if (m_occupied[i])
            continue;
        distance = aPoint.distance (m_qCorners.getPoint(i));
        if (distance < closest)
        {
            closest = distance;
            ndx = i;
        }
    }
    return (ndx != -1)? ndx: random (m_queuePoints);
}


// Do offset to all position related data,
void LinearQueue::DoOffset(DistanceUnit _xOffset, DistanceUnit _yOffset, DistanceUnit _zOffset)
{
	ProcessorQueue::DoOffset( _xOffset, _yOffset, _zOffset );
	m_actualCorners.DoOffset(  _xOffset, _yOffset, _zOffset );
}

void LinearQueue::Rotate( DistanceUnit _degree )
{
	ProcessorQueue::Rotate( _degree );
	m_actualCorners.Rotate( _degree );
}
void LinearQueue::ClearAllTempData()
{
	ProcessorQueue::ClearAllTempData();
	m_waitList.clear();
	memset ((void *)m_occupied, 0, m_queuePoints);
}