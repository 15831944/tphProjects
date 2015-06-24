#include "StdAfx.h"
#include ".\Goal_MoveInParkingLot.h"
#include "..\LandsideParkingLotInSim.h"
#include "..\LandsideParkingLotGraph.h"
#include "..\LandsideSimulation.h"
#include "..\LandsideResourceManager.h"
#include "..\ARCportEngine.h"
#include ".\Goal_MoveInRoad.h"
#include "Common\DynamicMovement.h"

//arrival pax no parking
void Goal_ParkingToLotSpot::Activate( CARCportEngine* pEngine )
{
	if(m_pDoor && m_pSpot){
		m_pSpot->setOccupyVehicle(m_pOwner);			
		DrivePathInParkingLot drivePath;	
		LandsideParkingLotInSim* pLot = m_pSpot->getParkingLot();

		if(pLot->getDriveGraph()->FindPath(m_pDoor,m_pSpot,drivePath))
		{
			AddSubgoalEnd( new Goal_MoveInParkingLotLane(m_pOwner,drivePath) );
		}		
		else{
			AddSubgoalEnd( new Goal_Dummy(m_pOwner) );
		}
		AddSubgoalEnd(new Goal_ParkToSpot(m_pOwner,m_pSpot) );

		//if(!pEngine->IsTerminal()) //wait for 30 mins  if not select terminal
		//{
		//	AddSubgoalEnd(new Goal_WaitForSometime(m_pOwner,ElapsedTime(1800L)));
		//}

		m_iStatus = gs_active;
	}

}



Goal_ParkingToLotSpot::Goal_ParkingToLotSpot( LandsideVehicleInSim* pEnt,LandsideParkingLotDoorInSim* pDoor ,LandsideParkingSpotInSim* mpSpot )
:LandsideVehicleSequenceGoal(pEnt)
,m_pSpot(mpSpot)
,m_pDoor(pDoor)
{
	m_sDesc = "Goal_ParkingToLotSpot";
}	







void Goal_LeaveParkingLot::Activate( CARCportEngine* pEngine )
{
	LandsideResourceInSim* pRes = m_pOwner->getLastState().getLandsideRes();
	ASSERT(pRes);
	LandsideParkingSpotInSim* pSpot = pRes->toParkLotSpot();
	ASSERT(pSpot);

	
	RemoveAllSubgoals();

	LandsideParkingLotInSim* pLot = pSpot->getParkingLot();
	LandsideParkingLotDoorInSim* pExitDoor = pLot->getExitDoor(m_pOwner);
	ASSERT(pExitDoor);

	DrivePathInParkingLot drivePath;			
	if(pLot->getDriveGraph()->FindPath(pSpot,pExitDoor,drivePath))
	{
		m_SubGoals.push_back( new Goal_MoveInParkingLotLane(m_pOwner,drivePath) );
	}
	m_SubGoals.push_back(new Goal_ProcessAtLotDoor(m_pOwner,pExitDoor) );

	m_iStatus = gs_active;
	
}





void Goal_BirthAtParkingLot::Process( CARCportEngine* pEngine )
{
	ActivateIfInactive(pEngine);
	ElapsedTime t = m_pOwner->curTime();
	LandsideSimulation* pLandSim = pEngine->GetLandsideSimulation();
	
	if(!m_pLot->IsSpotLess() ) 
	{
		LandsideParkingSpotInSim* pSpot = m_pLot->getFreeSpot(m_pOwner);
		if(pSpot)//got a free spot stay in there
		{
			//got to next state parking in lot			
			MobileState birthState;
			birthState.distInRes = 0;
			birthState.time = t;
			birthState.dSpeed = 0;
			birthState.pRes = pSpot;
			birthState.pos = pSpot->getPos();
			m_pOwner->initState(birthState);
			birthState.pos+=pSpot->getDir();
			m_pOwner->UpdateState(birthState);
			m_pLot->RemoveListener(m_pOwner);
			m_iStatus = gs_completed;
		}	
		else
		{
			CString sError;
			sError.Format(_T("Can not find Parking lot Spot for Arrival Pax's Car at %s"), m_pLot->getName().GetIDString().GetString() );
			throw SGoalException(sError);
		}
	}
	else//no parking lot throw error message 
	{			
		m_iStatus = gs_failed;
	}	
	//return m_iStatus;
}



void Goal_ProcessAtLotDoor::Activate( CARCportEngine* pEngine )
{
	nPortIdx = mpDoor->allocatVehicleToPort(m_pOwner);
	if(nPortIdx>=0)
	{
		m_iStatus = gs_active;		
	}
	else
	{
		mpDoor->AddListener(m_pOwner);
	}
	
}

