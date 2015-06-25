#include "StdAfx.h"
#include "AirsideMobElementBehavior.h"
#include "Airside/AirsideSimulation.h"
#include "Airside/AirsidePaxBusInSim.h"
#include "GroupLeaderInfo.h"

#include "TerminalMobElementBehavior.h"
#include "./Inputs/NonPaxRelativePosSpec.h"
#include "MobElementsDiagnosis.h"
#include "Common/ARCTracker.h"
#include "Engine/ARCportEngine.h"
#include "Main/TermPlanDoc.h"
#include "Main/Floor2.h"

#include "PaxEnplaneBehavior.h"
#include "SimEngineConfig.h"
#include "OnboardSimulation.h"
#include "OnboardFlightInSim.h"
#include "OnboardDoorInSim.h"
#include "Airside/FlightOpenDoors.h"


AirsidePassengerBehavior::AirsidePassengerBehavior(Person* pax)
:AirsideMobElementBaseBehavior(pax)
,m_pax(pax)
,m_IsOnBus(FALSE)
,m_ZPosition(1)
,m_bPaxBusSerivce(true)
,m_pPaxBus(NULL)
{
	if(m_pax->getLogEntry().isDeparting())
	{
		setState(WALK_OUT_GATE) ;
		m_bArrivalPax = false;
	}
	else
	{
	    setState(TAKE_OFF_FLIGHT) ;
		m_bArrivalPax = true;
	}
}
AirsidePassengerBehavior::AirsidePassengerBehavior(Person* pax ,int curentstate)
:AirsideMobElementBaseBehavior(pax)
,m_pax(pax)
,m_IsOnBus(FALSE)
,m_ZPosition(1)
,m_bPaxBusSerivce(true)
,m_pPaxBus(NULL)
{
	setState(curentstate) ;
	if (curentstate == WALK_OUT_GATE)
	{
		m_bArrivalPax = false;
	}
	else
	{
		m_bArrivalPax = true;
	}
}

AirsidePassengerBehavior::~AirsidePassengerBehavior()
{

}

