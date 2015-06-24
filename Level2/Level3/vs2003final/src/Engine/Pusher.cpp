// Pusher.cpp: implementation of the Pusher class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Pusher.h"
#include "PusherCheckMaxWaitTimeEvent.h"
#include "PusherScheduleEvent.h"
#include "engine\person.h"
#include "inputs\miscproc.h"
#include "TerminalMobElementBehavior.h"
#define MAX_WAIT_TIME 300l
#define SCHEDULE_EVENT 600l
#define MOVING_SPEED 300

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Pusher::Pusher()
//:m_bNeedToCheckMaxWaitTime( false ), m_bReleaseFirstIfFull( false )															)
{
	m_bNeedToCheckMaxWaitTime = false;
	m_bReleaseFirstIfFull = true		;
	m_bReleaseBaggageIfDestProcAvailable = false;
	m_bShouldGenerateScheduleEvent = true;

	m_bHasGeneratedScheduleEvent = false;

	m_timeMaxWaittime = ElapsedTime(300l);;
	m_timeSchecduleInterval = ElapsedTime(600l);
	m_dMovingSpeed = 300;
}

Pusher::~Pusher()
{

}
//init pusher's service slots and every flag
void Pusher::InitData( const MiscConveyorData * _pMiscData )
{

	CSegmentPath temp;
	if( _pMiscData )
	{
		m_bNeedToCheckMaxWaitTime = _pMiscData->GetPusherReleasedByMaxTime();
		m_bReleaseFirstIfFull = _pMiscData->GetPusherReleasedByFull();
		m_bReleaseBaggageIfDestProcAvailable = _pMiscData->GetPusherReleasedByNextAvail();
		m_bShouldGenerateScheduleEvent = _pMiscData->GetPusherReleasedBySchedPick() ;

		m_timeMaxWaittime = _pMiscData->GetRandomAmountTime();
		m_timeSchecduleInterval = _pMiscData->GetScheduleTime();
		m_dMovingSpeed = _pMiscData->GetSpeed() ;

		//capacity		
		m_lQueueCapacity = _pMiscData->GetQueueCapacity();
		if( m_lQueueCapacity == -1 )//is auto define capacity
		{
			m_lQueueCapacity = CaculateConveyorCapacity();
		}
		temp.init( *serviceLocationPath(), m_lQueueCapacity );
	}
	else
	{
		//capacity
		temp.init( *serviceLocationPath(),CaculateConveyorCapacity() );
	}
	
	
	
	std::vector< PointIdxPair >  vServicePoint = temp.getFlagPointList(-1,-1 );
	int iSize = vServicePoint.size();
	ServiceSlot tempSlot;
	for( int i=0,j=1; j<iSize; ++j,++i )
	{
		tempSlot.SetServicePoint( vServicePoint[i] );		
		tempSlot.SetTimeToForward( ElapsedTime( vServicePoint[i].first.distance( vServicePoint[j].first ) / m_dMovingSpeed ) );
		m_vServiceSlot.push_back( tempSlot );		
	}

	tempSlot.SetServicePoint( vServicePoint[iSize-1] );	
	tempSlot.SetTimeToForward( ElapsedTime(0l) );	
	
	m_vServiceSlot.push_back( tempSlot );
}