void Goal_ProcessAtLotDoor::Process( CARCportEngine* pEngine )
{
	ActivateIfInactive(pEngine);

	if(isActive())
	{
		m_pOwner->removeBlock(&blockAtDoor,m_pOwner->curTime());
		//move to the door pos
		CPoint2008 doorpos = mpDoor->getPortPos(nPortIdx);
		MobileState lastState = m_pOwner->getLastState();
		lastState.distInRes= 0;
		lastState.pos = doorpos;	
		lastState.pRes = mpDoor;
		m_pOwner->MoveToPos(lastState);	
		
		m_iStatus = gs_completed;		
	}
	else
	{
		m_pOwner->addBlock(&blockAtDoor);
	}
	//return m_iStatus;
}

Goal_ProcessAtLotDoor::Goal_ProcessAtLotDoor( LandsideVehicleInSim* pEnt, LandsideParkingLotDoorInSim* pDoor ) :LandsideVehicleGoal(pEnt)
{
	m_sDesc = "Goal_MoveToLotDoor";
	mpDoor = pDoor;
	nPortIdx = -1;
}


//bool Goal_ProcessAtLotDoor::DefaultHandleSignal( SSignal* pSignal )
//{
//	if(pSignal->getSource() == mpDoor)
//	{
//		m_pOwner->Activate(m_pOwner->curTime());
//		return true;
//	}
//	return false;
//}

bool Goal_MoveInParkingLotLane::MoveInPath( CARCportEngine* pEngine )
{
	double dt = (double)LandsideSimulation::tAwarenessTime;
	LandsideVehicleInSim* mpVehicle = m_pOwner;

	MobileState curState = mpVehicle->getLastState();
	double dCurSpeed = curState.dSpeed;
	double halfLen = mpVehicle->GetLength()*0.5;
	LandsideVehicleInSim* pConflictVehicle = NULL;
	double dMaxSpd = mpVehicle->getSpeed(curState.getLandsideRes(),curState.distInRes);	

	ConflictInfo cfinfo;
	_getConflictInfo(m_dLastDistInRoute,cfinfo);

	OpMoveType opType = _STOP;
	double dEndSpd = 0;
	double dOffsetS = 0;

	MoveOperationTable& opTable = m_pOwner->m_opTable;
	for(int iopType = _MAXBREAK; iopType<=_MAXACC; iopType++)
	{		
		double opAcc=  opTable[(OpMoveType)iopType];

		double _dEndSpd = dCurSpeed + opAcc * dt;
		_dEndSpd = MAX(0,_dEndSpd);
		_dEndSpd = MIN(_dEndSpd,dMaxSpd);
		
		DistanceUnit _dOffsetS = 0;
		if(opAcc!=0 && dCurSpeed!=_dEndSpd)
			_dOffsetS = (_dEndSpd*_dEndSpd-dCurSpeed*dCurSpeed)*0.5/opAcc;
		else 
			_dOffsetS = (_dEndSpd+dCurSpeed)*0.5*dt;
        DistanceUnit decDist  = m_pOwner->getDecDist(_dEndSpd);

		//check conflict 
		if(cfinfo.nearestConflict == cf_followingVehicle && cfinfo.pFollowingVehicle )
		{			
			if(cfinfo.sepToConflict < _dOffsetS + decDist )
			{
				pConflictVehicle = cfinfo.pFollowingVehicle;
				break;
			}
		}
		//
		dEndSpd = _dEndSpd;
		dOffsetS = _dOffsetS;
		opType = (OpMoveType)iopType;

		if(dEndSpd>=dMaxSpd)
			break;
	}

	


	if(dOffsetS >0)
	{
		mpVehicle->StartMove(mpVehicle->curTime());
		DoMovementsInPath(mpVehicle->curTime(),dOffsetS,dCurSpeed,dEndSpd);			
		return true;
	}
	else
	{		
		ASSERT(pConflictVehicle);
		if(pConflictVehicle)
		{	
			mpVehicle->WaitForVehicle(pConflictVehicle);			
		}
		else
		{
			curState.time+= LandsideSimulation::tAwarenessTime;
			mpVehicle->UpdateState(curState);
		}
		return false;
	}

	return true;
}

Goal_MoveInParkingLotLane::Goal_MoveInParkingLotLane( LandsideVehicleInSim* pEnt, const DrivePathInParkingLot& pDrivePath ) :LandsideVehicleGoal(pEnt)
{
	m_sDesc = "Goal_DriveInParkingLotLane";
	m_path  = pDrivePath;
	m_dLastDistInRoute = 0;

	//init walksways
	//mDrivePath.Update(pEnt);	
}