int AirsidePassengerBehavior::performanceMove(ElapsedTime p_time,bool bNoLog)
{
	PLACE_METHOD_TRACK_STRING();
	switch(getState())
	{
	case TAKE_ON_FLIGHT :
		ProcessWhenTakeOnFlight( p_time ) ;
		break;
	case ARRIVE_AT_GATE:
		ProcessWhenArriveAtGate(p_time) ;
		break;
	case TAKE_OFF_FLIGHT:
		ProcessWhenTakeOffFlight(p_time) ;
		break;
	case WAIT_FOR_BUS:
		ProcessWhenWaitforBus(p_time) ;
		break;
	case HOLDAREA_FOR_BUS:
		ProcessHoldAreaToBus(p_time);
		break;
	case TAKE_ON_BUS:
		ProcessWhenTakeOnBus(p_time);
		break;
	case TAKE_OFF_BUS_TOGATE:
		ProcessWhenTakeOffBusToGate(p_time) ;
		break;
	case WAITE_FOR_GATEOPEN:
		ProcessWhenWaitForGateOpen(p_time) ;
		break;
	case WALK_OUT_GATE:
		ProcessWhenWalkOutGate(p_time) ;
		break;
	case TAKE_OFF_BUS_TOFLIGHT: 
		ProcessWhenTakeOffBusToFlight(p_time) ;
		break;
	case WAIT_FOR_FLIGHT: 
		ProcessWhenWaitForFlight(p_time) ;
		break;
	case _DEATH: 
		ProcessWhenPassengerDeath(p_time) ;
		break ;
	case TAKE_OFF_FLGHT_WITH_NOBUS:
        ProcessWhenTakeOffFlightWithNoBus(p_time) ;
		break ;
	case EntryOnboard:
		{
			ProcessEntryOnbaord(p_time);
		}
		break;

	default:
		{
			ASSERT(0);
			ProcessWhenPassengerDeath(p_time) ;
		}
		break;
	}
	return 1;
}
void AirsidePassengerBehavior::ProcessWhenTakeOffFlightWithNoBus(ElapsedTime time)
{
	PLACE_METHOD_TRACK_STRING();
	CPoint2008 point ;
	if(!GetGatePosition(point))
	{
		setState(_DEATH) ;
		GenetateEvent(time) ;
		return ;
	}
	//m_pax->getLogEntry().setEntryTime(time) ;

	AirsideSimulation* pAirsideSimEngine = m_pax->getEngine()->GetAirsideSimulation();
	if(pAirsideSimEngine == NULL)
		return;
	AirsideFlightInSim* pAirsideFlight = pAirsideSimEngine->GetAirsideFlight(m_pax->GetCurFlightIndex());
	if (pAirsideFlight == NULL)
		return;
	pAirsideFlight->DecreasePaxNumber(time);

	m_pax->setState(Birth);

	
	m_pax->generateEvent(time,FALSE) ;
	m_pax->setBehavior( new TerminalMobElementBehavior(m_pax ) );

	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)m_pax->getBehavior(MobElementBehavior::TerminalBehavior);
	if (spTerminalBehavior)
	{
	//	spTerminalBehavior->m_bIsArrivalDoor = TRUE ;
		m_pax->SetFollowerArrivalDoor(true);
		m_bArrivalPax = false;
		spTerminalBehavior->SetTransferTypeState(TerminalMobElementBehavior::TRANSFER_DEPARTURE) ;
	}

}
void AirsidePassengerBehavior::ProcessWhenTakeOffFlight(ElapsedTime time)
{
	PLACE_METHOD_TRACK_STRING();
	
	if(!m_bPaxBusSerivce)
	{
		setState(TAKE_OFF_FLGHT_WITH_NOBUS) ;
		GenetateEvent(time ) ;
		return ;
	}

	AirsideFlightInSim* airFlight = NULL;
	AirsideSimulation* airsideSim = m_pax->getEngine()->GetAirsideSimulation() ;
	if(airsideSim )
	{
		airFlight = airsideSim->GetAirsideFlight(m_pax->GetCurFlightIndex()) ;
		if(airFlight)
			airFlight->SetShowStair(time);
	}
		

	CPoint2008 point,groundpoint ;
	if (GetFlightDoorPostion(time, point, groundpoint))
	{
		setLocation(point) ;

		WriteLog(time) ;

		setDestination(groundpoint);
		time += ElapsedTime(point.distance3D(groundpoint)/(m_pax->getSpeed()*0.7));
		setLocation(getDest()) ;
		WriteLog(time) ;
	}
	else
	{
		if(!GetStandPosition(point))
			return ;
		setLocation(point) ;
		WriteLog(time) ;
	}

	if(!GetBusPosition(point))
	{
		setDestination(GetServiceHoldAreaPoint());
		WriteLog(time + moveTime()) ;
		setState(HOLDAREA_FOR_BUS);
		return ;
	}


	CloseDoor(time);

	setDestination(point) ;
	setState(TAKE_ON_BUS) ;
	GenetateEvent(time + moveTime()) ;
}
void AirsidePassengerBehavior::ProcessWhenWaitforBus(ElapsedTime time)
{
	PLACE_METHOD_TRACK_STRING();
	setLocation(getDest()) ;
	WriteLog(time) ;
	//add the person to the waiting area 
	//do not realize
}
void AirsidePassengerBehavior::ProcessWhenTakeOnBus(ElapsedTime time)
{
	PLACE_METHOD_TRACK_STRING();
	setLocation(getDest()) ;
	WriteLog(time) ;
	m_IsOnBus = TRUE ;
	if(m_bArrivalPax)
	{
		if (!m_pPaxBus)
			return;
		m_pPaxBus->TakeOnPasseneger(m_pax,time);
	}
	else
	{
		TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)m_pax->getBehavior(MobElementBehavior::TerminalBehavior);
		if (spTerminalBehavior && spTerminalBehavior->GetAirsideBus())
		{
			spTerminalBehavior->GetAirsideBus()->TakeOnPasseneger(m_pax,time) ;
		}
	}
	
	//add the person to the bus 
	//do not realize
}
void AirsidePassengerBehavior::ProcessWhenTakeOffBusToGate(ElapsedTime time)
{
	PLACE_METHOD_TRACK_STRING();
	CPoint2008 point ;
	if(!GetGatePosition(point))
	{
		setState(_DEATH) ;
		GenetateEvent(time) ;
		return ;
	}
	m_IsOnBus = FALSE ;
	//WriteLog(time);
	setDestination(point) ;
	setState(ARRIVE_AT_GATE) ;
	GenetateEvent(time + moveTime()) ;
}
void AirsidePassengerBehavior::ProcessWhenWaitForGateOpen(ElapsedTime time) 
{
	PLACE_METHOD_TRACK_STRING();
	setLocation(getDest()) ;
	m_pax->getLogEntry().setEntryTime(time) ;
	WriteLog(time) ;
	//add the person to the waiting area 
	//do not realize 
}
void AirsidePassengerBehavior::ProcessWhenArriveAtGate(ElapsedTime time)
{
	PLACE_METHOD_TRACK_STRING();
	setLocation( getDest()) ;
	//m_pax->getLogEntry().setEntryTime(time) ;
	WriteLog(time) ;
	m_pax->setState(Birth);
	m_pax->setBehavior( new TerminalMobElementBehavior(m_pax ) );

	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)m_pax->getBehavior(MobElementBehavior::TerminalBehavior);
	if (spTerminalBehavior)
	{
		CPoint2008 point;
		GetGatePosition(point);
		Point gatePt;
		gatePt.init(point.getX(),point.getY(),point.getZ());

		spTerminalBehavior->setLocation(gatePt);
		spTerminalBehavior->setDestination(gatePt);
		spTerminalBehavior->SetTransferTypeState(TerminalMobElementBehavior::TRANSFER_DEPARTURE) ;
		m_pax->generateEvent(time,FALSE) ;
		spTerminalBehavior->m_bIsArrivalDoor = TRUE ;
	//	spTerminalBehavior->HasBusServer(m_bPaxBusSerivce?TRUE:FALSE);
	//	spTerminalBehavior->SetAirsideBus(m_pPaxBus);
		m_pPaxBus = NULL;
		m_bArrivalPax = false;
	}
	// 
}
void AirsidePassengerBehavior::ProcessWhenWalkOutGate(ElapsedTime time)
{
	PLACE_METHOD_TRACK_STRING();
	if(!m_pax->getEngine()->IsAirsideSel())
		return;

	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)m_pax->getBehavior(MobElementBehavior::TerminalBehavior);
	if(spTerminalBehavior && !spTerminalBehavior->IsWalkOnBridge())
	{
		ResetTerminalToAirsideLocation();
	}
	if(spTerminalBehavior && spTerminalBehavior->IsWalkOnBridge() )
	{
		Point pt = spTerminalBehavior->getPoint();
		setLocation(CPoint2008(pt.getX(),pt.getY(),pt.getZ()));
	}

	if(spTerminalBehavior&&spTerminalBehavior->HasBusServer() == FALSE)
	{
		setState(TAKE_OFF_BUS_TOFLIGHT) ;
		GenetateEvent(time) ;
		return ;
	}

	CPoint2008 point;
	if(!GetBusPosition(point))
	{
		setState(_DEATH) ;
		GenetateEvent(time) ;
		return ;
	}
	if(spTerminalBehavior&&!spTerminalBehavior->GetAirsideBus()->IsFull())
		setState(WAIT_FOR_BUS) ;
	setDestination(point) ;
	setState(TAKE_ON_BUS) ;
	GenetateEvent(time + moveTime()) ;
}

