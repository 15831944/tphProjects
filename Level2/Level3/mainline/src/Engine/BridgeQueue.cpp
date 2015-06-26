#include "StdAfx.h"
#include "BridgeQueue.h"
#include "TerminalMobElementBehavior.h"
#include "MOVEVENT.H"


void TraceFuncCall(const CString& fun, int nLine, const Person* p)
{	
	if(p && p->getID() == 635)
	{
		TRACE(_T("%d:"), Event::getCurEventNum());
		TRACE(_T("%s %d"), fun, nLine);
		Point pt;
		p->getTerminalPoint(pt);
		TRACE(_T("(%f,%f,%f)  %d"),pt.x, pt.y, pt.z, p->getTerminalBehavior()->getQueuePathSegmentIndex() );
		
		TRACE(_T("\n"));
	}
}
//#define TRACECALL(p) TraceFuncCall(__FUNCTION__, __LINE__,p);
#define TRACECALL(p)

BridgeQueue::BridgeQueue(const Point& tailPos,  const Path& path )
	: FixedQueue (path)
	, m_TailPos(tailPos)
{

}

void BridgeQueue::getTailPoint( Person *aPerson ) const
{
	TRACECALL(aPerson);
	aPerson->setTerminalDestination(m_TailPos);
	TRACECALL(aPerson);
	//__super::getTailPoint(aPerson);
}

void BridgeQueue::addToQueue( Person *aPerson )
{

#ifdef WANT_TO_TRACE_QUUEU_INFO
	if( m_bMustTraceQueueInfo )
	{
		TraceSinglePerson( aPerson, "Person Add to queue !" );
	}
#endif
	
	//aPerson->setState (WaitInQueue);
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	if (spTerminalBehavior)
	{
		spTerminalBehavior->EnterFromTerminalModeToOtherMode();
		spTerminalBehavior->HasArrivedFixQueue(FALSE) ;
		ASSERT(spTerminalBehavior->IsWalkOnBridge()==FALSE);

		m_approachList.addItem (aPerson);
		spTerminalBehavior->setNextIndex (APPROACHING);
		spTerminalBehavior->updateIndex();
		spTerminalBehavior->setNextIndex (getNextIndex (APPROACHING));
		spTerminalBehavior->setQueuePathSegmentIndex(ARC_OVERFLOW) ;

		//// determine the next segment and the next wait location
		if (onTailSegment (aPerson))
		{
			TRACECALL(aPerson);
			Person* aper = getTail() ;
			if(aper == NULL)
				EntryPoints.ResetEntryState() ;
			aPerson->setState(WaitInQueue);
			int index = cornerCount() - 1 ;
			aPerson->setTerminalDestination(m_TailPos);
			spTerminalBehavior->first = index ;

		}
		else
		{
			TRACECALL(aPerson);
			Person* aper = getTail() ;
			if(aper == NULL)
				EntryPoints.ResetEntryState() ;
			aPerson->setState(MoveToQueue);
			int index =  cornerCount() - 1 ;
			aPerson->setTerminalDestination(m_TailPos) ;
			spTerminalBehavior->first = index ;
			// if the Person's state is waiting it was heading for the tail
			// of the queue. Since it is no longer heading for the tail it
			// must reset its destination and event
		} 
	}
	TRACECALL(aPerson);
}



void BridgeQueue::approachQueue( Person *aPerson, ElapsedTime approachTime )
{
	TRACECALL(aPerson);
	// if Person is now on tail segment, set state, index, & destination
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	if (onTailSegment (aPerson) )
	{
		aPerson->setState (WaitInQueue);
		spTerminalBehavior->setNextIndex(getNextIndex (spTerminalBehavior->getCurrentIndex()));
		aPerson->setTerminalDestination(getLocation (aPerson, aPerson->getInStep()));
		//spTerminalBehavior->SetEntryPoint(getLocation (aPerson, aPerson->getInStep())) ;
		//Point destPt;
		//aPerson->getTerminalDest(destPt);
		//int index = cornerCount()-1;//EntryPoints.GetCurrentIndex() ;
		//if(index == -1)
		//	index = LAST_SEGMENT ;	
		//else
		//{
		//	ASSERT(spTerminalBehavior->IsWalkOnBridge());
		//	CString pronmae = spTerminalBehavior->getProcessor()->getIDName() ;
		//	if(destPt.distance(corner(index)) <= aPerson->getInStep() && spTerminalBehavior->getQueuePathSegmentIndex() >= index)
		//	{
		//		EntryPoints.NextEntryPoint();
		//	}
		//}

	}
	else if (spTerminalBehavior->getQueuePathSegmentIndex() == ARC_OVERFLOW)
	{
		ASSERT(!spTerminalBehavior->IsWalkOnBridge());
		aPerson->setTerminalDestination (m_TailPos);
	}
	else // set destination to the next queue corner
	{
		ASSERT(spTerminalBehavior->IsWalkOnBridge());
		ASSERT(spTerminalBehavior->HasArrivedFixQueue());
		aPerson->setTerminalDestination (corner (spTerminalBehavior->getQueuePathSegmentIndex()-1));
		//spTerminalBehavior->SetEntryPoint(corner (spTerminalBehavior->getQueuePathSegmentIndex()-1)) ;
	}
	aPerson->generateEvent (approachTime + aPerson->moveTime(),false);
	TRACECALL(aPerson);
}