//LandsideVehicleInSim * Goal_MoveInParkingLotLane::CheckConflict( const ARCPolygon& poly, const ARCVector3& dir,LandsideResourceInSim* pRes )
//{
//	LandsideVehicleInSim* pConflictVehcile = NULL;
//	for(int i=0;i<pRes->GetInResVehicleCount();i++)
//	{
//		LandsideVehicleInSim* pOtherVeh = pRes->GetInResVehicle(i);
//		if(m_pOwner == pOtherVeh)
//			continue;
//		if( !m_pOwner->bCanWaitFor(pOtherVeh) )
//			continue;
//
//		if( poly.IsOverlapWithOtherPolygon(pOtherVeh->getBBox(m_pOwner->curTime())) )
//			return pOtherVeh;
//
//	}
//	return NULL;
//}

//#define CONCERN_RADIUS 10000 //500 meters
//LandsideVehicleInSim* Goal_MoveInParkingLotLane::CheckNextConflict( const ARCPolygon& poly, const ARCVector3& dir )
//{
//	int nCurSeg = mDrivePath.GetSegIndex(m_dLastDistInRoute);
//	int nNextSeg = mDrivePath.GetSegIndex(m_dLastDistInRoute+CONCERN_RADIUS);
//
//	//search in the route for the ahead flight
//	for(int i=nCurSeg;i<nNextSeg+1;i++)
//	{
//		LandsideResourceInSim* pResource = mDrivePath.GetSegResource(i);
//		if(!pResource)
//			continue;
//
//		if (LandsideVehicleInSim* pOtherV = CheckConflict(poly,dir, pResource) )
//		{
//			return pOtherV;
//		}	
//	}
//	return false;
//}

void Goal_MoveInParkingLotLane::DoMovementsInPath( const ElapsedTime& fromTime , double dS,double dStarSpd, double dEndSpd )
{
	double dT = dS*2.0/(dStarSpd+dEndSpd);
	double dAcc = 0;
	if(dT > 0)
		dAcc = (dEndSpd - dStarSpd)/dT;

	double dFromIndex = m_path.getDistIndex(m_dLastDistInRoute);
	double dToIndex = m_path.getDistIndex(m_dLastDistInRoute+dS);	 

	DynamicMovement dymMove(dStarSpd,dEndSpd, dS);
	for(int iIndex = (int)dFromIndex+1;iIndex < dToIndex;iIndex++)
	{
		double dDist = m_path.getIndexDist(iIndex) -m_dLastDistInRoute;	
		const LandsidePosition& pos = m_path.PositionAt(iIndex);

		MobileState mobState;
		mobState.dAcc = dAcc;
		mobState.distInRes = pos.distInRes;
		mobState.dSpeed = dymMove.GetDistSpeed(dDist);
		mobState.pRes = pos.pRes;
		mobState.time = fromTime + dymMove.GetDistTime(dDist);
		mobState.pos = pos.pos;
		mobState.isConerPt = true;

		MobileState lastState = m_pOwner->getLastState();
		
		m_pOwner->UpdateState(mobState);

	}

	//double dDist = m_path.getIndexDist(dToIndex)-m_distInPath;	
	LandsidePosition pos = m_path.getIndexPosition(dToIndex);	
	//the last point
	MobileState mobState;
	mobState.dAcc = dAcc;
	mobState.distInRes = pos.distInRes;
	mobState.dSpeed = dEndSpd;
	mobState.pRes = pos.pRes;
	mobState.time = fromTime + dymMove.GetDistTime(dS);
	mobState.isConerPt = (dToIndex==(int)dToIndex);
	mobState.pos = pos.pos;

	MobileState lastState = m_pOwner->getLastState();
	
	m_pOwner->UpdateState(mobState);
	m_dLastDistInRoute += dS;
}

void Goal_MoveInParkingLotLane::Process( CARCportEngine* pEngine )
{
	ActivateIfInactive(pEngine);

	if(IsOutRoute(m_dLastDistInRoute)){
		m_iStatus = gs_completed;
		MobileState lastState = m_pOwner->getLastState();
		lastState.time = m_pOwner->curTime();
		m_pOwner->UpdateState(lastState);
		return ;
	}	
	MoveInPath(pEngine);	
	//return m_iStatus;
}

bool Goal_MoveInParkingLotLane::IsOutRoute( double dDist ) const
{
	return m_dLastDistInRoute > ( m_path.getAllLength() -m_pOwner->GetLength());
	return false;
}