void AirsidePassengerBehavior::ProcessWhenTakeOffBusToFlight(ElapsedTime time)
{
	PLACE_METHOD_TRACK_STRING();
	CPoint2008 point, groundpoint;
	if (GetFlightDoorPostion(time , point, groundpoint))
	{
		AirsideSimulation* airsideSim = m_pax->getEngine()->GetAirsideSimulation() ;
		if(airsideSim == NULL)
			return;

		TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)m_pax->getBehavior(MobElementBehavior::TerminalBehavior);
		if(spTerminalBehavior == NULL)
			return;

		AirsideFlightInSim* airFlight = airsideSim->GetAirsideFlight(m_pax->GetCurFlightIndex()) ;
		if (airFlight && spTerminalBehavior->HasBusServer() == TRUE)
			airFlight->SetShowStair(time);

		setDestination(groundpoint);
		time += moveTime();
		setLocation(getDest());
		WriteLog(time) ;
		//m_pax->flushLog(time) ;
	}
	else if( !GetStandPosition(point) )
	{
		setState(_DEATH) ;
		GenetateEvent(time) ;
		return ;
	}
	m_IsOnBus = FALSE ;
	//WriteLog(time);
	setDestination(point) ;
	setState(TAKE_ON_FLIGHT) ;
	time += ElapsedTime(point.distance3D(groundpoint)/(m_pax->getSpeed()*0.7));
	GenetateEvent(time);
}
void AirsidePassengerBehavior::ProcessWhenWaitForFlight(ElapsedTime time)
{
	PLACE_METHOD_TRACK_STRING();

	setLocation(getDest()) ;
	m_pax->getLogEntry().setEntryTime(time) ;
	WriteLog(time) ;
	//add person to the standwaitingarea 
}
void AirsidePassengerBehavior::ProcessWhenTakeOnFlight(ElapsedTime time )
{
	PLACE_METHOD_TRACK_STRING();
	setLocation(getDest()) ;
	WriteLog(time) ;


	AirsideSimulation* airsideSim = m_pax->getEngine()->GetAirsideSimulation() ;
	if(airsideSim == NULL)
		return;
	AirsideFlightInSim* airFlight = airsideSim->GetAirsideFlight(m_pax->GetCurFlightIndex()) ;

	airFlight->DecreasePaxNumber(time);
	//m_pax->getLogEntry().setEntryTime(time) ;
	setState(EntryOnboard);
	GenetateEvent(time);
}


