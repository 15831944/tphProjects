// ElevatorProc.cpp: implementation of the ElevatorProc class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ElevatorProc.h"
#include "ElevatorMoveEvent.h"
#include "inputs\miscproc.h"
#include "Inputs\procdata.h"
#include "engine\person.h"
#include "results\ElevatorLog.h"
#include "common\line.h"
#include "engine\procq.h"
#include "engine\terminal.h"
#include "TerminalMobElementBehavior.h"
#include "Common\FloorChangeMap.h"

#define  GAP_WIDTH_BETWWEN_WAITAREA_LIFTAREA 20
#define  GAP_WIDTH_BETWWEN_LIFTAREAS 20
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void DataOfEveryFloor::SetWaitArea( const Pollygon& _waitArea )
{
	m_waitArea.init( _waitArea.getCount(), _waitArea.getPointList() );
}

//return all lift areas and elevator waiting area ( for view drawing purpose );
void DataOfEveryFloor::GetLayoutData( std::vector<Pollygon>& _vLayout )
{
	_vLayout.clear();
	_vLayout = m_vLiftAreas;
	_vLayout.push_back( m_waitArea );
}
void DataOfEveryFloor::AddPersonIntoWaitList( Person* _pPerson )
{
	m_listPersonInWaitArea.push_back( _pPerson );
}
// set specific floor's stop flag
void DataOfEveryLift::SetStopFlag( int _iWhichFloor, bool _bFlag, bool _bUp )
{
	if( _bUp )
	{
		if( m_mapUpStopFlags.find( _iWhichFloor ) != m_mapUpStopFlags.end() )
		{
			m_mapUpStopFlags[ _iWhichFloor ] = _bFlag;
		}
	}
	else
	{
		if( m_mapDownStopFlags.find( _iWhichFloor ) != m_mapDownStopFlags.end() )
		{
			m_mapDownStopFlags[ _iWhichFloor ] = _bFlag;
		}
	}
}
bool DataOfEveryLift::GetStopFlag( int _iWhichFloor , bool _bUp)
{
	if( _bUp )
	{
		if( m_mapUpStopFlags.find( _iWhichFloor ) != m_mapUpStopFlags.end() )
		{
			return m_mapUpStopFlags[ _iWhichFloor ];
		}
		else
		{
			return false;
		}
	}
	else
	{
		if( m_mapDownStopFlags.find( _iWhichFloor ) != m_mapDownStopFlags.end() )
		{
			return m_mapDownStopFlags[ _iWhichFloor ];
		}
		else
		{
			return false;
		}
	}
}
void DataOfEveryLift::CaculateEntryExitPoint(  ElevatorProc* _pElevator  , int _iLiftIdx, int _iFloor )
{
	Pollygon liftArea = _pElevator->GetDataAtFloor( _iFloor ).GetLiftAreas().at( _iLiftIdx );
	Point ptEntryExitPoint;
	if( _pElevator->GetPosOfWaitArea( _iFloor ) )
	{
		Line tempLine(liftArea.getPoint( 0 ), liftArea.getPoint( 3 ) );
		tempLine.GetCenterPoint( ptEntryExitPoint );		
	}
	else
	{
		Line tempLine(liftArea.getPoint( 1 ), liftArea.getPoint( 2 ) );
		tempLine.GetCenterPoint( ptEntryExitPoint );		
	}
	m_mapEntryExitPoint[ _iFloor ] = ptEntryExitPoint;

}
// init stop flags
void DataOfEveryLift::InitLiftData( std::vector<int>& _vStopAtFloors ,std::vector<ElapsedTime>& _vNecessaryTime ,Terminal* _pTerm, ElevatorProc* _pElevator, int _iLiftIdx)
{
	m_pTerm = _pTerm;
	m_listPersonInLift.clear();
	m_mapUpStopFlags.clear();
	m_mapDownStopFlags.clear();
	m_mapEntryExitPoint.clear();

	m_vNecessaryTimeMoveToNextFloor = _vNecessaryTime;
	int iSize = _vStopAtFloors.size();
	for(int i=0; i<iSize; ++i )
	{
		m_mapUpStopFlags[ _vStopAtFloors[i] ] = false;
		m_mapDownStopFlags[ _vStopAtFloors[i] ] = false;		
		CaculateEntryExitPoint(_pElevator, _iLiftIdx, _vStopAtFloors[i] );
	}
}

