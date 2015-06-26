#include "StdAfx.h"
#include ".\landsidequeueprocess.h"
#include "PaxLandsideBehavior.h"
#include "PERSON.H"
#include "LandsidePaxSericeInSim.h"
#include "Common/ARCTracker.h"

LandsideQueueBase::LandsideQueueBase(const CPath2008 path)
:m_qCorners(path)
{

}

LandsideQueueBase::~LandsideQueueBase()
{

}

ARCVector3 LandsideQueue::getLocation( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys )
{
	if (pQueueProcess && pQueueProcess->getNextIndex() == QUEUE_HEAD)
		return corner (QUEUE_HEAD);

	PaxLandsideBehavior* pLandsideBehavior = pQueueProcess->GetlandsideBehavior();
	Person* pPerson = pLandsideBehavior->getPerson();
	ASSERT(pPerson);
	double dInStep = pPerson->getInStep();

	ARCVector3 locationPt;
	locationPt = pLandsideBehavior->getPoint();

	PaxLandsideBehavior *pNextBehavior;
	pNextBehavior = PeekWait (pQueueProcess->getNextIndex()-1,pLandsideQueueSys);
	if(!pNextBehavior)
		return corner (QUEUE_HEAD);

	ARCVector3 destPt;
	destPt = pNextBehavior->getDest();
	ARCVector3 vector;

	LandsidePaxQueueProcess* pNextQueueProcess = pLandsideQueueSys->FindPaxQueueProcess(pNextBehavior);
	if (pQueueProcess->getQueuePathSegmentIndex() == QUEUE_OVERFLOW)
		vector = locationPt - destPt;
	else if (pQueueProcess->getQueuePathSegmentIndex() == pNextQueueProcess->getQueuePathSegmentIndex())
		vector = corner (pNextQueueProcess->getQueuePathSegmentIndex()) - destPt;
	else
		vector = corner (pQueueProcess->getQueuePathSegmentIndex()) - destPt;

	vector.SetLength(dInStep);
	return destPt + vector;
}


bool LandsideQueue::isHead( PaxLandsideBehavior* pLandsideBehavior )
{
	if (m_waitList.IsEmpty())
		return false;
	
	return m_waitList.front() == pLandsideBehavior ? true : false;
}

PaxLandsideBehavior* LandsideQueue::GetLandsideBehavior( int idx ) const
{
	return m_waitList.getItem(idx);
}

PaxLandsideBehavior* LandsideQueue::GetHeadBehavior() const
{
	if(!m_waitList.IsEmpty())
		return m_waitList.front();
	return NULL;
}

PaxLandsideBehavior* LandsideQueue::GetTailBehavior() const
{
	if(!m_waitList.IsEmpty())
		return m_waitList.back();
	return NULL;
}

void LandsideQueue::RemoveWait( PaxLandsideBehavior* pLandsideBehavior )
{
	int index = m_waitList.findElement (pLandsideBehavior->GetPersonID());
	if (index != INT_MAX)
	{
		m_waitList.removeItem (index);
	}
}

int LandsideQueue::isTail( int index,LandsideQueueSystemProcess* pLandsideQueueSys)
{
	PaxLandsideBehavior *tail = m_waitList.getTail();
	LandsidePaxQueueProcess* pQueueProcess = pLandsideQueueSys->FindPaxQueueProcess(tail);
	return (pQueueProcess &&pQueueProcess->getCurrentIndex() == index);
}

int LandsideQueue::FindApproach(PaxLandsideBehavior* pLandsideBehavior)
{
	return m_approachList.findElement(pLandsideBehavior->GetPersonID());
}

PaxLandsideBehavior* LandsideQueue::PeekWait( int idx,LandsideQueueSystemProcess* pLandsideQueueSys )
{
	PaxLandsideBehavior *spLandsideBehavior = NULL;
	int queueIndex = m_waitList.getCount()-1;

	while (queueIndex >= 0)
	{
		spLandsideBehavior = m_waitList.getItem (queueIndex);
		if (spLandsideBehavior == NULL)
			return NULL;
		
		LandsidePaxQueueProcess* pLandsideQueueProc = pLandsideQueueSys->FindPaxQueueProcess(spLandsideBehavior);
	
		if (pLandsideQueueProc == NULL)
			return NULL;

		if (pLandsideQueueProc->getNextIndex() < idx)		
			break;
		else if (pLandsideQueueProc->getNextIndex() == idx)		
			return spLandsideBehavior;

		queueIndex--;
	}
	return NULL;
}