void AirsidePassengerBehavior::ProcessWhenPassengerDeath(ElapsedTime time)
{
	CloseDoor(time);
	WriteLog(time, 0, Death);
	m_pax->flushLog(time) ;
}
int AirsidePassengerBehavior::WriteLog(ElapsedTime time,const double speed, enum EntityEvents enumState /*= FreeMoving*/ )
{
	PLACE_METHOD_TRACK_STRING();
	MobEventStruct track;
	track.time = (long)time;
	track.state = enumState;
	track.procNumber = -1;
	track.bDynamicCreatedProc = FALSE;
	track.followowner	= false;
	track.reason = -1;
	track.backup = FALSE;
	
	track.m_IsRealZ = TRUE ;
	
	
	if(m_IsOnBus == FALSE)
       track.speed = 0 ;
	else
	   track.speed = (float)speed ;
	getPoint().getPoint (track.x, track.y, track.m_RealZ);
	m_pax->getLogEntry().addEvent (track);
	setDestination(getPoint()) ;

	MobElementsDiagnosis()->Diagnose( m_pax, time);
	// write log for follower.

	if (m_pax->m_pGroupInfo->IsFollower())
		return 1;
	
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pax->m_pGroupInfo;
	if(!pGroupLeaderInfo->isInGroup())
		return 1;
	Terminal* pterminal = const_cast<Terminal*> (m_pax->GetTerminal()) ;
	Person* pLeader = pGroupLeaderInfo->GetGroupLeader();
	AirsideMobElementBaseBehavior* pLeaderBehavior = pLeader->getAirsideBehavior();
	CPoint2008 location = pLeaderBehavior->getPoint() ;
	{
		MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
		int nFollowerCount = elemList.getCount();
		for( int i=0; i< nFollowerCount; i++ )
		{
			Person* _pFollower = (Person*) elemList.getItem( i );
			if(_pFollower == NULL)
				continue; //Logic Error. 

			track.time = (long)time;
			track.state = enumState;
			track.procNumber = -1;
			track.bDynamicCreatedProc = FALSE;
			track.followowner	= false;
			track.reason = -1;
			track.backup = FALSE;
			
			track.m_IsRealZ = TRUE ;
			
			location.getPoint (track.x, track.y, track.m_RealZ);
			_pFollower->getLogEntry().addEvent (track);
		}
	}
	int nCount = ((Passenger*)m_pax)->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = ((Passenger*)m_pax)->m_pVisitorList[i];
		if( pVis )
		{


			track.time = (long)time;
			track.state = enumState;
			track.procNumber = -1;
			track.bDynamicCreatedProc = FALSE;
			track.followowner	= false;
			track.reason = -1;
			track.backup = FALSE;
			track.m_IsRealZ = TRUE ;
			
			location.getPoint (track.x, track.y, track.m_RealZ);
			pVis->getLogEntry().addEvent (track);
			CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)pVis->m_pGroupInfo;
			MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
			int nFollowerCount = elemList.getCount();
			for( int i=0; i< nFollowerCount; i++ )
			{
				Person* _pFollower = (Person*) elemList.getItem( i );
				if(_pFollower == NULL)
					continue; //Logic Error. 
				track.time = (long)time;
				track.state = enumState;
				track.procNumber = -1;
				track.bDynamicCreatedProc = FALSE;
				track.followowner	= false;
				track.reason = -1;
				track.backup = FALSE;
				
				track.m_IsRealZ = TRUE ;
				location.getPoint (track.x, track.y, track.m_RealZ);
				_pFollower->getLogEntry().addEvent (track);
			}
		}
	}
	return 1 ;
}
BOOL AirsidePassengerBehavior::GetStandPosition(CPoint2008& point)
{
	PLACE_METHOD_TRACK_STRING();
   AirsideSimulation* airsideSim = m_pax->getEngine()->GetAirsideSimulation() ;
   //ASSERT(airsideSim) ;
   if(airsideSim == NULL)
	   return FALSE ;
   int flightId = m_pax->GetCurFlightIndex() ;
 
   AirsideFlightInSim* airFlight = airsideSim->GetAirsideFlight(flightId) ;
   ASSERT(airFlight) ;
   if(airFlight == NULL)
	   return FALSE ;
   StandInSim* stand = airFlight->GetOperationParkingStand();
   if(stand == NULL)
	   return FALSE ;
	CPoint2008 point2008 = stand->GetStandInput()->GetServicePoint() ;
	point.setX(point2008.getX()) ;
	point.setY(point2008.getY()) ;
	point.setZ(point2008.getZ()) ;
	return TRUE;
}
CPoint2008 AirsidePassengerBehavior::GetStandWaiteAreaPosition()
{
	//need realize
 return CPoint2008() ;
}

