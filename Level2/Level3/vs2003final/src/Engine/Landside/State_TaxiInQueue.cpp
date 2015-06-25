#include "stdafx.h"
#include "State_TaxiInQueue.h"
#include "..\LandsideTaxiQueueInSim.h"
#include "LandsideTaxiInSim.h"
#include "..\LandsideTaxiPoolInSim.h"
#include "..\MobileDynamics.h"
#include "..\ARCportEngine.h"
#include "Common\FileOutPut.h"




void State_ProcessAtTaxiQ::Execute( CARCportEngine* pEngine )
{
	if(!m_pOwner->IsAllPassengersGetOn())
		return;
	else
	{
		m_pOwner->ChangeToArrPaxPlan(pEngine);
		m_pOwner->SuccessProcessAtLayoutObject(m_pTaxiQ,m_spot, pEngine);
		return;
	}
}

void State_ProcessAtTaxiQ::Entry( CARCportEngine* pEngine )
{
	m_pTaxiQ->AddWaitingVehicle(m_pOwner);
	m_pTaxiQ->NoticeWaitingPax(curTime(),m_pOwner, pEngine->GetLandsideSimulation());
	Execute(pEngine);
}

//////////////////////////////////////////////////////////////////////////
void State_EnterTaxiPool::Execute( CARCportEngine* pEngine )
{
	int laneNum = m_pTaxiPool->m_pLanes.size();
	LandsideVehicleInSim *pVehBefore=NULL;
	LandsideVehicleInSim *pVehNow = getVehicle();
	for (int i=0;i<laneNum;i++)
	{
		pVehBefore = m_pTaxiPool->GetVehicleBefore(pVehNow);
		if(pVehBefore == NULL)
			break;
		pVehNow = pVehBefore;
	}
	//LandsideVehicleInSim* pVehBefore = m_pTaxiPool->GetVehicleBefore(m_pOwner);
	double dMaxSpeed = getVehicle()->getSpeed(m_pTaxiPool);
	double dCurSpeed = getVehicle()->getLastState().dSpeed;
	ElapsedTime curT = curTime();

	double dNextSpd = 0;
	DistanceUnit nextDist = m_path.getAllLength() - getVehicle()->GetLength()*0.5; 
	MobileGroundPerform perform = getVehicle()->mPerform;
	perform.mNoramlSpd = dMaxSpeed;

	if(pVehBefore)
	{
		/*LandsideVehicleInSim* pVeh;
		for (int i=0;i<laneNum;i++)
		{
			pVeh = m_pTaxiPool->GetVehicleBefore(pVeh);
		}*/
		nextDist = pVehBefore->getLastState().distInRes - pVehBefore->GetLength()*0.5  - m_pTaxiPool->GetVehicleSep() - getVehicle()->GetLength()*0.5;
	}

	if(m_dCurDist < nextDist)
	{
		//move to 
		MobileTravelTrace travelTrace(perform,nextDist-m_dCurDist,dCurSpeed, dNextSpd);
		double dFromIndex = m_path.getDistIndex(m_dCurDist);
		double dToIndex = m_path.getDistIndex(nextDist);	 

		
		m_pTaxiPool->GetPath().GetDistIndex(m_dCurDist);
		for(int iIndex = (int)dFromIndex+1;iIndex < dToIndex;iIndex++)
		{
			double dDist = m_path.getIndexDist(iIndex) - m_dCurDist;
			travelTrace.addDistItem(dDist);
		}
		travelTrace.BeginUse();
		MobileState state = getVehicle()->getLastState();
		for(int i=0;i<travelTrace.getItemCount();i++)
		{
			LandsidePosition lpos = m_path.getDistPosition(m_dCurDist + travelTrace.ItemAt(i).mdist);

			state.distInRes = lpos.distInRes;
			state.dSpeed = travelTrace.ItemAt(i).mdSpd;
			state.time  = travelTrace.ItemAt(i).mtTime + curT;
			state.pRes = lpos.pRes;
			state.pos = lpos.pos;
			state.isConerPt = true;
			getVehicle()->UpdateState(state);
		}
		m_dCurDist = nextDist;
		getVehicle()->StartMove(curT);
		getVehicle()->Continue();
		return;
	}


	if(!pVehBefore)
	{
		TransitToState(new State_WaitAtTaxiPoolHead(m_pOwner,m_pTaxiPool),getVehicle()->getLastState().time );
		return; 			
	}
	else
	{
		pVehBefore->AddObserver(getVehicle());
		//getVehicle()->ListenTo(pVehBefore);
		return;
	}
}