PaxLandsideBehavior* LandsideQueue::PeekApproach(int idx)
{
	if(idx >= 0 && idx < m_approachList.getCount())
	{
		return m_approachList.getItem(idx);
	}

	return NULL;
}

int LandsideQueue::getNextIndex( int index,LandsideQueueSystemProcess* pLandsideQueueSys )
{
	if (index == QUEUE_APPROACHING)
		return getTailIndex(pLandsideQueueSys);

	while (index && !PeekWait(index-1,pLandsideQueueSys)) 
		index--;

	return index;
}

int LandsideQueue::getTailIndex( LandsideQueueSystemProcess* pLandsideQueueSys ) 
{
	PaxLandsideBehavior *tail = m_waitList.getTail();
	LandsidePaxQueueProcess *pQueueProc = pLandsideQueueSys->FindPaxQueueProcess(tail);

	return (pQueueProc)? (pQueueProc->getNextIndex() + 1): 0;
}

void LandsideQueue::addToQueue( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time )
{
	pQueueProcess->SetQueueState(LandsidePaxQueueProcess::sp_waitinQueue);
	pQueueProcess->ArrivalQueue(false);
	PaxLandsideBehavior* spLandsideBehavior = pQueueProcess->GetlandsideBehavior();
	if (spLandsideBehavior)
	{
		m_approachList.addItem (spLandsideBehavior);
		pQueueProcess->setNextIndex(QUEUE_APPROACHING);
		pQueueProcess->updateIndex();
		pQueueProcess->setNextIndex (getNextIndex (QUEUE_APPROACHING,pLandsideQueueSys));
		pQueueProcess->setQueuePathSegmentIndex(QUEUE_OVERFLOW) ;

		//// determine the next segment and the next wait location

		if (onTailSegment (pQueueProcess,pLandsideQueueSys))
		{
			pQueueProcess->SetQueueState(LandsidePaxQueueProcess::sp_waitinQueue);
			spLandsideBehavior->setDestination(corner(QUEUE_LAST_SEGMENT));

		}
		else
		{
			pQueueProcess->SetQueueState(LandsidePaxQueueProcess::sp_movetoQueue);
			spLandsideBehavior->setDestination(corner(QUEUE_LAST_SEGMENT)) ;
			// if the Person's state is waiting it was heading for the tail
			// of the queue. Since it is no longer heading for the tail it
			// must reset its destination and event
		} 
	}
}

void LandsideQueue::leaveQueue( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time )
{
	PaxLandsideBehavior *spLandsideBehavior =  m_waitList.removeHead();
	if (!m_waitList.getTail())
	{
		updateApproaching (pLandsideQueueSys,time);
		return;
	}


	// test for a following Person in the queue
	// determine if there is a Person thinking they are at position 1

	PaxLandsideBehavior *spNextBehavior = PeekWait(1,pLandsideQueueSys);
	LandsidePaxQueueProcess* pNextQueueProc = pLandsideQueueSys->FindPaxQueueProcess(spNextBehavior);
	if (pNextQueueProc == NULL)
	{
		return;
	}

	if (pNextQueueProc->getCurrentIndex() != 1)
	{
		return;
	}

	ARCVector3 locationPt = spNextBehavior->getPoint();
	// write log entry in current position to start movement
	pNextQueueProc->SetQueueState(LandsidePaxQueueProcess::sp_startMove);
	spNextBehavior->setDestination(locationPt);
	spNextBehavior->WriteLogEntry(time, false);
	relayAdvance (pNextQueueProc, pLandsideQueueSys,time);
}

void LandsideQueue::relayAdvance( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time )
{
	PaxLandsideBehavior* spLandsideBehavior = pQueueProcess->GetlandsideBehavior();
	if (spLandsideBehavior)
	{
		getQueuePosition (pQueueProcess,pLandsideQueueSys);
		pQueueProcess->SetQueueState(LandsidePaxQueueProcess::sp_waitinQueue);
		pQueueProcess->GenerateQueueEvent(time + spLandsideBehavior->moveTime());
		Person* aPerson = spLandsideBehavior->getPerson();

		if (isTail (pQueueProcess->getCurrentIndex(),pLandsideQueueSys))
			// reset destination of all approaching Persons
			updateApproaching (pLandsideQueueSys,time);
		else
			// schedule event to have next person in queue advance
			generateAdvanceEvent (pQueueProcess->getCurrentIndex() + 1, time + 1l,
			pLandsideQueueSys,aPerson->getSpeed());

	}
}