BOOL AirsidePassengerBehavior::GetGateTerminalPoint(Point& point)
{
	PLACE_METHOD_TRACK_STRING();
	int flightid = 0 ;
	Flight* p_flt = NULL ;
	ProcessorID ID ;
	// Terminal* pterminal = const_cast<Terminal*> (m_pax->GetTerminal()) ;
	flightid =  m_pax->getLogEntry().getDepFlight();
	p_flt = m_pax->getEngine()->m_simFlightSchedule.GetFlightByFlightIndex(flightid);
	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)m_pax->getBehavior(MobElementBehavior::TerminalBehavior);

	if(m_pax->getLogEntry().isTurnaround())
	{
		if(spTerminalBehavior&&spTerminalBehavior->GetTransferTypeState() == TerminalMobElementBehavior::TRANSFER_ARRIVAL)
		{
			flightid = m_pax->getLogEntry().getArrFlight() ;
			p_flt = m_pax->getEngine()->GetAirsideSimulation()->GetAirsideFlight(flightid)->GetFlightInput();
			ID = p_flt->getArrGate() ;
		}
		else
		{
			flightid = m_pax->getLogEntry().getDepFlight() ;
			p_flt = m_pax->getEngine()->GetAirsideSimulation()->GetAirsideFlight(flightid)->GetFlightInput();
			ID = p_flt->getDepGate() ;
		}
	}
	else
	{
		if(m_pax->getLogEntry().isArriving())
		{
			flightid =  m_pax->getLogEntry().getArrFlight();
			p_flt = m_pax->getEngine()->GetAirsideSimulation()->GetAirsideFlight(flightid)->GetFlightInput();
			if(p_flt== NULL)
				return FALSE ;
			ID = p_flt->getArrGate() ;

		}
		else
		{
			flightid =  m_pax->getLogEntry().getDepFlight();
			p_flt = m_pax->getEngine()->GetAirsideSimulation()->GetAirsideFlight(flightid)->GetFlightInput();
			if(p_flt== NULL)
				return FALSE ;
			ID= p_flt->getDepGate() ;
		}
	}

#ifdef _DEBUG
	CString processid ; 
	processid = ID.GetIDString() ;
	TRACE(processid) ;
