#include "StdAfx.h"
#include ".\RunwayOccupancyStatus.h"
#include "CommonInSim.h"
#include "AirportStatusManager.h"
#include "AirsideResource.h"
#include "Clearance.h"
#include "../EngineDll.h"

RunwayOccupancyStatus::RunwayOccupancyStatus( AirportStatusManager* pStatusManager )
:m_pStatusManager(pStatusManager)
{
	m_pStatusManager->RegisterAirportStatus(this);
}

RunwayOccupancyStatus::~RunwayOccupancyStatus(void)
{
	m_pStatusManager = NULL;
}

void RunwayOccupancyStatus::UpdateStatus( AirsideResource* pOriResource,const ClearanceItem& item )
{
	if (m_vObserverList.empty()|| pOriResource->GetType() != AirsideResource::ResType_LogicRunway)
		return;

	AirsideMobileElementMode nextMode = item.GetMode();
	if (nextMode == OnTaxiToStand|| nextMode == OnTaxiToTempParking || nextMode == OnTakeoff || nextMode == OnTowToDestination || nextMode == OnTaxiToMeetingPoint)
	{
		AirsideResource* pDestResource = item.GetResource();
		ElapsedTime tNextTime = item.GetTime();

		NotifyObservers(tNextTime);		
	}

}