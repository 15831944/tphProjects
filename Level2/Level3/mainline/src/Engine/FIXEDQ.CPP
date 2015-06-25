#include "stdafx.h"
#include "engine\accessq.h"
#include "engine\procq.h"
#include "engine\fixedq.h"
#include "engine\person.h"
#include "common\states.h"
#include "engine\TracePredef.h"
#include "engine\movevent.h"
#include "TerminalMobElementBehavior.h"
//int FixedQueue::getNumOftwoPathSegment(int index,Person *aPerson)
//{
// return (int)(corner(countIndex).distance(corner(countIndex+1))/aPerson->getInStep()) ;
//}
//DOUBLE FixedQueue::GetDistanceBetweenPersonToHead(Person* person)
//{
//	if(person == NULL )
//		return 0 ;
//	DOUBLE distance = 0 ;
//	int cornIndex = countIndex ;
//	 Person* per  = person;
//  if(per != NULL)
//  {
//		distance = distance + per->getDest().distance(corner(per->getQueuePathSegmentIndex()-1 )) ;
//		distance = distance + GetDistanceFormCurrentSegmentToHead(per->getQueuePathSegmentIndex() - 1) ;
//	
//  }
//	return distance ;
//}
//DOUBLE FixedQueue::GetDistanceFormCurrentSegmentToHead(int cur_index)
//{
//	int index = cur_index ;
//	DOUBLE distance = 0 ;
//   for(;index > 0 ; index--)
//   {
//	   distance = distance + corner(index).distance(corner(index - 1)) ;
//   }
//   return distance ;
//}
//double FixedQueue::GetDistanceFromEntryPointToPathHead( CEntryPoint* point)
//{
//    return m_qCorners.GetRealDist(point->GetDest()) ; 
//}

void FixedQueue::addToQueue (Person *aPerson)
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
			aPerson->setTerminalDestination(corner(index));
			spTerminalBehavior->first_corner = index ;
			
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
				aPerson->setTerminalDestination(corner(index)) ;
				spTerminalBehavior->first_corner = index ;
				// if the Person's state is waiting it was heading for the tail
				// of the queue. Since it is no longer heading for the tail it
				// must reset its destination and event
		} 
	}
	



}

int FixedQueue::getSegment (Person *aPerson, DistanceUnit inStep) const
{
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return ARC_OVERFLOW;
	}

	if (spTerminalBehavior->getNextIndex() == HEAD)
	{
		int newSeg = m_qCorners.getCount();
		return (newSeg == 1)? ARC_OVERFLOW: 1;
	}

	// always called after getNextIndex, therefore there must be a pax
	// at nextIndex + 1
	Person *nextPerson = peekWait (spTerminalBehavior->getNextIndex()-1);

	TerminalMobElementBehavior* spNextBehavior = nextPerson->getTerminalBehavior();
	if (spNextBehavior == NULL)
	{
		return ARC_OVERFLOW;
	}
	int segment = spNextBehavior->getQueuePathSegmentIndex();

	if (segment == ARC_OVERFLOW)
		return ARC_OVERFLOW;

	Point destPt;
	nextPerson->getTerminalDest(destPt);
	if (destPt.distance (corner (segment)) > inStep)
		return segment;

	// the two remaining options occur when the people are not on the
	// same segment of the queue
	if (segment == LAST_SEGMENT)
		return ARC_OVERFLOW;
	else
		return segment + 1;
}


