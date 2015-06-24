#include "stdafx.h"
#include "PaxEnplaneBehavior.h"
#include "person.h"
#include "Engine\ARCportEngine.h"
#include "Engine\OnboardSimulation.h"
#include "Engine\OnboardFlightInSim.h"
#include "InputOnBoard\Door.h"
#include "OnboardSeatInSim.h"
#include "OnBoard\OnBoardDefs.h"
#include <inputonboard\Deck.h>
#include "Airside\AirsideSimulation.h"
#include <InputOnBoard\Seat.h>
//#include "InputOnBoard\Storage.h"
#include "OnboardStorageInSim.h"
#include "OnboardDoorInSim.h"
#include "PaxOnboardFreeMovingLogic.h"
#include "CorridorGraphMgr.h"
#include "EntryPointInSim.h"
#include "OnboardCellGraph.h"
#include "OnboardCellFreeMoveLogic.h"
#include "OnboardCellFreeMoveEvent.h"
#include "OnboardDeckGroundInSim.h"
#include "OnBoardDeviceInSim.h"
#include "OnboardCellSeatGivePlaceLogic.h"
#include "GroupLeaderInfo.h"
#include "GroupInfo.h"
#include "TargetLocationVariableInSim.h"
#include "CarryonStoragePrioritiesInSim.h"



PaxEnplaneBehavior::PaxEnplaneBehavior(Person* pPerson)
:PaxOnboardBaseBehavior(pPerson)
//,m_pDoor(NULL)
,m_dDistCheckingPointInCorridor(0.0)
,m_dDistTempPlaceInCorridor(0.0)
{

}

PaxEnplaneBehavior::~PaxEnplaneBehavior()
{

}
extern long eventNumber;
int PaxEnplaneBehavior::performanceMove(ElapsedTime p_time,bool bNoLog)
{

	long nAgentID = m_pPerson->getID();

	if (bNoLog == false)
	{ 
		//writeLogEntry (p_time, false); 
	}

	//----------------------------------------------------------------------------
	// Enplane Agent's state chart.
	switch ( getState() )
	{
	case EntryOnboard:
		processEnterOnBoardMode( p_time );
		break;

	case ArriveAtDoor:	
		processArriveAtEntryDoor( p_time );
		break;
	case ArriveAtDeviceEntry:
		processArrivalAtDeviceEntry(p_time);
		break;
	case ServiceOnDevice:
		processMovesOnDevice(p_time);
		break;
	case LeavesDevice:
		processLeavesDevice(p_time);
		break;
	case MoveToDest:
		ProcessMoveFromDevice(p_time);
		break;
	case ArriveAtCheckingPoint:
		processArriveAtCheckingPoint(p_time);		
		break;
	case ReturnToCheckingPoint:
		processReturnToCheckingPoint(p_time);
		break;
	case ArriveAtSeatEntry:
		//processArriveAtSeatEntry( p_time );
		//break;

	case ArriveAtStorage:
		processArriveAtStorage( p_time);
		break;

	case ArriveAtSeat:
		//processArriveAtSeat( p_time );
		break;

	case _DEATH:
		processDeath(p_time);
		break;
	default:
		{
			ASSERT(0);
		}
		break;
	}
	return 1;
}

