#include "stdafx.h"
#include "PaxDeplaneBehavior.h"
#include "person.h"
#include "Engine\ARCportEngine.h"
#include "Engine\OnboardSimulation.h"
#include "Engine\OnboardFlightInSim.h"
#include "InputOnBoard\Door.h"
#include "OnboardSeatInSim.h"
#include "Common\OnBoardException.h"
#include "Engine\SimEngineConfig.h"
#include "Engine\AirsideMobElementBehavior.h"
#include "engine/GroupLeaderInfo.h"
#include "Airside\AirsideSimulation.h"
#include "inputonboard\Deck.h"
#include "OnboardStorageInSim.h"
#include "Airside\VehicleRequestDispatcher.h"
#include "OnboardDoorInSim.h"
#include "InputOnBoard/Seat.h"
#include "PaxOnboardRoute.h"
#include "PaxOnboardFreeMovingLogic.h"
#include "CorridorGraphMgr.h"
#include "EntryPointInSim.h"
#include "OnboardDeckGroundInSim.h"
#include "OnBoardDeviceInSim.h"
#include "OnboardCellGraph.h"
#include "OnboardCellFreeMoveLogic.h"
#include "OnboardCellFreeMoveEvent.h"
#include "CarryonStoragePrioritiesInSim.h"

PaxDeplaneBehavior::PaxDeplaneBehavior(Person* pPerson)
:PaxOnboardBaseBehavior(pPerson)
,m_bGenerateStandupTime(false)
{
	
}

PaxDeplaneBehavior::~PaxDeplaneBehavior()
{
	
}

int PaxDeplaneBehavior::performanceMove(ElapsedTime p_time,bool bNoLog)
{
	long nAgentID = m_pPerson->getID();

	//----------------------------------------------------------------------------
	// Deplane Agent's state chart.
	 bDisallowGroup = true;
	switch ( getState() )
	{
	case EntryOnboard:
		processEnterOnBoardMode( p_time );
		break;
	case SitOnSeat:
		processSittingOnSeat( p_time);
		break;
	case StandUp:
		processStandUp( p_time );
		break;
	case ArriveAtSeatEntry:
		processArriveAtSeatEntry(p_time);
		break;
	case StrideOverSeat:
		processStrideOverSeat(p_time);
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
	case ArriveAtStorage:
		processArriveAtStorage(p_time);
		break;

	case ArriveAtCheckingPoint:
		processArriveAtCheckingPoint(p_time);		
		break;
	case ArriveAtDoor:
		processArriveAtExitDoor( p_time );
		break;

	case WaitTakeOff:
		{
			AirsideFlightInSim* pFlightInAirside = m_pPerson->getEngine()->GetAirsideSimulation()->GetAirsideFlight(m_pPerson->GetCurFlightIndex()) ;

			if(pFlightInAirside->HasBrrdgeServer(TRUE) || pFlightInAirside->GetServiceRequestDispatcher() == NULL 
				|| !pFlightInAirside->GetServiceRequestDispatcher()->HasVehicleServiceFlight(pFlightInAirside,VehicleType_PaxTruck))
				
			{
			
				bDisallowGroup=false;
				processEntryTerminal(p_time);
			}
			else
				processEntryAirside(p_time);
		}

	default:
		break;
	}

	return 1;
}

void PaxDeplaneBehavior::processEnterOnBoardMode(ElapsedTime p_time)
{
	if(m_pOnboardFlight == NULL)
	{
		//flush log
		setState(WaitTakeOff);
		GenerateEvent(p_time);
		return;
	}

	m_pOnboardFlight->AddCount();
	//assign door
	m_pDoor = m_pOnboardFlight->GetExitDoor();
	if(m_pDoor == NULL)
	{
		FlushLog(p_time);
		throw new OnBoardSimEngineException( new OnBoardDiagnose(m_pPerson->getID(),m_pOnboardFlight->getFlightID(),p_time,_T("cannot find exit door!")));

	}

	if(m_pSeat == NULL)
	{
		FlushLog(p_time);
		throw new OnBoardSimEngineException( new OnBoardDiagnose(m_pPerson->getID(),m_pOnboardFlight->getFlightID(),p_time,_T("cannot find seat!")));
	}

	//set passenger's position to seat
	ARCVector3 pos;
	m_pSeat->GetPosition(pos);
	setLocation(pos);
	setDestination(pos);
	setState(SitOnSeat);
	SetCurrentResource(m_pSeat);
	SetEntryTime(p_time);
	writeLog(p_time, false);


	//assign carryon
	AssignStorageDevice(p_time);
	
}

bool PaxDeplaneBehavior::CanLeadToEntryPoint()
{
	EntryPointInSim* pEntryPoint = m_pSeat->GetEntryPoint();
	if (pEntryPoint == NULL)
		return false;

	//if current seat create entry point, means it was the Left/right last seat
	if(pEntryPoint->GetCreateSeat() != m_pSeat)
	{
		OnboardSeatRowInSim* pSeatRowInSim = m_pSeat->GetSeatRowInSim();
		if (pSeatRowInSim != NULL)//have seat row
		{
			if (!pSeatRowInSim->CanLeadToEntryPoint(m_pSeat,pEntryPoint))
			{
				return false;
			}
		}
	}

	PaxCellLocation& pCellLocation = pEntryPoint->GetEntryLocation();
	if (!pCellLocation.CanReserve(this))
		return false;

	return true;
}

