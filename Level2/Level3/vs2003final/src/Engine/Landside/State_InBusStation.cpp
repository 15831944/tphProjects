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
	if(m_bOutPath)
	{	
		return getVehicle()->ChangeStateMoveToDest(pEngine);
	}
	//finding leave position 
	LandsidePosition exitPos;
	if(m_pBusStation->getParkingSpot().FindLeavePosition(getVehicle(), m_spot, exitPos) )
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

void State_LeaveBusStation::Exit( CARCportEngine* _pEngine )
{
	/*if(m_spot)
	{
		m_spot->OnVehicleExit(getVehicle(), curTime());
	}*/
	m_pBusStation->getParkingSpot().getInStretchSpots().ReleaseParkingPos(getVehicle(),curTime());
	m_pBusStation->NotifyObservers();//notify waiting vehicles
}

void State_LeaveBusStation::Entry( CARCportEngine* pEngine )
{
	m_pBusStation->DelWaitingBus(m_pOwner);
	Execute(pEngine);
}

State_LeaveBusStation::State_LeaveBusStation( LandsideVehicleInSim* pV, LandsideBusStationInSim* pBusStation,IParkingSpotInSim* spot )
:State_LandsideVehicle<LandsideVehicleInSim>(pV)
{
	m_pBusStation = pBusStation;  
	m_bOutPath = false;
	m_spot = spot;
}