void PaxEnplaneBehavior::processEnterOnBoardMode(ElapsedTime p_time)
{
	//Get onboard flight
	OnboardSimulation *pOnboardSimulation = NULL;
	if(m_pPerson && m_pPerson->getEngine())
		pOnboardSimulation = m_pPerson->getEngine()->GetOnboardSimulation();

	if(pOnboardSimulation == NULL)
	{
		//flush log
		setState(_DEATH);
		GenerateEvent(p_time);
		return;

	}
	AirsideFlightInSim *pFlight = m_pPerson->getEngine()->GetAirsideSimulation()->GetAirsideFlight(m_pPerson->GetCurFlightIndex());
	if(pFlight == NULL)
	{
		//flush log
		setState(_DEATH);
		GenerateEvent(p_time);
		return;
	}

	m_pOnboardFlight = pOnboardSimulation->GetOnboardFlightInSim(pFlight,false);
	m_pOnboardFlight->GenerateCloseDoorEvent(p_time);
	
	if(m_pOnboardFlight == NULL)
	{
		//flush log
		setState(_DEATH);
		GenerateEvent(p_time);
		return;
	}

	m_pOnboardFlight->DecCount();

	m_pOnboardFlight->ProcessCloseDoor(p_time);
	//assign door
//	OnboardDoorInSim *pDoor = m_pOnboardFlight->GetEntryDoor();

	//assign seat
	CString strMessage;
	OnboardSeatInSim *pSeat = m_pOnboardFlight->GetSeat(m_pPerson,strMessage);
	if(pSeat == NULL)
	{
		FlushLog(p_time);
		throw new OnBoardSimEngineException( new OnBoardDiagnose(m_pPerson->getID(),m_pOnboardFlight->getFlightID(),p_time,strMessage));
	}
	m_pSeat = pSeat;
	pSeat->AssignToPerson(m_pPerson);

	OnboardDoorInSim *pDoor = m_pOnboardFlight->GetDoor(m_pPerson,m_pSeat);
	if(pDoor == NULL)
	{
		FlushLog(p_time);
		throw new OnBoardSimEngineException( new OnBoardDiagnose(m_pPerson->getID(),m_pOnboardFlight->getFlightID(),p_time,_T("cannot find entry door!")));

	}
	m_pDoor = pDoor;
	//set passenger's position to Entry door
	ARCVector3 doorPos;
	pDoor->GetPosition(doorPos);
	SetEnterOnboardLocation(doorPos);
	setDestination(doorPos);
//	setLocation(doorPos);

	setState(ArriveAtDoor);
	
	//resouce
	SetEnplaneTime(p_time);
	//m_pPerson->getLogEntry().setEnplanTime(p_time);
	GenerateEvent(p_time);
}
void PaxEnplaneBehavior::processArriveAtEntryDoor(ElapsedTime p_time)
{
	//write log
	SetCurrentResource(m_pDoor);

	writeLog(p_time,false);
//	return;
	//set seat entry position,
	//for now, the value use the seat position, it need to be instead with the actual position

	if(m_pSeat && m_pSeat->GetEntryPoint())
	{	
		//check the door is available
		if(!m_pDoor->IsAvailable(this))
		{//occupancy by another guy
			//GenerateEvent(p_time + ElapsedTime(2L));//try after 2 seconds
	/*		OnboardCellInSim *pCell = m_pDoor->GetEntryCell();
			if(pCell)
				pCell->AddWaitingPax(this);*/
			m_pDoor->AddWaitingPax(this,p_time);
			//m_pDoor->ReleaseNextPax(this,p_time);

			return;
		}
		m_pDoor->DeletePaxWait(this);
		m_pDoor->ReleaseNextPax(p_time);

		OnboardCellInSim* pOriginalCell = m_pDoor->GetEntryCell();
		OnboardCellInSim* pDestCell = m_pSeat->GetEntryPoint()->GetOnboardCell();

		OnboardCellPath routePath;
		if (pOriginalCell->GetDeckGround() != pDestCell->GetDeckGround())
		{
			m_pDeviceInSim = SelectDeviceMoveDest(pOriginalCell,pDestCell);
			if (m_pDeviceInSim == NULL)
			{
				CDeck* pOriginalDeck = pOriginalCell->GetDeckGround()->GetDeck();
				CDeck* pDestDeck = pDestCell->GetDeckGround()->GetDeck();
				CString strError;
				strError.Format(_T("Can not move from deck: %s to deck: %s"),pOriginalDeck->GetName(),pDestDeck->GetName());
				throw new OnBoardSimEngineException( new OnBoardDiagnose(m_pPerson->getID(),m_pOnboardFlight->getFlightID(),p_time,strError));
			}
			else
			{
				OnboardCellInSim* pFromCellInSim = m_pDeviceInSim->GetEntryCell(pOriginalCell->GetDeckGround());
		//		OnboardCellInSim* pToCellInSim = m_pDeviceInSim->GetOnboardCell(pDestCell->GetDeckGround());

				OnboardCellPath originPath;
				OnboardCellPath destPath;

				if(m_pOnboardFlight->getCellGraph())
				{
					m_pOnboardFlight->getCellGraph()->FindPath(pOriginalCell,pFromCellInSim,routePath);
					//set end state
					m_pCellFreeMoveLogic->SetEndPaxState(ArriveAtDeviceEntry);
				}	
			}
		}
		else
		{
			//if(m_pOnboardFlight->getCellGraph())
			//	m_pOnboardFlight->getCellGraph()->FindPath(m_pDoor,m_pSeat,routePath);

			//cut to checking point
			FindCellPathToSeat(m_pDoor,m_pSeat,routePath);
			if(routePath.getCellCount() < 2)
			{
				FlushLog(p_time);
				throw new OnBoardSimEngineException( new OnBoardDiagnose(m_pPerson->getID(),m_pOnboardFlight->getFlightID(),p_time,_T("cannot find path to seat!")));
			}


			//get the assumption location to stow his belongs
			OnboardCellPath routeToStorDevice;
			OnboardCellPath routeDeviceToSeat;

			TargetLocationVariableInSim *pTargetLocation = m_pOnboardFlight->GetTargetLocation();
			if(pTargetLocation)
			{
				double dDistance = pTargetLocation->GetOffsetDist(*(m_pPerson->getType().GetPaxType()));
				if(dDistance > 0)
				{
					//divide the path to 2 parts
					//go to storage device and then move to seat entry
					int nDeviceCount = (int)dDistance/GRID_WIDTH;

					int nCellCount = routePath.getCellCount();
					for (int nCell = 0; nCell < nCellCount; ++ nCell)
					{
						if(nCellCount - nCell > nDeviceCount)
						{
							routeToStorDevice.addCell(routePath.getCell(nCell));
						}
						else
						{
							routeDeviceToSeat.addCell(routePath.getCell(nCell));
						}
					}
				}
				else
					routeToStorDevice = routePath ;


			}
			routePath.Clear();
			routePath = routeToStorDevice;
			m_routeDeviceToSeat = routeDeviceToSeat;

			//set end state
			if(m_routeDeviceToSeat.getCellCount() == 0)
				m_pCellFreeMoveLogic->SetEndPaxState(ArriveAtCheckingPoint);
			else
				m_pCellFreeMoveLogic->SetEndPaxState(ArriveAtStorage);


		}
	
 		//try to move to the seat entry
 		PaxCellLocation entryLocation = m_pDoor->getEntryLocation();
 		entryLocation.PaxOccupy(this);//take the place

 		//start moving
 		m_pCellFreeMoveLogic->setCellPath(routePath);//path
 		m_pCellFreeMoveLogic->setCurLocation(entryLocation);//start position
 		
 
 		//deliver to Free Moving Logic, the passenger would come back at state ArriveAtCheckingPoint
 		OnboardCellFreeMoveEvent *pMoveEvent = new OnboardCellFreeMoveEvent(m_pCellFreeMoveLogic);
 		pMoveEvent->setTime(p_time);
 		pMoveEvent->addEvent();

	}
	else
	{	
		FlushLog(p_time);
		throw new OnBoardSimEngineException( new OnBoardDiagnose(m_pPerson->getID(),m_pOnboardFlight->getFlightID(),p_time,_T("cannot find Seat!")));
	}

}