void PaxDeplaneBehavior::processSittingOnSeat(ElapsedTime p_time)
{
	//write log
	if (!m_bGenerateStandupTime)//did not generate stand up time
	{
		writeLog(p_time,false);

		ElapsedTime _waitingTime = random(60L)+60L;// constant.
		p_time += _waitingTime;
		m_eStandupPlaneTime = p_time;
		m_bGenerateStandupTime = true;
	}

	ARCVector3 seatPos;
	m_pSeat->GetPosition(seatPos);
	//can lead to entry point
	if (!CanLeadToEntryPoint())
		return;
	
	//set door
	
	if(m_pDoor)
	{
		setDestination(seatPos);
		setState(StandUp);
		GenerateEvent(p_time);
	}
	else
	{	
		FlushLog(p_time);
		throw new OnBoardSimEngineException( new OnBoardDiagnose(m_pPerson->getID(),m_pOnboardFlight->getFlightID(),p_time,_T("cannot find exit door!")));
	}
}

void PaxDeplaneBehavior::processStandUp(ElapsedTime p_time)
{
	SetCurrentResource(m_pSeat);
	writeLog(p_time);//calculate pick up carrion time
	//pick up his carryon from seat
	PickUpCarryOnFromSeat(p_time);
	
	ARCVector3 seatEntryPoint;
	m_pSeat->GetFrontPosition(seatEntryPoint);
	setDestination(seatEntryPoint);

	
	setState(ArriveAtSeatEntry);
	GenerateEvent(p_time + moveTime());
}

//PaxOnboardBaseBehavior::MobDir PaxDeplaneBehavior::GetDirectionSidle(CPoint2008& dir)const
//{
//	OnboardSeatInSim* pCreateSeatInSim =m_pSeat->GetEntryPoint()->GetCreateSeat();
//
//	OnboardCellInSim* pLastCellInSim = m_pSeat->GetEntryPoint()->GetOnboardCell();
//	OnboardCellInSim* pFirstCellInSim= pCreateSeatInSim->GetOnboardCellInSim();
//
//	CPoint2008 ptFirst = pFirstCellInSim->getLocation();
//	CPoint2008 ptLast = pLastCellInSim->getLocation();
//	CPoint2008 dirPt = ptLast-ptFirst;
//	dir = dirPt;
//	dir.Normalize();
//
//	ARCVector2 walkdir(dirPt);
//	double dWalkAngle = walkdir.AngleFromCoorndinateX();
//
//	//get the seat direction
//	CPoint2008 ptSeatLocation;
//	pCreateSeatInSim->GetLocation(ptSeatLocation);
//	//seat front position
//	ARCVector3 seatFrontVec;
//	pCreateSeatInSim->GetFrontPosition(seatFrontVec);
//	CPoint2008 ptSeatFrontPosition(seatFrontVec[VX],seatFrontVec[VY],seatFrontVec[VZ]);
//	CPoint2008 seatDirection = ptSeatFrontPosition - ptSeatLocation;
//	ARCVector2 seatDir(seatDirection);
//	double dSeatAngle = seatDir.AngleFromCoorndinateX();
//
//	double dRotate = dWalkAngle - dSeatAngle;
//	double dRadians = ARCMath::DegreesToRadians(dRotate);
//	if (sin(dRadians) > 0)
//	{
//		return PaxOnboardBaseBehavior::RIGHTSIDLE;
//	}
//
//	return PaxOnboardBaseBehavior::LEFTSIDLE;
//}

void PaxDeplaneBehavior::processArriveAtSeatEntry(ElapsedTime p_time)
{
	writeLog(p_time);

	SetCurrentResource(NULL);//move out of the seat

	//check the entry location is valid or not
	if (m_pSeat && m_pSeat->GetEntryPoint())
	{
		OnboardCellPath freeMovePath;
		OnboardSeatInSim* pCreateSeatInSim =m_pSeat->GetEntryPoint()->GetCreateSeat();
		ASSERT(pCreateSeatInSim);
		CPoint2008 dir;
		PaxOnboardBaseBehavior::MobDir emWalk = GetDirectionSidle(dir, false);
		ARCVector3 destPt = getDest();
		double disOffset = 1.0;//adjust direction when passenger sit seat is the create entry point seat
		destPt += dir*disOffset;
		setDestination(destPt);
		p_time += sidleMoveTime(emWalk);
		writeSidleLog(p_time,emWalk);

		if (m_pSeat != pCreateSeatInSim)
		{
			freeMovePath.addCell(pCreateSeatInSim->GetOnboardCellInSim());
			WriteSidleCellPathLog(freeMovePath,p_time,emWalk);
		}
		setState(StrideOverSeat);
		GenerateEvent(p_time);
	}
	else
	{
		FlushLog(p_time);
		throw new OnBoardSimEngineException( new OnBoardDiagnose(m_pPerson->getID(),m_pOnboardFlight->getFlightID(),p_time,_T("cannot find Seat!")));
	}
}