State_EnterTaxiQ::State_EnterTaxiQ( LandsideTaxiInSim* pTaxi, LandsideTaxiQueueInSim* pQ )
: State_MoveInRoad(pTaxi)
{
	m_pTaxiQ = pQ;
	m_spot = NULL;
}

void State_EnterTaxiQ::Execute( CARCportEngine* pEngine )
{
	if(m_spot)
	{
		__super::Execute(pEngine);
	}
	else
	{
		LandsideLaneNodeList path;
		m_spot =  m_pTaxiQ->getParkingSpot().FindParkingPos(getVehicle(),path);
		if(m_spot)
		{
			m_spot->SetPreOccupy(getVehicle());
			SetPath(path);			
			Execute(pEngine);
		}
		else 
		{
			m_pTaxiQ->AddObserver(getVehicle());
		}
	}
}

void State_EnterTaxiQ::OnMoveOutRoute( CARCportEngine* _pEngine )
{
	getVehicle()->DoParkingToSpot(m_spot);
	getVehicle()->ChangeVehicleState(new State_ProcessAtTaxiQ(getVehicle()->toTaxi(),m_pTaxiQ,m_spot));
}
//////////////////////////////////////////////////////////////////////////
void State_LeaveTaxiQ::Entry( CARCportEngine* pEngine )
{
	m_pTaxiQ->removeServiceVehicle(m_pOwner);
	m_pTaxiQ->DelWaitingVehicle(m_pOwner);
	Execute(pEngine);
}

void State_LeaveTaxiQ::Execute( CARCportEngine* pEngine )
{
	if(m_bOutPath)
	{	
		return getVehicle()->ChangeStateMoveToDest(pEngine);
	}
	//finding leave position 
	LandsidePosition exitPos;
	if(m_pTaxiQ->getParkingSpot().FindLeavePosition(getVehicle(), m_spot, exitPos) )
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

State_LeaveTaxiQ::State_LeaveTaxiQ( LandsideTaxiInSim* pTaxi, LandsideTaxiQueueInSim* pQ ,IParkingSpotInSim* spot)
:State_LandsideVehicle<LandsideVehicleInSim>(pTaxi)
{
	m_pTaxiQ = pQ; 
	m_bOutPath = false;
	m_spot = spot;
}

void State_LeaveTaxiQ::Exit( CARCportEngine* pEngine )
{
	if(m_spot)
	{
		m_spot->OnVehicleExit(getVehicle(), curTime());
	}
	m_pTaxiQ->NotifyObservers();//notify waiting vehicles
}




//////////////////////////////////////////////////////////////////////////
State_EnterTaxiPool::State_EnterTaxiPool( LandsideTaxiInSim* pTaxi, LandsideTaxiPoolInSim* pPool )
: State_LandsideVehicle<LandsideTaxiInSim>(pTaxi)
{
	m_pTaxiPool = pPool; 
	m_dCurDist = 0;
}


void State_EnterTaxiPool::Entry( CARCportEngine* pEngine )
{
	//ASSERT(!m_pOwner->getSerivceQueue());
	m_path.Clear();
	int iLaneNum = m_pTaxiPool->m_pLanes.size();
	int curLaneIdx;
	curLaneIdx = m_pTaxiPool->InResVehicle().size() % iLaneNum;
	m_path.addPath(m_pTaxiPool,m_pTaxiPool->m_pLanes.at(curLaneIdx),0);
	Execute(pEngine);
	m_pOwner->ChangeToPoolPlan(m_pTaxiPool,pEngine);
	m_pTaxiPool->NotifyVehicleArrival(m_pOwner);
}


void State_WaitAtTaxiPoolHead::Execute( CARCportEngine* pEngine )
{
	if(m_pOwner->getSerivceQueue())
	{
		m_pOwner->SuccessProcessAtLayoutObject(m_pTaxiPool,m_pTaxiPool,pEngine);
	}
}