//reset stop at flag of every lift according to current context
void DataOfEveryLift::UpdataLiftStopFlags( ElevatorProc* _pElevator , int _iLift )
{
	std::list<Person*>& waitList = _pElevator->GetDataAtFloor( m_iCurrentFloor ).GetPersonInElevatorWaitArea();
	int iSize = waitList.size();
	if( iSize == 0 )
	{
		_pElevator->GetDataOfLift( _iLift ).SetStopFlag( m_iCurrentFloor, false, true );
		_pElevator->GetDataOfLift( _iLift ).SetStopFlag( m_iCurrentFloor, false, false );
	}


	/*
	 *	random set stop flags , only for the unit test purpose. must be removed later
	 */
/*	
#ifdef _DEBUG
	TraceStopFlags();
	std::ofstream elevatorLogs( "Stop_at_elevator.log", std::ios::app );
	int iSize = m_mapUpStopFlags.size();
	int iIdx = random( iSize );
	std::map<int,bool>::iterator iter = m_mapUpStopFlags.begin();
	i=0;
	while( i != iIdx )
	{
		++i;
		++iter;
	}
	iter->second = true;
	elevatorLogs<<" random set up stop flag at : " << iter->first << " floor \n";

	iIdx = random( iSize );
	iter = m_mapDownStopFlags.begin();
	i=0;
	while( i != iIdx )
	{
		++i;
		++iter;
	}
	iter->second = true;
	elevatorLogs<<" random set down stop flag at : " << iter->first << " floor \n";
	elevatorLogs.close();

#endif
  */
	
}
#ifdef _DEBUG
void DataOfEveryLift::TraceStopFlags()
{
/*	std::ofstream elevatorLogs( "Stop_at_elevator.log", std::ios::app );
		elevatorLogs <<"*************begin****************" <<"\n";
		elevatorLogs << " Up Stops flags \n";
		std::map<int,bool>::iterator iterCurrent = m_mapUpStopFlags.begin();
		for( ; iterCurrent != m_mapUpStopFlags.end() ; ++iterCurrent )
		{
			elevatorLogs <<" floor : "<< iterCurrent->first <<" flag : " <<iterCurrent->second <<"\n";
		}

		elevatorLogs << " down Stops flags \n";
		iterCurrent = m_mapDownStopFlags.begin();
		for( ; iterCurrent != m_mapDownStopFlags.end() ; ++iterCurrent )
		{
			elevatorLogs <<" floor : "<< iterCurrent->first <<" flag : " <<iterCurrent->second <<"\n";
		}
		
		
		elevatorLogs <<"**************end***************" <<"\n";
	elevatorLogs.close();
*/
}
#endif
// move all person who stay in lift and his destination is current floor out of lift
void DataOfEveryLift::MovePersonOutElevator( const ElapsedTime& _currentTime ,int _iLift , ElevatorProc* _pElevator, ElapsedTime& _maxTimeOfPersonMovingOut )
{
	const Pollygon& waitArea = _pElevator->GetDataAtFloor( m_iCurrentFloor ).GetWaitArea();	
	int iSize = m_listPersonInLift.size();
	if( iSize > 0 )
	{

		Point ptEntryExitPoint = m_mapEntryExitPoint[ m_iCurrentFloor ];
		ptEntryExitPoint.setZ( m_iCurrentFloor * SCALE_FACTOR );

		while( iSize -- )
		{
			Person* pPerson = m_listPersonInLift.front();
			TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
			m_listPersonInLift.pop_front();
			if (spTerminalBehavior == NULL)
			{
				continue;
			}

			if( spTerminalBehavior->GetDestFloor() == m_iCurrentFloor )
			{			
				spTerminalBehavior->inElevotor =false;
				pPerson->setTerminalDestination(ptEntryExitPoint);
				ElapsedTime personExitTime = pPerson->moveTime();
				pPerson->writeLogEntry( personExitTime + _currentTime, false );

				Point ptExit = waitArea.getRandPoint();
				pPerson->setTerminalDestination( ptExit );
				ElapsedTime personMoveTime = pPerson->moveTime();
				pPerson->writeLogEntry( personMoveTime + _currentTime + personExitTime, false );
				pPerson->setState( LeaveServer );
				pPerson->generateEvent( personMoveTime + _currentTime + personExitTime,false );

				if( _maxTimeOfPersonMovingOut < personMoveTime + personExitTime  )
				{
					_maxTimeOfPersonMovingOut = personMoveTime + personExitTime;
				}

			}
			else
			{
				m_listPersonInLift.push_back( pPerson );
				spTerminalBehavior->inElevotor =true;
			}
		}

		WriteLog( _currentTime + _maxTimeOfPersonMovingOut );
	}
	
}
// move all person who wait for this lift into elevator
void DataOfEveryLift::MovePersonIntoElevator(  const ElapsedTime& _currentTime ,int _iLift, ElevatorProc* _pElevator , ElapsedTime& _maxTimeOfPersonMovingIn )
{
	std::list<Person*>& waitList = _pElevator->GetDataAtFloor( m_iCurrentFloor ).GetPersonInElevatorWaitArea();
	int iSize = waitList.size();
	if( iSize > 0 )
	{	

		int iFirstStopFloor = m_mapUpStopFlags.begin()->first ;
		int iLastStopFloor = m_mapUpStopFlags.rbegin()->first;
		

		//Update First and last floor's stop info according to current state:
		
		std::map<int,bool>::iterator iter = m_mapUpStopFlags.begin(),
									 iterLastPos = m_mapUpStopFlags.end(),
									 iterFirstPos = m_mapUpStopFlags.begin();

		bool bInitFirstPos = true;
		while (iter!= m_mapUpStopFlags.end())
		{
			if (iter->second)
			{
				//find first stop pos
				if (bInitFirstPos == true)
				{
					bInitFirstPos = false;
					iterFirstPos = iter;
				}
				//current last stop pos
				iterLastPos = iter;
			}
			iter++;
		}
		
		iFirstStopFloor = iterFirstPos->first ;
		if(iterLastPos!=m_mapUpStopFlags.end())
			iLastStopFloor = iterLastPos->first;

		Point ptEntryExitPoint = m_mapEntryExitPoint[ m_iCurrentFloor ];
		ptEntryExitPoint.setZ( m_iCurrentFloor * SCALE_FACTOR );
		while( iSize -- )
		{
			Person* pPerson = waitList.front();
			// check if exceed lift's max capacity
			//int iCurrentPersonCount = GetPersonCountInLift();
			//iCurrentPersonCount += pPerson->GetActiveTotalSize();
			//if( iCurrentPersonCount > _pElevator->GetCapacity() )
			//{				
			//	break;
			//}

			if( !_pElevator->canEnterLift(pPerson, m_listPersonInLift))
				break;

			
			waitList.pop_front();

			TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
			if (spTerminalBehavior == NULL)
			{
				continue;
			}
			int iDirection = spTerminalBehavior->GetDestFloor() - spTerminalBehavior->GetSourceFloor();
			if( iDirection > 0 )// want to "up"
			{
				if( m_enumDirection == UP  || m_iCurrentFloor == iFirstStopFloor )
				{
					pPerson->writeLogEntry( _currentTime, false );

					// entry the mid point of liftarea entry line
					pPerson->setTerminalDestination(ptEntryExitPoint);
					ElapsedTime entryTime = pPerson->moveTime();
					pPerson->writeLogEntry( entryTime + _currentTime, false );
					
					//pPerson->writeLogEntry( _currentTime );

					// let person move into lift area
					Point ptEntry = _pElevator->GetDataAtFloor( m_iCurrentFloor ).GetLiftAreas().at( _iLift ).getRandPoint();					
					ptEntry.setZ( m_iCurrentFloor * SCALE_FACTOR );
					pPerson->setTerminalDestination(ptEntry);
					ElapsedTime personMoveTime = pPerson->moveTime();
					pPerson->writeLogEntry( personMoveTime + _currentTime +entryTime, false );
					if( _maxTimeOfPersonMovingIn < personMoveTime + entryTime )
					{
						_maxTimeOfPersonMovingIn = personMoveTime + entryTime ;
					}

					// must also set person's dest floor's flag as true;
					SetStopFlag( spTerminalBehavior->GetDestFloor(), true, true );

					m_listPersonInLift.push_back( pPerson );
					spTerminalBehavior->inElevotor =true;
				}
				else
				{
					waitList.push_back( pPerson );
					spTerminalBehavior->inElevotor = false;
				}
			}
			else if( iDirection < 0 )//want to down
			{
				if( m_enumDirection == DOWN || m_iCurrentFloor == iLastStopFloor )
				{
					pPerson->writeLogEntry( _currentTime, false );
					
					// entry the mid point of liftarea entry line
					pPerson->setTerminalDestination(ptEntryExitPoint);
					ElapsedTime entryTime = pPerson->moveTime();
					pPerson->writeLogEntry( entryTime + _currentTime, false );

					// let person move into lift area
					Point ptEntry = _pElevator->GetDataAtFloor( m_iCurrentFloor ).GetLiftAreas().at( _iLift ).getRandPoint();					
					ptEntry.setZ( m_iCurrentFloor * SCALE_FACTOR );
					pPerson->setTerminalDestination(ptEntry);
					ElapsedTime personMoveTime = pPerson->moveTime();
					pPerson->writeLogEntry( personMoveTime + _currentTime + entryTime, false);

					if( _maxTimeOfPersonMovingIn < personMoveTime +entryTime)
					{
						_maxTimeOfPersonMovingIn = personMoveTime + entryTime ;
					}


					// must also set person's dest floor's flag as true;
					SetStopFlag( spTerminalBehavior->GetDestFloor(), true, false );
					m_listPersonInLift.push_back( pPerson );
					spTerminalBehavior->inElevotor =true;
				}
				else
				{
					waitList.push_back( pPerson );
					spTerminalBehavior->inElevotor =false;
				}
			}
			/* have handled this situation when person want to use elevator 
			else
			{
				// should throw exception
			}
			*/
		}

		WriteLog( _currentTime + _maxTimeOfPersonMovingIn );
	}
}
int DataOfEveryLift::GetPersonCountInLift()const
{
	std::list<Person*>::const_iterator iter = m_listPersonInLift.begin();
	std::list<Person*>::const_iterator iterEnd = m_listPersonInLift.end();
	int iSize = 0;
	for( ; iter != iterEnd; ++iter )
	{
		iSize += (*iter)->GetActiveTotalSize();
	}
	return iSize;
}
ElapsedTime DataOfEveryLift::GetNecessaryTimeToNextFloor( int _iPreFloor, int _iNextFloor )const
{
	ASSERT( _iPreFloor != _iNextFloor );
	ASSERT( _iPreFloor >=0 && (UINT)_iPreFloor < m_vNecessaryTimeMoveToNextFloor.size() + 1 );
	ASSERT( _iNextFloor >=0 && (UINT)_iNextFloor < m_vNecessaryTimeMoveToNextFloor.size() + 1 );
	if( _iPreFloor < _iNextFloor )
	{
		return m_vNecessaryTimeMoveToNextFloor[ _iPreFloor ];
	}
	else //( _iPreFloor > _iNextFloor )
	{
		return m_vNecessaryTimeMoveToNextFloor[ _iNextFloor ];
	}
	
}
//move lift to next floor to service person . if no person wait for service at all floor, then lift will halt
void DataOfEveryLift::MoveToNextFloor( const ElapsedTime& _currentTime , int _iLift, ElevatorProc* _pElevator )
{
	#ifdef _DEBUG
		TraceStopFlags();
	#endif

	WriteLog( _currentTime );
	int iPreFloor = m_iCurrentFloor;

	if( ShouldStopAtCurrentFloor() )
	{
		ElapsedTime tempTimeMoveOut;
		// move person out elevator ....;
		MovePersonOutElevator( _currentTime, _iLift, _pElevator, tempTimeMoveOut );

		// move person into elevator....;
		ElapsedTime tempTimeMoveIn;
		MovePersonIntoElevator( _currentTime, _iLift, _pElevator , tempTimeMoveIn );

		UpdataLiftStopFlags( _pElevator, _iLift );

		if( RefreshCurrentFloor() )
		{
			GenerateEvent( _currentTime + tempTimeMoveIn + tempTimeMoveOut + GetNecessaryTimeToNextFloor( iPreFloor, m_iCurrentFloor ), _iLift, _pElevator );
		}
	}
	else
	{

		if( RefreshCurrentFloor() )
		{
			GenerateEvent( _currentTime + GetNecessaryTimeToNextFloor( iPreFloor, m_iCurrentFloor ), _iLift, _pElevator );					

		}
		
	}

	#ifdef _DEBUG
		TraceStopFlags();
	#endif

}

