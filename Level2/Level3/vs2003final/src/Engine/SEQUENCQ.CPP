/*****
*
*   Class       SequentialQueue
*   Author      Lincoln Fellingham, Aviation Research Corporation
*   Date        May 6, 1994
*   Purpose     Contains methods from original SequentialQueue class
*               that handle indexing and ordering
*
*               SequentialQueue has become its superclass, containing
*               virtual functions that are common to both ordered and
*               random queues
*
*****/
#include "stdafx.h"
#include "engine\sequencq.h"
#include "engine\accessq.h"
#include "engine\person.h"
#include "engine\process.h"
#include "common\states.h"
#include "engine\TracePredef.h"
#include "TerminalMobElementBehavior.h"
#include "TERMINAL.H"
#include "PROCLIST.H"
#include <iostream>

 
//A waiting Person will have index of 0 (HEAD) && will not be moving.
int SequentialQueue::hasWait (void) const
{
    if (!m_waitList.getCount())
        return FALSE;

    Person *aPerson = (Person *)m_waitList.getHead();
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
    return (spTerminalBehavior&&spTerminalBehavior->isWait() && (spTerminalBehavior->getCurrentIndex() == HEAD));
}
                       
// Returns total count of Persons currently in the Queue, which includes the number of Persons in each group, minus visitors.
int SequentialQueue::getQueueLength (void) const
{
    Person *aPerson;
    int groupCount = m_waitList.getCount();
    int queueLength = 0;

    for (int i = 0; i < groupCount; i++)
    {
        aPerson = (Person *)m_waitList.getItem (i);
        queueLength += aPerson->GetActiveGroupSize();
    }
    return queueLength;
}

 
int SequentialQueue::getGroupCount (void) const
{
    return m_waitList.getCount();
}

 
int SequentialQueue::getTotalGroupCount (void) const
{
    return m_waitList.getCount() + m_approachList.getCount();
}


void SequentialQueue::printContents (void) const
{
    int queueLength = m_waitList.getCount();
	
    for (int i = 0; i < queueLength; i++)
	{
		TerminalMobElementBehavior* spTerminalBehavior = ((Person *)m_waitList.getItem(i))->getTerminalBehavior();
		if (spTerminalBehavior)
		{
			stdcerr << '\n' << *(spTerminalBehavior);
		}
	}
}

//The wiped queue length is returned. 
int SequentialQueue::wipe (ElapsedTime curTime)
{
    int count = getQueueLength();

    Person *aPerson;
    while (m_waitList.getCount())
    {
        aPerson = (Person *)m_waitList.removeHead();
        aPerson->flushLog (curTime);
//        delete aPerson;
    }
    return count;
}

//The ProcessorQueue::removePerson() is invoked.
void SequentialQueue::removePerson (const Person *aPerson)
{
    int index = m_waitList.findItem (aPerson);
    if (index != INT_MAX)
        m_waitList.removeItem (index);
    ProcessorQueue::removePerson (aPerson);
}


 
// Returns pointer to Person whose index is equal to the passed index.
// The search starts at position greater of (index and length of queue), and then decrements until Person with correct index is found.
// It returns NULL if no Person has the passed index.
Person *SequentialQueue::peekWait (int index) const
{
    Person *aPerson;
    int queueIndex = m_waitList.getCount()-1;

    while (queueIndex >= 0)
    {
        aPerson = (Person *)m_waitList.getItem (queueIndex);
		TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();

		if (spTerminalBehavior == NULL)
		{
			return NULL;
		}
        if (spTerminalBehavior->getNextIndex() < index)		
            break;
        else if (spTerminalBehavior->getNextIndex() == index)		
            return aPerson;

        queueIndex--;
    }
    return NULL;
}

int SequentialQueue::getNextIndex (int index) const
{
    if (index == APPROACHING)
        return getTailIndex();

    while (index && !peekWait (index-1)) 
        index--;

    return index;
}

int SequentialQueue::getTailIndex (void) const
{
    Person *tail = (Person *)m_waitList.getTail();
	TerminalMobElementBehavior* spTerminalBehavior = NULL;
	if (tail)
	{
		spTerminalBehavior = tail->getTerminalBehavior();
	}
	
    return (spTerminalBehavior)? (spTerminalBehavior->getNextIndex() + 1): 0;
}

 
int SequentialQueue::isTail (int index) const
{
    Person *tail = (Person *)m_waitList.getTail();
	TerminalMobElementBehavior* spTerminalBehavior = NULL;
	if (tail)
	{
		spTerminalBehavior = tail->getTerminalBehavior();
	}
	
    return (spTerminalBehavior &&spTerminalBehavior->getCurrentIndex() == index);
}

