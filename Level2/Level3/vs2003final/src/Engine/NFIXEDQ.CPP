#include "stdafx.h"
#include "engine\accessq.h"
#include "engine\procq.h"
#include "engine\nfixedq.h"
#include "engine\person.h"
#include "common\states.h"
#include "engine\TracePredef.h"
#include "TerminalMobElementBehavior.h"

void NonFixedQueue::addToQueue (Person *aPerson)
{

	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	if (spTerminalBehavior)
	{ 
		m_approachList.addItem (aPerson);
		aPerson->setState (WaitInQueue);
		spTerminalBehavior->setNextIndex (APPROACHING);
		spTerminalBehavior->updateIndex();
		getQueuePosition (aPerson);

	}
  

		
#ifdef WANT_TO_TRACE_QUUEU_INFO
	if( m_bMustTraceQueueInfo )
	{
		TraceSinglePerson( aPerson, std::string("Person Add to queue !" ) );
	}
#endif
	

}


void NonFixedQueue::arriveAtQueue (Person *aPerson, ElapsedTime arriveTime)
{

    int index = m_approachList.findItem (aPerson);
    m_approachList.removeItem (index);
    m_waitList.addToTail (aPerson);

	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	if (spTerminalBehavior)
	{
		// set position in queue
		spTerminalBehavior->updateIndex();

		updateApproaching (arriveTime);
		spTerminalBehavior->wait (TRUE);

		// if next Person has already advanced, continue moving
		continueAdvance (aPerson, arriveTime);
	}

   
#ifdef WANT_TO_TRACE_QUUEU_INFO
	if( m_bMustTraceQueueInfo )
	{
		char tempChar[32];
		arriveTime.printTime( tempChar );
		TraceSinglePerson( aPerson, std::string(" Person arrive at queue at ") + tempChar );
		TraceAllPersonInQueue( " arrive at queue. other person info : " );
	}
	
#endif
}


void NonFixedQueue::updateApproaching (ElapsedTime updateTime)
{
    Person *aPerson;
    for (int i = 0; i < m_approachList.getCount(); i++)
    {
        aPerson = peekApproaching (i);
        aPerson->setState (MoveToQueue);
        aPerson->setCurrentPosition (updateTime);
		TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
		if (spTerminalBehavior)
		{
			aPerson->setState (WaitInQueue);
			spTerminalBehavior->setNextIndex (APPROACHING);
			spTerminalBehavior->updateIndex();
			getQueuePosition (aPerson);
			aPerson->generateEvent (updateTime + aPerson->moveTime(),false);
		}

      
    }
}


void NonFixedQueue::getTailPoint (Person *aPerson) const
{
    if (!m_waitList.getCount())
        aPerson->setTerminalDestination(corner(HEAD));
    else
	{
		Point pt;
		((Person *)m_waitList.getTail())->getTerminalPoint(pt);
        aPerson->setTerminalDestination (pt);
	}
}