// reset current floor and lift's status ( such as, UP, Down, or halt )
/*
   up					down
   |	^			|	|
   |	|			|	|
   |	^A			|	|B
   |	|			|	|
   |	^			|	|
 ----------------------------- current floor
   |	|			|	|
   |	^			|	|
   |	|D			|	|C
   |	^			|	|
   |	|			|	|
   |	^			|	|
   |	|			|	|
   |	^			|	|
   |	|			|	|
 *	
 */
bool DataOfEveryLift::RefreshCurrentFloor()
{
	if( m_enumDirection == UP )
	{
		std::map<int,bool>::iterator iterCurrent = m_mapUpStopFlags.find( m_iCurrentFloor );
		std::map<int,bool>::iterator iter = iterCurrent;
		ASSERT( iter != m_mapUpStopFlags.end() );
		bool bStillMoveUp = false;

		// check Area "A" 
		//someone need go UP,need pick them and then diliver them Higher more
		while ( ++iter != m_mapUpStopFlags.end() )
		{
			if( iter->second )
			{
				bStillMoveUp = true;
				break;
			}
		}
		if( bStillMoveUp )
		{
			++iterCurrent;
			m_iCurrentFloor = iterCurrent->first;
			return true;
		}

		//Check Area "B" 
		//someone need DOWN,need go on UP to pick item first
		iterCurrent = m_mapDownStopFlags.find( m_iCurrentFloor );
		iter = iterCurrent;
		ASSERT( iter != m_mapDownStopFlags.end() );
		while ( ++iter != m_mapDownStopFlags.end() )
		{
			if( iter->second )
			{
				bStillMoveUp = true;
				break;
			}
		}
		if( bStillMoveUp )
		{
			++iterCurrent;
			m_iCurrentFloor = iterCurrent->first;
			return true;
		}

		bool bMoveDown = false;
		//check Area "C"
		iterCurrent = m_mapDownStopFlags.find( m_iCurrentFloor );
		iter = m_mapDownStopFlags.begin();
		while ( iter != iterCurrent )
		{
			if( iter->second )
			{
				bMoveDown = true;
				break;
			}
			++iter;
		}	
		if( bMoveDown )
		{
			--iterCurrent;
			m_iCurrentFloor = iterCurrent->first;
			m_enumDirection = DOWN ;
			return true;
		}

		//check Area "D"
		iterCurrent = m_mapUpStopFlags.find( m_iCurrentFloor );
		iter = m_mapUpStopFlags.begin();
		while ( iter != iterCurrent )
		{
			if( iter->second )
			{
				bMoveDown = true;
				break;
			}
			++iter;
		}	
		if( bMoveDown )
		{
			--iterCurrent;
			m_iCurrentFloor = iterCurrent->first;
			m_enumDirection = DOWN ;
			return true;
		}

		m_enumDirection = HALT ;
		return false;

	}
	else if( m_enumDirection == DOWN )
	{
		std::map<int,bool>::iterator iterCurrent = m_mapDownStopFlags.find( m_iCurrentFloor );
		std::map<int,bool>::iterator iter = m_mapDownStopFlags.begin();
		ASSERT( iter != m_mapDownStopFlags.end() );
		bool bStillMoveDown = false;

		// check Area "C"
		while ( iter != iterCurrent )
		{
			if( iter->second )
			{
				bStillMoveDown = true;
				break;
			}
			++iter;
		}	
		if( bStillMoveDown )
		{
			--iterCurrent;
			m_iCurrentFloor = iterCurrent->first;			
			return true;
		}
		

		//check Area "D"
		iterCurrent = m_mapUpStopFlags.find( m_iCurrentFloor );
		iter = m_mapUpStopFlags.begin();
		while ( iter != iterCurrent )
		{
			if( iter->second )
			{
				bStillMoveDown = true;
				break;
			}
			++iter;
		}	
		if( bStillMoveDown )
		{
			--iterCurrent;
			m_iCurrentFloor = iterCurrent->first;			
			return true;
		}

	
		bool bMoveUp = false;	
		iterCurrent = m_mapUpStopFlags.find( m_iCurrentFloor );
		iter = iterCurrent;
		// check Area "A"
		while ( ++iter != m_mapUpStopFlags.end() )
		{
			if( iter->second )
			{
				bMoveUp = true;
				break;
			}
		}
		if( bMoveUp )
		{
			++iterCurrent;
			m_iCurrentFloor = iterCurrent->first;
			m_enumDirection = UP ;
			return true;
		}

		//Check Area "B"
		iterCurrent = m_mapDownStopFlags.find( m_iCurrentFloor );
		iter = iterCurrent;
		ASSERT( iter != m_mapDownStopFlags.end() );
		while ( ++iter != m_mapDownStopFlags.end() )
		{
			if( iter->second )
			{
				bMoveUp = true;
				break;
			}
		}
		if( bMoveUp )
		{
			++iterCurrent;
			m_iCurrentFloor = iterCurrent->first;
			m_enumDirection = UP ;
			return true;
		}

		m_enumDirection = HALT ;
		return false;
	}

	m_enumDirection = HALT ;
	return false;
}

