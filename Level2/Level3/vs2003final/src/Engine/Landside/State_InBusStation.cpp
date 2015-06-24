#include "stdafx.h"
#include "State_InBusStation.h"
#include "..\LaneParkingSpot.h"
#include "..\LandsideBusStationInSim.h"


void State_ParkingBusStation::OnMoveOutRoute( CARCportEngine* _pEngine )
{
	getVehicle()->DoParkingToSpot(m_spot);	//
	getVehicle()->SuccessParkInBusStation(m_pBusStation,m_spot);
}

void State_ParkingBusStation::Execute( CARCportEngine* pEngine )
{
	if(m_spot)
	{
		__super::Execute(pEngine);
	}
	else
	{
		LandsideLaneNodeList path;
		m_spot =  m_pBusStation->getParkingSpot().FindParkingPos(getVehicle(),path);
		if(m_spot)
		{
			m_spot->SetPreOccupy(getVehicle());
			SetPath(path);			
			Execute(pEngine);
		}
		else
		{
			m_pBusStation->AddObserver(getVehicle());
			//getVehicle()->ListenTo(m_pBusStation);
			return;
		}
	}
}

State_ParkingBusStation::State_ParkingBusStation( LandsideVehicleInSim* pV, LandsideBusStationInSim* pBusStation )
:State_MoveInRoad(pV)
{
	m_pBusStation = pBusStation;  
	m_spot = NULL;
}

//////////////////////////////////////////////////////////////////////////
void State_LeaveBusStation::Execute( CARCportEngine* pEngine )
{
	if(m_bFindLeavePath)
	{
		OnMoveOutRoute(pEngine);
	}
	else
	{
		LandsideLaneNodeList mPath;
		if(m_bFindLeavePath = m_pBusStation->getParkingSpot().FindLeavePath(getVehicle(),m_spot,mPath))//find the path
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

					return getVehicle()->Continue();
				}								
			}
		}		
		getVehicle()->StepTime(pEngine);
	}
}

void State_LeaveBusStation::OnMoveOutRoute( CARCportEngine* _pEngine )
{
	m_pBusStation->getParkingSpot().ReleaseParkingPos(getVehicle(), curTime());	
	m_pBusStation->NotifyObservers();//SendSignal(new SNormalSignal());
	getVehicle()->ChangeStateMoveToDest(_pEngine);
}

void State_LeaveBusStation::Entry( CARCportEngine* pEngine )
{
	m_pBusStation->DelWaitingBus(m_pOwner);
	Execute(pEngine);
}
