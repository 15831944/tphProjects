#include "StdAfx.h"
#include ".\airsidevehiclestate.h"

CAirsideVehicleState::CAirsideVehicleState(void)
{
	m_tCurrentTime = ElapsedTime(0L);
	m_vSpeed = 0.0;    //cm/s
	m_vehicleMode = OnVehicleBirth;
	m_pResource = NULL;
	m_dist = 0.0;  
	tMinTurnAroundTime = ElapsedTime(0L);

	m_vStuffPercent = 0.0;
	m_pServiceFlight = NULL;
	m_tLeavePoolTime = ElapsedTime(0L);
	m_bAvailable = false;
	m_nServicePointCount = 0;
	m_pServiceTimeDistribution = NULL;
	m_pCurrentPointIndex = 0;
}

CAirsideVehicleState::~CAirsideVehicleState(void)
{
}