void PaxDeplaneBehavior::processStrideOverSeat(ElapsedTime p_time )
{
	OnboardSeatInSim* pCreateSeatInSim =m_pSeat->GetEntryPoint()->GetCreateSeat();
	ASSERT(pCreateSeatInSim);

	ASSERT(m_pSeat->GetEntryPoint());
	PaxCellLocation& entryLocation = m_pSeat->GetEntryPoint()->GetEntryLocation();
	if (entryLocation.CanReserve(this))
	{
		entryLocation.PaxReserve(this);

		if (entryLocation.IsAvailable(this))
		{
			CPoint2008 dir;
			PaxOnboardBaseBehavior::MobDir emWalk = GetDirectionSidle(dir, false);
			writeSidleLog(p_time,emWalk);

			PaxCellLocation& entryLocation = m_pSeat->GetEntryPoint()->GetEntryLocation();
			entryLocation.PaxOccupy(this);

			SetPaxLocation(entryLocation);

			OnboardCellPath freeMovePath;
			freeMovePath.addCell(pCreateSeatInSim->GetEntryPoint()->GetOnboardCell());

			WriteSidleCellPathLog(freeMovePath,p_time,emWalk);
			setState(ArriveAtCheckingPoint);
			GenerateEvent(p_time);
			return;
		}
	}

	setState(StrideOverSeat);
	GenerateEvent(p_time + ElapsedTime(2l));
}

void PaxDeplaneBehavior::processArriveAtCheckingPoint( ElapsedTime p_time )
{
	//write log
	writeLog(p_time);

	setState(ArriveAtStorage);
	GenerateEvent(p_time);
}

OnboardStorageInSim* PaxDeplaneBehavior::GetStorage()
{
	return NULL;
}

//change storage's location to pax's destination
ARCVector3 PaxDeplaneBehavior::GetStorageLocation(OnboardStorageInSim *pStorage) const 
{
	if(m_pSeat && m_pSeat->GetEntryPoint())
	{
		ARCVector3 ptEntryPoint;
		m_pSeat->GetEntryPoint()->GetLocation(ptEntryPoint);
		return ptEntryPoint;
	}

	return location;
}

void PaxDeplaneBehavior::processArriveAtStorage(ElapsedTime p_time)
{
	writeLog(p_time,false);
	PickUpCarryonsFromDevice(p_time);
	//end carrion
	writeLog(p_time,false);
	setState(OnboardFreeMoving);
	writeLog(p_time,false);

	OnboardCellInSim* pOriginalCell = m_pSeat->GetEntryPoint()->GetOnboardCell();
	OnboardCellInSim* pDestCell = m_pDoor->GetEntryCell();

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
			OnboardCellInSim* pFromCellInSim = m_pDeviceInSim->GetOnboardCell(pOriginalCell->GetDeckGround());

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
		if(m_pOnboardFlight->getCellGraph())
			m_pOnboardFlight->getCellGraph()->FindPath(m_pSeat,m_pDoor,routePath);
		//set end state
		m_pCellFreeMoveLogic->SetEndPaxState(ArriveAtDoor);
	}
	//adjust direction for pax move to door
	if (routePath.getCellCount() > 1)
	{
		OnboardCellInSim* firstCell = routePath.getCell(0);
		ASSERT(firstCell);
		OnboardCellInSim* nextCell = routePath.GetNextCell(firstCell);
		ASSERT(nextCell);

		CPoint2008 fistPos = firstCell->getLocation();
		CPoint2008 nextPos = nextCell->getLocation();
		CPoint2008 dir = CPoint2008(nextPos - fistPos);
		dir.Normalize();

		//offset point that pax move to adjust direction
		CPoint2008 adjustPos;
		adjustPos = fistPos + dir;//move a little distance that can not affect step event
		setDestination(adjustPos);
		writeLog(p_time,false);
	}
	//start moving
	m_pCellFreeMoveLogic->setCellPath(routePath);//path

	m_pSeat->PersonLeave();
	
	OnboardSeatRowInSim* pSeatRowInSim = m_pSeat->GetSeatRowInSim();
	if (pSeatRowInSim)
	{
		pSeatRowInSim->wakeupPax(p_time,m_pSeat->GetEntryPoint());
	}

	m_pCellFreeMoveLogic->setCurLocation(m_pSeat->GetEntryPoint()->GetEntryLocation());//start position


	
	//deliver to Free Moving Logic, the passenger would come back at state ArriveAtCheckingPoint
	OnboardCellFreeMoveEvent *pMoveEvent = new OnboardCellFreeMoveEvent(m_pCellFreeMoveLogic);
	pMoveEvent->setTime(p_time);
	pMoveEvent->addEvent();

}