// It schedules a LeaveQueue event but the Person is not actually removed until that event fires.
// This timing of events is required to force the correct animation of the time at which "next" actually starts to move.
void SequentialQueue::releaseNext (ElapsedTime p_time)
{
    Person *next = (Person *)m_waitList.getHead();
/*
#ifdef WANT_TO_TRACE_QUUEU_INFO
	if( m_bMustTraceQueueInfo )
	{
		char tempChar[32];
		p_time.printTime( tempChar );
		TraceSinglePerson( next, std::string(" queue release next person  at ") + tempChar );
		TraceAllPersonInQueue( " queue release next person  . other person info : " );
	}
	
#endif
	  */
    // write log entry in current position to start movement
	TerminalMobElementBehavior* spTerminalBehavior = NULL;
	if (next)
	{
		spTerminalBehavior = next->getTerminalBehavior();
	}
	
    if (spTerminalBehavior&&spTerminalBehavior->getCurrentIndex() == HEAD)
    {		
        next->setState (LeaveQueue);
        next->generateEvent (p_time,false);
    }
}

 
// Person has actually started moving.
void SequentialQueue::leaveQueue (const Person *, ElapsedTime p_time)
{

	MobileElement *pPerson =  m_waitList.removeHead();
#ifdef WANT_TO_TRACE_QUUEU_INFO
	if( m_bMustTraceQueueInfo && pPerson)
	{
		char tempChar[32];
		p_time.printTime( tempChar );
		TraceSinglePerson( (Person*)pPerson, std::string(" Person leave queue  at ") + tempChar );
		TraceAllPersonInQueue( " leave queue . other person info : " );
	}
	
#endif

    if (!m_waitList.getTail())
    {
        updateApproaching (p_time);
        return;
    }

	
    // test for a following Person in the queue
    // determine if there is a Person thinking they are at position 1

	Person *next = peekWait (1);
	if (!next)
		return;	
	TerminalMobElementBehavior* spTerminalBehavior = next->getTerminalBehavior();

	if (spTerminalBehavior == NULL)
	{
		return;
	}

	if (spTerminalBehavior->getCurrentIndex() != 1)
	{
		return;
	}
	
	Point locationPt;
	next->getTerminalPoint(locationPt);
    // write log entry in current position to start movement
    next->setState (StartMovement);
    next->setTerminalDestination(locationPt);
    next->writeLogEntry (p_time, false);
    relayAdvance (next, p_time);
}

 
void SequentialQueue::arriveAtWaitPoint (Person *p_person, ElapsedTime p_time)
{
#ifdef WANT_TO_TRACE_QUUEU_INFO
	if( m_bMustTraceQueueInfo )
	{
		char tempChar[32];
		p_time.printTime( tempChar );
		TraceSinglePerson( p_person, std::string("before ****Person arriveAtWaitPoint   at ") + tempChar );
		
	}
	
#endif
    // set position in queue
	TerminalMobElementBehavior* spTerminalBehavior = p_person->getTerminalBehavior();
	if (spTerminalBehavior)
	{
		spTerminalBehavior->updateIndex();
	}
    

    // if next Person has already advanced, continue moving
    continueAdvance (p_person, p_time);

#ifdef WANT_TO_TRACE_QUUEU_INFO
	if( m_bMustTraceQueueInfo )
	{
		char tempChar[32];
		p_time.printTime( tempChar );
		TraceSinglePerson( p_person, std::string("end ****Person arriveAtWaitPoint   at ") + tempChar );		
	}
	
#endif

}

