#include "StdAfx.h"
#include "State_InCurbside.h"
#include "..\LandsideCurbsideInSim.h"
#include "..\LandsideStrategyStateInSim.h"
#include "LandsidePaxVehicleInSim.h"
#include "Common\FileOutPut.h"

void State_ParkingToCurbside::OnMoveOutRoute( CARCportEngine* _pEngine )
{
	getVehicle()->DoParkingToSpot(m_spot);
	getVehicle()->SuccessParkInCurb(m_pCurb,m_spot);
}

State_ParkingToCurbside::State_ParkingToCurbside( LandsideVehicleInSim* pV, IParkingSpotInSim* pSpot,LandsideCurbSideInSim*pCurb )
:State_MoveInRoad(pV)
{
	m_pDestResource = pSpot;
	m_pCurb = pCurb;
	m_spot = pSpot;
}
//////////////////////////////////////////////////////////////////////////
void State_TryParkingCurbside::Entry( CARCportEngine* pEngine )
{
	CurbsideStrategyStateInSim* pStragy = getVehicle()->getCurbStragy();
	if(!pStragy)
	{
		pStragy = getVehicle()->BeginCurbStrategy(m_pCurb->getName(),pEngine);
	}
	ElapsedTime maxWaitSpotTime(0L);
	if(pStragy)
	{
		maxWaitSpotTime = pStragy->getMaxWaitSpotTime();
	}

	m_waitTimer.StartTimer(this, curTime()+ maxWaitSpotTime );
	Execute(pEngine);
}

void State_TryParkingCurbside::Execute( CARCportEngine * pEngine )
{
	LandsideLaneNodeList path;
	if(IParkingSpotInSim* spot =  m_pCurb->getParkingSpot().FindParkingPos(getVehicle(),path) )
	{
		spot->SetPreOccupy(m_pOwner);
		State_ParkingToCurbside* pNextState = new State_ParkingToCurbside(getVehicle(),spot,m_pCurb);
		pNextState->SetPath(path);
		getVehicle()->ChangeVehicleState(pNextState);
		return;
	}

	if( m_waitTimer.isEnded() )		//fail parking to curb
	{
		getVehicle()->FailedParkInCurb(m_pCurb,pEngine);
		return;
	}
	else
	{
		m_pCurb->AddObserver(getVehicle());
		///m_pOwner->ListenTo(m_pCurb);
	}
}
State_TryParkingCurbside::State_TryParkingCurbside( LandsideVehicleInSim* pV, LandsideCurbSideInSim* pCurb ) 
:State_LandsideVehicle<LandsideVehicleInSim>(pV)
{
	m_pCurb = pCurb ; 
}

//////////////////////////////////////////////////////////////////////////
State_LeaveCurbside::State_LeaveCurbside( LandsideVehicleInSim* pV, LandsideCurbSideInSim* pCurb ,IParkingSpotInSim* spot) 
:State_LandsideVehicle<LandsideVehicleInSim>(pV)
{
	m_pCurb = pCurb;
	m_bOutPath = false;
	m_spot = spot;
}

void State_LeaveCurbside::Exit(CARCportEngine* pEngine)
{
	if(m_spot)
	{
		m_spot->OnVehicleExit(getVehicle(), curTime());
	}
	
	m_pCurb->NotifyObservers();//notify waiting vehicles
}

void State_LeaveCurbside::Execute( CARCportEngine* pEngine )
{
	if(m_bOutPath)
	{	
		return getVehicle()->ChangeStateMoveToDest(pEngine);
	}
	//finding leave position 
	LandsidePosition exitPos;
	if(m_pCurb->getParkingSpot().FindLeavePosition(getVehicle(), m_spot, exitPos) )
	{
		LandsideLaneInSim* plane = exitPos.pRes->toLane();
		DistanceUnit distF = exitPos.distInRes - getVehicle()->GetHalfLength();
		DistanceUnit distT = exitPos.distInRes + getVehicle()->GetHalfLength();

		if(plane->isSpaceEmpty(getVehicle(), distF,distT))
		{
			double dSpeed = getVehicle()->getSpeed(plane, exitPos.distInRes);
			MobileState lastState = m_pOwner->getLastState();
			lastState.pos = exitPos.pos;
			lastState.pRes = plane;
			lastState.distInRes = exitPos.distInRes-1;
			lastState.dSpeed = dSpeed*0.5;
			m_pOwner->MoveToPos(lastState);	

			lastState.distInRes = exitPos.distInRes;
			m_pOwner->MoveToPos(lastState);

			m_bOutPath = true;
			return getVehicle()->Continue();
		}							
	}	
	return getVehicle()->StepTime(pEngine);
}



//////////////////////////////////////////////////////////////////////////
void State_DropPaxAtCurbside::Execute( CARCportEngine* pEngine )
{
	if(!m_pOwner->HavePaxGetOff(m_pCurb))
	{
		getVehicle()->SuccessProcessAtLayoutObject(m_pCurb,m_spot,pEngine);
	}
}

State_DropPaxAtCurbside::State_DropPaxAtCurbside( LandsidePaxVehicleInSim* pV,LandsideCurbSideInSim*pCub,IParkingSpotInSim* spot ) 
:State_LandsideVehicle<LandsidePaxVehicleInSim>(pV)
{
	m_pCurb=pCub;
	m_spot = spot;
}

void State_DropPaxAtCurbside::Entry( CARCportEngine* pEngine )
{
	m_pOwner->CallPaxGetOff(m_pCurb);
	Execute(pEngine);
}