void LandsideQueue::continueAdvance( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time )
{
	if (pQueueProcess&&pQueueProcess->getCurrentIndex() != QUEUE_HEAD && !PeekWait (pQueueProcess->getCurrentIndex()-1,pLandsideQueueSys))
		relayAdvance (pQueueProcess, pLandsideQueueSys,time);
}

void LandsideQueue::approachQueue( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time )
{
	PaxLandsideBehavior* spLandsideBehavior = pQueueProcess->GetlandsideBehavior();
	if (onTailSegment (pQueueProcess,pLandsideQueueSys) )
	{
		pQueueProcess->SetQueueState(LandsidePaxQueueProcess::sp_waitinQueue);
		pQueueProcess->setNextIndex(getNextIndex (pQueueProcess->getCurrentIndex(),pLandsideQueueSys));
		spLandsideBehavior->setDestination(getLocation (pQueueProcess, pLandsideQueueSys));
	}
	else if (pQueueProcess->getQueuePathSegmentIndex() == QUEUE_OVERFLOW)
		spLandsideBehavior->setDestination(corner (QUEUE_LAST_SEGMENT));
	else // set destination to the next queue corner
	{
		spLandsideBehavior->setDestination(corner (pQueueProcess->getQueuePathSegmentIndex()-1));
	}

	pQueueProcess->GenerateQueueEvent(time+ spLandsideBehavior->moveTime());
}

void LandsideQueue::updateApproaching(LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time )
{
	for (int i = 0; i < m_approachList.getCount(); i++)
	{
		PaxLandsideBehavior* spLandsideBehavior = PeekApproach(i);
		LandsidePaxQueueProcess* pAppraochProcess = pLandsideQueueSys->FindPaxQueueProcess(spLandsideBehavior);
		if (pAppraochProcess&&onTailSegment (pAppraochProcess,pLandsideQueueSys))
		{
			if (pAppraochProcess->HasArrivalQueue())
			{
				pAppraochProcess->SetQueueState(LandsidePaxQueueProcess::sp_waitinQueue);
				setPersonCurrentPosition(spLandsideBehavior->getPerson(),time);//aPerson->setCurrentPosition(updateTime) ;

				pAppraochProcess->setNextIndex (QUEUE_APPROACHING);
				pAppraochProcess->updateIndex();
				getQueuePosition (pAppraochProcess,pLandsideQueueSys);
				ElapsedTime updateTime = time + spLandsideBehavior->moveTime() ;
				pAppraochProcess->GenerateQueueEvent(updateTime);
			}
			else
			{
				pAppraochProcess->SetQueueState(LandsidePaxQueueProcess::sp_movetoQueue);
				setPersonCurrentPosition(spLandsideBehavior->getPerson(),time);//aPerson->setCurrentPosition(updateTime) ;

				pAppraochProcess->setNextIndex (QUEUE_APPROACHING);
				pAppraochProcess->updateIndex();
				getQueuePosition (pAppraochProcess,pLandsideQueueSys);
				spLandsideBehavior->setDestination(corner(QUEUE_LAST_SEGMENT));
				ElapsedTime updateTime = time + spLandsideBehavior->moveTime() ;
				pAppraochProcess->GenerateQueueEvent(updateTime);
			}
		}

		// if the Person's state is waiting it was heading for the tail
		// of the queue. Since it is no longer heading for the tail it
		// must reset its destination and event
		else if (pAppraochProcess->GetQueueState() == LandsidePaxQueueProcess::sp_waitinQueue)
		{
			pAppraochProcess->SetQueueState(LandsidePaxQueueProcess::sp_movetoQueue);
			setPersonCurrentPosition(spLandsideBehavior->getPerson(),time);//aPerson->setCurrentPosition (updateTime);
			approachQueue (pAppraochProcess,pLandsideQueueSys, time);
		}
	}
}