void Pusher::beginService (Person *_pPerson, ElapsedTime curTime)
{
	// write processor "begin service" log
	writeLogEvent( _pPerson, curTime, BeginService );

	_pPerson->setState( MovingOnPusher );
	
	int iIdx = m_approaching.findItem( _pPerson );
	if( iIdx != INT_MAX )
	{
		m_approaching.removeItem( iIdx );
	}

	TerminalMobElementBehavior* spTerminalBehavior = _pPerson->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return;
	}

	if( m_bReleaseBaggageIfDestProcAvailable )
	{
		Processor* pNextProc = spTerminalBehavior->TryToSelectNextProcessor( curTime );	
		if( pNextProc )
		{
			
			spTerminalBehavior->SetDirectGoToNextProc( pNextProc );
			_pPerson->setState( LeaveServer );
			_pPerson->generateEvent( curTime ,false);
			return;			
		}


		if ( m_lQueueCapacity == GetBaggageOnPusherCount() +1  )//is the last one
		{
			Processor* pDestProc = GetRandomDestProc( _pPerson );
			if( pDestProc )
			{
				if( pDestProc->GetStopReason() >=0 )
				{
					m_lReason = pDestProc->GetStopReason();
					writeLogEvent( NULL, curTime, ProcessorStateStop ,getLoad() );	
					writeAdditionLog( ProcessorStateStop, curTime, m_lReason );
				}
				else
				{
					m_lReason = pDestProc->getIndex();
					writeLogEvent( NULL, curTime, ProcessorStateStop ,getLoad() );	
					writeAdditionLog( ProcessorStateStop, curTime, m_lReason );
				}				
			}
			else
			{
				m_lReason = -1;
				writeLogEvent( NULL, curTime, ProcessorStateStop ,getLoad() );	
				writeAdditionLog( ProcessorStateStop, curTime, m_lReason );
			}
			
		}
	}
	if( m_bNeedToCheckMaxWaitTime )
	{
		GenerateCheckMaxTimeEvent( _pPerson, curTime + m_timeMaxWaittime );
	}

	if( m_bShouldGenerateScheduleEvent && !m_bHasGeneratedScheduleEvent )
	{
		GenerateScheduleEvent( curTime + m_timeSchecduleInterval );
		m_bHasGeneratedScheduleEvent = true;
	}
	if( m_bReleaseFirstIfFull )
	{
		ReleaseFirstBaggageIfFull( _pPerson, curTime );
	}


	// find the first empty slot from start service slot on pusher
	int iFirstEmptyPos = -1;
	int iSize = m_vServiceSlot.size();
	for( int i=0; i<iSize; ++i )
	{
		if( m_vServiceSlot[i].GetBaggageOnSlot() == NULL && m_vServiceSlot[i].GetServicePoint().second >= 0 )
		{
			iFirstEmptyPos = i;
			break;
		}
	}

	if( iFirstEmptyPos >= 0 )
	{
		MoveForwardBaggageOnPusher( curTime, 0, iFirstEmptyPos );	

		ASSERT( m_vServiceSlot.size() >= 3 );
		_pPerson->setTerminalDestination( m_vServiceSlot[1].GetServicePoint().first  );
		DistanceUnit dSpeed = _pPerson->getSpeed();
		_pPerson->setSpeed( m_dMovingSpeed );

		curTime += _pPerson->moveTime();
		_pPerson->writeLogEntry( curTime, false );
		_pPerson->setSpeed( dSpeed );
		// let the new comer move onto the pusher 
		m_vServiceSlot[1].SetBaggageOnSlot( _pPerson );
	}
}
bool Pusher::isVacant( Person* pPerson )const
{
	if( m_bReleaseFirstIfFull )
	{
		return true;
	}
	else
	{
		int iAllBaggageCount = GetBaggageOnPusherCount() + m_approaching.getCount();
		return iAllBaggageCount < this->m_lQueueCapacity;
	}
}

// notify conveyor that one of its dest processor is available now
bool Pusher::destProcAvailable ( const ElapsedTime& _tCurrentTime )
{
	if( m_bReleaseBaggageIfDestProcAvailable )
	{
		if( RandomReleaseBaggage( _tCurrentTime ) )
		{
			m_lReason = -1;
			writeLogEvent( NULL, _tCurrentTime, ProcessorStateRun ,getLoad() );	
			writeAdditionLog( ProcessorStateRun, _tCurrentTime, m_lReason );
			return true;
		}
		return false;		
	}
	else
	{
		return false;
	}
	
}