// generate a new elevator move event
void DataOfEveryLift::GenerateEvent( const ElapsedTime& _currentTime , int _iLift, ElevatorProc* _pElevator )
{
	ElevatorMoveEvent* pEvent = new ElevatorMoveEvent( _pElevator, _iLift );
	pEvent->setTime( _currentTime );
	pEvent->addEvent();
}

// write event logs
void DataOfEveryLift::WriteLog( const ElapsedTime& _currentTime )
{
	/* write elevator's log */
	ElevatorEventStruct tempEvent;
	tempEvent.time = (long)_currentTime;
	tempEvent.x = (float)m_ptLogPoint.getX();
	tempEvent.y = (float)m_ptLogPoint.getY();
	tempEvent.z = (short)(m_iCurrentFloor * SCALE_FACTOR) ;
	//tempEvent.state = 

	CString csStr;
	csStr.Format( "%s, floor=%d", _currentTime.printTime(), m_iCurrentFloor );

	m_logOfLift.addEvent( tempEvent );

/*
	long trackCount = m_logOfLift.getCurrentCount();
	ElevatorEventStruct *log = NULL;
    m_logOfLift.getLog (log);
    m_logOfLift.setEventList (log, trackCount);	
	delete [] log;
	log = NULL;
*/

	/* write all person in lift logs */
	WritePersonInLiftLog( _currentTime );

/*
#ifdef _DEBUG
	char timeData[32];
	_currentTime.printTime( timeData );
	std::ofstream elevatorLogs( "x_test_elevator.log", std::ios::app );
		elevatorLogs << " Time = " << timeData << " x ,y, z = " << tempEvent.x 
			<<" , "<< tempEvent.y <<" , "<< tempEvent.z <<"\n";
	elevatorLogs.close();
#endif
*/
}

/* write all person in lift logs */
void DataOfEveryLift::WritePersonInLiftLog( const ElapsedTime& _currentTime )
{
	std::list<Person*>::iterator iter = m_listPersonInLift.begin();
	std::list<Person*>::iterator iterEnd = m_listPersonInLift.end();
	for( ; iter != iterEnd; ++iter )
	{
		Point ptPreDest;
		(*iter)->getTerminalDest(ptPreDest);
		ptPreDest.setZ( m_iCurrentFloor * SCALE_FACTOR );
		(*iter)->setTerminalDestination( ptPreDest );
		(*iter)->setTerminalLocation( ptPreDest );
		(*iter)->writeLogEntry( _currentTime, false );
	}
}

// test if this lift should stop at current floor to provide service
bool DataOfEveryLift::ShouldStopAtCurrentFloor( )
{
	if( m_enumDirection == UP )
	{
		if( m_mapUpStopFlags.find( m_iCurrentFloor ) != m_mapUpStopFlags.end() )
		{
			return m_mapUpStopFlags[ m_iCurrentFloor ];
		}
		else
		{
			return false;
		}
	}
	
	else if( m_mapDownStopFlags.find( m_iCurrentFloor ) != m_mapDownStopFlags.end() )
	{
		return m_mapDownStopFlags[ m_iCurrentFloor ];
	}
	
	return false;
}
/*
bool DataOfEveryLift::ShouldMoveTo( int _iCurrentFloor, bool _bHeadingToTop, bool _bCheckUpStopFlags )
{
	if( _bCheckUpStopFlags )
	{
		if( _bHeadingToTop )
		{
			std::map<int,bool>::reverse_iterator iter = m_mapUpStopFlags.rbegin();
			std::map<int,bool>::reverse_iterator iterEnd = m_mapUpStopFlags.rend();
			for( ; iter != iterEnd && iter->first != _iCurrentFloor ; ++iter )
			{
				if( iter->second )
				{
					return true;
				}
			}
			return false;
		}
		else
		{
			std::map<int,bool>::iterator iter = m_mapUpStopFlags.begin();
			std::map<int,bool>::iterator iterEnd = m_mapUpStopFlags.end();
			for( ; iter != iterEnd && iter->first != _iCurrentFloor ; ++iter )
			{
				if( iter->second )
				{
					return true;
				}
			}
			return false;
		}
	}
	else
	{
		if( _bHeadingToTop )
		{
			std::map<int,bool>::reverse_iterator iter = m_mapDownStopFlags.rbegin();
			std::map<int,bool>::reverse_iterator iterEnd = m_mapDownStopFlags.rend();
			for( ; iter != iterEnd && iter->first != _iCurrentFloor ; ++iter )
			{
				if( iter->second )
				{
					return true;
				}
			}

			return false;
		}
		else
		{
			std::map<int,bool>::iterator iter = m_mapDownStopFlags.begin();
			std::map<int,bool>::iterator iterEnd = m_mapDownStopFlags.end();
			for( ; iter != iterEnd && iter->first != _iCurrentFloor ; ++iter )
			{
				if( iter->second )
				{
					return true;
				}
			}

			return false;
		}
	}
}
*/
ElevatorProc::ElevatorProc()
{
	m_dWaitareaLength = 400.0;
	m_dWaitareaWidth = 150.0;
	m_dLiftAreaLength = 200.0;
	m_dLiftAreaWidth = 150.0;
	m_iNumberOfLift = 1;
	m_dOrientation = 0.0;
	m_iCapacity= 0;
}