void PaxEnplaneBehavior::processArrivalAtDeviceEntry(ElapsedTime p_time)
{
	SetCurrentResource(NULL);
	ASSERT(m_pDeviceInSim);
	OnboardCellInSim* pOriginalCell = m_pDoor->GetEntryCell();
	OnboardCellPath freeMovePath;
	freeMovePath.addCell(m_pDeviceInSim->GetEntryCell(pOriginalCell->GetDeckGround()));
	freeMovePath.addCell(m_pDeviceInSim->GetOnboardCell(pOriginalCell->GetDeckGround()));
	WriteCellPathLog(freeMovePath,p_time);
	
	setState(ServiceOnDevice);
	GenerateEvent(p_time);
}

void PaxEnplaneBehavior::processMovesOnDevice(ElapsedTime p_time)
{
	SetCurrentResource(m_pDeviceInSim);
	ASSERT(m_pDeviceInSim);
	OnboardCellInSim* pOriginalCell = m_pDoor->GetEntryCell();
	OnboardCellInSim* pDestCell = m_pSeat->GetEntryPoint()->GetOnboardCell();
	OnboardCellPath freeMovePath;
	freeMovePath.addCell(m_pDeviceInSim->GetOnboardCell(pOriginalCell->GetDeckGround()));
	freeMovePath.addCell(m_pDeviceInSim->GetOnboardCell(pDestCell->GetDeckGround()));
	WriteCellPathLog(freeMovePath,p_time);

	setState(LeavesDevice);
	GenerateEvent(p_time);
}

void PaxEnplaneBehavior::processLeavesDevice(ElapsedTime p_time)
{
	ASSERT(m_pDeviceInSim);
	OnboardCellInSim* pDestCell = m_pSeat->GetEntryPoint()->GetOnboardCell();
	OnboardCellPath freeMovePath;
	freeMovePath.addCell(m_pDeviceInSim->GetOnboardCell(pDestCell->GetDeckGround()));
	freeMovePath.addCell(m_pDeviceInSim->GetEntryCell(pDestCell->GetDeckGround()));
	WriteCellPathLog(freeMovePath,p_time);

	setState(MoveToDest);
	GenerateEvent(p_time);
}

void PaxEnplaneBehavior::ProcessMoveFromDevice(ElapsedTime p_time)
{
	ASSERT(m_pDeviceInSim);
	OnboardCellInSim* pDestCell = m_pSeat->GetEntryPoint()->GetOnboardCell();
	OnboardCellInSim* pDeviceEntryCell = m_pDeviceInSim->GetEntryCell(pDestCell->GetDeckGround());

	OnboardCellPath routePath;
	if(m_pOnboardFlight->getCellGraph())
	{
		//cut to checking point
		FindCellPathToSeat(pDeviceEntryCell,m_pSeat,routePath);
		if(routePath.getCellCount() < 2)
		{
			FlushLog(p_time);
			throw new OnBoardSimEngineException( new OnBoardDiagnose(m_pPerson->getID(),m_pOnboardFlight->getFlightID(),p_time,_T("cannot find path to seat!")));
		}

		//m_pOnboardFlight->getCellGraph()->FindPath(pDeviceEntryCell,pDestCell,routePath);
	}

	////try to move to the seat entry
	//PaxCellLocation entryLocation = m_pDoor->getEntryLocation();
	//entryLocation.PaxOccupy(this);//take the place
	
	
	//get the assumption location to stow his belongs
	OnboardCellPath routeToStorDevice;
	OnboardCellPath routeDeviceToSeat;

	TargetLocationVariableInSim *pTargetLocation = m_pOnboardFlight->GetTargetLocation();
	if(pTargetLocation)
	{
		double dDistance = pTargetLocation->GetOffsetDist(*(m_pPerson->getType().GetPaxType()));
		if(dDistance > 0)
		{
			//divide the path to 2 parts
			//go to storage device and then move to seat entry
			int nDeviceCount = (int)dDistance/GRID_WIDTH;
			
			int nCellCount = routePath.getCellCount();
			for (int nCell = 0; nCell < nCellCount; ++ nCell)
			{
				if(nCellCount - nCell > nDeviceCount)
				{
					routeToStorDevice.addCell(routePath.getCell(nCell));
				}
				else
				{
					routeDeviceToSeat.addCell(routePath.getCell(nCell));
				}
			}
		}
	}
	m_routeDeviceToSeat = routeDeviceToSeat;

	//start moving
	PaxCellLocation curLocation;
	curLocation.setPathCell(pDeviceEntryCell);
	curLocation.setCellCenter(pDeviceEntryCell);
	curLocation.PaxOccupy(this);
	
	m_pCellFreeMoveLogic->setCellPath(routeToStorDevice);//path
	m_pCellFreeMoveLogic->setCurLocation(curLocation);//start position

	//set end state
	if(m_routeDeviceToSeat.getCellCount() == 0)
		m_pCellFreeMoveLogic->SetEndPaxState(ArriveAtCheckingPoint);
	else
		m_pCellFreeMoveLogic->SetEndPaxState(ArriveAtStorage);

	//deliver to Free Moving Logic, the passenger would come back at state ArriveAtCheckingPoint
	OnboardCellFreeMoveEvent *pMoveEvent = new OnboardCellFreeMoveEvent(m_pCellFreeMoveLogic);
	pMoveEvent->setTime(p_time);
	pMoveEvent->addEvent();
}

