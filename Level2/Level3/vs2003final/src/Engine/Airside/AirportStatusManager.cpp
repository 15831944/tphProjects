#include "StdAfx.h"
#include ".\airportstatusmanager.h"
#include "AirportStatus.h"
#include "AirsideResource.h"
#include "Clearance.h"

AirportStatusManager::AirportStatusManager(void)
{
	m_vStatuslist.clear();
}

AirportStatusManager::~AirportStatusManager(void)
{
	m_vStatuslist.clear();
}

void AirportStatusManager::RegisterAirportStatus(AirportStatus* pStatus)
{
	m_vStatuslist.insert(pStatus);
}

void AirportStatusManager::RemoveAirportStatus(AirportStatus* pStatus)
{
	m_vStatuslist.erase(pStatus);
}

void AirportStatusManager::NotifyAirportStatuses(AirsideResource* pOriResource,const ClearanceItem& item)
{
	int nCount = m_vStatuslist.size();
	for(std::set<AirportStatus*>::iterator itr= m_vStatuslist.begin();itr!= m_vStatuslist.end();itr++)
	{
		(*itr)->UpdateStatus(pOriResource,item);
	}
}