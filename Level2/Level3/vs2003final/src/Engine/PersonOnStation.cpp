// PersonOnStation.cpp: implementation of the CPersonOnStation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PersonOnStation.h"
#include "proclist.h"
#include "Movevent.h"
#include "Person.h"
#include ".\Results\TrainLogEntry.h"
#include "MovingTrain.h"
#include "Inputs\AllCarSchedule.h"
#include "hold.h"
#include "engine\terminal.h"

#include <Common/ProbabilityDistribution.h>
#include "../Common/ARCTracker.h"
#include <Inputs/procdist.h>
#include "TerminalMobElementBehavior.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPersonOnStation::CPersonOnStation( Person* _pPerson, int _nProcIndex, Terminal* _pTerm,ProcessorDistribution* _pDestStationList )
{
	m_pPerson = _pPerson;
	m_nProcIndex = _nProcIndex;
	m_enumState = ENTER_STATION;
	m_pTerm = _pTerm;
	m_nCarIndex = -1;
	m_nEntryDoorIndex = -1;
	m_nExitDoorIndex = -1;
	m_pDestStationList = _pDestStationList;
}


CPersonOnStation::~CPersonOnStation()
{
}


// return false for ready to be deleted.
// 
bool CPersonOnStation::Process( ElapsedTime _time )
{
	PLACE_METHOD_TRACK_STRING();
	IntegratedStation* pStation = GetSourceStation();
	TerminalMobElementBehavior* spTerminalBehavior = m_pPerson->getTerminalBehavior();

	if (spTerminalBehavior == NULL)
	{
		return false;
	}

	if( !pStation )
		return false;

	switch( m_enumState )					
	{
		case ENTER_STATION:	// same as FreeMove
			{
				// check if has in constraint set to WAITAREA_IN_CONSTRAINT
				// else
				// WriteLog: Birth - NO, FreeMove - YES
				if( m_pPerson->getState() == FreeMoving )
					m_pPerson->writeLogEntry( _time, false );

				if( !GetSchedule( _time ) )
					return false;
				
				HoldingArea* pPreBoardArea = pStation->GetPreBoardingArea();

				assert( pPreBoardArea );
				if( pPreBoardArea->inConstraintLength() == 0 )
					ArriveAtPreBoardArea( pStation, _time );
				else
				{
					m_enumState = WAITAREA_IN_CONSTRAINT;
					m_pPerson->setTerminalDestination( pPreBoardArea->inConstraint(0) );

					// Gen Next Event: + moveTime
					m_pPerson->generateEvent( _time + m_pPerson->moveTime(),false );
				}
				break;
			}
		case WAITAREA_IN_CONSTRAINT:
			{
				m_pPerson->setState( WaitForVehicle );
				m_pPerson->writeLogEntry( _time, false );
				
				HoldingArea* pPreBoardArea = pStation->GetPreBoardingArea();
				assert( pPreBoardArea );
				int nPathIndex = 1;
				while( nPathIndex < pPreBoardArea->inConstraintLength())
				{
					m_pPerson->setTerminalDestination( pPreBoardArea->inConstraint(nPathIndex++) );
					_time += m_pPerson->moveTime();
					m_pPerson->writeLogEntry( _time, false );
				}
				ArriveAtPreBoardArea( pStation, _time );
			
			}
			break;
		case WAITAREA_ARRIVE_AT_SERVER:
			{
				spTerminalBehavior->SetPersonInStationSystem( true );
				// WriteLog: WaitForVehicle
				m_pPerson->setState( WaitForVehicle );
				m_pPerson->writeLogEntry( _time, false );

				m_enumState = WAITAREA_LEAVE_SERVER;

				// Gen Next Event: vehicle arrival time
				GetSchedule( _time );
				ElapsedTime timeLinkStation;
				IntegratedStation* pLinkedStation =  GetBestStationToDepature( _time,timeLinkStation );
				if( pLinkedStation && pLinkedStation != pStation && m_curArrTime > timeLinkStation)
				{//need to go to other station
					Point ptNextPoint = pLinkedStation->GetPreBoardingArea()->GetServiceLocation();
					m_pPerson->setTerminalDestination( ptNextPoint );
					//m_pPerson->writeLogEntry( _time + )
					m_enumState = WAITAREA_ARRIVE_AT_SERVER;
					m_nProcIndex = pLinkedStation->getIndex();
					pStation->removePerson( m_pPerson );
					pLinkedStation->addPerson( m_pPerson );
					spTerminalBehavior->setProcessor( pLinkedStation );
					m_pPerson->generateEvent( _time + m_pPerson->moveTime(),false );

				}
				else
				{
					if( m_curArrTime > _time)
						m_pPerson->generateEvent( m_curArrTime ,false);
					else
						m_pPerson->generateEvent( _time ,false);
				}
				
				break;
			}
		case WAITAREA_LEAVE_SERVER:
			{
				// WriteLog, board vehicle
				m_pPerson->setState( BoardVehicle );
				m_pPerson->writeLogEntry( _time, false );

				m_enumState = PERSON_IN_ENTRYDOORQUEUE;

				// Select Car and Door
				Point locationPt;
				m_pPerson->getTerminalPoint(locationPt);
				SelectBestCarAndEntryDoor( pStation, locationPt);
				Processor* pProc = GetCurrentEntryDoor( pStation );	// pick car and entry door.
				assert( pProc );

				// Set Dest to the Queue Tail Point
				m_pPerson->setState( MoveToQueue );	// for usesage
				pProc->GetQueueTailPoint( m_pPerson );	// set person's next dest.

				// Add Person to the Queue ( might make state to WaitInQueue )
				pProc->addToQueue( m_pPerson, _time );

				// Gen Next Event : + moveTime()
				m_pPerson->generateEvent( _time + m_pPerson->moveTime() ,false);
				
			}

			break;
		case PERSON_IN_ENTRYDOORQUEUE:
			{
				// WriteLog: // boarding V
				int nPersonState = m_pPerson->getState();
		
				m_pPerson->setState( BoardVehicle );
				m_pPerson->writeLogEntry( _time, false );
				m_pPerson->setState( nPersonState );

				ProcessEntryDoorQueue( _time, pStation );
			}
			break;

		case PERSON_ARRIVE_AT_ENTRYDOOR:
			{
				// WriteLog: // boarding vehicle
				int nPersonState = m_pPerson->getState();
				m_pPerson->setState( BoardVehicle );
				m_pPerson->writeLogEntry( _time, false );
				m_pPerson->setState( nPersonState );

				m_enumState = PERSON_LEAVE_ENTRYDOOR;

				CCar* pCar = GetCurrentCarStation( pStation );
				HoldingArea* pCarArea = pCar->GetCarArea();
				assert( pCarArea );
				Point ptInCar = pCarArea->GetServiceLocation();
				ElapsedTime t;
				Point locationPt;
				m_pPerson->getTerminalPoint(locationPt);
				double dLxy = ptInCar.distance(locationPt);
				double dLz = 0.0;
				double dL = dLxy;
				dLz = fabsl(ptInCar.getZ() - locationPt.getZ());
				Point pt(dLxy, dLz, 0.0);
				dL = pt.length();
				double time = dL;
				t = (float) (time / (double)m_pPerson->getSpeed());

				// Check if exceed the capacity
				MovingTrain* pTrain = pStation->GetTrain();
				TrainCarInSim* _pCar = NULL;
				if (pTrain)
				{
					_pCar = GetCarInTrain(pTrain,m_nCarIndex); //pTrain->getCar(m_nCarIndex);
				}
				if( !pTrain || !_pCar || !_pCar->Valid()\
					|| (_time - t)> m_curDepTime )
				{
					GetSchedule( m_curDepTime );
					m_enumState = PERSON_ARRIVE_AT_ENTRYDOOR;
					m_pPerson->generateEvent( m_curArrTime,false);
				}
				else
				{
					//_pCar->AddPax(m_pPerson->GetActiveGroupSize());
					_pCar->AddPax(m_pPerson);
					m_pPerson->generateEvent( _time,false);
				}
			}
			break;
		case PERSON_LEAVE_ENTRYDOOR:
			{
				// WriteLog: // boarding vehicle
				int nPersonState = m_pPerson->getState();
				m_pPerson->setState( BoardVehicle );
				m_pPerson->writeLogEntry( _time, false );
				m_pPerson->setState( nPersonState );

				m_enumState = PERSON_ARRIVE_AT_CAR;

				// Door make available
				Processor* pProc = GetCurrentEntryDoor( pStation );	// pick car and entry door.]
			    pProc->makeAvailable( m_pPerson, _time, true );

				// Set dest to a position in car area.
				CCar* pCar = GetCurrentCarStation( pStation );
				HoldingArea* pCarArea = pCar->GetCarArea();
				assert( pCarArea );
				Point ptInCar = pCarArea->GetServiceLocation();
				m_ptOffset = ptInCar - pStation->getServicePoint( 0 );
				m_pPerson->setTerminalDestination( ptInCar );

				// Gen event
				m_pPerson->generateEvent( _time + m_pPerson->moveTime(),false );

			}
			break;
		case PERSON_ARRIVE_AT_CAR:
			{
				// WriteLog: OnVehicle
				m_pPerson->setState( OnVehicle );
				m_pPerson->writeLogEntry( _time, false );

				m_enumState = CAR_LEAVE_STATION;

				// Gen next event for vehicle start move
				m_pPerson->generateEvent( m_curDepTime,false );
			
			}
			break;
		case CAR_LEAVE_STATION:
			{		
				// WriteLog: onvehicle
				m_pPerson->writeLogEntry( _time, false );
				// WriteLog on the railway.

				//***********************************************
				//  How to rotate the passenger
				//
				//		1. with exist offset write log
				//		2. if not last point in the segment
				//			a. get new heading
				//			b. if not first point
				//				aa. cal diff to the prev heading;
				//				bb. rotate diff ( keep the offset )
				//				cc. write new log
				//			c. keep heading in prevheading.
				//
				//***********************************************

				ElapsedTime eventTime;
				int nCount = m_movePath.size();
				double dPrevHeading = 0.0;

				for( int i=0; i<nCount; i++ )
				{
					CViehcleMovePath movePath = m_movePath[i];
					int nItemCount = movePath.GetCount();
					for( int m=0; m<nItemCount; m++ )
					{
						CTimePoint timePt = movePath.GetItem( m );
						eventTime = timePt.GetTime();
						Point ptInCar = timePt + m_ptOffset;
						m_pPerson->setTerminalDestination( ptInCar  );
						m_pPerson->writeLogEntry( eventTime, false );
						if( m < nItemCount - 1 )
						{
							CTimePoint nextPoint = movePath.GetItem( m+1 );
							double dNewHeading = ( nextPoint - timePt ).getHeading();
							if( m != 0 || i != 0 )
							{
								double dDiffHeading = dNewHeading - dPrevHeading;
								m_ptOffset.rotate( dDiffHeading );
								Point ptInCar = timePt + m_ptOffset;
								m_pPerson->setTerminalDestination( ptInCar  );
								m_pPerson->writeLogEntry( eventTime, false );								
							}
							dPrevHeading = dNewHeading;
						}
					}
				}

				spTerminalBehavior->SetDestStation( m_nDestIndex );
				m_nEntryDoorIndex = -1;
				m_enumState = CAR_ARRIVE_AT_STATION;
				Processor* pDestProc = m_pTerm->procList->getProcessor( m_nDestIndex );
				if( !pDestProc || pDestProc->getProcessorType() != IntegratedStationProc )
					return false;
				IntegratedStation* pDestStation = (IntegratedStation*)pDestProc;

				// Get New Point in the new CAR
				Point ptInCar = pDestStation->getServicePoint( 0 ) + m_ptOffset;
				m_pPerson->setTerminalDestination( ptInCar );

				// Gen Event for move time
				ElapsedTime delayTime;
				delayTime.setPrecisely((long)01);
				m_pPerson->generateEvent( eventTime + delayTime,false);
			}
			break;
		case CAR_ARRIVE_AT_STATION:
			{				
				// WriteLog : leave vehicle
//				m_pPerson->setState( LeaveVehicle );
//				m_pPerson->writeLogEntry( _time );
				m_enumState = PERSON_ARRIVAE_AT_EXITDOOR;

				// Set Dest as Exit Door
				Processor* pDestProc = m_pTerm->procList->getProcessor( m_nDestIndex );
				if( !pDestProc || pDestProc->getProcessorType() != IntegratedStationProc )
					return false;
				IntegratedStation* pDestStation = (IntegratedStation*)pDestProc;

				Point locationPt;
				m_pPerson->getTerminalPoint(locationPt);
				SelectBestExitDoor( pDestStation, locationPt);
				Processor* pProc = GetExitDoor( pDestStation );	// pick car and exit door.
				assert( pProc );
				pProc->getNextLocation( m_pPerson );
				MovingTrain* pTrain = pDestStation->GetTrain();
				if (pTrain == NULL)
					return false;
				
				TrainCarInSim* _pCar = pDestStation->GetTrain()->getCar(m_pPerson);
				if (_pCar == NULL)
					return false;
				
				_pCar->DeletePax(m_pPerson);
				// Gen Event moveTime
				m_pPerson->generateEvent( _time + m_pPerson->moveTime(),false );
			}
			break;
		case PERSON_ARRIVAE_AT_EXITDOOR:
			{
				// WriteLog : Leave Vehicle.
				m_pPerson->writeLogEntry( _time, false );
				m_enumState = PERSON_LEAVE_EXITDOOR;

				// Write log for each point of out constraint.
				Processor* pDestProc = m_pTerm->procList->getProcessor( m_nDestIndex );
				if( !pDestProc || pDestProc->getProcessorType() != IntegratedStationProc )
					return false;
				IntegratedStation* pDestStation = (IntegratedStation*)pDestProc;
				Processor* pProc = GetExitDoor( pDestStation );	// pick car and exit door.
				assert( pProc );
				int nOutConstrCount = pProc->outConstraintLength();
				for( int i=0; i<nOutConstrCount; i++ )
				{
					m_pPerson->setTerminalDestination( pProc->outConstraint( i ) );
					_time += m_pPerson->moveTime();
					m_pPerson->writeLogEntry( _time, false );
				}

				// Gen Event for exit
				m_pPerson->generateEvent( _time,false);
			}

			break;
		case PERSON_LEAVE_EXITDOOR:
			{
				spTerminalBehavior->SetPersonInStationSystem( false );
			}
			return false;

	}
	return true;
}