//If a person is from tail, reset destination of all approaching Persons; otherwise, schedules event to have next person in queue advance.
void SequentialQueue::relayAdvance (Person *p_person, ElapsedTime p_time)
{
	
	
#ifdef WANT_TO_TRACE_QUUEU_INFO
	if( m_bMustTraceQueueInfo )
	{
		char tempChar[32];
		p_time.printTime( tempChar );
		TraceSinglePerson( p_person, std::string("before ****Person relayAdvance queue  at ") + tempChar );
		TraceAllPersonInQueue( "befor*** relayAdvance queue . other person info : " );
	}
	
#endif
    // detemine next waiting location and schedule event
	TerminalMobElementBehavior* spTerminalBehavior = p_person->getTerminalBehavior();
	if (spTerminalBehavior)
	{
		getQueuePosition (p_person);
		p_person->setState (WaitInQueue);
		p_person->generateEvent (p_time + p_person->moveTime(),false);

		if (isTail (spTerminalBehavior->getCurrentIndex()))
				// reset destination of all approaching Persons
			updateApproaching (p_time);
		else
		// schedule event to have next person in queue advance
			generateAdvanceEvent (spTerminalBehavior->getCurrentIndex() + 1, p_time + 1l,
			p_person->getSpeed());

	}

  
#ifdef WANT_TO_TRACE_QUUEU_INFO
	if( m_bMustTraceQueueInfo )
	{
		char tempChar[32];
		p_time.printTime( tempChar );
		TraceSinglePerson( p_person, std::string(" end *****************Person relayAdvance queue  at ") + tempChar );
		TraceAllPersonInQueue( " end***************relayAdvance queue . other person info : " );
	}
	
#endif
	
}

//Gets next person in queue at index p_ndx. If found, generates an advanced event and adds it event list.
void SequentialQueue::generateAdvanceEvent (int p_ndx, ElapsedTime p_time, DistanceUnit p_maxSpeed)
{
    Person *next = peekWait (p_ndx);
	
	if (!next)
	{
		return;
	}

	TerminalMobElementBehavior* spTerminalBehavior = next->getTerminalBehavior();
    if (!spTerminalBehavior || spTerminalBehavior->getCurrentIndex() != p_ndx)
        return;
	
	
    spTerminalBehavior->setQueueSpeed (p_maxSpeed);
    next->setState (StartMovement);
    next->generateEvent (p_time,false);
}


void SequentialQueue::continueAdvance (Person *aPerson, ElapsedTime p_time)
{
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
    if (spTerminalBehavior&&spTerminalBehavior->getCurrentIndex() != HEAD && !peekWait (spTerminalBehavior->getCurrentIndex()-1))
        relayAdvance (aPerson, p_time);
}

