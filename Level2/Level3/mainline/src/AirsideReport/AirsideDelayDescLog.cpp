#include "StdAfx.h"
#include ".\airsidedelaydesclog.h"
#include "../Engine/Airside/AirsideFlightDelay.h"


CAirsideDelayDescLog::CAirsideDelayDescLog(void)
{
}

CAirsideDelayDescLog::~CAirsideDelayDescLog(void)
{
}

void CAirsideDelayDescLog::LoadData( const CString& strFilePath )
{

	m_delayLogs.LoadData(strFilePath);

	int nCount = m_delayLogs.GetItemCount();
	
	for (int i =0; i < nCount;++ i)
	{
		AirsideFlightDelay *pDelay = m_delayLogs.GetItem(i);
		AddFlightDelay( pDelay);
	}
}
void CAirsideDelayDescLog::AddFlightDelay(AirsideFlightDelay *pDelay)
{

	if (pDelay != NULL)
	{
		long lUniqueID =pDelay->GetUniqueID();
		m_mapLogDesc[lUniqueID]  = pDelay;
	}

}

void CAirsideDelayDescLog::Sort()
{

}

AirsideFlightDelay* CAirsideDelayDescLog::GetDelayDesc( int nUniqueID )
{
	AirsideFlightDelay* pRetDelay = NULL;

	std::map<int,AirsideFlightDelay *>::iterator iter = m_mapLogDesc.find(nUniqueID);
	if (iter != m_mapLogDesc.end())
	{
		pRetDelay = m_mapLogDesc[nUniqueID];
	}

	return pRetDelay;
}