// select the closest car and entry door.
//==================================================
//	Calculate all the distances to the entry door
//	service location. 
//	Pick the closest on first
//	Do the check
//		within the list order find any one's queue
//		len - this queue len > 2
//		pick this one, and continue compare with the
//		new one.
//==================================================
void CPersonOnStation::SelectBestCarAndEntryDoor( IntegratedStation* _pStation, Point _ptCurPos )
{
	SortedContainer<CEntryDoorInfo> sortedDoorList;
	sortedDoorList.OwnsElements( TRUE );
	int nCarCount = _pStation->GetCarCount();
	for( int i=0; i<nCarCount; i++ )
	{
		CCar* pCar = _pStation->GetCar( i );
		if(_pStation->GetTrain() && _pStation->GetTrain()->getCar(i) == NULL)
			continue;

		int nEntryDoorCount = pCar->GetEntryDoorCount();
		for( int m=0; m<nEntryDoorCount; m++ )
		{
			CEntryDoorInfo* pInfo = new CEntryDoorInfo;
			Processor* pEntryDoor = pCar->GetEntryDoor( m );
			Point ptServiceLocation = pEntryDoor->getServicePoint( 0 );
			pInfo->m_dDistance = ptServiceLocation.distance( _ptCurPos );
			pInfo->m_nQueueLen = pEntryDoor->getTotalQueueLength();
			pInfo->m_nCarIdx = i;
			pInfo->m_nDoorIdx = m;
			sortedDoorList.addItem( pInfo );
		}
	}

	// Select
	m_nCarIndex = -1;
	m_nEntryDoorIndex = -1;
	if( sortedDoorList.size() == 0 )
		return;

	int nPickIdx = 0;
	UINT nTestIdx = 1;
	while( nTestIdx < sortedDoorList.size() )
	{
		if( sortedDoorList[nTestIdx]->m_nQueueLen + 2 < sortedDoorList[nPickIdx]->m_nQueueLen )
			nPickIdx = nTestIdx;
		nTestIdx++;
	}
	m_nCarIndex = sortedDoorList[nPickIdx]->m_nCarIdx;
	m_nEntryDoorIndex = sortedDoorList[nPickIdx]->m_nDoorIdx;
}

