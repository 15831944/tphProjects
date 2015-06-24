#include "StdAfx.h"
#include ".\takeoffqueuestatus.h"
#include "CommonInSim.h"
#include "AirportStatusManager.h"
#include "AirsideResource.h"
#include "Clearance.h"
#include "../EngineDll.h"

TakeoffQueueStatus::TakeoffQueueStatus( AirportStatusManager* pStatusManager )
:m_pStatusManager(pStatusManager)
{
	m_pStatusManager->RegisterAirportStatus(this);
}

TakeoffQueueStatus::~TakeoffQueueStatus(void)
{
	m_pStatusManager = NULL;
}

void TakeoffQueueStatus::UpdateStatus( AirsideResource* pOriResource,const ClearanceItem& item )
{
	if (m_vObserverList.empty())
		return;

	AirsideMobileElementMode nextMode = item.GetMode();
	if (nextMode == OnQueueToRunway || nextMode == OnPreTakeoff)
	{
		AirsideResource* pDestResource = item.GetResource();
		ElapsedTime tNextTime = item.GetTime();
		if (pOriResource->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
		{
			//int nCount = m_vObserverList.size();
			//for(std::set<CAirsideObserver*>::iterator itr= m_vObserverList.begin();itr!= m_vObserverList.end();itr++)
			//{
			//	(*itr)->Update(tnextTime);

			//}
			NotifyObservers(tNextTime);
		}
	}

}