void PaxDeplaneBehavior::processArrivalAtDeviceEntry(ElapsedTime p_time)
{
	ASSERT(m_pDeviceInSim);
	OnboardCellInSim* pOriginalCell = m_pSeat->GetEntryPoint()->GetOnboardCell();
	OnboardCellPath freeMovePath;
	freeMovePath.addCell(m_pDeviceInSim->GetEntryCell(pOriginalCell->GetDeckGround()));
	freeMovePath.addCell(m_pDeviceInSim->GetOnboardCell(pOriginalCell->GetDeckGround()));
	WriteCellPathLog(freeMovePath,p_time);

	setState(ServiceOnDevice);
	GenerateEvent(p_time);
}

void PaxDeplaneBehavior::processMovesOnDevice(ElapsedTime p_time)
{
	ASSERT(m_pDeviceInSim);
	SetCurrentResource(m_pDeviceInSim);

	OnboardCellInSim* pOriginalCell = m_pSeat->GetEntryPoint()->GetOnboardCell();
	OnboardCellInSim* pDestCell = m_pDoor->GetEntryCell();
	OnboardCellPath freeMovePath;
	freeMovePath.addCell(m_pDeviceInSim->GetOnboardCell(pOriginalCell->GetDeckGround()));
	freeMovePath.addCell(m_pDeviceInSim->GetOnboardCell(pDestCell->GetDeckGround()));
	WriteCellPathLog(freeMovePath,p_time);

	setState(LeavesDevice);
	GenerateEvent(p_time);
}

void PaxDeplaneBehavior::processLeavesDevice(ElapsedTime p_time)
{
	SetCurrentResource(NULL);

	ASSERT(m_pDeviceInSim);
	OnboardCellInSim* pDestCell = m_pDoor->GetEntryCell();
	OnboardCellPath freeMovePath;
	freeMovePath.addCell(m_pDeviceInSim->GetOnboardCell(pDestCell->GetDeckGround()));
	freeMovePath.addCell(m_pDeviceInSim->GetEntryCell(pDestCell->GetDeckGround()));
	WriteCellPathLog(freeMovePath,p_time);

	setState(MoveToDest);
	GenerateEvent(p_time);
}

void PaxDeplaneBehavior::ProcessMoveFromDevice(ElapsedTime p_time)
{
	SetCurrentResource(NULL);
	ASSERT(m_pDeviceInSim);
	OnboardCellInSim* pDestCell = m_pDoor->GetEntryCell();
	OnboardCellInSim* pDeviceEntryCell = m_pDeviceInSim->GetEntryCell(pDestCell->GetDeckGround());

	OnboardCellPath routePath;
	if(m_pOnboardFlight->getCellGraph())
	{
		m_pOnboardFlight->getCellGraph()->FindPath(pDeviceEntryCell,pDestCell,routePath);
	}

	//try to move to the seat entry
	PaxCellLocation entryLocation = m_pDoor->getEntryLocation();
	entryLocation.PaxOccupy(this);//take the place

	//start moving
	m_pCellFreeMoveLogic->setCellPath(routePath);//path
	m_pCellFreeMoveLogic->setCurLocation(entryLocation);//start position
	m_pCellFreeMoveLogic->SetEndPaxState(ArriveAtDoor);


	//deliver to Free Moving Logic, the passenger would come back at state ArriveAtCheckingPoint
	OnboardCellFreeMoveEvent *pMoveEvent = new OnboardCellFreeMoveEvent(m_pCellFreeMoveLogic);
	pMoveEvent->setTime(p_time);
	pMoveEvent->addEvent();
}

void PaxDeplaneBehavior::processArriveAtExitDoor(ElapsedTime p_time)
{
	SetCurrentResource(m_pDoor);
	writeLog(p_time,false);

	//ElapsedTime _waitingTime = 10L;// constant.
	setState(WaitTakeOff);
	GenerateEvent (p_time);	
}

void PaxDeplaneBehavior::GenerateEvent( ElapsedTime time )
{
	GenerateEvent(m_pPerson, time);	
}
void PaxDeplaneBehavior::GenerateEvent(Person *pPerson, ElapsedTime time )
{
	enum EVENT_OPERATION_DECISTION enumRes =pPerson->m_pGroupInfo->ProcessEvent( time, bDisallowGroup );
	if( enumRes == REMOVE_FROM_PROCESSOR_AND_NO_EVENT )
		return;

	if( enumRes == NOT_GENERATE_EVENT )
		return;

	pPerson->MobileElement::generateEvent( time, false);	

}

void PaxDeplaneBehavior::FlushLog( ElapsedTime p_time )
{
	if(m_pPerson)
		m_pPerson->flushLog(p_time,false);
}

void PaxDeplaneBehavior::setState( short newState )
{
	if(m_pPerson)
		m_pPerson->setState(newState);
}

int PaxDeplaneBehavior::getState( void ) const
{
	return m_pPerson->getState();
}

