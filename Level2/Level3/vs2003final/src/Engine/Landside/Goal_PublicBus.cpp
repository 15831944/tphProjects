#include "StdAfx.h"
#include ".\goal_publicbus.h"

#include "Engine\LandsideBusStationInSim.h"
#include "Landside/LandsideBusStation.h"
#include "Goal_MoveInRoad.h"
#include "Landside/CLandSidePublicVehicleType.h"
#include "Engine\ARCportEngine.h"
#include "Engine/LandsideResourceManager.h"
#include "Engine/LandsideSimulation.h"

void Goal_ParkingBusStation::Activate(CARCportEngine* pEngine)
{
	RemoveAllSubgoals();

	mPath.DeleteClear();
	if(mpSpot = m_pBusStation->getParkingSpot().FindParkingPos(m_pOwner,mPath))
	{		
		m_iStatus = gs_active;
		mpSpot->SetPreOccupy(m_pOwner);
		AddSubgoalFront(new Goal_MoveInRoad(m_pOwner,m_pBusStation,mPath,true,false));
		m_pBusStation->RemoveListener(m_pOwner);	

		if (m_pOwner->WaitInResourceQueue(m_pBusStation))
		{
			LandsideResourceQueueItem* pQueue = new LandsideResourceQueueItem;
			LandsideResourceInSim* pRes = m_pBusStation;
			pQueue->m_nVehicleID = m_pOwner->getID();
			pQueue->m_strVehicleType = m_pOwner->getName().toString();
			pQueue->m_nResourceID = pRes->getLayoutObject()->getInput()->getID();
			pQueue->m_strResName = pRes->getLayoutObject()->getInput()->getName().GetIDString();
			pQueue->m_eTime = m_pOwner->curTime();
			pQueue->m_enumOperation = LandsideResourceQueueItem::QO_Exit;

			m_pOwner->LeaveResourceQueue(m_pBusStation->getLayoutObject());
		
			pEngine->GetLandsideSimulation()->getOutput()->m_SimLogs.getResourceQueueLog().AddItem(pQueue);
		}
	}
	else
	{
		//ASSERT(FALSE);
		//check vehicle can go to curbside and set resource queue
		if (m_pOwner->WaitInResourceQueue(m_pBusStation) == false)
		{
			LandsideResourceQueueItem* pQueue = new LandsideResourceQueueItem;
			LandsideResourceInSim* pRes = m_pBusStation;
			pQueue->m_nVehicleID = m_pOwner->getID();
			pQueue->m_strVehicleType = m_pOwner->getName().toString();
			pQueue->m_nResourceID = pRes->getLayoutObject()->getInput()->getID();
			pQueue->m_strResName = pRes->getLayoutObject()->getInput()->getName().GetIDString();
			pQueue->m_eTime = m_pOwner->curTime();
			pQueue->m_enumOperation = LandsideResourceQueueItem::QO_Entry;

			m_pOwner->EnterResourceQueue(m_pBusStation->getLayoutObject());
			pEngine->GetLandsideSimulation()->getOutput()->m_SimLogs.getResourceQueueLog().AddItem(pQueue);
		}
	}
}

void Goal_ParkingBusStation::PostProcessSubgoals(CARCportEngine* pEngine)
{
	if(mpSpot)
	{
		MobileState lastmvPt = m_pOwner->getLastState();
		MobileState nextState = lastmvPt;


		nextState.pRes = mpSpot->GetLane();
		nextState.distInRes = mpSpot->GetDistInLane();
		nextState.dSpeed = 0;
		nextState.pos = mpSpot->GetParkingPos();	

		nextState.time = lastmvPt.time + lastmvPt.moveTime(nextState);

		m_pOwner->UpdateState(nextState);		
		nextState.pos += mpSpot->GetParkingPosDir().Normalize();
		m_pOwner->UpdateState(nextState);

	}
}

void Goal_WaitForSometime::Process(CARCportEngine* pEngine)
{
	ActivateIfInactive(pEngine);
	MobileState state = m_pOwner->getLastState();
	state.time = m_pOwner->curTime()+ m_waitT;
	m_pOwner->UpdateState(state); 
	m_iStatus = gs_completed;
	//return m_iStatus;

}


Goal_LeaveBusStation::Goal_LeaveBusStation( LandsideVehicleInSim* pEnt,LandsideBusStationInSim* pStation ) 
: LandsideVehicleSequenceGoal(pEnt)
{
	m_pStation=pStation;
	m_sDesc = "Goal_LeaveBusStation";
}
Goal_LeaveBusStation::~Goal_LeaveBusStation(){ mPath.DeleteClear(); }


//void Goal_LeaveBusStation::doProcess( CARCportEngine* pEngine )
//{
//	if(m_pStation->getParkingSpot().FindLeavePath(m_pOwner,mPath))
//	{		
//		//write log to leave the bus station
//		//
//		LandsideLaneNodeInSim* pNode = mPath.back();
//		MobileState state;
//		state.pos = pNode->GetPosition();
//		state.distInRes = pNode->m_distInlane;
//		state.mAtRes = pNode->mpLane;
//		state.dir  =  pNode->GetDir();
//		state.dSpeed = m_pOwner->getSpeed(pNode->mpLane);
//		m_pOwner->MoveToPos(state);
//		
//		m_pStation->DelWaitingBus(m_pOwner);
//
//
//		//
//		m_pStation->getParkingSpot().ReleaseParkingPos(m_pOwner, m_pOwner->curTime());
//		setComplete();
//	}
//	
//}

void Goal_LeaveBusStation::Activate( CARCportEngine* pEngine )
{
	RemoveAllSubgoals();

	mPath.DeleteClear();	
	if(m_pStation->getParkingSpot().FindLeavePath(m_pOwner,mPath))
	{		
		m_iStatus = gs_active;		
		AddSubgoalFront(new Goal_MoveInRoad(m_pOwner,NULL,mPath,false,false));
		m_pStation->RemoveListener(m_pOwner);
	}
	else
	{
		m_pStation->AddListener(m_pOwner);
	}
}

void Goal_LeaveBusStation::PostProcessSubgoals( CARCportEngine* pEngine )
{
	m_pStation->getParkingSpot().ReleaseParkingPos(m_pOwner, m_pOwner->curTime());	
}