// release any baggage on pusher ramdomly
bool Pusher::RandomReleaseBaggage( const ElapsedTime& _tCurrentTime )
{
	typedef std::pair<Person*, int >pairPersonIdx;
	std::vector<pairPersonIdx>vAllBaggageOnPusher;

	int iSize = m_vServiceSlot.size();
	pairPersonIdx temp;
	for( int i=0; i<iSize; ++i )
	{
		Person* pBaggage = m_vServiceSlot[i].GetBaggageOnSlot();
		if( pBaggage )
		{
			temp.first = pBaggage;
			temp.second = i;
			vAllBaggageOnPusher.push_back( temp );
		}
	}

	int iCount = vAllBaggageOnPusher.size();
	if( iCount > 0 )
	{
		int iIdx = vAllBaggageOnPusher[random( iCount )].second;		
		Person* pBaggage = m_vServiceSlot[ iIdx ].GetBaggageOnSlot();
		TerminalMobElementBehavior* spTerminalBehavior = pBaggage->getTerminalBehavior();
		m_vServiceSlot[ iIdx ].SetBaggageOnSlot( NULL );

		pBaggage->writeLogEntry( _tCurrentTime, false );
		pBaggage->setState( LeaveServer );
		// can not generate event to let him to leave server just like nomal, because
		// such a situation will cause dest unavailable error,such as, now him generate a
		// event to go, (have not be added into next processor ) ,if 'now' a new baggage
		// is coming , it check there is at least one dest is avaiable, then it will leave 
		// server too, so error occurs
		spTerminalBehavior->processServerDeparture( _tCurrentTime );
		//pBaggage->generateEvent( _tCurrentTime );
		return true;
	}

	return false;
}
//to generate a max time checking event in order to release baggage which is on pusher more than max time
void Pusher::GenerateCheckMaxTimeEvent( Person* _pPerson, const ElapsedTime& _eventTime )
{
	PusherCheckMaxWaitTimeEvent* pEvent = new PusherCheckMaxWaitTimeEvent( this, _pPerson->getID() );
	pEvent->setTime( _eventTime );
	pEvent->addEvent();
}

// to generate a schedule event in order to release all baggage on pusher
void Pusher::GenerateScheduleEvent(  const ElapsedTime& _eventTime )
{
	PusherScheduleEvent* pEvent = new PusherScheduleEvent( this);
	pEvent->setTime( _eventTime );
	pEvent->addEvent();
}

//when a new baggage comes, firstly we should check if the pusher is full, 
//that is all slot are not empty, if it is full,  then release the first coming baggage, 
//and move all other baggages forward , finally, let the new baggage enter into the last service slot
void Pusher::ReleaseFirstBaggageIfFull( Person *_pPerson, const ElapsedTime& _eventTime )
{
	if( IsFull())	
	{
		int iSize = m_vServiceSlot.size();
		ASSERT( iSize > 0 );
	 
		Person* pBaggage = m_vServiceSlot[ iSize-2 ].GetBaggageOnSlot();
		if(pBaggage)
		{
			pBaggage->writeLogEntry( _eventTime, false );

			pBaggage->setTerminalDestination( m_vServiceSlot[ iSize-1 ].GetServicePoint().first );

			DistanceUnit dSpeed = _pPerson->getSpeed();
			pBaggage->setSpeed( m_dMovingSpeed );

			ElapsedTime nextEventTime = _eventTime + pBaggage->moveTime();
			pBaggage->writeLogEntry( nextEventTime, false );

			pBaggage->setState( LeaveServer );
			pBaggage->generateEvent( nextEventTime ,false);	
			pBaggage->setSpeed( dSpeed );
		}

		
		m_vServiceSlot[ iSize-2 ].SetBaggageOnSlot( NULL );
	}
}

//try to move forward  every baggage on pusher ,that is,
//for each baggage on pusher, if next service slot is empty, 
//then move forward, otherwise ,stay on current service slot
void Pusher::MoveForwardBaggageOnPusher( const ElapsedTime& _eventTime , int _iStartPos, int _iEndPos )
{
	int iSize = m_vServiceSlot.size();
	ASSERT( iSize > 0 );
	for( int i=_iEndPos -1 ,j=_iEndPos; i>=_iStartPos; --i ,--j )
	{
		if( m_vServiceSlot[j].GetBaggageOnSlot() == NULL && m_vServiceSlot[j].GetServicePoint().second >= 0 )
		{
			// from current slot to start slot, find the first  pos which is not empty
			int iPreFirstHasBaggage = j;
			while( iPreFirstHasBaggage >= _iStartPos )
			{
				if( m_vServiceSlot[iPreFirstHasBaggage].GetBaggageOnSlot() != NULL && m_vServiceSlot[iPreFirstHasBaggage].GetServicePoint().second >= 0 )
				{
					break;
				}
				iPreFirstHasBaggage--;
			}

			if( iPreFirstHasBaggage >= _iStartPos )
			{
				Person* pBaggage = m_vServiceSlot[iPreFirstHasBaggage].GetBaggageOnSlot();
				if( pBaggage )
				{
					pBaggage->writeLogEntry( _eventTime, false );
					m_vServiceSlot[iPreFirstHasBaggage].SetBaggageOnSlot( NULL );

					
					for( int s=iPreFirstHasBaggage+1; s<=j; ++s )
					{
						pBaggage->setTerminalDestination( m_vServiceSlot[s].GetServicePoint().first )	;
						pBaggage->writeLogEntry( _eventTime + m_vServiceSlot[s-1].GetTimeToForward(), false );						
						m_vServiceSlot[s].SetBaggageOnSlot( NULL );
					}
					
					m_vServiceSlot[j].SetBaggageOnSlot( pBaggage );					
				}
			}
			
		}
	}
}