// base on m_nCarIndex pick a best car, or just return the car.
CCar* CPersonOnStation::GetCurrentCarStation( IntegratedStation* _pStation )
{
	if( m_nCarIndex < 0 )
		return NULL;

	CCar *pCar = _pStation->GetCar( m_nCarIndex );
	if(pCar == NULL)
		pCar = _pStation->GetCar(0);
	return pCar ;
}
	
// base on m_nEntryDoorIndex pick a best entry door or just return the entry door.
Processor* CPersonOnStation::GetCurrentEntryDoor( IntegratedStation* _pStation )
{
	CCar* pCar = GetCurrentCarStation( _pStation );
	if( !pCar || m_nEntryDoorIndex < 0 )
		return NULL;

	return pCar->GetEntryDoor( m_nEntryDoorIndex );
}


// select the closest exit door.
void CPersonOnStation::SelectBestExitDoor( IntegratedStation* _pStation, Point _ptCurPos )
{
	int nCarCount = _pStation->GetCarCount();
	CCar* pCar = NULL;

	//first, find car by person's positon
	for( int i=0; i<nCarCount; i++ )
	{
		CCar* pCarCur = _pStation->GetCar( i );

		HoldingArea*pCarArea = pCarCur->GetCarArea();
		Path* pLocation = pCarCur->GetCarArea()->serviceLocationPath();

		Pollygon serviceArea;
		serviceArea.init (pLocation->getCount(), pLocation->getPointList());
		if (serviceArea.contains(_ptCurPos))
		{
			m_nCarIndex = i;//Need Modify
			pCar = pCarCur;
			break;
		}
	}
	//maybe in car's holding area vex,use car index stored by selectbestEnterdoor
	if(pCar == NULL)
		pCar = GetCurrentCarStation( _pStation );

	double dDistance = -1.0;
	int nExitDoorCount = pCar->GetExitDoorCount();
	for( int i=0; i<nExitDoorCount; i++ )
	{
		Processor* pExitDoor = pCar->GetExitDoor( i );
		Point ptServiceLocation = pExitDoor->getServicePoint( 0 );
		double d = ptServiceLocation.distance( _ptCurPos );
		if( dDistance < 0 || dDistance < d )
		{
			dDistance = d;
			m_nExitDoorIndex = i;
		}
	}	
}