Point FixedQueue::getLocation (Person *aPerson, DistanceUnit inStep) const
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
	Point vector;
	if (spTerminalBehavior->getQueuePathSegmentIndex() == ARC_OVERFLOW)
		vector = locationPt - destNextPt;
	else if (spTerminalBehavior->getQueuePathSegmentIndex() == spNextBehavior->getQueuePathSegmentIndex())
		vector = corner (spNextBehavior->getQueuePathSegmentIndex()) - destNextPt;
	else
		vector = corner (spTerminalBehavior->getQueuePathSegmentIndex()) - destNextPt;

	vector.length (inStep);
	return destNextPt + vector;
}
// If Person has only arrived at a corner of the path, call approachQueue to determine next destination and return.
// Otherwise, it removes the Person from m_approachList, adds him to waitList, set pathIndex, resets destination of all other approaching passengers.
// If next Person in queue has advanced, continue movement.
void FixedQueue::arriveAtQueue (Person *aPerson, ElapsedTime arriveTime)
{
#ifdef WANT_TO_TRACE_QUUEU_INFO
	char tempChar[32];
		arriveTime.printTime( tempChar );
		TraceSinglePerson( aPerson, std::string(" Person arrive at queue at ") + tempChar );
		TraceAllPersonInQueue( " arrive at queue. other person info : " );
#endif
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	spTerminalBehavior->HasArrivedFixQueue(TRUE) ;
    if (aPerson->getState() == MoveToQueue)
    {

        if (spTerminalBehavior->getQueuePathSegmentIndex() == ARC_OVERFLOW)
		{
			spTerminalBehavior->setQueuePathSegmentIndex(spTerminalBehavior->first_corner);
			Person* aper = getTail() ;
			if(aper != NULL )
			{
				Point pt;
				aper->getTerminalPoint(pt);
				if(pt.distance(corner(spTerminalBehavior->first_corner)) <= aPerson->getInStep())
				{
					int index = EntryPoints.GetCurrentIndex() ;
					if(index == -1 )
						index = cornerCount() - 1 ;
					if(spTerminalBehavior->first_corner < index)
					{
						setPersonCurrentPosition(aPerson,arriveTime);//aPerson->setCurrentPosition (arriveTime);
						spTerminalBehavior->setQueuePathSegmentIndex(index + 1);
						aPerson->setTerminalDestination(corner(index )) ;
						spTerminalBehavior->SetEntryPoint(corner(index )) ;
						if(index < (cornerCount() - 1))
						   spTerminalBehavior->first_corner = index + 1;
						aPerson->generateEvent(arriveTime+aPerson->moveTime() ,FALSE) ;
						return ;
						// aPerson->setState(MoveToQueue) ;
					}
				}
			}else
				EntryPoints.ResetEntryState() ;
           
		}
        else if (!onTailSegment (aPerson))
            spTerminalBehavior->decQueuePathSegmentIndex();
        approachQueue (aPerson, arriveTime);
        return;
    }
    // place the Person in the list of waiting Persons
    int index = m_approachList.findItem (aPerson);
    m_approachList.removeItem (index);
    m_waitList.addToTail (aPerson);
    // set position in queue
    spTerminalBehavior->updateIndex();
    // update all approaching Persons
	updateApproaching (arriveTime);
    spTerminalBehavior->wait (TRUE);
    // if next Person has already advanced, continue moving
    continueAdvance (aPerson, arriveTime);
}


// If Person is now on tail segment, the function sets him state, index and destination.
// Else, it only sets destination to the next queue corner.
// Finally, an event about this person is generated.
void FixedQueue::approachQueue (Person *aPerson, ElapsedTime approachTime)
{
#ifdef WANT_TO_TRACE_QUUEU_INFO
	if( m_bMustTraceQueueInfo )
	{
		char tempChar[32];
		approachTime.printTime( tempChar );
		TraceSinglePerson( aPerson, std::string(" Person approachQueue  at ") + tempChar );
		TraceAllPersonInQueue( " approachQueue . other person info : " );
	}
	
#endif
    // if Person is now on tail segment, set state, index, & destination
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
    if (onTailSegment (aPerson) )
    {
        aPerson->setState (WaitInQueue);
        spTerminalBehavior->setNextIndex(getNextIndex (spTerminalBehavior->getCurrentIndex()));
        aPerson->setTerminalDestination(getLocation (aPerson, aPerson->getInStep()));
		spTerminalBehavior->SetEntryPoint(getLocation (aPerson, aPerson->getInStep())) ;
		Point destPt;
		aPerson->getTerminalDest(destPt);
		int index = EntryPoints.GetCurrentIndex() ;
		if(index == -1)
			index = LAST_SEGMENT ;	
		else
		{
			CString pronmae = spTerminalBehavior->getProcessor()->getIDName() ;
			if(destPt.distance(corner(index)) <= aPerson->getInStep() && spTerminalBehavior->getQueuePathSegmentIndex() >= index)
			{
				EntryPoints.NextEntryPoint();
			}
		}

    }
    else if (spTerminalBehavior->getQueuePathSegmentIndex() == ARC_OVERFLOW)
        aPerson->setTerminalDestination (corner (LAST_SEGMENT));
    else // set destination to the next queue corner
	{
        aPerson->setTerminalDestination (corner (spTerminalBehavior->getQueuePathSegmentIndex()-1));
		spTerminalBehavior->SetEntryPoint(corner (spTerminalBehavior->getQueuePathSegmentIndex()-1)) ;
	}
    aPerson->generateEvent (approachTime + aPerson->moveTime(),false);
}
void FixedQueue::getQueuePosition (Person *aPerson) const
{
	// determine next queue position
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	spTerminalBehavior->setNextIndex (getNextIndex (spTerminalBehavior->getCurrentIndex()));

	// determine the next segment and the next wait location
	DistanceUnit inStep = aPerson->getInStep();
	spTerminalBehavior->setQueuePathSegmentIndex (getSegment (aPerson, inStep));
	aPerson->setTerminalDestination (getLocation (aPerson, inStep));
	spTerminalBehavior->SetEntryPoint(getLocation (aPerson, inStep)) ;
}

