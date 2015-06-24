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

State_ParkingToCurbside::State_ParkingToCurbside( LandsideVehicleInSim* pV, LaneParkingSpot* pSpot,LandsideCurbSideInSim*pCurb )
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
	if(LaneParkingSpot* spot =  m_pCurb->getParkingSpot().FindParkingPos(getVehicle(),path) )
	{
		spot->SetPreOccupy(m_pOwner);
		State_ParkingToCurbside* pNextState = new State_ParkingToCurbside(getVehicle(),spot,m_pCurb);
		pNextState->SetPath(path);
		TransitToState(pNextState,pEngine);
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
void State_LeaveCurbside::Execute( CARCportEngine* pEngine )
{
	if(m_bOutPath)
	{	
		m_pCurb->getParkingSpot().ReleaseParkingPos(getVehicle(), curTime());
		m_pCurb->NotifyObservers();//notify waiting vehicles
		getVehicle()->ChangeStateMoveToDest(pEngine);
	}
	else
	{
		LandsideLaneNodeList mPath;
		if( m_pCurb->getParkingSpot().FindLeavePath(m_pOwner,m_spot,mPath))//find the path
		{		
			ASSERT(!mPath.empty());
			{
				LandsideLaneNodeInSim* pNode = mPath.back();
				LandsideLaneInSim* plane =  pNode->mpLane;
				DistanceUnit distF = pNode->m_distInlane - getVehicle()->GetHalfLength();
				DistanceUnit distT = pNode->m_distInlane + getVehicle()->GetHalfLength();

				double dSpeed = getVehicle()->getSpeed(plane, pNode->m_distInlane);

				if(plane->isSpaceEmpty(getVehicle(), distF,distT))
				{
					MobileState lastState = m_pOwner->getLastState();
					lastState.pos = pNode->m_pos;
					lastState.pRes = plane;
					lastState.distInRes = pNode->m_distInlane-1;
					lastState.dSpeed = dSpeed*0.5;
					m_pOwner->MoveToPos(lastState);	

					lastState.distInRes = pNode->m_distInlane;
					m_pOwner->MoveToPos(lastState);

					m_bOutPath = true;
					return getVehicle()->Continue();
				}							
			}
		}		
		getVehicle()->StepTime(pEngine);
	}
}

State_LeaveCurbside::State_LeaveCurbside( LandsideVehicleInSim* pV, LandsideCurbSideInSim* pCurb ,LaneParkingSpot* spot) 
:State_LandsideVehicle<LandsideVehicleInSim>(pV)
{
	m_pCurb = pCurb;
	m_bOutPath = false;
	m_spot = spot;
}


//////////////////////////////////////////////////////////////////////////
void State_DropPaxAtCurbside::Execute( CARCportEngine* pEngine )
{
	if(!m_pOwner->HavePaxGetOff(m_pCurb))
	{
		getVehicle()->SuccessProcessAtLayoutObject(m_pCurb,m_spot,pEngine);
	}
}

State_DropPaxAtCurbside::State_DropPaxAtCurbside( LandsidePaxVehicleInSim* pV,LandsideCurbSideInSim*pCub,LaneParkingSpot* spot ) 
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
