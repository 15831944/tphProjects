#include "StdAfx.h"
#include "OnBoardDefs.h"
#include "EnplaneAgentBehavior.h"
#include "carrierSpaceDiagnosis.h"
#include "LocatorSite.h"
#include "EnplaneAgent.h"
#include "carrierSpace/carrierDefs.h"
#include "OnBoardCarrierSpaces.h"
#include "Services/SeatAssignmentService.h"
#include "Engine\ARCportEngine.h"
#include "Engine\OnboardSimulation.h"
#include "Engine\OnboardFlightInSim.h"
#include "InputOnBoard\Door.h"
#include "../OnboardSeatInSim.h"


EnplaneAgentBehavior::EnplaneAgentBehavior(Person* pPerson)
//:m_pPerson( pPerson )
//,m_pDoor(NULL)
//,m_pSeat(NULL)
//,m_pOnboardFlight(NULL)
{

}

EnplaneAgentBehavior::~EnplaneAgentBehavior()
{
}

//extern long eventNumber;
//int EnplaneAgentBehavior::performanceMove(ElapsedTime p_time,bool bNoLog)
//{
//
//	long nAgentID = m_pPerson->getID();
// 	
//	if (bNoLog == false)
//	{ 
//		//writeLogEntry (p_time, false); 
//	}
//
//	//----------------------------------------------------------------------------
//	// Enplane Agent's state chart.
//	switch ( getState() )
//	{
//	case EntryOnboard:
//        processEnterOnBoardMode( p_time );
//		break;
//	case ArriveAtDoor:	
//		processArriveAtEntryDoor( p_time );
//		break;
//
//	case MoveToSeat:
//		processMoveToSeat( p_time );
//		break;
//
//	case ArriveAtSeat:
//		processArriveAtSeat( p_time );
//		break;
//	//case SeatingOnChair:
//	//	processSeatingOnChair(p_time);
//	//	break;
//	//case WaitingForCarrierTakeOff:
//	//	processCarrierTakeoffWaiting( p_time );
//	//	break;
//	case _DEATH:
//		processDeath(p_time);
//		break;
//	default:
//		{
//			ASSERT(0);
//		}
//		break;
//	}
//
//	return 1;
//}
//
//void EnplaneAgentBehavior::processEnterOnBoardMode(ElapsedTime p_time)
//{
//	//Get onboard flight
//	OnboardSimulation *pOnboardSimulation = NULL;
//	if(m_pPerson && m_pPerson->getEngine())
//		pOnboardSimulation = m_pPerson->getEngine()->GetOnboardSimulation();
//
//	if(pOnboardSimulation == NULL)
//	{
//		//flush log
//		setState(_DEATH);
//		GenetateEvent(p_time);
//		return;
//
//	}
//	Flight *pFlight = m_pPerson->getEngine()->m_simFlightSchedule.getItem (m_pPerson->GetCurFlightIndex());
//	if(pFlight == NULL)
//	{
//		//flush log
//		setState(_DEATH);
//		GenetateEvent(p_time);
//		return;
//	}
//	
//	m_pOnboardFlight = pOnboardSimulation->GetOnboardFlightInSim(pFlight,false);
//	if(m_pOnboardFlight == NULL)
//	{
//		//flush log
//		setState(_DEATH);
//		GenetateEvent(p_time);
//		return;
//	}
//		
//	//assign door
//	CDoor *pDoor = m_pOnboardFlight->GetEntryDoor();
//	if(pDoor == NULL)
//	{
//		FlushLog(p_time);
//		throw new OnBoardSimEngineException( new OnBoardDiagnose(m_pPerson->getID(),m_pOnboardFlight->getFlightID(),p_time,_T("cannot find entry door!")));
//		
//	}
//	m_pDoor = pDoor;
//	
//	//assign seat
//	OnboardSeatInSim *pSeat = m_pOnboardFlight->GetSeat();
//	if(pSeat == NULL)
//	{
//		FlushLog(p_time);
//		throw new OnBoardSimEngineException( new OnBoardDiagnose(m_pPerson->getID(),m_pOnboardFlight->getFlightID(),p_time,_T("cannot find Seat!")));
//	}
//	m_pSeat = pSeat;
//	pSeat->AssignToPerson(m_pPerson);
//
//	//set passenger's position to Entry door
//	//m_pPerson->setDestination();
//	//m_pPerson->setLocation();
//
//	setState(ArriveAtDoor);
//	GenetateEvent(p_time);
//}
//void EnplaneAgentBehavior::processArriveAtEntryDoor(ElapsedTime p_time)
//{
//	//write log
//
//	//set seat
//	//m_pPerson->setDestination()
//
//	setState(MoveToSeat);
//	GenetateEvent(p_time + m_pPerson->moveTime());
//
//	
//	
//}
//void EnplaneAgentBehavior::processMoveToSeat( ElapsedTime p_time )
//{
//	//write log
//	ARCVector3 seatPos;
//	if(m_pSeat)
//		m_pSeat->GetSeatPosition(seatPos);
//	//
//
//	setState(ArriveAtSeat);
//	GenetateEvent(p_time + m_pPerson->moveTime());
//}
//
//void EnplaneAgentBehavior::processArriveAtSeat(ElapsedTime p_time)
//{
//
//	//do nothing, wait for take off
//	setState(SitOnSeat);
//
////	m_pPerson->setDestination(Point());
//	
//	m_pPerson->writeLog(p_time + ElapsedTime(60L),false);
//	//FlushLog(p_time + ElapsedTime(60L));
//}
//
//void EnplaneAgentBehavior::processSeatingOnChair(ElapsedTime p_time)
//{
//	/*m_pAgent->setState( WaitingForCarrierTakeOff );
//	ElapsedTime _waitingTime = 0L;
//	m_pAgent->generateEvent( p_time + _waitingTime,false);*/
//}
//
//
//void EnplaneAgentBehavior::processCarrierTakeoffWaiting(ElapsedTime p_time)
//{
//	//if( seatAssignService()->isAllAgentSeatingDown() )
//	//{
//	//	UnsortedContainer< MobileAgent >& paxList = 
//	//		seatAssignService()->GetMobileAgentList();
//	//	for (int i = 0 ; i < paxList.getCount(); i++)
//	//	{
//	//		MobileAgent* pAgent = paxList.getItem( i );
//	//		pAgent->flushLog( p_time, false );
//	//	}
//	//	return;
//	//}
//	//m_pAgent->setState( WaitingForCarrierTakeOff );
//	//ElapsedTime _waitingTime = 60L;
//	//m_pAgent->generateEvent( p_time + _waitingTime,true);
//
//}
//
//void EnplaneAgentBehavior::setState( short newState )
//{
//	if(m_pPerson)
//		m_pPerson->setState(newState);
//}
//
//int EnplaneAgentBehavior::getState( void ) const
//{
//	return m_pPerson->getState();
//}
//
//void EnplaneAgentBehavior::GenetateEvent( ElapsedTime time )
//{
//	if(m_pPerson)
//		m_pPerson->generateEvent(time, false);
//}
//
//void EnplaneAgentBehavior::FlushLog(ElapsedTime p_time)
//{
//	if(m_pPerson)
//		m_pPerson->flushLog(p_time,false);
//}
//
//void EnplaneAgentBehavior::processDeath( ElapsedTime p_time )
//{
//	FlushLog(p_time);
//}