ElevatorProc::~ElevatorProc()
{

}
void ElevatorProc::SetPosOfWaitArea( int _iWhichFloor, bool _bLeft )
{
	ASSERT( m_iMinFloor <= _iWhichFloor && m_iMaxFloor >= _iWhichFloor );
	m_vPosOfWaitarea[ _iWhichFloor - m_iMinFloor ] = _bLeft;
}
bool ElevatorProc::GetPosOfWaitArea( int _iWhichFloor )const 
{
	ASSERT( m_iMinFloor <= _iWhichFloor && m_iMaxFloor >= _iWhichFloor );
	return m_vPosOfWaitarea[ _iWhichFloor - m_iMinFloor ] ;
}
//caculate the proc's layout on every floor ( include the position and shap of lift area, elevator waiting area)
void ElevatorProc::InitLayout()
{
	m_vAllFloorsData.clear();

	Pollygon waitArea;
	Pollygon elevatorArea;

	ASSERT( m_location.getCount() );

	Point ptService = m_location.getPoint( 0 );
	Point ptArray[ 4 ];

	if( m_iNumberOfLift % 2 == 0 )
	{
		int iHalfNum = m_iNumberOfLift / 2 ;
		ptArray[0].setX( ptService.getX() - ( iHalfNum - 1 + 0.5 )*GAP_WIDTH_BETWWEN_LIFTAREAS - ( iHalfNum -1 )*m_dLiftAreaWidth );
		ptArray[0].setY( ptService.getY() );

		ptArray[1].setX( ptArray[0].getX() );
		ptArray[1].setY( ptService.getY() + m_dLiftAreaWidth  );

		ptArray[2].setX( ptArray[1].getX() - m_dLiftAreaLength  );
		ptArray[2].setY( ptArray[1].getY()  );

		ptArray[3].setX( ptArray[2].getX() );
		ptArray[3].setY( ptArray[0].getY() );
		elevatorArea.init( 4, ptArray );
		
	}
	else
	{
		if( m_iNumberOfLift == 1 )
		{
			ptArray[0].setX( ptService.getX() + m_dLiftAreaWidth/2 );
			ptArray[0].setY( ptService.getY() );

			ptArray[1].setX( ptArray[0].getX() );
			ptArray[1].setY( ptService.getY() + m_dLiftAreaWidth  );

			ptArray[2].setX( ptArray[1].getX() - m_dLiftAreaLength  );
			ptArray[2].setY( ptArray[1].getY()  );

			ptArray[3].setX( ptArray[2].getX() );
			ptArray[3].setY( ptArray[0].getY() );
		}
		else
		{
			int iHalfNum = m_iNumberOfLift / 2 ;
			ptArray[0].setX( ptService.getX() - iHalfNum*GAP_WIDTH_BETWWEN_LIFTAREAS - m_dLiftAreaLength /2 -( iHalfNum -1 )*m_dLiftAreaLength );
			ptArray[0].setY( ptService.getY() );

			ptArray[1].setX( ptArray[0].getX() );
			ptArray[1].setY( ptService.getY() + m_dLiftAreaWidth  );

			ptArray[2].setX( ptArray[1].getX() - m_dLiftAreaLength  );
			ptArray[2].setY( ptArray[1].getY()  );

			ptArray[3].setX( ptArray[2].getX() );
			ptArray[3].setY( ptArray[0].getY() );

		}

		elevatorArea.init( 4, ptArray );
	}

	std::vector<Pollygon>vLiftWaitAreas;
	for( int i=0; i<m_iNumberOfLift; ++i )
	{
		vLiftWaitAreas.push_back( elevatorArea );
		elevatorArea.DoOffset(GAP_WIDTH_BETWWEN_WAITAREA_LIFTAREA + m_dLiftAreaLength ,0.0, 0.0 );
	}
	
	DataOfEveryFloor tempData;
	

	ptArray[0].setX( ptService.getX() - m_dWaitareaLength / 2 );
	ptArray[0].setY( ptService.getY() -  m_dWaitareaWidth - GAP_WIDTH_BETWWEN_WAITAREA_LIFTAREA );

	ptArray[1].setX( ptService.getX() - m_dWaitareaLength / 2 );
	ptArray[1].setY( ptService.getY() - GAP_WIDTH_BETWWEN_WAITAREA_LIFTAREA );

	ptArray[2].setX( ptService.getX() + m_dWaitareaLength / 2 );
	ptArray[2].setY( ptService.getY() - GAP_WIDTH_BETWWEN_WAITAREA_LIFTAREA );

	ptArray[3].setX( ptService.getX() + m_dWaitareaLength / 2 );
	ptArray[3].setY( ptService.getY() - m_dWaitareaWidth - GAP_WIDTH_BETWWEN_WAITAREA_LIFTAREA );
	

	ASSERT( m_vPosOfWaitarea.size() == m_iMaxFloor - m_iMinFloor + 1 );
	for(int i=0; i<= m_iMaxFloor - m_iMinFloor; ++i )
	{
		bool bWaitAreaInfrontLift = m_vPosOfWaitarea[i] ;
		std::vector<Point>vLiftDoors;
		int iSize = vLiftWaitAreas.size();
		for( int j=0; j<iSize; ++j )
		{
			int iPointCount = vLiftWaitAreas[j].getCount();
			Point* pPoint = vLiftWaitAreas[j].getPointList();
			for( int k=0; k<iPointCount; ++k )
			{
				pPoint[k].setZ( (m_iMinFloor + i) * SCALE_FACTOR );
			}

			// caculate the location point of lift's door 
			Point ptEntryExitPoint;
			if( bWaitAreaInfrontLift )
			{
				Line tempLine(pPoint[0], pPoint[3]);
				tempLine.GetCenterPoint( ptEntryExitPoint );		
			}
			else
			{
				Line tempLine(pPoint[1], pPoint[2]);
				tempLine.GetCenterPoint( ptEntryExitPoint );		
			}

			vLiftDoors.push_back( ptEntryExitPoint );
		}

		tempData.SetLiftDoors( vLiftDoors );
		tempData.SetLiftAreas( vLiftWaitAreas );

		Point ptEntryExitPoint;	
		waitArea.init( 4, ptArray );
		if( !bWaitAreaInfrontLift )
		{
			waitArea.DoOffset( 0.0, GAP_WIDTH_BETWWEN_WAITAREA_LIFTAREA*2 + m_dLiftAreaWidth + m_dWaitareaWidth ,0.0 );
		}
	
		waitArea.DoOffset( 0.0,0.0, (m_iMinFloor + i) * SCALE_FACTOR  );
		tempData.SetWaitArea( waitArea );

		m_vAllFloorsData.push_back( tempData );
	}


	Rotate( m_dOrientation );
}