void FixedQueue::updateApproaching (ElapsedTime updateTime)
{

    Person *aPerson;
    for (int i = 0; i < m_approachList.getCount(); i++)
    {
        aPerson = peekApproaching (i);
		TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)aPerson->getBehavior(MobElementBehavior::TerminalBehavior);
        if (spTerminalBehavior&&onTailSegment (aPerson))
        {
			if(spTerminalBehavior->HasArrivedFixQueue())
			{
				aPerson->setState (MoveToQueue);
				setPersonCurrentPosition(aPerson,updateTime);//aPerson->setCurrentPosition(updateTime) ;
			}else
			{
				if(EntryPoints.GetCurrentIndex() != -1 )
					spTerminalBehavior->first_corner = EntryPoints.GetCurrentIndex() ;
				MobileElementMovementEvent dummy;
				ElapsedTime destTime = dummy.removeMoveEvent (aPerson);
				if (destTime >= ElapsedTime(0l))
				{
					aPerson->setTerminalDestination(corner(spTerminalBehavior->first_corner)) ;
					aPerson->writeLogEntry (updateTime, false);
				}
			}
			aPerson->setState (WaitInQueue);
			spTerminalBehavior->setNextIndex (APPROACHING);
			spTerminalBehavior->updateIndex();
			getQueuePosition (aPerson);
			ElapsedTime time = updateTime + aPerson->moveTime() ;
			aPerson->generateEvent (time,false);

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


 
int FixedQueue::onTailSegment (const Person *aPerson) const
{
    Person *tail = getTail();
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();

    // if there are no Persons waiting, its based purely on segment
    if (tail == NULL)
        return (spTerminalBehavior->getQueuePathSegmentIndex() == 1 || (cornerCount() == 1 &&
            spTerminalBehavior->getQueuePathSegmentIndex() == ARC_OVERFLOW));

	TerminalMobElementBehavior* spTailTerminalBehavior = (TerminalMobElementBehavior*)tail->getBehavior(MobElementBehavior::TerminalBehavior);
    // if the queue is overflowing all Persons are on the tail segment
    if (spTailTerminalBehavior->getQueuePathSegmentIndex() == ARC_OVERFLOW)
        return TRUE;

    // if aPerson's segment is the same as the tail's, onTail == TRUE
    if (spTailTerminalBehavior->getQueuePathSegmentIndex() >= spTerminalBehavior->getQueuePathSegmentIndex())
        return TRUE;

    DistanceUnit inStep = aPerson->getInStep();

    // if aPerson's segment is one greater than the tail's...
    if (spTailTerminalBehavior->getQueuePathSegmentIndex() + 1 == spTerminalBehavior->getQueuePathSegmentIndex() ||
            (spTailTerminalBehavior->getQueuePathSegmentIndex() == LAST_SEGMENT && spTerminalBehavior->getQueuePathSegmentIndex()
            == ARC_OVERFLOW))

        // and the tail's location plus aPerson's inStep would exceed
        // the next corner location, onTail == TRUE
	{
		Point pt;
		tail->getTerminalPoint(pt);
        if (pt.distance (corner(spTailTerminalBehavior->getQueuePathSegmentIndex())) <= inStep)
            return TRUE;
	}

    return FALSE;
}

void FixedQueue::getTailPoint(Person *aPerson)const
{
	aPerson->setTerminalDestination(corner (LAST_SEGMENT));
}

void FixedQueue::setPersonCurrentPosition( Person *aPerson,const ElapsedTime& arriveTime )
{
	aPerson->setCurrentPosition(arriveTime);
}

int FixedQueue::getFixQEntryPointIndex()
{
	Person* aper = getTail() ;
	if(aper == NULL)
		EntryPoints.ResetEntryState();
	int index = EntryPoints.GetCurrentIndex();
	if(index == -1 || index > (cornerCount() - 1))
		index = cornerCount() - 1;
	return index;
}