// base on m_nExitDoorIndex pick a best exit door or just return the exit door.
Processor* CPersonOnStation::GetExitDoor( IntegratedStation* _pStation )
{
	CCar* pCar = GetCurrentCarStation( _pStation );
	if( !pCar || m_nExitDoorIndex < 0 )
		return NULL;

	return pCar->GetExitDoor( m_nExitDoorIndex );
}
	

/*

void CPersonOnStation::SelectBestCar()
{
	
}

// base on m_nCarIndex pick a best car, or just return the car.
CCar CPersonOnStation::GetCar( IntegratedStation* _pStation )
{
	if( m_nCarIndex == -1 )
	{
		int nCarCount = _pStation->GetCarCount();
		m_nCarIndex = random( nCarCount );
	}

	return _pStation->GetCar( m_nCarIndex );
}

// Select the closest entry door relative to the _ptCurPos
void CPersonOnStation::SelectBestEntryDoor( IntegratedStation* _pStation, Point _ptCurPos )
{
	CCar GetCar()
}


// base on m_nEntryDoorIndex pick a best entry door or just return the entry door.
Processor CPersonOnStation::GetEntryDoor( IntegratedStation* _pStation )
{
	CCar aCar = GetCar( _pStation );
	if( m_nEntryDoorIndex == -1 )
	{
		int nEntryDoorCount = aCar.GetEntryDoorCount();
		m_nEntryDoorIndex = random( nEntryDoorCount );
	}

	Processor
	return aCar.GetEntryDoor( m_nEntryDoorIndex );
}



// base on m_nExitDoorIndex pick a best exit door or just return the exit door.
Processor CPersonOnStation::GetExitDoor( IntegratedStation* _pStation )
{
	CCar aCar = GetCar( _pStation );
	if( m_nExitDoorIndex == -1 )
	{
		int nExitDoorCount = aCar.GetExitDoorCount();
		m_nExitDoorIndex = random( nExitDoorCount );
	}
	return aCar.GetEntryDoor( m_nExitDoorIndex );
}

*/