void PaxEnplaneBehavior::processArriveAtCheckingPoint( ElapsedTime p_time )
{
	//OnboardCellPath freeMovePath;
	//freeMovePath.addCell(m_pSeat->GetEntryPoint()->GetOnboardCell());
	//freeMovePath.addCell(m_pSeat->GetOnboardCellInSim());

	//WriteCellPathLog(freeMovePath,p_time);

	//setState(ArriveAtStorage);
	//GenerateEvent(p_time);

	EntryPointInSim *pEntryPoint = m_pSeat->GetEntryPoint();
	OnboardCellSeatGivePlaceLogic *pLogic = pEntryPoint->GetGivePlaceLogic();
	pLogic->ProcessEvent(this,p_time);

}

void PaxEnplaneBehavior::processReturnToCheckingPoint(ElapsedTime p_time)
{
	EntryPointInSim *pEntryPoint = m_pSeat->GetEntryPoint();
	OnboardCellSeatGivePlaceLogic *pLogic = pEntryPoint->GetGivePlaceLogic();
	pLogic->ProcessEvent(this,p_time);
}

void PaxEnplaneBehavior::processArriveAtSeatEntry(ElapsedTime p_time)
{
	m_curLocation.PaxClear(this, p_time);

	writeLog(p_time,true);

	CPoint2008 seatPos;
	m_pSeat->GetLocation(seatPos);
	setDestination(seatPos);
	p_time+=moveTime();
//	setLocation(seatPos);
	setState(ArriveAtSeat);
	writeLog(p_time,true);

	GenerateEvent(p_time);
}


OnboardStorageInSim* PaxEnplaneBehavior::GetStorage()
{
	return NULL;
}
//change storage's location to pax's destination
ARCVector3 PaxEnplaneBehavior::GetStorageLocation(OnboardStorageInSim *pStorage) const 
{
	if (pStorage)// will be implemented
	{
		//ARCVector3 ptPos = pStorage->GetPosition();
		//ptPos[VZ] = deck z value
	}
	return location;
}

void PaxEnplaneBehavior::processArriveAtStorage(ElapsedTime p_time)
{

	//stow carry on
	double dSearchDist = m_pOnboardFlight->GetTargetLocation()->GetSearchRadius(*(m_pPerson->getType().GetPaxType()));

	std::vector<OnboardStorageInSim*> vStorageInSim = m_pOnboardFlight->GetCanReachStorageDevice(m_pSeat->GetGround(),
																									m_curLocation.getCellCenter()->getLocation(),
																									dSearchDist);
	if(vStorageInSim.size() > 0)
		StorageOperation(p_time,vStorageInSim);

	//move to seat
	if(m_routeDeviceToSeat.getCellCount() > 0)
	{

		//start moving
		m_pCellFreeMoveLogic->setCellPath(m_routeDeviceToSeat);//path
		m_pCellFreeMoveLogic->setCurLocation(m_curLocation);//start position

		m_pCellFreeMoveLogic->SetEndPaxState(ArriveAtCheckingPoint);
		//deliver to Free Moving Logic, the passenger would come back at state ArriveAtCheckingPoint
		OnboardCellFreeMoveEvent *pMoveEvent = new OnboardCellFreeMoveEvent(m_pCellFreeMoveLogic);
		pMoveEvent->setTime(p_time);
		pMoveEvent->addEvent();
	}

	////write log
	////writeLog(p_time);

	//m_curStatus.m_nState = ArriveAtStorage;//set current status

	////hands up
	//ElapsedTime eHandsuptime = ElapsedTime(2L);
	//setState(PutHandsUp);
	//writeLog(p_time);// write log for carry on
	//p_time = p_time + eHandsuptime;


	////storage carry on
	//ElapsedTime eOperationTime = ElapsedTime(5L);
	//setState(StorageCarryOn);
	//writeLog(p_time); //write log for carry on
	//p_time = p_time + eOperationTime;


	////hand down
	//ElapsedTime eHandsdowntime = ElapsedTime(2L);
	//setState(PutHandsDown);
	//writeLog(p_time);
	//p_time = p_time + eHandsdowntime;

	////setState(OnboardFreeMoving);
	//setState(ArriveAtSeatEntry);
	//GenerateEvent(p_time);

}