//Changes jumper to anothe queue. 
//If the jumper is in tail, reset destination of all approaching Persons; otherwise, schedule event to have next person in queue advance. Then it updates the jumper to head for new proc immediately.
void SequentialQueue::balanceQueue (Processor *changedProc, ElapsedTime p_time)
{
    Person *jumper;

	#ifdef WANT_TO_TRACE_QUUEU_INFO
		if( m_bMustTraceQueueInfo )
		{
			ofsstream echoFile ("SimEngineLog.txt", stdios::app);
			echoFile	<< "\n"<<"******************* balanceQueue bengin***************************"<<"\n";
			echoFile	<< "\n"<<"******************* balanceQueue bengin***************************"<<"\n";			
			echoFile	<< "\n"<<"******************* balanceQueue bengin***************************"<<"\n";			
			echoFile.close();
		}
	#endif


    for (int i = 0; i < m_waitList.getCount(); i++)
    {
        jumper = (Person *)m_waitList.getItem(i);
		
		TerminalMobElementBehavior* spTerminalBehavior = jumper->getTerminalBehavior();
		//if (isTail (jumper->getCurrentIndex()))
		//	updateApproaching (p_time);
		//else
			// schedule event to have next person in queue advance
		if (spTerminalBehavior == NULL)
		{
			continue;
		}
		//check close state
		if(changedProc->isClosed())
		{
			continue;
		}
		//check open state
		if (changedProc->isOpened()==false)
		{
			continue;
		}
		//check recycle frequency
		if(changedProc->CheckRecycleFrequency(jumper) == true)
		{
			continue;
		}
		//check retail processor queue length
		if (changedProc->isQueueLengthVacant(jumper) == false)
		{
			continue;
		}
		//check person whether can jump queue
		if (!changedProc->isVacant(jumper))
		{
			continue;
		}
		
		//check can lead to reclaim
		if (jumper->GetTerminal())
		{
			if(jumper->GetTerminal()->GetProcessorList())
			{
				Processor* pReclaim = jumper->GetTerminal()->GetProcessorList()->getProcessor(jumper->getBagCarosel());
				if (pReclaim && changedProc->CanLeadToReclaim(jumper->getType(),*(pReclaim->getID())) == false)
					continue;
			}
		}
	
		
        if (	/* !changedProc->canServe (jumper->getType()) */  // can use this method this way !!!
				!changedProc->leadsToGate (jumper->getGateIndex())
			||	!spTerminalBehavior->canJumpToProcessor(changedProc)
			)
			continue;


        if (changedProc->getTotalQueueLength() < i )
        {
			#ifdef WANT_TO_TRACE_QUUEU_INFO
				if( m_bMustTraceQueueInfo )
				{
					ofsstream echoFile ("SimEngineLog.txt", stdios::app);
					echoFile	<< "\n"<<"******************* before jumper change queue ***************************"<<"\n";
					echoFile	<< "\n"<<"******************* jumper ID = " << jumper->getID() <<"**************************"<<"\n";
					echoFile	<< "\n"<<"******************* all person's index  ***************************"<<"\n";
						
						for( int t=0; t<m_waitList.getCount(); ++t )
						{
							Person* pPerson = (Person *)m_waitList.getItem(t);
							echoFile << " ID : " << pPerson->getID() <<"  index in queue :" <<pPerson->getCurrentIndex() <<" next idx in queue : " << pPerson->getNextIndex()<<"\n";
						}
					echoFile	<< "\n"<<"*******************end all person's index  ***************************"<<"\n";
					echoFile.close();

					/*
					TraceAllPersonInQueue( " all person in queue waitlist now: " );
					*/
				}
			#endif

				
            m_waitList.removeItem (i--);
			
			if (isTail (spTerminalBehavior->getCurrentIndex()))
				// reset destination of all approaching Persons
				updateApproaching (p_time);
			else
			{
				if(m_waitList.getItem(i+1)!=NULL)
				{
					TerminalMobElementBehavior* spNextTerminalBehavior = m_waitList.getItem(i+1)->getTerminalBehavior();
					if (spNextTerminalBehavior)
					{
						generateAdvanceEvent(spNextTerminalBehavior->getCurrentIndex(),p_time+1l,0);
					}
					
				}
				else
					generateAdvanceEvent (spTerminalBehavior->getCurrentIndex() + 1, p_time + 1l, 0);
			}
				// schedule event to have next person in queue advance
			//	generateAdvanceEvent (jumper->getCurrentIndex() + 1, p_time + 1l, 0);
			//generateAdvanceEvent(jumper->getCurrentIndex(),p_time +1l,0);
			
            // update person to head for new proc immediately
            spTerminalBehavior->changeQueue (changedProc, p_time);



			#ifdef WANT_TO_TRACE_QUUEU_INFO
				if( m_bMustTraceQueueInfo )
				{
					ofsstream echoFile ("SimEngineLog.txt", stdios::app);
					echoFile	<< "\n"<<"******************* end jumper change queue ***************************"<<"\n";
					echoFile	<< "\n"<<"******************* all person's index  ***************************"<<"\n";
						
						for( int t=0; t<m_waitList.getCount(); ++t )
						{
							Person* pPerson = (Person *)m_waitList.getItem(t);
							echoFile << " ID : " << pPerson->getID() <<"  index in queue :"<<pPerson->getCurrentIndex() <<"\n";
						}
					echoFile	<< "\n"<<"*******************end all person's index  ***************************"<<"\n";
					echoFile.close();

					/*
					char tempChar[32];
					p_time.printTime( tempChar );
					TraceSinglePerson( jumper, std::string(" Person balance  queue  at ") + tempChar );
					TraceAllPersonInQueue( " balance queue . other person info : " );
					*/
				}
				
			#endif
			
        }
    }

	
	#ifdef WANT_TO_TRACE_QUUEU_INFO
		if( m_bMustTraceQueueInfo )
		{
			ofsstream echoFile ("SimEngineLog.txt", stdios::app);
			echoFile	<< "\n"<<"******************* balanceQueue end***************************"<<"\n";
			echoFile	<< "\n"<<"******************* balanceQueue end***************************"<<"\n";			
			echoFile	<< "\n"<<"******************* balanceQueue end***************************"<<"\n";			
			echoFile.close();
		}
	#endif
}

 
 