void Goal_MoveInParkingLotLane::_getConflictInfo( DistanceUnit distRad ,ConflictInfo& conflictInfo )
{
	LandsideResourceInSim* pCurRes = m_pOwner->getLastState().getLandsideRes();
	LandsideResourceInSim* pItrRes = NULL;
	int fIndex = (int)m_path.getDistIndex(m_dLastDistInRoute);


	for(int iIndex = fIndex; iIndex < m_path.getPtCount();iIndex++ )
	{
		if(m_path.getIndexDist(iIndex) - m_dLastDistInRoute > distRad )
			break;

		const LandsidePosition& landsidepos =  m_path.PositionAt(iIndex);
		LandsideResourceInSim* pthisRes = landsidepos.pRes;	
		if(pthisRes == pItrRes)continue;
		pItrRes = pthisRes;
		if(!pthisRes){ ASSERT(FALSE); continue; }

		//break if dist too far			
		DistanceUnit distInRes = landsidepos.distInRes;
		if(pthisRes == pCurRes)
			distInRes = m_pOwner->getLastState().distInRes;	

		//get following vehicle in resource
		DistanceUnit distAhead;
		LandsideVehicleInSim* pHeadV = pthisRes->GetAheadVehicle(m_pOwner,distInRes,distAhead);
		if(pHeadV)
		{
			DistanceUnit distHeadInPath = distAhead - landsidepos.distInRes + m_path.getIndexDist(iIndex);
			DistanceUnit sep = distHeadInPath - m_dLastDistInRoute - m_pOwner->GetHalfLength() - pHeadV->GetHalfLength();
			if( sep < conflictInfo.sepToConflict )
			{
				conflictInfo.nearestConflict = cf_followingVehicle;
				conflictInfo.sepToConflict = sep;
				conflictInfo.pFollowingVehicle = pHeadV;
			}	
		}
	}
}

void Goal_ParkToSpot::Process( CARCportEngine* pEngine )
{
	//move to spot
	MobileState state = m_pOwner->getLastState();

	const CPoint2008& spotpos = mpSpot->getPos();
	ARCVector3 spotdir = mpSpot->getDir();
	spotdir.Normalize();

	state.pRes = mpSpot;
	state.dSpeed= m_pOwner->getSpeed(mpSpot,0);
	state.pos = spotpos;
	state.distInRes = 0;

	m_pOwner->MoveToPos(state);

	//write dir log
	state.dSpeed =0;
	state.pos -= spotdir;
	m_pOwner->MoveToPos(state);	

	m_iStatus = gs_completed;

	
}

void Goal_ParkingAtParkingLot::Activate( CARCportEngine*pEngine )
{
	m_pSpot = m_pLot->getFreeSpot(m_pOwner);
	if(m_pSpot){
		m_pSpot->setOccupyVehicle(m_pOwner);
	}
	else
	{
		setFailed();
	}

	
	if( m_pSpot ){
		DrivePathInParkingLot drivePath;	
		if( m_pLot->getDriveGraph()->FindPath(m_pDoor,m_pSpot,drivePath) )
		{
			m_MoveinParkinglot = new Goal_MoveInParkingLotLane(m_pOwner,drivePath);
		}
		setActive();
		m_state = _processAtDoor;
	}
}

void Goal_ParkingAtParkingLot::doProcess( CARCportEngine* pEngine )
{
	switch(m_state)
	{
	case _processAtDoor:
		ProcessAtLotDoor(pEngine);
		break;
	case _moveInDriveLane:
		MoveInDriveLane(pEngine);
		break;
	case _parkingToSpot:
		ParkingToSpot(pEngine);
		break;
	}
	//setComplete();
}

void Goal_ParkingAtParkingLot::ProcessAtLotDoor( CARCportEngine* pEngine )
{
	int nPortIdx = m_pDoor->allocatVehicleToPort(m_pOwner);
	if(nPortIdx>=0)
	{
		CPoint2008 doorpos = m_pDoor->getPortPos(nPortIdx);
		MobileState lastState = m_pOwner->getLastState();
		lastState.distInRes= 0;
		lastState.pos = doorpos;	
		lastState.pRes = m_pDoor;
		m_pOwner->MoveToPos(lastState);	
		m_state = _moveInDriveLane;		
	}
}

void Goal_ParkingAtParkingLot::MoveInDriveLane( CARCportEngine* pEngine )
{
	if(m_MoveinParkinglot)
	{
		m_MoveinParkinglot->Process(pEngine);
		if(!m_MoveinParkinglot->isComplete())
		{
			return;
		}
	}
	m_state = _parkingToSpot;
}

void Goal_ParkingAtParkingLot::ParkingToSpot( CARCportEngine* pEngine )
{
	//move to spot
	MobileState state = m_pOwner->getLastState();

	const CPoint2008& spotpos = m_pSpot->getPos();
	ARCVector3 spotdir = m_pSpot->getDir();
	spotdir.Normalize();

	state.pRes = m_pSpot;
	state.dSpeed= m_pOwner->getSpeed(m_pSpot,0);
	state.pos = spotpos;
	state.distInRes = 0;

	m_pOwner->MoveToPos(state);

	//write dir log
	state.dSpeed =0;
	state.pos -= spotdir;
	m_pOwner->MoveToPos(state);	
	
	//complte
	setComplete();
}