#endif
	Terminal* p_ter = const_cast<Terminal*>(m_pax->GetTerminal()) ;
	Processor* pro = p_ter->GetTerminalProcessorList()->getProcessor(ID) ;
	if(pro == NULL)
		return FALSE ;
	point = pro->GetServiceLocation();
	return TRUE;
}

BOOL AirsidePassengerBehavior::GetGatePosition(CPoint2008& point)
{
	PLACE_METHOD_TRACK_STRING();
	Point pt;
	if (!GetGateTerminalPoint(pt))
		return FALSE;

	int nFloor = (int)(pt.getZ() / SCALE_FACTOR);

	CTermPlanDoc* pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
	if (pDoc)
	{
		double dLz = pDoc->GetFloorByMode(EnvMode_Terminal).m_vFloors[nFloor]->RealAltitude();
		point.setPoint(pt.getX(),pt.getY(),dLz);
		return TRUE;
	}
	return  FALSE;
}
CPoint2008 AirsidePassengerBehavior::GetGateWaiteAreaPosition()
{
	return CPoint2008() ;
}
BOOL  AirsidePassengerBehavior::GetBusPosition(CPoint2008& point)
{
	PLACE_METHOD_TRACK_STRING();
	CAirsidePaxBusInSim* pCurPaxBus = NULL;

	if (m_pPaxBus)
	{
		pCurPaxBus = m_pPaxBus;
	}
	else
	{
		TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)m_pax->getBehavior(MobElementBehavior::TerminalBehavior);

		if (spTerminalBehavior == NULL)
			return FALSE;
		
		if(spTerminalBehavior->GetAirsideBus() == NULL)
			return FALSE;

		pCurPaxBus = spTerminalBehavior->GetAirsideBus();
	}

	CPoint2008 point2008 = pCurPaxBus->GetPosition() ;
	point.setX(point2008.getX()) ;
	point.setY(point2008.getY() ) ;
	point.setZ(point2008.getZ() ) ;
	_offSetInBus = pCurPaxBus->GetRanddomPoint() ;
	m_ZPosition = (float)(pCurPaxBus->GetVehicleLength()/2 - abs(_offSetInBus.getX()) ) / (float)(pCurPaxBus->GetVehicleLength()/2 + abs(_offSetInBus.getX()) ) ;
	CPoint2008 _off = _offSetInBus ;
	_off.rotate( pCurPaxBus->getDirect(point));
	point.setX(point2008.getX() + _off.getX()) ;
	point.setY(point2008.getY() + _off.getY()) ;
	point.setZ(point2008.getZ() + _off.getZ()) ;


	return TRUE ;
}
BOOL AirsidePassengerBehavior::IsBusArrived() 
{
	return FALSE ;
}
BOOL AirsidePassengerBehavior::IsFlightArrived()
{
	return TRUE ;
}
BOOL AirsidePassengerBehavior::IsGateOpen()
{
	return TRUE ;
}

bool AirsidePassengerBehavior::GetFlightDoorPostion( const ElapsedTime& t,CPoint2008& doorpoint, CPoint2008& groundpoint)
{
	PLACE_METHOD_TRACK_STRING();
	AirsideSimulation* airsideSim = m_pax->getEngine()->GetAirsideSimulation() ;
	if(airsideSim == NULL)
		return false;
	AirsideFlightInSim* airFlight = airsideSim->GetAirsideFlight(m_pax->GetCurFlightIndex()) ;

	//std::vector< std::pair<CPoint2008, CPoint2008> > vDoorpoints;
	//if (!airFlight->GetOpenDoorAndStairGroundPostions(vDoorpoints))
	//	return false;
	CFlightOpenDoors* openDoors = airFlight->OpenDoors(t);
	if(!openDoors)
		return false;

	if (m_pax->getEngine()->IsOnboardSel())
	{
		OnboardSimulation* pOnboardSim = m_pax->getEngine()->GetOnboardSimulation();
		if (pOnboardSim)
		{
			bool bArrival = airFlight->IsArrivingOperation()?true:false;
			OnboardFlightInSim* pOnboardFlightInSim = pOnboardSim->GetOnboardFlightInSim(airFlight,bArrival);
			if (pOnboardFlightInSim)
			{
				CString strError;
				OnboardSeatInSim* pSeatInSim = pOnboardFlightInSim->GetSeat(m_pax,strError);
				OnboardDoorInSim* pDoorInSim = pOnboardFlightInSim->GetDoor(m_pax,pSeatInSim);
				if (pDoorInSim)
				{
					doorpoint = pDoorInSim->GetConnectionPos();
					groundpoint = pDoorInSim->GetGroundPos();
					CPoint2008 stand ;
					GetStandPosition(stand) ;
					groundpoint.setZ(stand.getZ());
				}
			}
		}	
	}
	else
	{
		if(openDoors->getCount()==0)
			return false;

		const COpenDoorInfo& doorInfo = openDoors->getDoor(0);
		doorpoint  = doorInfo.mDoorPos; 
		groundpoint = doorInfo.mGroundPos;
		CPoint2008 stand ;
		GetStandPosition(stand) ;
		groundpoint.setZ(stand.getZ());
	}
	return true;
	
}