void CPersonOnStation::ProcessEntryDoorQueue( ElapsedTime _time, IntegratedStation* _pStation )
{
	int nPersonState = m_pPerson->getState();
	Processor* pProc = GetCurrentEntryDoor( _pStation );	// pick car and entry door.


	TerminalMobElementBehavior* spTerminalBehavior = m_pPerson->getTerminalBehavior();
	if( nPersonState == LeaveQueue )
	{
 		if( _time > m_curDepTime )
 		{
 			GetSchedule( _time );
 			m_pPerson->generateEvent( m_curArrTime,false );
 			return;
 		}

		// Leave Queue: remove from HEAD, move the queue
		pProc->leaveQueue( m_pPerson, _time, true );
		spTerminalBehavior->wait( FALSE );
		m_pPerson->setState( ArriveAtServer );
		m_enumState = PERSON_ARRIVE_AT_ENTRYDOOR;

		// set dest to service location of entry door.
		pProc->getNextLocation( m_pPerson );

		ElapsedTime tLeaveTime;
		ElapsedTime delayTime;
		delayTime.setPrecisely((long)01);
		tLeaveTime = _time + m_pPerson->moveTime() + delayTime;
		if (tLeaveTime == m_curArrTime)
		{
			tLeaveTime += delayTime;
		}

		// gen next event _ moveTime()
		m_pPerson->generateEvent(tLeaveTime /*_time + m_pPerson->moveTime()*/ ,false);
		return;
	}

    // must first check for relayAdvance event
	if( nPersonState == StartMovement )
		pProc->moveThroughQueue( m_pPerson, _time );
    // m_nWaiting flag will be set to TRUE when pax arrives at queue tail
    // allows distinction between arrive at tail and movement within queue
	else if( !spTerminalBehavior->isWait() )
		pProc->arriveAtQueue( m_pPerson, _time, true );
    else
		pProc->notifyQueueAdvance( m_pPerson, _time, true );
}