void PaxDeplaneBehavior::processEntryAirside( ElapsedTime p_time )
{
	//if (m_pPerson->m_pGroupInfo && m_pPerson->m_pGroupInfo->IsFollower())
	//	return;
	m_curLocation.PaxClear(this,p_time);
	if( !simEngineConfig()->isSimAirsideMode() )
	{
		FlushLog(p_time);
		return;
	}

	// arriving passenger.
	if(false == m_pPerson->getLogEntry().isArriving())
	{
		FlushLog( p_time );
		return;
	}

	// not departing and turn around, impossible, just flush log.
	if(m_pPerson->getLogEntry().isDeparting()&& !m_pPerson->getLogEntry().isTurnaround())
	{
		FlushLog( p_time );
		return;
	}

	//By now, AirsideMobElementBehavior can just handle passenger's movement logic.
	if(m_pPerson->getLogEntry().GetMobileType() == 0)
	{
		EntityEvents state = TAKE_OFF_FLIGHT ;
		m_pOnboardFlight->DecCount();
		m_pPerson->setBehavior( new AirsideMobElementBehavior( m_pPerson,state));
		m_pPerson->generateEvent( p_time, false);
		return;//to Airside mode.

	}
	else
	{
		FlushLog(p_time);
		return;
	}

	ASSERT(FALSE);
	return;	
}

void PaxDeplaneBehavior::processEntryTerminal( ElapsedTime p_time )
{
	m_curLocation.PaxClear(this,p_time);

	if( !simEngineConfig()->isSimTerminalMode() )
	{
		FlushLog(p_time);
		return;
	}

	// arriving passenger.
	if(false == m_pPerson->getLogEntry().isArriving())
	{
		FlushLog( p_time );
		return;
	}

	// not departing and turn around, impossible, just flush log.
	if(m_pPerson->getLogEntry().isDeparting()&& !m_pPerson->getLogEntry().isTurnaround())
	{
		FlushLog( p_time );
		return;
	}

	//By now, AirsideMobElementBehavior can just handle passenger's movement logic.
	if(m_pPerson->getLogEntry().GetMobileType() == 0)
	{
		m_pPerson->setState(Birth);
		m_pPerson->setBehavior( new TerminalMobElementBehavior( m_pPerson));
		writeLog(p_time);
		GenerateEvent( p_time);
		return;//to Airside mode.

	}
	else
	{
		FlushLog(p_time);
		return;
	}

	ASSERT(FALSE);
	return;	
}

bool PaxDeplaneBehavior::AssignSeatForFollower(ElapsedTime p_time)
{
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	if(pGroupLeaderInfo && pGroupLeaderInfo->IsFollower())
		return true;

	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i< nFollowerCount; i++ )
	{
		Person* _pFollower = (Person*) elemList.getItem( i );
		if(_pFollower == NULL)
			continue;  

		MobElementBehavior* spBehavior = _pFollower->getCurrentBehavior();
		if (spBehavior == NULL || (spBehavior && spBehavior->getBehaviorType()!= OnboardBehavior))
		{
			CString strMessage;
			OnboardSeatInSim* pSeat = m_pOnboardFlight->GetSeat(m_pPerson,strMessage);
			if (pSeat == NULL)
				return false;

			PaxDeplaneBehavior* pBehavior = new PaxDeplaneBehavior(_pFollower);
			pBehavior->m_pOnboardFlight = m_pOnboardFlight;
			pBehavior->m_pDoor = m_pDoor;
			pBehavior->m_pSeat = pSeat;

			_pFollower->setBehavior(pBehavior);
			pSeat->AssignToPerson(_pFollower);

			ARCVector3 pos ;//= pSeat->GetFrontPosition();
			pBehavior->setDestination(pos);
			pBehavior->setLocation(pos);
			pBehavior->setState(SitOnSeat);			
		}

	}
	return true;

}

void PaxDeplaneBehavior::setFlight( OnboardFlightInSim* pFlight )
{
	m_pOnboardFlight = pFlight;
}

void PaxDeplaneBehavior::setSeat( OnboardSeatInSim* pSeat )
{
	m_pSeat = pSeat;
}

PaxOnboardBaseBehavior* PaxDeplaneBehavior::GetOnboardBehavior(Person* pPerson)
{
	PaxOnboardBaseBehavior* pOnboardBehavior = new PaxDeplaneBehavior(pPerson); 
	return pOnboardBehavior;
}

bool PaxDeplaneBehavior::AssignStorageDevice(const ElapsedTime& eTime)
{
	//get all baggage
	//visitor separate

	//get the storage closest to the seat
	ARCVector3 vecSeat;
	//the seat create the row  
	OnboardSeatInSim *pSeatCreateRow = NULL;
	if(m_pSeat->GetEntryPoint())
		pSeatCreateRow = m_pSeat->GetEntryPoint()->GetCreateSeat();
	if(pSeatCreateRow == NULL)
	{
		//take all carryon to seat
		return false;
	}
	pSeatCreateRow->GetFrontPosition(vecSeat);
	std::vector<OnboardStorageInSim*> vStorageInSim = GetCanReachStorageDevice(m_pSeat->GetGround(),vecSeat);

	Passenger* pPassenger = (Passenger*)m_pPerson;
	int nCount = pPassenger->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = (PaxVisitor*)pPassenger->m_pVisitorList[i];
		if (pVis == NULL)
			continue;
		
		 int nPriority = m_pOnboardFlight->GetCarryonPriority()->GetDevicePriority(*(pVis->getType().GetPaxType()),
																						pVis->getType().GetTypeIndex());

		 if(nPriority == STORAGEDEVICE&& vStorageInSim.size() > 0)
		 {
			int nDeviceCount =  vStorageInSim.size();
			bool bAssign = false;
			for (int nDevice = 0; nDevice < nDeviceCount; ++ nDevice )
			{
				bAssign = AssignCarryOnIntoDevice(eTime,pVis,vStorageInSim[nDevice]);
				if(bAssign)
				{
					pPassenger->m_pVisitorList[i] = NULL;
					break;
				}
			}

			if(!bAssign)
			{
				pPassenger->m_pVisitorList[i] = NULL;
				AssignCarryOnIntoSeat(eTime,pVis);
			}
		 }
		 else
		 {
			pPassenger->m_pVisitorList[i] = NULL;
			AssignCarryOnIntoSeat(eTime,pVis);
		 }
	}

	return false;

}


