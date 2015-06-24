#include "StdAfx.h"
#include ".\vehiclebirthevent.h"
#include "AirsideVehicleInSim.h"


VehicleBirthEvent::VehicleBirthEvent(AirsideVehicleInSim* vehicle)
:CAirsideMobileElementEvent(vehicle)
{
	m_pVehicle = vehicle;
	setTime(m_pVehicle->GetBirthTime());
}

VehicleBirthEvent::~VehicleBirthEvent(void)
{
}

int VehicleBirthEvent::Process()
{
	
	m_pVehicle->SetTime(time);
	m_pVehicle->WakeUp(time);
	return TRUE;

}
