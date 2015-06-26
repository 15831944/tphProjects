// Splitter.cpp: implementation of the Splitter class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Splitter.h"
#include "engine\person.h"
#include "TerminalMobElementBehavior.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Splitter::Splitter()
{

}

Splitter::~Splitter()
{

}
void Splitter::beginService (Person *_pPerson, ElapsedTime _curTime)
{
//	TerminalMobElementBehavior* spTerminalBehavior = _pPerson->getTerminalBehavior();
//	if (spTerminalBehavior == NULL)
//		return;
//	
//	Processor* pNextProc = spTerminalBehavior->TryToSelectNextProcessor( _curTime );
//	if( pNextProc )
//	{
//		if( pNextProc->getProcessorType() == ConveyorProc )
//		{
//			Conveyor* pConveyor = (Conveyor*)pNextProc;
////			if ( pConveyor->GetSubConveyorType() == FLOW_BELT )			
////			{
//				// write processor "begin service" log
//				writeLogEvent (_pPerson, _curTime, BeginService);
//				
//				int index = m_approaching.findItem (_pPerson);
//				assert( index != INT_MAX );
//				m_approaching.removeItem (index);
//
//				spTerminalBehavior->SetDirectGoToNextProc( pNextProc );
//				_pPerson->setState( LeaveServer );
//				_pPerson->generateEvent( _curTime,false );
//				return;
////			}
//		}
//	}

	CSimpleConveyor::beginService( _pPerson, _curTime );
}


// release the head person
bool Splitter::releaseHeadPerson( Person* _pPerson, ElapsedTime _curTime )
{
	
	//assert( _pPerson );
	//TerminalMobElementBehavior* spTerminalBehavior = _pPerson->getTerminalBehavior();
	//if (spTerminalBehavior == NULL)
	//{
	//	return false;
	//}

	//if( spTerminalBehavior->GetDirectGoToNextProc() == NULL )
	//{
	//	Processor* pNextProc = spTerminalBehavior->TryToSelectNextProcessor( _curTime, false, true );
	//	if( pNextProc )
	//	{
	//		if( pNextProc->getProcessorType() == ConveyorProc )
	//		{
	//			Conveyor* pConveyor = (Conveyor*)pNextProc;
	//			if ( pConveyor->GetSubConveyorType() != FLOW_BELT )				
	//			{
	//				spTerminalBehavior->SetDirectGoToNextProc( pNextProc );
	//				
	//				// remove the pax from this processor
	//				m_pSlots[ m_iCapacity -1 ].first = NULL;
	//				m_pSlots[ m_iCapacity -1 ].second = -1l;

	//				m_iOccapuiedCount--;
	//				assert( m_iOccapuiedCount >=0 );

	//				////////////////////////////////////////////////////////////////////////
	//				if( m_iCapacity == 1 )
	//				{
	//					writePersonLog( _pPerson, m_location, _curTime );
	//				}
	//				////////////////////////////////////////////////////////////////////////

	//				// move to next processor
	//				_pPerson->setState( LeaveServer );
	//				_pPerson->move( _curTime ,false);	
	//				
	//				return true;
	//			}
	//		}
	//	}

	//	return false;
	//}
	//else
	//{
	
		return CSimpleConveyor::releaseHeadPerson( _pPerson, _curTime );	
	//}	
}





















