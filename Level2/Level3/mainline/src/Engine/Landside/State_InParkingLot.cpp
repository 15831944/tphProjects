#include "StdAfx.h"
#include "State_InParkingLot.h"
#include "..\ARCportEngine.h"
#include "..\LandsideSimulation.h"
#include "Common\DynamicMovement.h"

#include "..\MobElementsDiagnosis.h"
#include "Common\FileOutPut.h"

//////////////////////////////////////////////////////////////////////////
void State_EntryLotDoor::Execute( CARCportEngine* pEngine )
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
		ElapsedTime endT = m_pOwner->getLastState().time;
		TransitToState(new State_FindingParkSpot(m_pOwner,m_pDoor->getParkingLot()),endT);
	}
	else
	{
		m_pOwner->StepTime(pEngine);
	}	
}
void State_ExitLotDoor::Execute( CARCportEngine* pEngine )
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
		m_pOwner->ChangeStateMoveToDest();
	}
	else
	{
		m_pOwner->StepTime(pEngine);
	}	
}

//////////////////////////////////////////////////////////////////////////
void State_FindingParkSpot::Execute( CARCportEngine* pEngine ) 
{
	LandsideParkingSpotInSim* mpSpot = m_pLot->getFreeSpot(m_pOwner);
	if(mpSpot)
	{
		mpSpot->setOccupyVehicle(m_pOwner);
		TransitToState(new State_MoveAndParkingToSpot(m_pOwner,mpSpot),pEngine);
	}
	else
	{
		CString sError;
		sError.Format(_T("Can not find a free spot in %s"), m_pLot->getName().GetIDString().GetString() );
		m_pOwner->ShowError(sError, "Simulation Error");
		m_pOwner->OnTerminate(pEngine);
	}
}

//////////////////////////////////////////////////////////////////////////
bool State_MoveInParkingLot::MoveInPath( CARCportEngine* pEngine )
{

	double dt = (double)LandsideSimulation::tAwarenessTime;
	LandsideVehicleInSim* mpVehicle = m_pOwner;
	getVehicle()->WaitForVehicle(NULL);

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

			if( MobElementsDiagnosis()->isInList(m_pOwner->getID()) )
			{
				CFileOutput oufile("D:\\landsidebug\\cf.log");
				oufile.Log(curTime().printTime());
				oufile.Log("wait for").Log(pConflictVehicle->getID()).Line();				
			}
		}
		return false;
	}
	return false;
}

void State_MoveInParkingLot::_getConflictInfo( DistanceUnit distRad ,ConflictInfo& conflictInfo )
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

void State_MoveInParkingLot::DoMovementsInPath( const ElapsedTime& fromTime , double dS,double dStarSpd, double dEndSpd )
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

bool State_MoveInParkingLot::IsOutRoute() const
{
	if(m_dLastDistInRoute>m_path.getAllLength()-m_pOwner->GetLength())
	{ return true; }
	return false;
}

State_MoveInParkingLot::State_MoveInParkingLot( LandsideVehicleInSim* pV ) 
:State_LandsideVehicle<LandsideVehicleInSim>(pV)
{
	m_dLastDistInRoute = 0;
}

void State_MoveInParkingLot::Execute( CARCportEngine* pEngine )
{
	LandsideVehicleInSim* pVehicle = getVehicle();

	if(IsOutRoute())
	{		
		return OnMoveOutRoute(pEngine);
	}

	if( MoveInPath(pEngine) )
	{
		pVehicle->Continue();
		return ;		
	}
	pVehicle->StepTime(pEngine);
}

//////////////////////////////////////////////////////////////////////////
State_NonPaxVParkingInLotSpot::State_NonPaxVParkingInLotSpot( LandsideNonPaxVehicleInSim* pV,LandsideParkingSpotInSim* spot )
:State_LandsideVehicle<LandsideNonPaxVehicleInSim>(pV)
,m_spot(spot)
{
	
}

void State_NonPaxVParkingInLotSpot::Entry( CARCportEngine* pEngine )
{
	LandsideParkingLotInSim* plot = m_spot->getParkingLot();  
	ElapsedTime m_tExittime = curTime() + m_pOwner->GetParkingLotWaitTime(pEngine,plot->getName());
	m_sExitTimer.StartTimer(this,m_tExittime);
}

void State_NonPaxVParkingInLotSpot::Execute( CARCportEngine* pEngine )
{
	if(m_sExitTimer.isEnded())
	{
		return m_pOwner->SuccessProcessAtLayoutObject(m_spot->getParkingLot(),m_spot,pEngine);
	}
	ASSERT(FALSE);
}

//////////////////////////////////////////////////////////////////////////
void State_MoveAndParkingToSpot::Entry( CARCportEngine* pEngine )
{
	LandsideParkingLotInSim* pLot = m_pSpot->getParkingLot();
	if( !pLot->getDriveGraph()->FindPath(m_pOwner->getLastState().getLandsideRes(), m_pSpot, m_path) )
	{
		CString sError;
		sError.Format(_T("Can not Find Drive Path to %s"),m_pSpot->print().GetString());
		m_pOwner->ShowError(sError,"Simulation Error");
	}
	Execute(pEngine);
}


void State_MoveAndParkingToSpot::ParkingToSpot()
{
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
}

State_MoveAndParkingToSpot::State_MoveAndParkingToSpot( LandsideVehicleInSim* pV, LandsideParkingSpotInSim* pSpot )
:State_MoveInParkingLot(pV)
,m_pSpot(pSpot)
{

}

void State_MoveAndParkingToSpot::OnMoveOutRoute( CARCportEngine* _pEngine )
{
	ParkingToSpot();
	m_pOwner->SuccessParkInLotSpot(m_pSpot);
}

void State_LeaveParkingLot::Entry( CARCportEngine* pEngine )
{	
	m_pExitDoor = m_pLot->getExitDoor(m_pOwner);
	if(m_pExitDoor)
	{
		LandsideResourceInSim* pCurRes = m_pOwner->getLastState().getLandsideRes();
		if(!m_pLot->getDriveGraph()->FindPath(pCurRes,m_pExitDoor,m_path))
		{
			CString strWarning;
			strWarning.Format(_T("Can not Find Path from %s to %s"), pCurRes->print().GetString(), m_pExitDoor->print().GetString());
			getVehicle()->ShowError(strWarning,"Simulation Error");
		}
		Execute(pEngine);
	}
	else
	{
		//error terminate
		CString sError;
		sError.Format(_T("Can not Find Exit Door of %s"),  m_pLot->getName().GetIDString().GetString() );
		m_pOwner->ShowError(sError,"Definition Error");
		m_pOwner->Terminate(curTime());
	}
}


void State_LeaveParkingLot::OnMoveOutRoute( CARCportEngine* pEngine )
{
	TransitToState(new State_ExitLotDoor(m_pOwner,m_pExitDoor),pEngine);
}