bool CPersonOnStation::IsDone()
{
	return m_enumState == PERSON_LEAVE_EXITDOOR;
}


void CPersonOnStation::ArriveAtPreBoardArea( IntegratedStation* _pStation, ElapsedTime _time )
{
	m_enumState = WAITAREA_ARRIVE_AT_SERVER;

	// Set Dest -> Position in Holding Area.
	HoldingArea* pPreBoardingArea = _pStation->GetPreBoardingArea();
	m_pPerson->setTerminalDestination( pPreBoardingArea->GetServiceLocation() );

	// Gen Next Event: + moveTime
	m_pPerson->generateEvent( _time + m_pPerson->moveTime(),false );
}


IntegratedStation* CPersonOnStation::GetSourceStation()
{
	Processor* pProc = m_pTerm->procList->getProcessor( m_nProcIndex );
	if( !pProc || pProc->getProcessorType() != IntegratedStationProc )
		return NULL;

	return (IntegratedStation*)pProc;
}

IntegratedStation* CPersonOnStation::GetDestStation()
{
	Processor* pProc = m_pTerm->procList->getProcessor( m_nDestIndex );
	if( !pProc || pProc->getProcessorType() != IntegratedStationProc )
		return NULL;

	return (IntegratedStation*)pProc;
}


bool CPersonOnStation::GetSchedule( ElapsedTime _time )
{
	ASSERT( m_pTerm );
	IntegratedStation* pSourceStation = GetSourceStation();
	IntegratedStation* pDestStation = GetDestStation();
	if( !pSourceStation || !pDestStation )
		return false;

	m_movePath.clear();

	if( !m_pTerm->m_pAllCarSchedule->SelectBestSchedule( pSourceStation, pDestStation, _time, m_movePath ) )
		return false;

	int nSegCount = m_movePath.size();
	m_curArrTime = m_movePath[0].GetArrTime();
	m_curDepTime = m_movePath[0].GetDepTime();
	m_destArrTime = m_movePath[nSegCount-1].GetDestArrTime();
	return true;
}