int ElevatorProc::GetFloorCount()const
{
	ASSERT( m_vAllFloorsData.size() == m_vPosOfWaitarea.size() );
	ASSERT( m_vAllFloorsData.size() == m_iMaxFloor - m_iMinFloor + 1 );
	return m_vAllFloorsData.size();
}

// get related data of floor ( note: m_iMinFloor <= _iWhichFloor <= m_iMaxFloor )
DataOfEveryFloor& ElevatorProc::GetDataAtFloor( int _iWhichFloor )
{
	if( m_iMinFloor > _iWhichFloor || m_iMaxFloor < _iWhichFloor )
	{
		throw new ARCLackOfNecessaryInfo( "elevator processor need necessary data which should be defined in processor behavior  " + getID()->GetIDString() );
	}
	//ASSERT( m_iMinFloor <= _iWhichFloor && m_iMaxFloor >= _iWhichFloor );
	//// TRACE ("\n%d\n", m_vAllFloorsData.size() );
	return m_vAllFloorsData[ _iWhichFloor - m_iMinFloor ];
}

int ElevatorProc::readSpecialField(ArctermFile& procFile)
{
	procFile.getFloat( m_dWaitareaLength );
	procFile.getFloat( m_dWaitareaWidth );
	procFile.getFloat( m_dLiftAreaLength );
	procFile.getFloat( m_dLiftAreaWidth );

	procFile.getInteger( m_iNumberOfLift );
	procFile.getInteger( m_iMinFloor );
	procFile.getInteger( m_iMaxFloor );

	int iSize = 0 ;
	procFile.getInteger( iSize );
	for( int i=0; i<iSize; ++i )
	{
		int iValue=0;
		procFile.getInteger( iValue );
		if( iValue )
		{
			m_vPosOfWaitarea.push_back( true );
		}
		else
		{
			m_vPosOfWaitarea.push_back( false);
		}
	}
	if( !procFile.getFloat( m_dOrientation ) )
	{
		m_dOrientation = 0.0;
	}
	return TRUE;
}
int ElevatorProc::writeSpecialField(ArctermFile& procFile)const
{
	procFile.writeFloat( (float)m_dWaitareaLength );
	procFile.writeFloat( (float)m_dWaitareaWidth );
	procFile.writeFloat( (float)m_dLiftAreaLength );
	procFile.writeFloat( (float)m_dLiftAreaWidth );
	
	procFile.writeInt( m_iNumberOfLift );
	procFile.writeInt( m_iMinFloor );
	procFile.writeInt( m_iMaxFloor );
	
	int iSize = m_vPosOfWaitarea.size();
	procFile.writeInt( iSize );
	for( int i=0; i<iSize; ++i )
	{
		if( m_vPosOfWaitarea[i] )
		{
			procFile.writeInt( 1 );
		}
		else
		{
			procFile.writeInt( 0 );
		}
	}

	procFile.writeFloat((float)m_dOrientation );
	return TRUE;
}

// process elevator move event 
void ElevatorProc::ElevatorMoveEventComes( int _iLift ,const ElapsedTime& _currentTime )
{
//	ASSERT( _iLift >= 0 && (UINT)_iLift < m_vAllLiftData.size() );

	m_vAllLiftData[ _iLift ].MoveToNextFloor( _currentTime , _iLift, this );

}

DataOfEveryLift& ElevatorProc::GetDataOfLift( int _iLift )
{
	ASSERT( _iLift >= 0 && (UINT)_iLift < m_vAllLiftData.size() );
	return m_vAllLiftData[ _iLift ];
}

// init all lift data of elevator  ( such as the log point of lift ,lift's stop flags )
bool ElevatorProc::InitElevatorData( std::vector<double>& _vFloorsAltitude ,Terminal* _pTerm)
{
	// should throw exception to notify user define necessary data
	if(m_vStopAtFloors.size() <= 0 )
	{
// 		FORMATMSG fmsg;
// 		fmsg.strProcessor = getID()->GetIDString();
// 		fmsg.strMsgType = "elevator processor need necessary data which should be defined in processor behavior  ";
// 		_pTerm->SendSimFormatMessage(fmsg);
// 		return false;
		MiscData* pMiscData = new MiscElevatorData;
		((MiscElevatorData*)pMiscData)->clearStopAtFloor();
		for(int i=0;i<m_iMaxFloor+1;i++)
		{
			BOOL bStop = FALSE;
			if (i <= m_iMaxFloor && i >= m_iMinFloor)
				bStop = TRUE;

			((MiscElevatorData*)pMiscData)->addStopAtFloor(bStop);
		}
		initSpecificMisc(pMiscData);
	}
	std::vector<ElapsedTime>vNecessaryTime;
	int iSize = _vFloorsAltitude.size();
	for( int j=1, k=0; j<iSize ; ++k,++j )
	{
		double dTime = (_vFloorsAltitude[j] - _vFloorsAltitude[k] ) / ( m_dSpeed *SCALE_FACTOR ); 
		vNecessaryTime.push_back( ElapsedTime( dTime ) );
	}

	m_vAllLiftData.clear();
	for( int i=0; i<m_iNumberOfLift; ++i )
	{
		DataOfEveryLift tempData;
		ASSERT( m_vAllFloorsData.size() > 0 );		
		tempData.SetLogPoint( m_vAllFloorsData[0].GetLiftAreas().at( i ).getPoint( 0 ) );
		tempData.SetCurrentFloor( m_vStopAtFloors[0] );
		tempData.InitLiftData( m_vStopAtFloors , vNecessaryTime ,_pTerm, this, i );
		m_vAllLiftData.push_back( tempData );
	}	
	return true;
}
// init elevator's stop at which floor data
void ElevatorProc::initSpecificMisc (const MiscData *miscData)
{
	if( miscData )
	{
		m_vStopAtFloors.clear();
		MiscElevatorData* pMiscData = (MiscElevatorData*)miscData;
		int iCount = pMiscData->getStopAfFloorCount();
		for( int i=0; i<iCount; ++i )
		{
			if( pMiscData->getStopAtFloor( i ) )
			{
				m_vStopAtFloors.push_back( i );
			}
		}
		m_dSpeed = pMiscData->getSpeed( MiscElevatorData::EST_NORMAL );
		//m_iCapacity = pMiscData->getCapacity();
	}
}