Point BridgeQueue::getLocation( Person *aPerson, DistanceUnit inStep ) const
{
	TRACECALL(aPerson);
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
	{
		//ASSERT(spTerminalBehavior->IsWalkOnBridge()==FALSE);
		vector = ARCPoint3(locationPt) -ARCPoint3(destNextPt);
	}
	else if (spTerminalBehavior->getQueuePathSegmentIndex() == spNextBehavior->getQueuePathSegmentIndex())
	{
		ASSERT(spTerminalBehavior->IsWalkOnBridge());
		vector = ARCPoint3(corner (spNextBehavior->getQueuePathSegmentIndex())) - ARCPoint3(destNextPt);
	}
	else
	{
		ASSERT(spTerminalBehavior->IsWalkOnBridge());
		vector = ARCPoint3(corner (spTerminalBehavior->getQueuePathSegmentIndex())) - ARCPoint3(destNextPt);
	}
	vector.SetLength (inStep);
	ARCPoint3 ret = ARCPoint3(destNextPt) + vector;
	return Point(ret.x,ret.y,ret.z);
}

void BridgeQueue::setPersonCurrentPosition(Person *aPerson, const ElapsedTime& moveTime )
{
	TRACECALL(aPerson);
	aPerson->setCurrentPositionRealZ(moveTime);
	TRACECALL(aPerson);
}

void BridgeQueue::arriveAtQueue( Person *aPerson, ElapsedTime arriveTime )
{
	TRACECALL(aPerson);
	TerminalMobElementBehavior* pb = aPerson->getTerminalBehavior();
	if(!pb->HasArrivedFixQueue())
	{
		//from terminal z to airside z
		double realz = corner(cornerCount()-1).getZ();
		aPerson->writeLogEntry(arriveTime,false);
		aPerson->SetWalkOnBridge(true);

		Point ptDest;
		aPerson->getTerminalDest(ptDest);
		ptDest.z = realz;
		aPerson->setTerminalDestination(ptDest);
		aPerson->writeLogEntry(arriveTime,false);
	}   
	
    FixedQueue::arriveAtQueue(aPerson, arriveTime);
	TRACECALL(aPerson);
}

void BridgeQueue::updateApproaching(ElapsedTime updateTime)
{
	
	Person *aPerson;
	for (int i = 0; i < m_approachList.getCount(); i++)
	{
		aPerson = peekApproaching (i);
		TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)aPerson->getBehavior(MobElementBehavior::TerminalBehavior);
		TRACECALL(aPerson);
		if (spTerminalBehavior&&onTailSegment (aPerson))
		{
			if(spTerminalBehavior->HasArrivedFixQueue())
			{
				ASSERT(spTerminalBehavior->IsWalkOnBridge());
				
				aPerson->setState (MoveToQueue);
				setPersonCurrentPosition(aPerson,updateTime);//aPerson->setCurrentPosition(updateTime) ;

				spTerminalBehavior->setNextIndex (APPROACHING);
				spTerminalBehavior->updateIndex();
				getQueuePosition (aPerson);
				ElapsedTime time = updateTime + aPerson->moveTime() ;
				aPerson->generateEvent (time,false);
			}
			else
			{
				//if(EntryPoints.GetCurrentIndex() != -1 )
					spTerminalBehavior->first = cornerCount()-1;//EntryPoints.GetCurrentIndex() ;
				aPerson->setCurrentPosition(updateTime);

				aPerson->setState (MoveToQueue);
				spTerminalBehavior->setNextIndex (APPROACHING);
				spTerminalBehavior->updateIndex();
				//getQueuePosition (aPerson);
				aPerson->setTerminalDestination( m_TailPos ) ;
				ElapsedTime time = updateTime + aPerson->moveTime() ;
				aPerson->generateEvent (time,false);
			}

		}

		// if the Person's state is waiting it was heading for the tail
		// of the queue. Since it is no longer heading for the tail it
		// must reset its destination and event
		else if (aPerson->getState() == WaitInQueue)
		{
			aPerson->setState (MoveToQueue);
			setPersonCurrentPosition(aPerson,updateTime);//aPerson->setCurrentPosition (updateTime);
			approachQueue (aPerson, updateTime);
		}
	}
}



void BridgeQueue::leaveQueue(const Person *aPerson, ElapsedTime p_time)
{
	TRACECALL(aPerson);
	__super::leaveQueue(aPerson,p_time);
	TRACECALL(aPerson);
}

void BridgeQueue::arriveAtWaitPoint(Person *p_person, ElapsedTime p_time)
{
	TRACECALL(p_person);
	__super::arriveAtWaitPoint(p_person, p_time);
	TRACECALL(p_person);
}

void BridgeQueue::relayAdvance(Person *p_person, ElapsedTime p_time)
{
	TRACECALL(p_person);
	__super::relayAdvance(p_person, p_time);
	TRACECALL(p_person);
}



void BridgeQueue::getQueuePosition(Person *aPerson) const
{
	TRACECALL(aPerson);
	__super::getQueuePosition(aPerson);
	TRACECALL(aPerson);
}

int BridgeQueue::getSegment(Person *aPerson, DistanceUnit inStep) const
{
	TRACECALL(aPerson);
	int nRet=  __super::getSegment(aPerson,inStep);
	return nRet;
}
