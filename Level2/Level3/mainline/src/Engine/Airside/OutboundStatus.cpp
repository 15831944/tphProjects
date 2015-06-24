#include "StdAfx.h"
#include ".\outboundstatus.h"
#include "CommonInSim.h"
#include "AirportStatusManager.h"
#include "AirsideResource.h"
#include "Clearance.h"
#include "AirsideObserver.h"
OutboundStatus::OutboundStatus(AirportStatusManager* pStatusManager)
:m_pStatusManager(pStatusManager)
{
	m_pStatusManager->RegisterAirportStatus(this);
}

OutboundStatus::~OutboundStatus(void)
{
	m_pStatusManager = NULL;
}

void OutboundStatus::UpdateStatus(AirsideResource* pOriResource,const ClearanceItem& item)
{
	if (m_vObserverList.empty())
		return;

	AirsideMobileElementMode nextMode = item.GetMode();

	if (nextMode == OnTaxiToRunway || nextMode == OnPreTakeoff)	
	{
		ElapsedTime tNextTime = item.GetTime();
		AirsideResource* pDestResource = item.GetResource();

		if ( (( pOriResource->GetType() == AirsideResource::ResType_Stand ||pOriResource->GetType() == AirsideResource::ResType_StandLeadOutLine)&& nextMode == OnTaxiToRunway)||
			(pOriResource->GetType() == AirsideResource::ResType_TaxiwayDirSeg && nextMode == OnPreTakeoff) )
		{
			NotifyObservers(tNextTime);
		}

	}



}