void PaxEnplaneBehavior::processArriveAtSeat(ElapsedTime p_time)
{
	if(!m_pSeat)
		return;
	
	//////////////////////////////////////////////////////////////////////////
	//write arrive at seat log
// 	writeLog(p_time);
// 
//  	ARCVector3 seatPos;
//  	m_pSeat->GetPosition(seatPos);
//  	setDestination(seatPos);
//  	p_time += moveTime();
//  	writeLog(p_time,false);
	setState(ArriveAtSeat);
//	setLocation(seatPos);
	double dRot = m_pSeat->m_pSeat->GetRotation();
	ARCVector2 vdir(5,0);
	vdir.Rotate(dRot);
	setDestination(location+ ARCVector3(vdir,0) );

	ElapsedTime nextT = p_time+ moveTime();
	writeLog(nextT,true);

	//////////////////////////////////////////////////////////////////////////
	setState(SitOnSeat);
	setLocation(m_ptDestination);
	writeLog(nextT,false);
}





void PaxEnplaneBehavior::GenerateEvent( ElapsedTime eventTime )
{
	GenerateEvent(m_pPerson, eventTime);
}

void PaxEnplaneBehavior::GenerateEvent(Person *pPerson, ElapsedTime eventTime )
{

	bool bDisallowGroup = true;
	
	enum EVENT_OPERATION_DECISTION enumRes =pPerson->m_pGroupInfo->ProcessEvent( eventTime, bDisallowGroup );
	if( enumRes == REMOVE_FROM_PROCESSOR_AND_NO_EVENT )
	{
		return;
	}
	if( enumRes == NOT_GENERATE_EVENT )
		return;
	
	pPerson->MobileElement::generateEvent( eventTime, false);

}

void PaxEnplaneBehavior::FlushLog(ElapsedTime p_time)
{
	if(m_pPerson)
		m_pPerson->flushLog(p_time,false);
}

void PaxEnplaneBehavior::processDeath( ElapsedTime p_time )
{
	FlushLog(p_time);
}


void PaxEnplaneBehavior::FindCellPathToSeat( OnboardDoorInSim *pDoor, OnboardSeatInSim *pSeatInSim,OnboardCellPath& movePath ) const
{
	OnboardCellInSim *pDoorCell = pDoor->GetEntryCell();

	//EntryPointInSim *pEntryPoint = pSeatInSim->GetEntryPoint();

	//std::vector<PaxCellLocation >vCheckCell = pEntryPoint->GetCheckingPoints();

	//ASSERT(vCheckCell.size() > 0);
	//if(vCheckCell.size() == 0)
	//{
	//	//movePath.addCell(pDoorCell);
	//	//movePath.addCell(pEntryPoint->GetOnboardCell());
	//	////pop up a message

	//	return;
	//}

	////get first checking point
	////get second checking point

	//for (int nCheckPint = 0; nCheckPint < static_cast<int>(vCheckCell.size()); ++ nCheckPint)
	//{	
	//	OnboardCellPath tmpCellPath;
	//	PaxCellLocation checkLocation = vCheckCell[nCheckPint];
	//	m_pOnboardFlight->getCellGraph()->FindPath(pDoorCell, checkLocation.getCellCenter(), tmpCellPath);
	//	if(tmpCellPath.getCellCount() < movePath.getCellCount() ||
	//		movePath.getCellCount() == 0)
	//	{
	//		movePath = tmpCellPath;
	//		m_paxCheckingLocation = checkLocation;
	//	}
	//}
	FindCellPathToSeat(pDoorCell, pSeatInSim, movePath);
}

void PaxEnplaneBehavior::FindCellPathToSeat( OnboardCellInSim *pStartCell, OnboardSeatInSim *pSeatInSim, OnboardCellPath& movePath ) const
{
	EntryPointInSim *pEntryPoint = pSeatInSim->GetEntryPoint();

	std::vector<PaxCellLocation >vCheckCell = pEntryPoint->GetCheckingPoints();

	ASSERT(vCheckCell.size() > 0);
	if(vCheckCell.size() == 0)
	{
		//movePath.addCell(pDoorCell);
		//movePath.addCell(pEntryPoint->GetOnboardCell());
		////pop up a message

		return;
	}

	//get first checking point
	//get second checking point

	for (int nCheckPint = 0; nCheckPint < static_cast<int>(vCheckCell.size()); ++ nCheckPint)
	{	
		OnboardCellPath tmpCellPath;
		PaxCellLocation checkLocation = vCheckCell[nCheckPint];
		m_pOnboardFlight->getCellGraph()->FindPath(pStartCell, checkLocation.getCellCenter(), tmpCellPath);
		if (tmpCellPath.getCellCount() > 0)
		{
			if(tmpCellPath.getCellCount() < movePath.getCellCount() ||
				movePath.getCellCount() == 0)
			{
				movePath = tmpCellPath;
				m_paxCheckingLocation = checkLocation;
			}
		}
	}
}

PaxOnboardBaseBehavior* PaxEnplaneBehavior::GetOnboardBehavior(Person* pPerson)
{
	PaxOnboardBaseBehavior* pOnboardBehavior = new PaxEnplaneBehavior(pPerson); 
	return pOnboardBehavior;
}