CPoint2008 AirsidePassengerBehavior::GetServiceHoldAreaPoint()
{
	PLACE_METHOD_TRACK_STRING();
	CPoint2008 point = getPoint();

	// init the hold are 
	Point vPath[4];
	vPath[0].setPoint(point.getX()-500,point.getY()+500,point.getZ());
	vPath[1].setPoint(point.getX()+500,point.getY()+500,point.getZ());
	vPath[2].setPoint(point.getX()+500,point.getY()-500,point.getZ());
	vPath[3].setPoint(point.getX()-500,point.getY()-500,point.getZ());

	Pollygon serviceArea;
	serviceArea.init(4,vPath);

	CPoint2008 outPoint;
	outPoint.setPoint(serviceArea.getRandPoint().getX(),serviceArea.getRandPoint().getY(),serviceArea.getRandPoint().getZ());
	return outPoint;
}

void AirsidePassengerBehavior::ProcessHoldAreaToBus(ElapsedTime time)
{
	PLACE_METHOD_TRACK_STRING();

	CPoint2008 point;
	if(!GetBusPosition(point))
	{
		setDestination(GetServiceHoldAreaPoint());
		WriteLog(time + moveTime()) ;
		setState(HOLDAREA_FOR_BUS);
		return ;
	}


	AirsideSimulation* airsideSim = m_pax->getEngine()->GetAirsideSimulation() ;
	if(airsideSim == NULL)
		return;

	AirsideFlightInSim* airFlight = airsideSim->GetAirsideFlight(m_pax->GetCurFlightIndex()) ;
	CloseDoor(time);
	setDestination(point) ;
	setState(TAKE_ON_BUS) ;
	GenetateEvent(time + moveTime()) ;
}

void AirsidePassengerBehavior::CloseDoor(const ElapsedTime& time)
{
	CARCportEngine* pEngine = m_pax->getEngine();

	if (pEngine == NULL)
		return;

	AirsideSimulation* pAirsideSimEngine = pEngine->GetAirsideSimulation();
	if(pAirsideSimEngine == NULL)
		return;
	AirsideFlightInSim* pAirsideFlight = pAirsideSimEngine->GetAirsideFlight(m_pax->GetCurFlightIndex());
	if (pAirsideFlight == NULL)
		return;
	
	if (pEngine->IsOnboardSel())
	{
		OnboardSimulation* pOnboardSimEngine = pEngine->GetOnboardSimulation();
		bool bArrival = pAirsideFlight->IsArrivingOperation() ? true : false;
		OnboardFlightInSim* pOnboardFlightInSim = pOnboardSimEngine->GetOnboardFlightInSim(pAirsideFlight,bArrival);

		if (pOnboardFlightInSim == NULL)
			return;
		
		pOnboardFlightInSim->ProcessCloseDoor(time);
	}
	else
	{
		pAirsideFlight->DecreasePaxNumber(time);
	}
}

void AirsidePassengerBehavior::WakeupHoldArea(ElapsedTime time)
{
	PLACE_METHOD_TRACK_STRING();
	if (getState() == HOLDAREA_FOR_BUS)
	{
		GenetateEvent(time);
	}
}