bool PaxDeplaneBehavior::AssignCarryOnIntoDevice(const ElapsedTime& eTime, PaxVisitor* pCarryon, OnboardStorageInSim* pStorageInSim)
{
	//current time
	ARCVector3 vecStoragePos = pStorageInSim->GetRandomPoint();
	//pStorageInSim->GetPosition(vecStoragePos);
	
	ASSERT(pCarryon);
	if(pCarryon == NULL)
		return true;

	if (pCarryon->m_pGroupInfo->IsFollower())
		return true;

	if(pCarryon->getOnboardBehavior()->GetVolume(m_pOnboardFlight) * pCarryon->GetActiveGroupSize() > pStorageInSim->GetAvailableSpace())
		return false;//full


	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)pCarryon->m_pGroupInfo;
	//check in group
	pGroupLeaderInfo->SetInGroup(false);

	PaxDeplaneBehavior* pCarryonBehavior = (PaxDeplaneBehavior*)pCarryon->getOnboardBehavior();
	pCarryonBehavior->setState(WaitAtTempPlace);
	pCarryonBehavior->setDestination(vecStoragePos);
	pCarryonBehavior->setLocation(vecStoragePos);
	pCarryonBehavior->writeLog(eTime);
	m_mapCarryonStorage[pCarryon] = pStorageInSim;
	//pStorageInSim->addVisitor(pCarryon);

	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i< nFollowerCount; i++ )
	{
		PaxVisitor* _pFollower = (PaxVisitor*) elemList.getItem( i );
		if(_pFollower == NULL)
			continue;

		pCarryonBehavior = (PaxDeplaneBehavior*)_pFollower->getOnboardBehavior();
		if(pCarryonBehavior == NULL)
			continue;

		pCarryonBehavior->setState(WaitAtTempPlace);
		pCarryonBehavior->setDestination(vecStoragePos);
		pCarryonBehavior->setLocation(vecStoragePos);
		pCarryonBehavior->writeLog(eTime);
	//	pStorageInSim->addVisitor(pCarryon);
		m_mapCarryonStorage[_pFollower] = pStorageInSim;
	}
	return true;

}

void PaxDeplaneBehavior::PickUpCarryonsFromDevice(ElapsedTime& eTime)

{
	//put down the carry on in his hand
	PutDownCarryon(eTime);

	//storage device
	std::map<PaxVisitor *, OnboardStorageInSim *>::iterator iter = m_mapCarryonStorage.begin();

	std::vector<PaxVisitor *> vCarryon;
	for (; iter != m_mapCarryonStorage.end(); ++iter)
	{
		PickUpCarryOnFromStorage(eTime,(*iter).first, (*iter).second );
		vCarryon.push_back((*iter).first);
	}
	setState(PutHandsDown);
	writeLog(eTime,true);//

	RegroupAndAttachCarryOn(eTime,m_vCarryonAtSeat);
	RegroupAndAttachCarryOn(eTime,vCarryon);

}