void PaxEnplaneBehavior::CarryonMoveToSeatFront(ElapsedTime& time)
{
	//set visitor separate
	Passenger* pPassenger = (Passenger*)m_pPerson;
	int nCount = pPassenger->m_pVisitorList.size();
	
	ElapsedTime eCarryonMoveTime;
	eCarryonMoveTime.setPrecisely(50);
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = (PaxVisitor*)pPassenger->m_pVisitorList[i];
		if (pVis == NULL)
			continue;
		PaxEnplaneBehavior* pOnboardBehavior = (PaxEnplaneBehavior*)pVis->getOnboardBehavior();
		ASSERT(pOnboardBehavior);
		//non passenger move to seat front
		{
			CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)pVis->m_pGroupInfo;
			//check in group
			pGroupLeaderInfo->SetInGroup(false);
			ARCVector3 frontSeat;
			m_pSeat->GetFrontPosition(frontSeat);
			pOnboardBehavior->setState(MoveToDeckGround);
			pOnboardBehavior->setDestination(frontSeat);
			pOnboardBehavior->setLocation(frontSeat);
			time += eCarryonMoveTime;//give 2secs to move to seat front

			pOnboardBehavior->writeLog(time);

			//write follower log
			MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
			int nFollowerCount = elemList.getCount();
			for( int i=0; i< nFollowerCount; i++ )
			{
				PaxVisitor* _pFollower = (PaxVisitor*) elemList.getItem( i );
				if(_pFollower == NULL)
					continue;

				PaxOnboardBaseBehavior* pOnboardFollowerBehavior = _pFollower->getOnboardBehavior();
				ASSERT(pOnboardFollowerBehavior);
				{
					pOnboardFollowerBehavior->setDestination(frontSeat);
					pOnboardBehavior->setLocation(frontSeat);
					pOnboardFollowerBehavior->writeLog(time);
				}
			}			
		}
	}
}

void PaxEnplaneBehavior::SeparateAtSeat(ElapsedTime& p_time,PaxVisitor* pVis)
{
	ASSERT(pVis);
	PaxOnboardBaseBehavior* pOnboardBehavior = pVis->getOnboardBehavior();
	ASSERT(pOnboardBehavior);
	if (pOnboardBehavior == NULL)
		return;


	//write current position
	pOnboardBehavior->setState(MoveToStorage); //start
	pOnboardBehavior->writeLog(p_time);

	//move to seat storage
	ARCVector3 seatPt;
	seatPt = m_pSeat->GetRandomPoint();
	pOnboardBehavior->setDestination(seatPt);
	p_time += pOnboardBehavior->moveTime();
	pOnboardBehavior->setState(MoveUnderSeatGround);
	pOnboardBehavior->writeLog(p_time);

	pOnboardBehavior->setState(StorageCarryOn); //end
	pOnboardBehavior->SetCurrentResource(m_pSeat);
	pOnboardBehavior->writeLog(p_time);

	pOnboardBehavior->SetCurrentResource(NULL);

	m_pSeat->addVisitor(pVis);
}

void PaxEnplaneBehavior::VisitorGroupSeparateAtSeat(ElapsedTime& p_time,PaxVisitor* pVis)
{
	ASSERT(pVis);
	if(pVis == NULL)
		return;

	if (pVis->m_pGroupInfo->IsFollower())
		return;
	
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)pVis->m_pGroupInfo;
	//check in group
	pGroupLeaderInfo->SetInGroup(false);
	SeparateAtSeat(p_time,pVis);

	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i< nFollowerCount; i++ )
	{
		PaxVisitor* _pFollower = (PaxVisitor*) elemList.getItem( i );
		if(_pFollower == NULL)
			continue;

		SeparateAtSeat(p_time,_pFollower);
	}
}

void PaxEnplaneBehavior::SeatOperation(ElapsedTime& p_time)
{
	//passenger has visitor list
	if (m_pPerson->getType().GetTypeIndex() != 0)
		return;

	//put carrion down seat front
	CarryonMoveToSeatFront(p_time);

	//set visitor separate
	Passenger* pPassenger = (Passenger*)m_pPerson;
	int nCount = pPassenger->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = (PaxVisitor*)pPassenger->m_pVisitorList[i];
		if (pVis == NULL)
			continue;
		
		pPassenger->m_pVisitorList[i] = NULL;
		VisitorGroupSeparateAtSeat(p_time,pVis);
	}
}

void PaxEnplaneBehavior::VisitorGroupSeparateAtStorage(ElapsedTime& time,PaxVisitor* pVis,std::vector<OnboardStorageInSim*> vStorageInSim)
{
	ASSERT(pVis);
	if(pVis == NULL)
		return;

	if (pVis->m_pGroupInfo->IsFollower())
		return;

	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)pVis->m_pGroupInfo;
	//check in group
	pGroupLeaderInfo->SetInGroup(false);
	
	for (int nDevice = 0; nDevice < (int)vStorageInSim.size(); ++nDevice)
	{
		OnboardStorageInSim *pDevice = vStorageInSim[nDevice];
		if(pDevice && pDevice->GetAvailableSpace() > pVis->getOnboardBehavior()->GetVolume(m_pOnboardFlight))
		{
			SeparateAtStorageOperation(time,pVis,pDevice);
			break;
		}
	}



	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i< nFollowerCount; i++ )
	{
		PaxVisitor* _pFollower = (PaxVisitor*) elemList.getItem( i );
		if(_pFollower == NULL)
			continue;

		for (int nDevice = 0; nDevice < (int)vStorageInSim.size(); ++nDevice)
		{
			OnboardStorageInSim *pDevice = vStorageInSim[nDevice];
			if(pDevice && pDevice->GetAvailableSpace() > pVis->getOnboardBehavior()->GetVolume(m_pOnboardFlight))
			{
				SeparateAtStorageOperation(time,_pFollower,pDevice);
				break;
			}
		}
	}
}