int LandsideQueue::onTailSegment( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys )
{
	LandsidePaxQueueProcess *pTailProcess = pLandsideQueueSys->GetTailQueueProcess();
	PaxLandsideBehavior* spLandsideBehavior = pQueueProcess->GetlandsideBehavior();

	// if there are no Persons waiting, its based purely on segment
	if (pTailProcess == NULL)
		return (pQueueProcess->getQueuePathSegmentIndex() == 1 || (cornerCount() == 1 &&
		pQueueProcess->getQueuePathSegmentIndex() == QUEUE_OVERFLOW));

	PaxLandsideBehavior* spTailBehavior = pTailProcess->GetlandsideBehavior();
	// if the queue is overflowing all Persons are on the tail segment
	if (pTailProcess->getQueuePathSegmentIndex() == QUEUE_OVERFLOW)
		return TRUE;

	Person* aPerson = spLandsideBehavior->getPerson();
	// if aPerson's segment is the same as the tail's, onTail == TRUE
	if (pTailProcess->getQueuePathSegmentIndex() >= pQueueProcess->getQueuePathSegmentIndex())
		return TRUE;

	DistanceUnit inStep = aPerson->getInStep();

	// if aPerson's segment is one greater than the tail's...
	if (pTailProcess->getQueuePathSegmentIndex() + 1 == pQueueProcess->getQueuePathSegmentIndex() ||
		(pTailProcess->getQueuePathSegmentIndex() == QUEUE_LAST_SEGMENT && pQueueProcess->getQueuePathSegmentIndex()
		== QUEUE_OVERFLOW))

		// and the tail's location plus aPerson's inStep would exceed
		// the next corner location, onTail == TRUE
	{
		ARCVector3 pt;
		pt = spTailBehavior->getPoint();
		if (pt.DistanceTo(corner(pTailProcess->getQueuePathSegmentIndex())) <= inStep)
			return TRUE;
	}

	return FALSE;
}

void LandsideQueue::arriveAtQueue( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time )
{
	pQueueProcess->ArrivalQueue(true);
	if (pQueueProcess->GetQueueState() == LandsidePaxQueueProcess::sp_movetoQueue)
	{

		if (pQueueProcess->getQueuePathSegmentIndex() == QUEUE_OVERFLOW)
		{
			pQueueProcess->setQueuePathSegmentIndex(QUEUE_LAST_SEGMENT);
		}
		else if (!onTailSegment (pQueueProcess,pLandsideQueueSys))
			pQueueProcess->decQueuePathSegmentIndex();
		approachQueue (pQueueProcess,pLandsideQueueSys, time);
		return;
	}
	// place the Person in the list of waiting Persons
	int index = FindApproach(pQueueProcess->GetlandsideBehavior());
	if(index != INT_MAX)
		m_approachList.removeItem (index);

	m_waitList.addItem(pQueueProcess->GetlandsideBehavior());
	// set position in queue
	pQueueProcess->updateIndex();
	// update all approaching Persons
	updateApproaching (pLandsideQueueSys,time);
	pQueueProcess->SetWaitTag(true);
	// if next Person has already advanced, continue moving
	continueAdvance (pQueueProcess, pLandsideQueueSys,time);
}

ARCVector3 calcMidPoint (const ARCVector3& ptStart, const ARCVector3& ptEnd,DistanceUnit speed,
	const ElapsedTime& remainingTravelTime)
{
	ARCVector3 vector (ptStart, ptEnd);

	if (!vector.Length())
		return ptStart;
	double distance = (remainingTravelTime.getPrecisely()*1.0/TimePrecision) * speed;
	vector.SetLength(distance);
	return (ptEnd - vector);
}

void LandsideQueue::setPersonCurrentPosition( Person *aPerson,const ElapsedTime& arriveTime )
{
	MobileElementMovementEvent dummy;
	ElapsedTime destTime = dummy.removeMoveEvent (aPerson);
	PaxLandsideBehavior* spLandsideBehavior = (PaxLandsideBehavior* )aPerson->getLandsideBehavior();

	if (destTime >= ElapsedTime(0l))
	{
		ARCVector3 ptMid = calcMidPoint(spLandsideBehavior->getPoint(),spLandsideBehavior->getDest(),aPerson->getSpeed(),destTime - arriveTime);
		spLandsideBehavior->setDestination (ptMid);
	}
	spLandsideBehavior->WriteLogEntry(arriveTime);
}

