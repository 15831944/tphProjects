#include "StdAfx.h"
#include "VehicleMoveEvent.h"
#include "AirsideVehicleInSim.h"


VehicleMoveEvent::VehicleMoveEvent(AirsideVehicleInSim * pVehicle)
:CAirsideMobileElementEvent(pVehicle)
{
	m_pVehicle = pVehicle; 
}
int VehicleMoveEvent::Process()
{
	ASSERT(m_pVehicle);
	
	//BEGIN_LOGTIME("VehicleMoveEvent::Process")

	//long lFindStartTime = GetTickCount();
	m_pVehicle->MoveOnRoute(time);
	//long lFindEndTime = GetTickCount();
	//static long totalMoveTime = 0L;
	//totalMoveTime += lFindEndTime - lFindStartTime;
	//std::ofstream echoFile ("c:\\findveh.log", stdios::app);
	//echoFile<<"VehicleMoveEvent::Process()          "<<lFindEndTime - lFindStartTime<<"          "
	//	<<totalMoveTime
	//	<<"\r\n";
	//echoFile.close();
	//END_LOGTIME

	return TRUE;
}

int VehicleServiceEvent::Process()
{
	if(getVehicle())
		getVehicle()->ServiceFlight(time);
	return TRUE;
}

VehicleServiceEvent::VehicleServiceEvent( AirsideVehicleInSim* pVehicle ) : CAirsideMobileElementEvent(pVehicle)
{

}