//When ExceedMaxWaitTimeEvent comes, 
//we travel the whole service slots to find if the interst baggage is still on the pusher, 
//if yes, release it
void Pusher::ReleaseBaggageIfStillOnPusher( long _lWhichBaggage, const ElapsedTime& _tEventTime )
{
	int iSize = m_vServiceSlot.size();
	for( int i=0; i<iSize; ++i )
	{
		Person* pBaggage = m_vServiceSlot[i].GetBaggageOnSlot();
		if( pBaggage && pBaggage->getID() == _lWhichBaggage  )
		{
			pBaggage->writeLogEntry( _tEventTime, false );
			pBaggage->setState( LeaveServer );
			pBaggage->generateEvent( _tEventTime,false );

			m_vServiceSlot[i].SetBaggageOnSlot( NULL);
			return;
		}
	}
}

//check if there is no service slot can be available
bool Pusher::IsFull()const
{
	int iSize = m_vServiceSlot.size();
	for( int i=0; i<iSize; ++i )
	{
		if( m_vServiceSlot[i].GetBaggageOnSlot() == NULL && m_vServiceSlot[i].GetServicePoint().second >= 0 )
		{
			return false;
		}
	}

	return true;
}

// return the baggage count on pusher now
int Pusher::GetBaggageOnPusherCount()const
{
	int iTotalCount=0;
	int iSize = m_vServiceSlot.size();
	for( int i=0; i<iSize; ++i )
	{
		if( m_vServiceSlot[i].GetBaggageOnSlot() != NULL && m_vServiceSlot[i].GetServicePoint().second >= 0 )
		{
			++iTotalCount;
		}
	}
	return iTotalCount;
}

void Pusher::addPersonIntoApproachList( Person *_pPerson )
{
	m_approaching.addItem( _pPerson );
}

// release all baggage on pusher when schedule event comes
void Pusher::ReleaseAllBaggageOnPusher( const ElapsedTime& _tEventTime )
{
	int iSize = m_vServiceSlot.size();
	for( int i=0; i<iSize; ++i )
	{
		Person* pBaggage = m_vServiceSlot[i].GetBaggageOnSlot();
		if( pBaggage )
		{
			pBaggage->writeLogEntry( _tEventTime, false );
			pBaggage->setState( LeaveServer );
			pBaggage->generateEvent( _tEventTime,false );	
			
			m_vServiceSlot[i].SetBaggageOnSlot( NULL);
		}
	}
}
long Pusher::getLoad()const
{
	return GetBaggageOnPusherCount() + m_approaching.getCount();
}
void Pusher::writeAdditionLog( EntityEvents _eState, const ElapsedTime& _time, long _lReason )
{
	int iSize = m_vServiceSlot.size();
	for( int i=0; i<iSize; ++i )
	{
		Person* pBaggage = m_vServiceSlot[i].GetBaggageOnSlot();
		if( pBaggage )
		{
			pBaggage->setState( _eState );
			TerminalMobElementBehavior* spTerminalBehavior = pBaggage->getTerminalBehavior();
			if (spTerminalBehavior)
			{
				spTerminalBehavior->SetStopReason( _lReason );
			}
			
			pBaggage->writeLogEntry( _time, false );			
		}
	}
}

void Pusher::removePersonFromApproachList( const Person* _pPerson )
{
	int index = m_approaching.findItem (_pPerson);
	if(index != INT_MAX )
		m_approaching.removeItem (index);
}