void LandsideQueue::getQueuePosition( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys )
{
	// determine next queue position
	PaxLandsideBehavior* spLandsideBehavior = pQueueProcess->GetlandsideBehavior();
	pQueueProcess->setNextIndex (getNextIndex (pQueueProcess->getCurrentIndex(),pLandsideQueueSys));

	// determine the next segment and the next wait location
	Person* aPerson = spLandsideBehavior->getPerson();
	DistanceUnit inStep = aPerson->getInStep();
	pQueueProcess->setQueuePathSegmentIndex (getSegment (pQueueProcess,pLandsideQueueSys));
	spLandsideBehavior->setDestination(getLocation (pQueueProcess, pLandsideQueueSys));
}

int LandsideQueue::getSegment( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys ) 
{
	PaxLandsideBehavior* spLandsideBehavior = pQueueProcess->GetlandsideBehavior();
	if (spLandsideBehavior == NULL)
	{
		return QUEUE_OVERFLOW;
	}

	if (pQueueProcess->getNextIndex() == QUEUE_HEAD)
	{
		int newSeg = m_qCorners.getCount();
		return (newSeg == 1)? QUEUE_OVERFLOW: 1;
	}

	// always called after getNextIndex, therefore there must be a pax
	// at nextIndex + 1
	PaxLandsideBehavior *spNextBehavior = PeekWait (pQueueProcess->getNextIndex()-1,pLandsideQueueSys);
	if (spNextBehavior == NULL)
	{
		return QUEUE_OVERFLOW;
	}
	LandsidePaxQueueProcess* pNexQueueProc = pLandsideQueueSys->FindPaxQueueProcess(spNextBehavior);


	int segment = pNexQueueProc->getQueuePathSegmentIndex();

	if (segment == QUEUE_OVERFLOW)
		return QUEUE_OVERFLOW;

	Person* aPerson = spLandsideBehavior->getPerson();
	DistanceUnit inStep = aPerson->getInStep();
	ARCVector3 destPt;
	destPt = spNextBehavior->getDest();
	if (destPt.DistanceTo(corner (segment)) > inStep)
		return segment;

	// the two remaining options occur when the people are not on the
	// same segment of the queue
	if (segment == QUEUE_LAST_SEGMENT)
		return QUEUE_OVERFLOW;
	else
		return segment + 1;
}

void LandsideQueue::generateAdvanceEvent( int p_ndx, ElapsedTime p_time,LandsideQueueSystemProcess* pLandsideQueueSys,DistanceUnit p_maxSpeed )
{
	PaxLandsideBehavior *spNextBehavior = PeekWait(p_ndx,pLandsideQueueSys);
	LandsidePaxQueueProcess* spNextQueueProc = pLandsideQueueSys->FindPaxQueueProcess(spNextBehavior);
	if (!spNextQueueProc || spNextQueueProc->getCurrentIndex() != p_ndx)
	{
		return;
	}

	Person* aPerson = spNextBehavior->getPerson();
	if (p_maxSpeed)
	{
		aPerson->setSpeed(MIN(p_maxSpeed,spNextQueueProc->GetNormalSpeed()));
	}
	
	spNextQueueProc->SetQueueState(LandsidePaxQueueProcess::sp_startMove);
	spNextQueueProc->GenerateQueueEvent(p_time);
}

void LandsideQueue::arriveAtWaitPoint( LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time )
{
	if (pQueueProcess)
	{
		pQueueProcess->updateIndex();
	}

	// if next Person has already advanced, continue moving
	continueAdvance (pQueueProcess,pLandsideQueueSys, time);
}

int LandsideQueue::hasWait( LandsideQueueSystemProcess* pLandsideQueueSys )
{
	if (!m_waitList.getCount())
		return FALSE;

	PaxLandsideBehavior *spLandsideBehavior = m_waitList.getHead();
	LandsidePaxQueueProcess* pQueueProcess = pLandsideQueueSys->FindPaxQueueProcess(spLandsideBehavior);
	return (pQueueProcess&&pQueueProcess->GetWaitTag() && (pQueueProcess->getCurrentIndex() == QUEUE_HEAD));
}

