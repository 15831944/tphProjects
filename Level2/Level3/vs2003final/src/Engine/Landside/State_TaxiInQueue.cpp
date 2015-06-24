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
	LandsideVehicleInSim* pVehBefore = m_pTaxiPool->GetVehicleBefore(m_pOwner);
	double dMaxSpeed = getVehicle()->getSpeed(m_pTaxiPool);
	double dCurSpeed = getVehicle()->getLastState().dSpeed;
	ElapsedTime curT = curTime();

	double dNextSpd = 0;
	DistanceUnit nextDist = m_path.getAllLength() - getVehicle()->GetLength()*0.5; 
	MobileGroundPerform perform = getVehicle()->mPerform;
	perform.mNoramlSpd = dMaxSpeed;

	if(pVehBefore)
	{
		nextDist = pVehBefore->getLastState().distInRes - getVehicle()->GetLength()*0.5- pVehBefore->GetLength()*0.5 - m_pTaxiPool->GetVehicleSep();
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
	if(m_bMoveOut)
	{	
		m_pTaxiQ->getParkingSpot().ReleaseParkingPos(getVehicle(), curTime());	
		m_pTaxiQ->NotifyObservers();
		getVehicle()->ChangeStateMoveToDest(pEngine);
	}
	else
	{
		LandsideLaneNodeList mPath;
		if( m_pTaxiQ->getParkingSpot().FindLeavePath(getVehicle(),m_spot,mPath))//find the path
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
					MobileState lastState = getVehicle()->getLastState();
					lastState.pos = pNode->m_pos;
					lastState.pRes = plane;
					lastState.distInRes = pNode->m_distInlane-1;
					lastState.dSpeed = dSpeed*0.5;
					getVehicle()->MoveToPos(lastState);	

					lastState.distInRes = pNode->m_distInlane;
					getVehicle()->MoveToPos(lastState);

					m_bMoveOut = true;
					return getVehicle()->Continue();
				}								
			}
		}		
		getVehicle()->StepTime(pEngine);
	}

}

State_LeaveTaxiQ::State_LeaveTaxiQ( LandsideTaxiInSim* pTaxi, LandsideTaxiQueueInSim* pQ ,LaneParkingSpot* spot)
:State_LandsideVehicle<LandsideVehicleInSim>(pTaxi)
{
	m_pTaxiQ = pQ; 
	m_bMoveOut = false;
	m_spot = spot;
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
	m_path.addPath(m_pTaxiPool, &m_pTaxiPool->GetPath(), 0);
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