Point ElevatorProc::GetServiceLocationOnFloor ( int _nFloor ) const
{
	ASSERT( _nFloor >= 0 );
	if(_nFloor - m_iMinFloor<0 || _nFloor - m_iMinFloor> (int)m_vAllFloorsData.size() )
	{
		//////////////////////////////////////////////////////////////////////////
		return m_vAllFloorsData[0].GetWaitArea().getRandPoint();
	}

	return m_vAllFloorsData[ _nFloor - m_iMinFloor ].GetWaitArea().getRandPoint();
}
void ElevatorProc::beginService (Person *_pPerson, ElapsedTime curTime)
{
	TerminalMobElementBehavior* spTerminalBehavior = _pPerson->getTerminalBehavior();
	if (spTerminalBehavior)
	{
		int iSourceFloor = spTerminalBehavior->GetSourceFloor();
		m_vAllFloorsData[ iSourceFloor-m_iMinFloor ].AddPersonIntoWaitList( _pPerson );
		SelectBestLiftToService( _pPerson );
		StartElevatorIfNecessary( _pPerson , curTime);
	}
	

}

//select a best lift to service person who arrival at elevator
// now , we let all lift to try to serive. need be optimized later if necessary
void ElevatorProc::SelectBestLiftToService( Person* _pPerson )
{
	TerminalMobElementBehavior* spTerminalBehavior = _pPerson->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return;
	}

	int iSourceFloor = spTerminalBehavior->GetSourceFloor();

	// select all to service . need be optimized later
	for( int i=0; i<m_iNumberOfLift; ++i )
	{
		m_vAllLiftData[i].SetStopFlag( iSourceFloor, true, true );
		m_vAllLiftData[i].SetStopFlag( iSourceFloor, true, false );
	}
}

// start any lift whose status is HALT
void  ElevatorProc::StartElevatorIfNecessary( Person* _pPerson , const ElapsedTime& _curTime)
{
	TerminalMobElementBehavior* spTerminalBehavior = _pPerson->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return;
	}

	int iSourceFloor = spTerminalBehavior->GetSourceFloor();
	int iDestFloor = spTerminalBehavior->GetDestFloor();

	for( int i=0; i<m_iNumberOfLift; ++i )
	{
		if( m_vAllLiftData[ i ].GetDirection() == HALT )
		{
			int iCurrentFloor = m_vAllLiftData[ i ].GetCurrentFloor();
			if( iSourceFloor > iCurrentFloor )
			{
				m_vAllLiftData[ i ].SetDirection( UP );
			}
			else if( iSourceFloor < iCurrentFloor )
			{
				m_vAllLiftData[ i ].SetDirection( DOWN );
			}
			else
			{
				if( iSourceFloor < iDestFloor )
				{
					m_vAllLiftData[ i ].SetDirection( UP );
				}
				else
				{
					m_vAllLiftData[ i ].SetDirection( DOWN );
				}
				
			}

			m_vAllLiftData[ i ].GenerateEvent( _curTime, i, this );
		}
	}
}
void ElevatorProc::FlushLog()
{
	for( int i=0; i<m_iNumberOfLift; ++i )
	{
		m_vAllLiftData[ i ].FlushLog();
	}
	for( UINT j=0;j<m_vAllFloorsData.size();j++)
	{
		m_vAllFloorsData[j].ClearPersonIntowaitList();
	}
}

void ElevatorProc::DoOffset( DistanceUnit _xOffset, DistanceUnit _yOffset )
{
	Processor::DoOffset( _xOffset, _yOffset );
	InitLayout(  );
	/*
	for(std::vector<DataOfEveryFloor>::iterator itFloorData=m_vAllFloorsData.begin();
		itFloorData!=m_vAllFloorsData.end(); itFloorData++) {

		Pollygon p((*itFloorData).GetWaitArea());
		p.DoOffset(_xOffset, _yOffset, 0.0);
		(*itFloorData).SetWaitArea(p);
		int nAreaCount = (*itFloorData).GetLiftAreas().size();
		for(int iArea=0; iArea<nAreaCount; iArea++) {
			(*itFloorData).GetLiftAreas()[iArea].DoOffset(_xOffset, _yOffset, 0.0);
		}
	}
	*/
}

void ElevatorProc::Rotate( DistanceUnit _degree )
{
	DistanceUnit dX =  m_location.getPoint(0).getX();
	DistanceUnit dY = m_location.getPoint(0).getY();	
	Processor::DoOffset( -dX ,-dY );
	for(std::vector<DataOfEveryFloor>::iterator itFloorData=m_vAllFloorsData.begin();
		itFloorData!=m_vAllFloorsData.end(); itFloorData++) {

		Pollygon p((*itFloorData).GetWaitArea());
		p.DoOffset(-dX, -dY, 0.0);
		(*itFloorData).SetWaitArea(p);
		int nAreaCount = (*itFloorData).GetLiftAreas().size();
		for(int iArea=0; iArea<nAreaCount; iArea++) {
			(*itFloorData).GetLiftAreas()[iArea].DoOffset(-dX, -dY, 0.0);
		}

		std::vector<Point>vDoors;
		itFloorData->GetLiftDoors(vDoors );
		int nDoorCount = vDoors.size();
		for( int i=0; i<nDoorCount; ++i )
		{
			vDoors[i].DoOffset( -dX , -dY , 0.0 );
		}
		itFloorData->SetLiftDoors( vDoors );
	}

		
	for(std::vector<DataOfEveryFloor>::iterator itFloorData=m_vAllFloorsData.begin();
		itFloorData!=m_vAllFloorsData.end(); itFloorData++) {

		Pollygon p((*itFloorData).GetWaitArea());
		p.Rotate( _degree );
		(*itFloorData).SetWaitArea(p);
		int nAreaCount = (*itFloorData).GetLiftAreas().size();
		for(int iArea=0; iArea<nAreaCount; iArea++) {
			(*itFloorData).GetLiftAreas()[iArea].Rotate( _degree );
		}

		std::vector<Point>vDoors;
		itFloorData->GetLiftDoors(vDoors );
		int nDoorCount = vDoors.size();
		for( int i=0; i<nDoorCount; ++i )
		{
			vDoors[i].rotate( _degree );
		}
		itFloorData->SetLiftDoors( vDoors );
	}

	Processor::DoOffset( dX ,dY );
	for(std::vector<DataOfEveryFloor>::iterator itFloorData=m_vAllFloorsData.begin();
		itFloorData!=m_vAllFloorsData.end(); itFloorData++) {

		Pollygon p((*itFloorData).GetWaitArea());
		p.DoOffset(dX, dY, 0.0);
		(*itFloorData).SetWaitArea(p);
		int nAreaCount = (*itFloorData).GetLiftAreas().size();
		for(int iArea=0; iArea<nAreaCount; iArea++) {
			(*itFloorData).GetLiftAreas()[iArea].DoOffset(dX, dY, 0.0);
		}

		std::vector<Point>vDoors;
		itFloorData->GetLiftDoors(vDoors );
		int nDoorCount = vDoors.size();
		for( int i=0; i<nDoorCount; ++i )
		{
			vDoors[i].DoOffset( dX , dY , 0.0 );
		}
		itFloorData->SetLiftDoors( vDoors );
	}

	//InitLayout();
}
void DataOfEveryLift::FlushLog()
{
    long trackCount = m_logOfLift.getCurrentCount();
    ElevatorEventStruct *log = NULL;
    m_logOfLift.getLog (log);
    // write first member of group
    m_logOfLift.setEventList (log, trackCount);
	delete [] log;
	log = NULL;
	m_pTerm->m_pElevatorLog->updateItem (m_logOfLift, m_logOfLift.GetIndex());
	m_logOfLift.clearLog();
}