void PaxDeplaneBehavior::PickUpCarryOnFromStorage( ElapsedTime& p_time, PaxVisitor* pCarryon, OnboardStorageInSim *pStorage )
{
	SetCurrentResource(pStorage);

	//get the destination of passenger
	
	AdjustDirectionStorage(p_time,pStorage,true);
	writeLog(p_time,true);

	//passenger's face direction
	ARCVector3 vEntryPoint;
	m_pSeat->GetEntryPoint()->GetLocation(vEntryPoint);
	ARCVector3 vSeatFront;
	m_pSeat->GetEntryPoint()->GetCreateSeat()->GetFrontPosition(vSeatFront);

	ARCVector3 vDirFace = vSeatFront - vEntryPoint;
	
	//get a random location
	ARCVector3 vPaxPos = getPoint();
	vDirFace.SetLength(PAXWIDTH/2);
	
	ARCVector3 vVistorPos = vPaxPos + vDirFace;


	//start carryon operation, set the flag
	PaxDeplaneBehavior* pCarryonBehavior = (PaxDeplaneBehavior*)pCarryon->getOnboardBehavior();
	pCarryonBehavior->setState(MoveToStorage);//start 
	pCarryonBehavior->writeLog(p_time);


    //passenger's operation
	//hands up
	ElapsedTime eHandsuptime = ElapsedTime(2L);
	setState(PutHandsDown);
	writeLog(p_time,true);// wirte log for carryon
	p_time = p_time + eHandsuptime;

	//get the storage device 
	{
		pCarryonBehavior->writeLog(p_time);
		//get the carry on from storage device
		ARCVector3 vStoragePos;
		pStorage->GetPosition(vStoragePos);
		double dZpos =  vStoragePos[VZ];

		ARCVector3 vVisitorStrPoas = ARCVector3(vVistorPos[VX], vVistorPos[VY], dZpos);
		pCarryonBehavior->setDestination(vVisitorStrPoas);
		p_time += ElapsedTime(1L);//pCarryonBehavior->moveTime();
		pCarryonBehavior->writeLog(p_time);


		if(m_vCarryonAtSeat.size() == 0 && m_mapCarryonStorage.size() == 1)// the passenger has only one carryon
		{
			//no action any more

			ElapsedTime eOperationTime;//put the baggage down time
			eOperationTime.setPrecisely(50);
			p_time += eOperationTime;	
			
			//
			pCarryonBehavior->setState(StorageCarryOn);//end
			pCarryonBehavior->SetCurrentResource(pStorage);
			pCarryonBehavior->writeLog(p_time,true); //write log for carry on
		}
		else
		{
			//write the passenger's log
			setState(PutHandsUp);
			writeLog(p_time,true); //write log for carry on

			//move to ground
			double dGround = m_pSeat->GetGround()->getHeight();
			vVisitorStrPoas = ARCVector3(vVistorPos[VX], vVistorPos[VY], dGround);
			pCarryonBehavior->setDestination(vVisitorStrPoas);
			ElapsedTime eOperationTime;
			eOperationTime.setPrecisely(50);
			p_time += eOperationTime;//pCarryonBehavior->moveTime();
			pCarryonBehavior->SetCurrentResource(pStorage);
			pCarryonBehavior->setState(StorageCarryOn);
			pCarryonBehavior->writeLog(p_time);


		}
		pCarryonBehavior->setState(WaitAtTempPlace);
		pCarryonBehavior->SetCurrentResource(NULL);
	}

	//setState(WaitAtTempPlace);
	writeLog(p_time,false);

	//end the operation
	SetCurrentResource(NULL);

}

void PaxDeplaneBehavior::RegroupAndAttachCarryOn(const ElapsedTime& eTime, std::vector<PaxVisitor *> vCarryon )
{
	//attach the leader
	std::vector<PaxVisitor *> vLeader;

	int nCarryCount = static_cast<int>(vCarryon.size());
	for (int nCarry = 0; nCarry < nCarryCount; ++ nCarry)
	{
	 	PaxVisitor* pCarryon = vCarryon[nCarry];
		if(!pCarryon->m_pGroupInfo->IsFollower())
		{
			CGroupLeaderInfo *pLeaderInfo = (CGroupLeaderInfo *)pCarryon->m_pGroupInfo;
			if(pLeaderInfo == NULL)
				continue;

			pLeaderInfo->SetInGroup(true);

			//attach the carry on to passenger
			((Passenger *)m_pPerson)->AttachCarryon(pCarryon);
			PaxDeplaneBehavior* pCarryonBehavior = (PaxDeplaneBehavior*)pCarryon->getOnboardBehavior();
			//if(pCarryonBehavior)
				//pCarryonBehavior->writeLog(eTime);

			//vLeader.push_back(pCarryon);
		}
	}

	//write a log to all the carryon
	Passenger* pPassenger = (Passenger*)m_pPerson;
	int nCount = pPassenger->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = (PaxVisitor*)pPassenger->m_pVisitorList[i];
		if (pVis == NULL)
			continue;
		PaxDeplaneBehavior* pCarryonBehavior = (PaxDeplaneBehavior*)pVis->getOnboardBehavior();
		ASSERT(pCarryonBehavior);
		//non passenger move to ground
		if(pCarryonBehavior)
			pCarryonBehavior->writeLog(eTime);		
	}


	////group the follower
	//int nLeaderCount = static_cast<int>(vLeader.size());
	//for (int nLeader = 0 ; nLeader < nLeaderCount; ++nLeader)
	//{
	//	
	// 	PaxVisitor* pCarryon = vCarryon[nLeader];

	//	//check leader
	//	if(pCarryon->m_pGroupInfo->IsFollower())
	//		continue;
	//	
	//	//leader info
	//	CGroupLeaderInfo *pLeaderInfo = (CGroupLeaderInfo *)pCarryon->m_pGroupInfo;
	//	if(pLeaderInfo == NULL)
	//		continue;

	//	//get all follower
	//	MobileElementList& pGroupFollowerList = pLeaderInfo->GetFollowerList();

	//	int nFollowerCount = pGroupFollowerList.getCount();
	//	for( int i=0; i<nFollowerCount; i++ )
	//	{
	//		Person* _pFollower = (Person*) pGroupFollowerList.getItem( i );


	//	}
	//}
}