void AirsidePassengerBehavior::setDestination(CPoint2008 p)
{
	PLACE_METHOD_TRACK_STRING();
	if (m_pax->getType().GetTypeIndex() != 0)
	{
		return;
	}

	m_ptDestination = p;
	SetFollowerDestination(location,m_ptDestination,Person::m_pRotation);

	
	int nCount = ((Passenger*)m_pax)->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = ((Passenger*)m_pax)->m_pVisitorList[i];
		if( pVis )
		{
			AirsideMobElementBaseBehavior* spFollowerBehavior = pVis->getAirsideBehavior();
			if (spFollowerBehavior)
			{
				spFollowerBehavior->setDestination( p );
			}
		}
	}
}


void AirsidePassengerBehavior::ProcessEntryOnbaord( ElapsedTime p_time )
{
	PLACE_METHOD_TRACK_STRING();
	// do not need departing to onboard mode.
	if( !simEngineConfig()->isSimOnBoardMode())// if no selected, 
	{	
		WriteLog(p_time, 0, Death);
		m_pax->flushLog( p_time,true );
		return;
	}

	//----------------------------------------------------
	// sim Terminal and airside mode together.

	// arriving passenger do not need departing to airside.
	if(m_pax->getLogEntry().isArriving() && !m_pax->getLogEntry().isTurnaround())
	{
		WriteLog(p_time, 0, Death);
		m_pax->flushLog( p_time );
		return;
	}

	// not arriving and departing, impossible, just flush log.
	if(false == m_pax->getLogEntry().isDeparting())
	{
		WriteLog(p_time, 0, Death);
		m_pax->flushLog( p_time );
		return;
	}
	// handle departing passenger's mode transfer action.

	//Must be Passenger!!!  (Passenger*)m_pPerson
	//By now, AirsideMobElementBehavior can just handle passenger's movement logic.
	if(m_pax->getLogEntry().GetMobileType() == 0)
	{
		//set enplane behavior
		m_pax->setState(EntryOnboard);
		m_pax->setBehavior(new PaxEnplaneBehavior(m_pax)); 

		GenetateEvent(p_time);
		return;
	}
	else
	{
		// NonPax type(visitor) exit terminal mode.
		//ASSERT(m_pPerson->getLogEntry().GetMobileType() == 1);

		// if pax type is 2, baggage, throw exception.
		// by now, simEngine doesnot support baggage move alone to airside mode.

		WriteLog(p_time, 0, Death);
		m_pax->flushLog (p_time);
	}

}
void AirsidePassengerBehavior::SetFollowerDestination(const CPoint2008& _ptCurrent, const CPoint2008& _ptDestination, float* _pRotation )
{
	PLACE_METHOD_TRACK_STRING();

	if (m_pMobileElemment->m_pGroupInfo->IsFollower())
		return;

	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pMobileElemment->m_pGroupInfo;
	if(!pGroupLeaderInfo->isInGroup())
		return;

	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i< nFollowerCount; i++ )
	{
		Person* _pFollower = (Person*) elemList.getItem( i );
		if(_pFollower == NULL)
			continue;  

		MobElementBehavior* spBehavior = _pFollower->getCurrentBehavior();
		if (spBehavior == NULL)
		{
			_pFollower->setBehavior(new AirsidePassengerBehavior(_pFollower,_pFollower->getState()));
		}
		else if (spBehavior->getBehaviorType()!= AirsideBehavior)
		{
			_pFollower->setBehavior(new AirsidePassengerBehavior(_pFollower,_pFollower->getState()));
		}

		CPoint2008 ptDestination(0.0, 0.0, 0.0);
		ptDestination = _ptDestination;
		float fDir = (i> (MAX_GROUP-1)) ? (float)0.0 : _pRotation[i+1];
		ptDestination.offsetCoords( _ptCurrent, (double)fDir, GROUP_OFFSET );
		if( _pFollower->getState() != Death )
		{
			//_pFollower->setAirsideDestination( ptDestination );

			MobElementBehavior* spBehavior = _pFollower->getCurrentBehavior();
			if (spBehavior&&spBehavior->getBehaviorType() == MobElementBehavior::AirsideBehavior)
			{
				AirsideMobElementBaseBehavior* spAirsideBehavior = _pFollower->getAirsideBehavior();
				if (spAirsideBehavior == NULL)
				{
					_pFollower->setBehavior(new AirsidePassengerBehavior(_pFollower,getState()));
				}

				spAirsideBehavior = _pFollower->getAirsideBehavior();
				spAirsideBehavior->setDestination(ptDestination);
			}
		}
	}

}