void LandsideQueue::releaseNext( ElapsedTime p_time, LandsideQueueSystemProcess* pLandsideQueueSys )
{
	 PaxLandsideBehavior *spLandsideBehavior = m_waitList.getHead();
	 LandsidePaxQueueProcess* pQueueProcess =  pLandsideQueueSys->FindPaxQueueProcess(spLandsideBehavior);

    if (pQueueProcess&&pQueueProcess->getCurrentIndex() == QUEUE_HEAD)
    {		
		pQueueProcess->SetQueueState(LandsidePaxQueueProcess::sp_leaveQueue);
		pQueueProcess->GenerateQueueEvent(p_time + ElapsedTime(0.6));
    }
}


LandsideQueueEvent::LandsideQueueEvent( LandsidePaxQueueProcess *pQueueProc )
:m_pQueueProcess(pQueueProc)
{

}

LandsideQueueEvent::~LandsideQueueEvent( void )
{

}

int LandsideQueueEvent::process( CARCportEngine* pEngine )
{
	if (m_pQueueProcess && m_pLandsideQueueSys->ExsitPaxQueueProcess(m_pQueueProcess))
	{
		m_pQueueProcess->ProcessMove(time);
	}

	return 1;
}

int LandsideQueueEvent::kill( void )
{
	return 1;
}

const char * LandsideQueueEvent::getTypeName( void ) const
{
	return _T("QueueProcessEvent");
}

void LandsideQueueEvent::SetLandsideQueueSystem( LandsideQueueSystemProcess* pLandsideQueueSys )
{
	m_pLandsideQueueSys = pLandsideQueueSys;
}


LandsidePaxQueueProcess::LandsidePaxQueueProcess(PaxLandsideBehavior* pLandsideBehavior)
:m_queueState(sp_movetoQueue)
,m_nSegment(QUEUE_OVERFLOW)
,m_pLandsideBehavior(pLandsideBehavior)
{
	m_nCurrentIndex = 0;
	m_nNextIndex = 0;
	m_bHasWaitTag = false;
	m_bArrivalAtQueue = false;
	m_dSpeed = pLandsideBehavior->getPerson()->getSpeed();
}

LandsidePaxQueueProcess::~LandsidePaxQueueProcess()
{

}

void LandsidePaxQueueProcess::WriteQueuePaxLog( const ElapsedTime& p_time )
{
	//ARCVector3 ptLocation = m_pLandsideBehavior->getPoint();
	//ARCVector3 ptDest = m_pLandsideBehavior->getDest();

	//double dLength = ptLocation.DistanceTo(ptDest);
	//double dInStep = m_pLandsideBehavior->getPerson()->getInStep();
	//if (dLength < dInStep)
	//{
	//	return;
	//}

	m_pLandsideBehavior->WriteLogEntry(p_time);
}

void LandsidePaxQueueProcess::ProcessMove(ElapsedTime p_time )
{
	WriteQueuePaxLog(p_time);
	LandsideQueue* pQueue = m_pLandsideQueueSys->GetlandsideQueue();
	ASSERT(pQueue);

	if (m_queueState == sp_leaveQueue)
	{
		leaveQueue(p_time);
		m_bHasWaitTag = false;
		return;
	}
	// must first check for relayAdvance event
	if (m_queueState == sp_startMove)
		moveThroughQueue (p_time);
	// m_nWaiting flag will be set to TRUE when pax arrives at queue tail
	// allows distinction between arrive at tail and movement within queue
	else if (!m_bHasWaitTag)
		arriveAtQueue ( p_time );
	else
		notifyQueueAdvance ( p_time );
	
}

void LandsidePaxQueueProcess::Initialize(int state,LandsideQueueSystemProcess* pQueueProcess)
{
	m_endState = state;
	m_pLandsideQueueSys = pQueueProcess;
}


PaxLandsideBehavior* LandsidePaxQueueProcess::GetlandsideBehavior()
{
	return m_pLandsideBehavior;
}

void LandsidePaxQueueProcess::StartMove( ElapsedTime p_time )
{
	WriteQueuePaxLog(p_time);
	LandsideQueue* pQueue = m_pLandsideQueueSys->GetlandsideQueue();
	pQueue->addToQueue(this,m_pLandsideQueueSys,p_time);

	GenerateQueueEvent(p_time + m_pLandsideBehavior->moveTime());

}