void SequentialQueue::getQueuePosition (Person *aPerson) const
{
    // determine next queue position
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();
	if (spTerminalBehavior)
	{
		spTerminalBehavior->setNextIndex (getNextIndex (spTerminalBehavior->getCurrentIndex()));

		// determine the next segment and the next wait location
		DistanceUnit inStep = aPerson->getInStep();
		spTerminalBehavior->setQueuePathSegmentIndex (getSegment (aPerson, inStep));
		aPerson->setTerminalDestination(getLocation (aPerson, inStep));
		spTerminalBehavior->SetEntryPoint(getLocation (aPerson, inStep)) ;
	}
    
}

 
int SequentialQueue::getSegment (Person *aPerson, DistanceUnit inStep) const
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
	if(nextPerson == NULL)
		return ARC_OVERFLOW ;

	TerminalMobElementBehavior* spNextTerminalBehavior = nextPerson->getTerminalBehavior();

	if (spNextTerminalBehavior == NULL)
		return ARC_OVERFLOW;

    int segment = spNextTerminalBehavior->getQueuePathSegmentIndex();

    if (segment == ARC_OVERFLOW)
        return ARC_OVERFLOW;

    if (spNextTerminalBehavior->getDest().distance (corner (segment)) > inStep)
        return segment;

    // the two remaining options occur when the people are not on the
    // same segment of the queue
    if (segment == LAST_SEGMENT)
        return ARC_OVERFLOW;
    else
        return segment + 1;
}

 
Point SequentialQueue::getLocation (Person *aPerson, DistanceUnit inStep) const
{
	TerminalMobElementBehavior* spTerminalBehavior = aPerson->getTerminalBehavior();

    if (spTerminalBehavior && spTerminalBehavior->getNextIndex() == HEAD)
        return corner (HEAD);

	Point locationPt;
	aPerson->getTerminalPoint(locationPt);

    Person *nextPerson;
    nextPerson = peekWait (spTerminalBehavior->getNextIndex()-1);
	if(!nextPerson)
		return corner (HEAD);

	Point destPt;
	nextPerson->getTerminalDest(destPt);

	TerminalMobElementBehavior* spNextTerminalBehavior = aPerson->getTerminalBehavior();
    Point vector;

    if (spTerminalBehavior->getQueuePathSegmentIndex() == ARC_OVERFLOW)
        vector = locationPt - destPt;
    else if (spTerminalBehavior->getQueuePathSegmentIndex() == spNextTerminalBehavior->getQueuePathSegmentIndex())
        vector = corner (spNextTerminalBehavior->getQueuePathSegmentIndex()) - destPt;
    else
        vector = corner (spTerminalBehavior->getQueuePathSegmentIndex()) - destPt;

    vector.length (inStep);
    return destPt + vector;
}

ElapsedTime SequentialQueue::GetMaxWaitTime()
{
	Person *pPerson;
	ElapsedTime tempTime( 86399l );//23:59:59
    for (int i = 0; i < m_waitList.getCount(); i++)
    {
        pPerson = (Person *)m_waitList.getItem(i);
		TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
		if( spTerminalBehavior&&spTerminalBehavior->getEntryQueueTime() < tempTime )
		{
			tempTime = spTerminalBehavior->getEntryQueueTime();
		}
	}

	return tempTime;
}

void SequentialQueue::TraceAllPersonInQueue( std::string _strOtherInfo )
{
	ProcessorQueue::TraceAllPersonInQueue( _strOtherInfo );
	
	ofsstream echoFile ("queue_trace.log", stdios::app);
	echoFile<< "\n"<<"*******************bengin***************************"<<"\n";;
	echoFile << _strOtherInfo;
	echoFile << " " << " Trace wait list person : "<<"\n";
	int iCount = m_waitList.getCount();
	Person* pPerson = NULL;
	for( int i=0; i<iCount; ++i )
	{
		pPerson = (Person*)m_waitList.getItem( i );
		TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
		if (spTerminalBehavior)
		{
			echoFile << *spTerminalBehavior;
		}
		echoFile << "\n";
	}

	echoFile	<< "\n"<<"*******************end***************************"<<"\n";;
	echoFile.close();
}
void SequentialQueue::ClearAllTempData()
{
	ProcessorQueue::ClearAllTempData();
	m_waitList.clear();
}