Point ElevatorProc::GetEntryPoint(const CString& _strMobType,int _iCurFloor, const CString& _strTime)
{
	if(getServicePoint( 0 ).getZ()/ SCALE_FACTOR ==_iCurFloor) 
	{
		if( In_Constr.getCount() )
			return In_Constr.getPoint( 0 );
		if( m_pQueue != NULL )
			return	m_pQueue->corner( m_pQueue->cornerCount() - 1 );
		return getServicePoint(0);
	}
	else
	{
		int nFloorCount=m_vStopAtFloors.size();
		for(int i=0;i<nFloorCount;i++)
		{
			if(m_vStopAtFloors[i] == _iCurFloor)
			{
				ServicerMidle = getServicePoint(0);
				ServicerMidle.setZ(_iCurFloor * SCALE_FACTOR);
				return ServicerMidle;
			}
		}
	}
		return getServicePoint(0);
}

bool ElevatorProc::CheckIfOnTheSameFloor(int _iCurFloor)
{
	int nFloorCount=m_vStopAtFloors.size();
	for(int i=0;i<nFloorCount;i++)
	{
		if(m_vStopAtFloors[i] == _iCurFloor)
		{
			return true;
		}
	}
	return false;
}

Processor* ElevatorProc::CreateNewProc()
{
	Processor* pProc = new ElevatorProc;
	return pProc;
}


bool ElevatorProc::PrepareCopy(Processor* _pDestProc, const int _nFloor, const int _nDestFloorTotal)
{
	int nFloorMin = GetMinFloor();
	int nFloorMax = GetMaxFloor();
	if (nFloorMax - nFloorMin + _nFloor >= _nDestFloorTotal)
	{
		MessageBox(NULL, _T("The Destination Model has not enough floors"), _T("Copy Error"), MB_OK | MB_ICONERROR);
		return false;
	}

	ElevatorProc * pProc = (ElevatorProc*)_pDestProc;

	*pProc = *this;

	pProc->SetMinFloor(_nFloor);
	pProc->SetMaxFloor(_nFloor + nFloorMax - nFloorMin);

	return true;
}

bool ElevatorProc::CopyOtherData(Processor* _pDestProc)
{
	ElevatorProc * pProc = (ElevatorProc*)_pDestProc;

	pProc->InitLayout();

	return true;
}

	// Acquire service m_location.
Point ElevatorProc::AcquireServiceLocation( Person* _pPerson  )
{ 
	Point pt;
	_pPerson->getTerminalPoint(pt);
	int nFloor = (int)pt.getZ() / (int)SCALE_FACTOR;
	return GetServiceLocationOnFloor( nFloor ); 
}


// check if elevator can serve current person according to 
// lift's capacity attributes and current occupancy MobileElements,
// if Elevator proc cannot serve while arriving at elevator,
// mobile element just wait in elevator's holding area.
bool ElevatorProc::isVacant( Person* pPerson ) const
{
	return TRUE;
}

// elevator's capacity setting in behavior dialog == each lift's capacity.
//
bool ElevatorProc::canEnterLift(Person* pPerson, std::list<Person*>& listPerson) const
{
	ASSERT(pPerson != NULL);
	if(m_ProcessorCapacity.GetCount() < 1)
		return true;

	ProcessorOccupantsState procOccupied;
	//Get all  Occupant item info, passenger, follower, visitor, ...
	std::list<Person*>::const_iterator iter = listPerson.begin();
	std::list<Person*>::const_iterator iterEnd = listPerson.end();
	for( ; iter != iterEnd; ++iter )
	{
		TerminalMobElementBehavior* spIterTerminalBehavior = (*iter)->getTerminalBehavior();
		if (spIterTerminalBehavior)
		{
			spIterTerminalBehavior->getFollowersAndVisitorsOccupancyInfo(procOccupied);
		}
	
	}

	ProcessorOccupantsState procWillOccupied;
	TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
	if (spTerminalBehavior)
	{
		spTerminalBehavior->getFollowersAndVisitorsOccupancyInfo(procWillOccupied);
	}
	

	return m_ProcessorCapacityInSim.IsVacant(pPerson, procOccupied, procWillOccupied);

}

void ElevatorProc::GetDataAtFloorLayoutData( int nFloor, std::vector<Pollygon>& outAreas )
{
	return GetDataAtFloor(nFloor).GetLayoutData(outAreas);
}

void ElevatorProc::GetDataAtFloorLiftDoors( int nFloor, std::vector<Point>& outDoors )
{
	return GetDataAtFloor(nFloor).GetLiftDoors(outDoors);
}

std::vector<Pollygon>& ElevatorProc::GetDataAtFloorLiftAreas( int nFloor )
{
	return GetDataAtFloor(nFloor).GetLiftAreas();
}

void ElevatorProc::UpdateFloorIndex( const FloorChangeMap& changMap )
{
	__super::UpdateFloorIndex(changMap);
	
	int iNewMinFloor = changMap.getNewFloor(m_iMinFloor);
	int iNewMaxFloor = changMap.getNewFloor(m_iMaxFloor);
	
	if(iNewMaxFloor<iNewMinFloor)
		std::swap(iNewMinFloor,iNewMaxFloor);

	std::vector<bool> vNewPosWaitArea;
	vNewPosWaitArea.resize(iNewMaxFloor-iNewMinFloor+1, true);

	for(int i=m_iMinFloor;i<=m_iMaxFloor;i++)
	{
		int nNewFloor = changMap.getNewFloor(i);
		vNewPosWaitArea[nNewFloor-iNewMinFloor] = m_vPosOfWaitarea[i-m_iMinFloor];  
	}

	m_iMaxFloor = iNewMaxFloor;
	m_iMinFloor = iNewMinFloor;
	m_vPosOfWaitarea = vNewPosWaitArea;

	InitLayout();
}

//Point ElevatorProc::GetPipeExitPoint(int iCurFloor, CString& _curTime,Point& outPoint,TerminalMobElementBehavior *terminalMob)
//{
//	return GetEntryPoint(terminalMob->getPersonErrorMsg(),iCurFloor,_curTime);
//}