//the value of return: 
//true: all the carrion move to storage
bool PaxEnplaneBehavior::CalculateMoveToStorageCarrion(std::vector<PaxVisitor*>& vVisitorList)
{
	Passenger* pPassenger = (Passenger*)m_pPerson;
	int nCount = pPassenger->m_pVisitorList.size();
	
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = (PaxVisitor*)pPassenger->m_pVisitorList[i];
		if (pVis == NULL)
			continue;
		
		FlightCarryonPriorityInSim * pPriority = m_pOnboardFlight->GetCarryonPriority();
		if(pPriority)
		{
			int nType = pPriority->GetDevicePriority(*(pVis->getType().GetPaxType()),pVis->getType().GetTypeIndex());
			if(nType == STORAGEDEVICE)
				vVisitorList.push_back(pVis);
		}
	}

	return vVisitorList.empty()?false:true;
}

void PaxEnplaneBehavior::StorageOperation(ElapsedTime& time,std::vector<OnboardStorageInSim*> vStorageInSim)
{
	OnboardStorageInSim*pStorageInSim = vStorageInSim[0];

	//passenger has visitor list
	if (m_pPerson->getType().GetTypeIndex() != 0)
		return;

	//put down non passenger to deck ground, init carrion who did not want put down
	std::vector<PaxVisitor*> vVisitorList;
	if (CalculateMoveToStorageCarrion(vVisitorList))
	{
		CarryonMoveToDeckGround(time);
	}

	//no carrion want to move to storage
	if (vVisitorList.empty())
		return;

	//carrion move to storage and carrion separate
	Passenger* pPassenger = (Passenger*)m_pPerson;
	int nCount = pPassenger->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = (PaxVisitor*)pPassenger->m_pVisitorList[i];
		if (pVis == NULL)
			continue;
		PaxEnplaneBehavior* pOnboardBehavior = (PaxEnplaneBehavior*)pVis->getOnboardBehavior();
		if(std::find(vVisitorList.begin(),vVisitorList.end(),pVis) != vVisitorList.end())
		{

			//check non passenger type is single and group size is single

			CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)pVis->m_pGroupInfo;
			if (nCount > 1 || pGroupLeaderInfo->GetGroupSize() > 1)
			{
				//check if have enough space
				//pStorageInSim
			

				if (!pVis->m_pGroupInfo->IsFollower())
				{
					std::vector<double> vCarryoVolume;
					double dLeadVolume = pVis->getOnboardBehavior()->GetVolume(m_pOnboardFlight);
					vCarryoVolume.push_back(dLeadVolume);

					CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)pVis->m_pGroupInfo;
					MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
					int nFollowerCount = elemList.getCount();
					for( int i=0; i< nFollowerCount; i++ )
					{
						PaxVisitor* _pFollower = (PaxVisitor*) elemList.getItem( i );
						if(_pFollower == NULL)
							continue;

						double dFollowVolume = pVis->getOnboardBehavior()->GetVolume(m_pOnboardFlight);
						vCarryoVolume.push_back(dFollowVolume);
					}

					//check storage device available space
					int nDeviceCount = vStorageInSim.size();
					int nCurCarryon = 0;
					bool bHaveEnoughSpace = false;
					for (int nDevice = 0; nDevice < nDeviceCount; ++ nDevice)
					{
						OnboardStorageInSim *pTemp = vStorageInSim[nDevice];
						if(pTemp)
						{	
							double dIncrmentVolume = 0;
							for (int nCarryon = nCurCarryon; nCarryon < (int)vCarryoVolume.size(); ++nCarryon)
							{
								if(pTemp->GetAvailableSpace() - dIncrmentVolume < vCarryoVolume[nCarryon])
									break;
								dIncrmentVolume += vCarryoVolume[nCarryon];
								nCurCarryon += 1;
							}
						}
					}
					if(nCurCarryon != (int)vCarryoVolume.size())
						continue;//cannot 
				}

				pPassenger->m_pVisitorList[i] = NULL;
				VisitorGroupSeparateAtStorage(time,pVis,vStorageInSim);


			}
			else
			{		
				
				double dLeadVolume = pVis->getOnboardBehavior()->GetVolume(m_pOnboardFlight);
				//check storage device available space
				int nDeviceCount = vStorageInSim.size();
				for (int nDevice = 0; nDevice < nDeviceCount; ++ nDevice)
				{
					OnboardStorageInSim *pTemp = vStorageInSim[nDevice];
					if(pTemp)
					{
						if(pTemp->GetAvailableSpace() - dLeadVolume >= 0)
						{
							// start stow the carryon
							AdjustDirectionStorage(time,pStorageInSim,false);
							pPassenger->m_pVisitorList[i] = NULL;
							pOnboardBehavior->setState(MoveToStorage);
							pOnboardBehavior->writeLog(time);

							//storage carry on
							ElapsedTime eOperationTime;
							eOperationTime.setPrecisely(100l);
							setState(StorageCarryOn);

							writePassengerLog(time); //write log for carry on
							{
								ARCVector3 ptStorage;
								ptStorage = pStorageInSim->GetRandomPoint();

								//stow operation
								pOnboardBehavior->setState(StorageCarryOn);
								pOnboardBehavior->setDestination(ptStorage);
								pOnboardBehavior->SetCurrentResource(pStorageInSim);
								time += eOperationTime;
								pOnboardBehavior->writeLog(time);

								pStorageInSim->addVisitor(pVis);
								pOnboardBehavior->SetCurrentResource(NULL);

							}
							writePassengerLog(time);
							return;
						}
					}
				}			
			}
		}
	}

	//pick up carrion and move to seat
	//pick up operation should do after carrion move to storage
	std::vector<PaxVisitor*> seatVisitoList;
	for (i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = (PaxVisitor*)pPassenger->m_pVisitorList[i];
		if (pVis == NULL)
			continue;
		PaxEnplaneBehavior* pOnboardBehavior = (PaxEnplaneBehavior*)pVis->getOnboardBehavior();
		if(std::find(vVisitorList.begin(),vVisitorList.end(),pVis) == vVisitorList.end())
		{
			//first write current log
			pOnboardBehavior->writeLog(time);
			pOnboardBehavior->setState(OnboardFreeMoving);
			seatVisitoList.push_back(pVis);
		}
	}

	//check move to seat visitor. not empty need pick up storage operation
	if (!seatVisitoList.empty())
	{
		ElapsedTime eHandsuptime;
		eHandsuptime.setPrecisely(300l);
		setState(PutHandsUp);
		writePassengerLog(time);// write log for carry on
		time += eHandsuptime;
		writeLog(time);
	}

}
//put down all of non passenger to deck ground
void PaxEnplaneBehavior::CarryonMoveToDeckGround(ElapsedTime& time)
{
	//set visitor separate
	Passenger* pPassenger = (Passenger*)m_pPerson;
	int nCount = pPassenger->m_pVisitorList.size();
	
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = (PaxVisitor*)pPassenger->m_pVisitorList[i];
		if (pVis == NULL)
			continue;
		ElapsedTime eMoveTime = time;
		PaxEnplaneBehavior* pOnboardBehavior = (PaxEnplaneBehavior*)pVis->getOnboardBehavior();
		ASSERT(pOnboardBehavior);
		//non passenger move to ground
		{
			//check non passenger type is single and group size is single
			CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)pVis->m_pGroupInfo;
			if (nCount > 1 || pGroupLeaderInfo->GetGroupSize() > 1)
			{
				ARCVector3 seatPoint;
				m_pSeat->GetPosition(seatPoint);
				pOnboardBehavior->setState(MoveToDeckGround);
				ARCVector3 destPt = pOnboardBehavior->getPoint();
				destPt.n[VZ] = seatPoint.n[VZ];
				pOnboardBehavior->setDestination(destPt);
				eMoveTime += ElapsedTime(2l);//give 2 secs to move to deck ground
				pOnboardBehavior->writeLog(eMoveTime);			
			}
		}
	}
	time += ElapsedTime(2L);//make passenger wait for put down all non passenger
}