//find a best station from share preboarding area stations , from which person can go to his dest station
// and the train stop at that station is the nearlst arrival one from current time
IntegratedStation* CPersonOnStation::GetBestStationToDepature( const ElapsedTime& _time , ElapsedTime& _timeArrival )
{
	assert( m_pDestStationList );
	//IntegratedStation* pStation = GetSourceStation();
	//IntegratedStation* pDestStation = GetDestStation();
	//ProcessorID procID;
	//procID.SetStrDict( m_pTerm->inStrDict );
	//procID.setID("STA-A");
	//GroupIndex gIndex = m_pTerm->procList->getGroupIndex( procID );
	
	
	IntegratedStation* pStation = GetSourceStation();
	std::vector<IntegratedStation*>& vShareStation = pStation->GetSharePreboardingAreaStation();
	std::vector<IntegratedStation*>vAllSourceStation;
	if((int)vShareStation.size() == 0)
	{
		return NULL;
	}
	vAllSourceStation.push_back( pStation );
	for( UINT i=0; i<vShareStation.size(); ++i )
	{
		vAllSourceStation.push_back( vShareStation[i] );					
	}
	

	assert( m_pDestStationList );
	std::vector<IntegratedStation*>vAllDestStation;
	for( i=0; i<(UINT)m_pDestStationList->getCount();++i )
	{
		ProcessorID* pID = m_pDestStationList->getGroup(i);
		GroupIndex gIndex = m_pTerm->procList->getGroupIndex( *pID );
		ASSERT( gIndex.start >= 0 );
		for( int j=gIndex.start; j<=gIndex.end; ++j )
		{
			Processor* pProc = m_pTerm->procList->getProcessor( j );
			if( pProc->getProcessorType() == IntegratedStationProc )
			{
				vAllDestStation.push_back( (IntegratedStation*)pProc );
			}			
		}
	}
	bool bTimeArrivalNoSetYet = true;
	IntegratedStation* pBestSource = NULL;
	IntegratedStation* pBestDest = NULL;
	
	for( i=0; i<vAllSourceStation.size(); ++i )
	{
		IntegratedStation* pSourceStation = vAllSourceStation[ i ];
		for( UINT j=0; j<vAllDestStation.size(); ++j )
		{
			IntegratedStation* pDestStation = vAllDestStation[ j ];
			std::vector<CViehcleMovePath> vMovePath;
			if( m_pTerm->m_pAllCarSchedule->SelectBestSchedule(pSourceStation, pDestStation, _time, vMovePath ) )
			{
				if( vMovePath[0].GetArrTime() < _timeArrival || bTimeArrivalNoSetYet )
				{
					bTimeArrivalNoSetYet = false;
					pBestSource= pSourceStation;
					pBestDest = pDestStation;
					_timeArrival = vMovePath[0].GetArrTime();					
				}
			}

		}
	}	
	if( pBestDest && pBestSource )
	{
		m_nDestIndex = pBestDest->getIndex();// change dest station
		return pBestSource;
	}

	return NULL;
	

	/*
	std::vector<IntegratedStation*>& vStationList = pStation->GetSharePreboardingAreaStation();
	_timeArrival.set(23,59,59);//23:59:59
	int iBestStation = -1;
	for( int i=0; i<vStationList.size(); ++i )
	{
		IntegratedStation* pShareStation = vStationList[i];
		std::vector<CViehcleMovePath> vMovePath;
		if( m_pTerm->m_pAllCarSchedule->SelectBestSchedule( pShareStation, pDestStation, _time, vMovePath ) )
		{
			if( _timeArrival < m_movePath[0].GetArrTime() )
			{
				_timeArrival = m_movePath[0].GetArrTime();
				iBestStation = i;
			}
		}
	}

	if( iBestStation > -1 )
	{
		return vStationList[ iBestStation ];
	}
	else
	{
		return NULL;
	}
	*/
}

TrainCarInSim* CPersonOnStation::GetCarInTrain(MovingTrain* pTrain, int nCarIndex )
{
	TrainCarInSim *pTrainCar = pTrain->getCar(m_nCarIndex);
	if(pTrainCar == NULL)
		pTrainCar = pTrain->getCar(0);
	return pTrainCar;
}

