#include "StdAfx.h"
#include "OnBoardDefs.h"
#include "DeplaneAgentBehavior.h"
#include "carrierSpaceDiagnosis.h"
#include "LocatorSite.h"
#include "DeplaneAgent.h"
#include "carrierSpace/carrierDefs.h"
#include "OnBoardCarrierSpaces.h"
#include "Services/SeatAssignmentService.h"

DeplaneAgentBehavior::DeplaneAgentBehavior(DeplaneAgent* pAgent)
//:m_pAgent( pAgent )
{
	//debug.
	//mNeedStandup = false;
}

DeplaneAgentBehavior::~DeplaneAgentBehavior()
{
}

//extern long eventNumber;
//int DeplaneAgentBehavior::performanceMove(ElapsedTime p_time,bool bNoLog)
//{
//
//	long nAgentID = m_pAgent->getID();
//
//	
//	if (bNoLog == false){ m_pAgent->writeLogEntry (p_time, false); }
//
//	//----------------------------------------------------------------------------
//	// Deplane Agent's state chart.
//	switch ( m_pAgent->getState() )
//	{
//	case EnterOnBoardMode:
//		processEnterOnBoardMode( p_time );
//		break;
//	case WayFindingOnDeck:
//		processWayFinding( p_time );
//		break;
//	case FreeMovingOnDeck:
//		processGeneralMovement( p_time );
//		break;
//	case ArriveAtExitDoor:
//		processArriveAtExitDoor( p_time );
//		break;
//	case SeatingOnChair:
//		processSeatingOnChair( p_time);
//		break;
//	case WaitingForLeaveCarrier:
//		processWaitingForLeaveCarrier( p_time );
//		break;
//	default:
//
//		break;
//	}
//
//	return 1;
//}
//
//void DeplaneAgentBehavior::processEnterOnBoardMode(ElapsedTime p_time)
//{
//// 	if(m_pAgent->getAssignedSeat() == NULL )
//// 	{
//// 		CString strErrorMsg;
//// 		strErrorMsg.Format(_T("Seat assignment failed. Please check aircraft capacity and seats count. Deplane Pax:%s"), m_pAgent->screenMobAgent());
//// 		m_pAgent->setState( Death );
//// 		return; //throw new OnBoardSimEngineException( new OnBoardDiagnose( strErrorMsg));
//// 	}
//// 	m_pAgent->setLocatorSite(m_pAgent->getAssignedSeat()->getLocatorSite());
//// 	
//// 	carrierExitDoor* pBestExit = m_pAgent->getExitDoor();
//// 	if(NULL == pBestExit)
//// 	{
//// 		m_pAgent->setState( Death );
//// 		return;
//// 		//throw new OnBoardSimEngineException( new OnBoardDiagnose(_T("cannot find entry door")));
//// 	}
//// 	m_pAgent->setFinalLocatorSite(pBestExit->getLocatorSite());
//// 	
//// 	Point vdir =  m_pAgent->getAssignedSeat()->getEntryLocation() - m_pAgent->getAssignedSeat()->getLocation();
//// 	vdir.length(1);
//// 	Point birthLocation  = m_pAgent->getAssignedSeat()->getLocation() - vdir;
//// 	m_pAgent->setDestination(birthLocation);
//// 	m_pAgent->writeLogEntry(p_time,false);
//// 
//// 	// debug purpose
//// 	m_pAgent->setDestination( m_pAgent->getAssignedSeat()->getLocation() );
//// 
//// 	m_pAgent->initBirthLocation();
//// 
//// 	//<TODO::> Exit through stair detecting.
//// 	m_pAgent->setState( WaitingForLeaveCarrier );
//// 
//// 	m_pAgent->generateEvent (p_time,false);
//}
//
//void DeplaneAgentBehavior::processWayFinding(ElapsedTime p_time)
//{
//	if( !m_pAgent->findMovementPath() )
//	{
//		m_pAgent->getLocatorSite()->makeAvailable(p_time);
//		CString strErrorMsg;
//		strErrorMsg.Format("%s. Cannot find movement path to destination", m_pAgent->screenMobAgent());
//		m_pAgent->setState( Death );
//		return;
//		//throw new OnBoardSimEngineException( new OnBoardDiagnose( strErrorMsg));
//	}
//
//	ElapsedTime _findingTime = 5L;// constant.
//
//	m_pAgent->setState( FreeMovingOnDeck );
//	m_pAgent->generateEvent (p_time + _findingTime,false);
//}
//
//
//void DeplaneAgentBehavior::processGeneralMovement(ElapsedTime p_time)
//{
//	LocatorSite* pMobSite = 
//		m_pAgent->getMovementPath()->getNextLocatorSite();
//
//	if(pMobSite == NULL)
//	{
//		m_pAgent->setState( ArriveAtExitDoor );
//		m_pAgent->generateEvent (p_time,false);
//		return;
//	}
//
//	if(pMobSite->requestService( m_pAgent))
//	{
//		m_pAgent->getLocatorSite()->makeAvailable(p_time);
//
//		m_pAgent->setLocatorSite( pMobSite );
//		m_pAgent->setState( FreeMovingOnDeck );
//		m_pAgent->generateEvent (p_time + m_pAgent->moveTime() ,false);
//	}
//	else
//	{
//		m_pAgent->getMovementPath()->addWaitingLocatorSite( pMobSite );
//		// waiting. 
//		ElapsedTime _waitingTime = 1L;// constant.
//		m_pAgent->setState( FreeMovingOnDeck );
//		m_pAgent->generateEvent (p_time + _waitingTime ,false);
//
//	}
//}
//
//void DeplaneAgentBehavior::processArriveAtExitDoor(ElapsedTime p_time)
//{
//	m_pAgent->getLocatorSite()->makeAvailable(p_time);
//
//	ElapsedTime _waitingTime = 5L;// constant.
//	m_pAgent->generateEvent (p_time + _waitingTime,false);	
//	m_pAgent->flushLog( p_time, false);
//	//m_pAgent->setState( Death );
//}
//
//
//void DeplaneAgentBehavior::processSeatingOnChair(ElapsedTime p_time)
//{
//	m_pAgent->setState( WayFindingOnDeck );
//	m_pAgent->generateEvent (p_time,false);
//}
//
//void DeplaneAgentBehavior::processWaitingForLeaveCarrier(ElapsedTime p_time)
//{
//	if(!mNeedStandup)
//	{
//		int rKey = random( 120 );///. 5m
//		long var = (long)rKey;
//		ElapsedTime seatingTime = var;// constant.
//
//
//		m_pAgent->setState( WaitingForLeaveCarrier );
//		m_pAgent->generateEvent (p_time + seatingTime,false);
//		mNeedStandup = true;
//		//stand up at next event time.
//	}
//	else
//	{
//		m_pAgent->setState( SeatingOnChair );
//		m_pAgent->generateEvent (p_time,false);
//	}
//
//}