void PaxDeplaneBehavior::AssignCarryonToSeat( const ElapsedTime& eTime )
{
	//////seat position
	////Passenger* pPassenger = (Passenger*)m_pPerson;
	////int nCount = pPassenger->m_pVisitorList.size();
	////for( int i=nCount-1; i>=0; i-- )
	////{
	////	PaxVisitor* pVis = (PaxVisitor*)pPassenger->m_pVisitorList[i];
	////	if (pVis == NULL)
	////		continue;

	////	pPassenger->m_pVisitorList[i] = NULL;
	////	AssignCarryOnIntoSeat(eTime,pVis);
	////}


}

void PaxDeplaneBehavior::AssignCarryOnIntoSeat( const ElapsedTime& eTime, PaxVisitor* pCarryon )
{
	//current time
	ARCVector3 vecSeatPos = m_pSeat->GetRandomPoint();


	ASSERT(pCarryon);
	if(pCarryon == NULL)
		return;

	if (pCarryon->m_pGroupInfo->IsFollower())
		return;

	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)pCarryon->m_pGroupInfo;
	//check in group
	pGroupLeaderInfo->SetInGroup(false);

	PaxDeplaneBehavior* pCarryonBehavior = (PaxDeplaneBehavior*)pCarryon->getOnboardBehavior();
	pCarryonBehavior->setState(WaitAtTempPlace);
	pCarryonBehavior->setDestination(vecSeatPos);
	pCarryonBehavior->setLocation(vecSeatPos);	
	pCarryonBehavior->writeLog(eTime);
	m_vCarryonAtSeat.push_back(pCarryon);
	//m_pSeat->addVisitor(pCarryon);

	//all its member
	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i< nFollowerCount; i++ )
	{
		PaxVisitor* _pFollower = (PaxVisitor*) elemList.getItem( i );
		if(_pFollower == NULL)
			continue;

		PaxDeplaneBehavior* pOnboardBehavior = (PaxDeplaneBehavior*)_pFollower->getOnboardBehavior();
		pOnboardBehavior->setState(WaitAtTempPlace);
		pOnboardBehavior->setDestination(vecSeatPos);
		pOnboardBehavior->setLocation(vecSeatPos);
		pOnboardBehavior->writeLog(eTime);
		//m_pSeat->addVisitor(pCarryon);
		m_vCarryonAtSeat.push_back(_pFollower);
	}

}

void PaxDeplaneBehavior::PickUpCarryOnFromSeat( ElapsedTime& eTime )
{
	//storage device
	std::vector<PaxVisitor *>::iterator iter = m_vCarryonAtSeat.begin();

	for (; iter != m_vCarryonAtSeat.end(); ++iter)
	{
		PickUpCarryOnFromSeat(eTime,*iter );
	}
	writeLog(eTime);

	eTime += ElapsedTime(1L);//regroup time
	RegroupAndAttachCarryOn(eTime, m_vCarryonAtSeat);

}
//pick up carry on from seat
void PaxDeplaneBehavior::PickUpCarryOnFromSeat( ElapsedTime& eTime, PaxVisitor* pCarryon )
{	
	PaxDeplaneBehavior* pCarryonBehavior = (PaxDeplaneBehavior*)pCarryon->getOnboardBehavior();
	
	//write log to end the waiting state
	pCarryonBehavior->setState(MoveToStorage);
	pCarryonBehavior->writeLog(eTime);

	//move to seat front position
	ARCVector3 seatPitch;
	m_pSeat->GetFrontPosition(seatPitch);
	

	pCarryonBehavior->setDestination(seatPitch);
	eTime += pCarryonBehavior->moveTime();

	pCarryonBehavior->setState(StorageCarryOn);
	pCarryonBehavior->SetCurrentResource(m_pSeat);
	pCarryonBehavior->writeLog(eTime);

	pCarryonBehavior->setState(WaitAtTempPlace);
	pCarryonBehavior->SetCurrentResource(NULL);
    
}


void PaxDeplaneBehavior::PutDownCarryon( ElapsedTime& eTime )
{
	m_vCarryonAtSeat.clear();//clear the items, for put new element, because it will group later again
	Passenger* pPassenger = (Passenger*)m_pPerson;
	int nCount = pPassenger->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = (PaxVisitor*)pPassenger->m_pVisitorList[i];
		if (pVis == NULL)
			continue;
		PaxDeplaneBehavior* pCarryonBehavior = (PaxDeplaneBehavior*)pVis->getOnboardBehavior();
		ASSERT(pCarryonBehavior);
		//non passenger move to ground
		{
			ARCVector3 seatPoint;
			m_pSeat->GetPosition(seatPoint);
			pCarryonBehavior->setState(MoveToDeckGround);
			ARCVector3 destPt = getPoint();
			destPt.n[VZ] = seatPoint.n[VZ];
			pCarryonBehavior->setDestination(destPt);
			eTime += pCarryonBehavior->moveTime();
			pCarryonBehavior->writeLog(eTime);
		}

		//detach to passenger
		pPassenger->m_pVisitorList[i] = NULL;
		m_vCarryonAtSeat.push_back(pVis);
	}
}