void LandsidePaxQueueProcess::GenerateQueueEvent( const ElapsedTime& time )
{
	LandsideQueueEvent* pQueueEvnet = new LandsideQueueEvent(this);
	pQueueEvnet->removeMoveEvent (m_pLandsideBehavior->getPerson());
	pQueueEvnet->init(m_pLandsideBehavior->getPerson(),time,false);
	pQueueEvnet->SetLandsideQueueSystem(m_pLandsideQueueSys);
	pQueueEvnet->addEvent();
	m_pLandsideBehavior->getPerson()->SetPrevEventTime(time);
}

void LandsidePaxQueueProcess::moveThroughQueue( const ElapsedTime& time )
{
	LandsideQueue* pQueue = m_pLandsideQueueSys->GetlandsideQueue();
	if (pQueue)
	{
		pQueue->relayAdvance(this,m_pLandsideQueueSys,time);
	}
}

void LandsidePaxQueueProcess::arriveAtQueue( const ElapsedTime& time )
{
	LandsideQueue* pQueue = m_pLandsideQueueSys->GetlandsideQueue();
	if (pQueue)
	{
		int prevState = m_queueState;
		pQueue->arriveAtQueue (this,m_pLandsideQueueSys,time);
		if (prevState == sp_waitinQueue)
		{
			processPerson ( time );
		}
	}
}
	

void LandsidePaxQueueProcess::notifyQueueAdvance( const ElapsedTime& time )
{
	LandsideQueue* pQueue = m_pLandsideQueueSys->GetlandsideQueue();
	if (pQueue)
	{
		pQueue->arriveAtWaitPoint (this,m_pLandsideQueueSys, time);
		processPerson (time );
	}
}

void LandsidePaxQueueProcess::leaveQueue( const ElapsedTime& time )
{
	LandsideQueue* pQueue = m_pLandsideQueueSys->GetlandsideQueue();
	if (pQueue)
	{
	//	pQueue->leaveQueue(this,m_pLandsideQueueSys,time);
		m_pLandsideQueueSys->LeaveQueueProcess(m_pLandsideBehavior,m_endState,time);
	}
}

void LandsidePaxQueueProcess::processPerson( const ElapsedTime& time )
{
	   if (m_nCurrentIndex == QUEUE_HEAD)
	   {
		   m_queueState = sp_leaveQueue;
		   GenerateQueueEvent(time + ElapsedTime(0.6));
	   }
}

void LandsidePaxQueueProcess::setNextIndex( int nIndex )
{
	m_nNextIndex = nIndex;
}

LandsideQueueSystemProcess::LandsideQueueSystemProcess(LandsidePaxSericeInSim* pPaxServiceInsim)
{
	m_pQueue = new LandsideQueue(pPaxServiceInsim->GetQueue());
	m_pPaxServiceInSim = pPaxServiceInsim;
}

LandsideQueueSystemProcess::~LandsideQueueSystemProcess( void )
{
	Clear();
}

void LandsideQueueSystemProcess::EnterQueueProcess( PaxLandsideBehavior* pLandsideBehavior,int state,const ElapsedTime& p_time )
{
	LandsidePaxQueueProcess* pPaxQueue = new LandsidePaxQueueProcess(pLandsideBehavior);
	pPaxQueue->Initialize(state,this);
	m_vPaxQueueProc.push_back(pPaxQueue);
	pPaxQueue->StartMove(p_time);
}

void LandsideQueueSystemProcess::LeaveQueueProcess( PaxLandsideBehavior* pLandsideBehavior,int state,const ElapsedTime& p_time )
{
 	pLandsideBehavior->setState(state);
 	pLandsideBehavior->GenerateEvent(p_time);
}

LandsidePaxQueueProcess* LandsideQueueSystemProcess::FindPaxQueueProcess( PaxLandsideBehavior* pLandsideBehavior )
{
	for (size_t i = 0; i < m_vPaxQueueProc.size(); i++)
	{
		LandsidePaxQueueProcess* pQueueProc = m_vPaxQueueProc.at(i);
		if (pLandsideBehavior == pQueueProc->GetlandsideBehavior())
		{
			return pQueueProc;
		}
	}
	return NULL;
}


