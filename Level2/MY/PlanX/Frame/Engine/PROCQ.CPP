/*****
*
*   Class       ProcessorQueue
*   Author      Lincoln Fellingham, Aviation Research Corporation
*   Date        December 2, 1992
*   Revision    July 14, 1993 - expanded upwards for subclassing non
*                               fixed queues
*                             - improved flexibility of waiting positions
*                             - developed rules for handling overflow
*
*               January 20, 1994 - added queue jumping methods
*                                - changed Queue to AccessibleQueue
*
*   Purpose     Member of Processor that handles queue functions
*
*               m_approachList is a list of all Persons that are currently
*               moving towards the queue
*
*               waitList is the list of Persons waiting for service
*               the waitList is always contiguous and has no bearing to
*               a Person's physical location in the queue
*               a Person's index in the waitList actually indicates the
*               number of Persons in front of them before the Person is
*               even aware of any changes
*
*               m_qCorners is the list of Points that define the shape of
*               the queue
*
*               a Person's pathIndex determines the location they think
*               they are at, from HEAD (0) to many higher than the
*               qlength of the queue, as the queue can be reduced
*               quickly while they are not moving (before they receive
*               the message)
*
*               a Person's nextIndex is used when moving to store the
*               position in the queue that they are moving to. remember
*               that the Person in front of them can also advance before
*               the Person arrives at their destination
*
*               a Person's pathSegment stores the index into the m_qCorners
*               list of the point that they have most recently passed.
*               if they have not yet made it to the physical tail of the
*               queue (as defined in the Processor's layout) this value
*               will equal ARC_OVERFLOW
*
*****/
#include "stdafx.h"

#include "procq.h"
#include "engine\person.h"
#include "TerminalMobElementBehavior.h"
int ProcessorQueue::getTotalQueueLength (void) const
{
    Person *aPerson;
    int groupCount = m_approachList.getCount();
    int approachCount = 0;

    for (int i = 0; i < groupCount; i++)
    {
        aPerson = (Person *)m_approachList.getItem(i);
        approachCount += aPerson->GetActiveGroupSize();
    }
    return approachCount + getQueueLength();
}


/*
void ProcessorQueue::printApproaching (void) const
{
//    cout << '\n';
//    for (int i = 0; i < m_approachList.getCount(); i++)
//        cout << ((Person *)m_approachList.getItem(i))->getID() << ' ';
//    cout << '\n';
}
*/
 
void ProcessorQueue::removePerson (const Person *aPerson)
{
    int index = m_approachList.findItem (aPerson);
    if (index != INT_MAX)
        m_approachList.removeItem (index);
}

ProcessorQueue::ProcessorQueue ( const ProcessorQueue& _anothrProcQueue )
{
	// MobileElementList's assign operator '=' must works correctly
	m_approachList=_anothrProcQueue.m_approachList;
	m_qCorners=_anothrProcQueue.m_qCorners;
}

ProcessorQueue& ProcessorQueue::operator = ( const ProcessorQueue& _anothrProcQueue )
{
	if( this != &_anothrProcQueue )
	{
		m_approachList=_anothrProcQueue.m_approachList;
		m_qCorners=_anothrProcQueue.m_qCorners;
	}

	return *this;
}

void ProcessorQueue::ClearAllTempData()
{
	m_approachList.clear();
}

void ProcessorQueue::TraceSinglePerson( Person* _pPerson, std::string _strOtherInfo )
{
	ofsstream echoFile ("SimEngineLog.txt", stdios::app);
    echoFile	<< "\n"<<"*******************bengin***************************"<<"\n";
	echoFile << _strOtherInfo;
	TerminalMobElementBehavior* spTerminalBehavior = _pPerson->getTerminalBehavior();
	if(spTerminalBehavior)
	{
		echoFile <<" " << *spTerminalBehavior;
	}
	
	echoFile	<< "\n"<<"*******************end***************************"<<"\n";;
	echoFile.close();
}

void ProcessorQueue::TraceAllPersonInQueue( std::string _strOtherInfo )
{
	ofsstream echoFile ("SimEngineLog.txt", stdios::app);
	echoFile<< "\n"<<"*******************bengin***************************"<<"\n";;
	echoFile << _strOtherInfo;
	echoFile << " " << " Trace approach list person : "<<"\n";
	int iCount = m_approachList.getCount();
	Person* pPerson = NULL;
	for( int i=0; i<iCount; ++i )
	{
		pPerson = (Person*)m_approachList.getItem( i );
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