void PaxEnplaneBehavior::SeparateAtStorageOperation(ElapsedTime& time,PaxVisitor* pVisitor,OnboardStorageInSim* pStorageInSim)
{
	PaxEnplaneBehavior* pOnboardBehavior = (PaxEnplaneBehavior*)pVisitor->getOnboardBehavior();
	ASSERT(pOnboardBehavior);
	ASSERT(pStorageInSim);

 	ARCVector3 ptCenter;
 	pStorageInSim->GetPosition(ptCenter);
// 	ARCVector3 vectEntryDir = ptCenter - getPoint();
// 	ARCVector3 adjustPt;
// 	vectEntryDir.Normalize();
// 	vectEntryDir.SetLength(0.1);
// 	adjustPt = getPoint() + vectEntryDir;
// 	m_ptDestination = adjustPt;

	AdjustDirectionStorage(time,pStorageInSim,false);
	writeLog(time,true);
	//start moving to storage device
	pOnboardBehavior->writeLog(time);//start to move to storage

	//hands up
	ElapsedTime eHandsuptime;
	eHandsuptime.setPrecisely(300l);
	setState(PutHandsUp);
	writePassengerLog(time);// write log for carry on
	{
		//first write current log
		pOnboardBehavior->writeLog(time);
		//move to storage height
		ARCVector3 destPt = getPoint();
		destPt.n[VZ] = ptCenter.n[VZ];
		pOnboardBehavior->setDestination(destPt);
		time += /*pOnboardBehavior->moveTime()*/eHandsuptime;
		pOnboardBehavior->writeLog(time);
	}


	//storage carry on
	ElapsedTime eOperationTime;
	eOperationTime.setPrecisely(100l);
	setState(StorageCarryOn);

	writePassengerLog(time); //write log for carry on
	{
		ARCVector3 ptStorage;
		ptStorage = pStorageInSim->GetRandomPoint();
		pOnboardBehavior->setDestination(ptStorage);
		
		time += eOperationTime;

		pOnboardBehavior->setState(StorageCarryOn);
		pOnboardBehavior->SetCurrentResource(pStorageInSim);
		pOnboardBehavior->writeLog(time);

		pStorageInSim->addVisitor(pVisitor);

		pOnboardBehavior->SetCurrentResource(NULL);
	}

	writePassengerLog(time);
}