bool LandsideQueueSystemProcess::ExsitPaxQueueProcess( LandsidePaxQueueProcess* pQueueProcess )
{
	for (size_t i = 0; i < m_vPaxQueueProc.size(); i++)
	{
		LandsidePaxQueueProcess* pQueueProc = m_vPaxQueueProc.at(i);
		if (pQueueProc == pQueueProcess)
		{
			return true;
		}
	}
	return false;
}

LandsideQueue* LandsideQueueSystemProcess::GetlandsideQueue() const
{
	return m_pQueue;
}

void LandsideQueueSystemProcess::Release( PaxLandsideBehavior* pLandsideBehavior )
{
	LandsidePaxQueueProcess* pQueueProc = FindPaxQueueProcess(pLandsideBehavior);
	if (pQueueProc)
	{
		std::vector<LandsidePaxQueueProcess*>::iterator iter = std::find(m_vPaxQueueProc.begin(),m_vPaxQueueProc.end(),pQueueProc);
		if (iter != m_vPaxQueueProc.end())
		{
			m_vPaxQueueProc.erase(iter);
			delete pQueueProc;
		}
	}
}

void LandsideQueueSystemProcess::Clear()
{
	if (m_pQueue)
	{
		delete m_pQueue;
		m_pQueue = NULL;
	}

	for (UINT i = 0; i < m_vPaxQueueProc.size(); i++)
	{
		delete m_vPaxQueueProc[i];
	}
	m_vPaxQueueProc.clear();
}

void LandsideQueueSystemProcess::LeaveQueue( const ElapsedTime& p_time )
{
	PaxLandsideBehavior* pLandsideBehavior = m_pQueue->GetHeadBehavior();
	if (pLandsideBehavior)
	{
		LandsidePaxQueueProcess* pPaxQueueProc = FindPaxQueueProcess(pLandsideBehavior);
		if (pPaxQueueProc)
		{
		//	m_pQueue->RemoveWait(pLandsideBehavior);
			m_pQueue->leaveQueue(pPaxQueueProc,this,p_time+ElapsedTime(0.6));
			Release(pLandsideBehavior);
		//	InvokeWaitQueue(p_time);
		}
	}
}

void LandsideQueueSystemProcess::FlushOnVehiclePaxLog( CARCportEngine* pEngine,const ElapsedTime& t )
{
	
	for (UINT i = 0; i < m_vPaxQueueProc.size(); i++)
	{
		LandsidePaxQueueProcess* pPaxQueueProc = m_vPaxQueueProc[i];
		if (pPaxQueueProc == NULL)
			continue;
		
		PaxLandsideBehavior* pLandsideBehavior = pPaxQueueProc->GetlandsideBehavior();
		if (pLandsideBehavior == NULL)
			continue;
		
		Person* pPerson = pLandsideBehavior->getPerson();
		if (pPerson == NULL)
			continue;
		
		if (pPerson->getState() != Death)
		{
			pPerson->flushLog(t,false);
		}
	}
}

void LandsideQueueSystemProcess::InvokeWaitQueue( const ElapsedTime& time )
{
	m_pQueue->releaseNext(time,this);
}

LandsidePaxQueueProcess* LandsideQueueSystemProcess::GetTailQueueProcess()
{
	PaxLandsideBehavior* spLandsideBehavior = m_pQueue->GetTailBehavior();
	return FindPaxQueueProcess(spLandsideBehavior);
}




LandsideQueueWakeUpEvent::LandsideQueueWakeUpEvent( LandsideQueueSystemProcess* pQueueProcessSys,ElapsedTime eTime )
:m_queueProcessSys(pQueueProcessSys)
{
	setTime(eTime);
}

LandsideQueueWakeUpEvent::~LandsideQueueWakeUpEvent()
{

}

int LandsideQueueWakeUpEvent::process( CARCportEngine* pEngine )
{
	if (m_queueProcessSys)
	{
		m_queueProcessSys->InvokeWaitQueue(time);
	}
	return 1;
}

int LandsideQueueWakeUpEvent::kill( void )
{
	return 1;
}

const char * LandsideQueueWakeUpEvent::getTypeName( void ) const
{
	return _T("QueueWakeupEvent");
}

int LandsideQueueWakeUpEvent::getEventType( void ) const
{
	return 2;
}
