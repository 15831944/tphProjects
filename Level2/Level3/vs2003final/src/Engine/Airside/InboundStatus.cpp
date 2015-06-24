#include "StdAfx.h"
#include ".\inboundstatus.h"
#include "CommonInSim.h"
#include "AirportStatusManager.h"
#include "AirsideResource.h"
#include "Clearance.h"
#include "AirsideObserver.h"

InboundStatus::InboundStatus(AirportStatusManager* pStatusManager)
:m_pStatusManager(pStatusManager)
{
	m_pStatusManager->RegisterAirportStatus(this);
}

InboundStatus::~InboundStatus(void)
{
	m_pStatusManager = NULL;
}

void InboundStatus::UpdateStatus(AirsideResource* pOriResource,const ClearanceItem& item)
{
	if (m_vObserverList.empty())
		return;

	AirsideMobileElementMode nextMode = item.GetMode();

	if (nextMode == OnTaxiToStand || nextMode == OnHeldAtStand || nextMode == OnEnterTempStand ||OnEnterStand == nextMode || OnExitStand == nextMode )	
	{
		ElapsedTime tNextTime = item.GetTime();
		//if ( ( (pOriResource->GetType() == AirsideResource::ResType_RunwayPort || pOriResource->GetType() == AirsideResource::ResType_Stand) && nextMode == OnTaxiToStand )
		//	||((pOriResource->GetType() == AirsideResource::ResType_TaxiwayDirSeg ||  AirsideResource::ResType_StandLeadInLine == pOriResource->GetType()) && ( nextMode == OnHeldAtStand|| nextMode == OnEnterTempStand )) )
		//{
			//int nCount = m_vObserverList.size();
			//for(std::set<CAirsideObserver*>::iterator itr= m_vObserverList.begin();itr!= m_vObserverList.end();itr++)
			//{
			//	(*itr)->Update(tNextTime);
			//}
			NotifyObservers(tNextTime);
		//}
	}



}