#include "StdAfx.h"
#include "BridgeQueue.h"
#include "TerminalMobElementBehavior.h"
#include "MOVEVENT.H"



BridgeQueue::BridgeQueue(const Point& tailPos,  const Path& path )
	: FixedQueue (path)
	, m_TailPos(tailPos)
{

}

void BridgeQueue::getTailPoint( Person *aPerson ) const
{
	__super::getTailPoint(aPerson);
}

void BridgeQueue::addToQueue( Person *aPerson )
{
#ifdef WANT_TO_TRACE_QUUEU_INFO
	if( m_bMustTraceQueueInfo )
	{
		TraceSinglePerson( aPerson, "Person Add to queue !" );
	}
#endif
	aPerson->setState (WaitInQueue);
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	if (spTerminalBehavior)
	{
		spTerminalBehavior->HasArrivedFixQueue(FALSE) ;
		m_approachList.addItem (aPerson);
		spTerminalBehavior->setNextIndex (APPROACHING);
		spTerminalBehavior->updateIndex();
		spTerminalBehavior->setNextIndex (getNextIndex (APPROACHING));
		spTerminalBehavior->setQueuePathSegmentIndex(ARC_OVERFLOW) ;

		//// determine the next segment and the next wait location

		if (onTailSegment (aPerson))
		{
			Person* aper = getTail() ;
			if(aper == NULL)
				EntryPoints.ResetEntryState() ;
			aPerson->setState(WaitInQueue);
			int index = EntryPoints.GetCurrentIndex() ;
			if(index == -1 || index > (cornerCount() - 1))
				index = cornerCount() - 1 ;
			aPerson->setTerminalDestination(m_TailPos);
			spTerminalBehavior->first = index ;

		}
		else
		{
			Person* aper = getTail() ;
			if(aper == NULL)
				EntryPoints.ResetEntryState() ;
			aPerson->setState(MoveToQueue);

			int index = EntryPoints.GetCurrentIndex() ;
			if(index == -1 || index > (cornerCount() - 1))
				index = cornerCount() - 1 ;
			aPerson->setTerminalDestination(m_TailPos) ;
			spTerminalBehavior->first = index ;
			// if the Person's state is waiting it was heading for the tail
			// of the queue. Since it is no longer heading for the tail it
			// must reset its destination and event
		} 
	}



}



void BridgeQueue::approachQueue( Person *aPerson, ElapsedTime approachTime )
{
	if(TerminalMobElementBehavior *spTerminalBehavior= aPerson->getTerminalBehavior())
		spTerminalBehavior->SetWalkOnBridge(TRUE);

	int index = cornerCount() - 1 ;
	aPerson->setTerminalLocation(corner(index));
	aPerson->setTerminalDestination(corner(index));
	aPerson->writeLog(approachTime,false);

	__super::approachQueue(aPerson,approachTime);
}

Point BridgeQueue::getLocation( Person *aPerson, DistanceUnit inStep ) const
{
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	Point locationPt;
	Point destPt;
	aPerson->getTerminalPoint(locationPt);
	aPerson->getTerminalDest(destPt);

	if (spTerminalBehavior->getNextIndex() == HEAD)
		return corner (HEAD);

	Person *nextPerson;
	nextPerson = peekWait (spTerminalBehavior->getNextIndex()-1);
	Point locationNextPt;
	Point destNextPt;
	nextPerson->getTerminalPoint(locationNextPt);
	nextPerson->getTerminalDest(destNextPt);
	TerminalMobElementBehavior* spNextBehavior = nextPerson->getTerminalBehavior();
	ARCVector3  vector;

	if (spTerminalBehavior->getQueuePathSegmentIndex() == ARC_OVERFLOW)
		vector = ARCPoint3(locationPt) -ARCPoint3(destNextPt);
	else if (spTerminalBehavior->getQueuePathSegmentIndex() == spNextBehavior->getQueuePathSegmentIndex())
		vector = ARCPoint3(corner (spNextBehavior->getQueuePathSegmentIndex())) - ARCPoint3(destNextPt);
	else
		vector = ARCPoint3(corner (spTerminalBehavior->getQueuePathSegmentIndex())) - ARCPoint3(destNextPt);

	vector.SetLength (inStep);
	ARCPoint3 ret = ARCPoint3(destNextPt) + vector;
	return Point(ret.x,ret.y,ret.z);
}

void BridgeQueue::setPersonCurrentPosition(Person *aPerson, const ElapsedTime& moveTime )
{
	aPerson->setCurrentPositionRealZ(moveTime);
}
