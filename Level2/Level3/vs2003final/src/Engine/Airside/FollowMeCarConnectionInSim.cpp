#include "StdAfx.h"
#include ".\followmecarconnectioninsim.h"
#include "AirsideFlightInSim.h"
#include "StandInSim.h"
#include "../../Common/AirportDatabase.h"
#include "RunwayInSim.h"
#include "../../InputAirside/FollowMeConnectionData.h"
#include "../../Common/ALTObjectID.h"

FollowMeCarConnectionInSim::FollowMeCarConnectionInSim(CAirportDatabase* pAirportDB)
{
	m_pFollowMeCarConnection = new CFollowMeConnectionData(pAirportDB);
	m_pFollowMeCarConnection->ReadData();
}

FollowMeCarConnectionInSim::~FollowMeCarConnectionInSim(void)
{
	delete m_pFollowMeCarConnection;
	m_pFollowMeCarConnection = NULL;
}

CFollowMeConnectionStand* FollowMeCarConnectionInSim::GetAvailableFollowMeCarConnectionData( AirsideFlightInSim* pFlight,StandInSim* pDest)
{
	LogicRunwayInSim* pRunway = pFlight->GetLandingRunway();
	if (pRunway == NULL || pDest == NULL)
		return NULL;

	CFollowMeConnectionRunway* pRwyData = NULL;
	int nCount = m_pFollowMeCarConnection->GetData().size();
	for (int i =0; i < nCount; i++)
	{
		CFollowMeConnectionItem* pItem = m_pFollowMeCarConnection->GetData().at(i);
		if (!pFlight->fits(pItem->GetFlightConstraint()))
			continue;
		
		int nTimeCount = pItem->GetData()->size();
		for (int j = 0; j < nTimeCount; j++)
		{
			CFollowMeConnectionTimeRangeItem* pTimeItem = pItem->GetData()->at(j);
			if (pTimeItem->GetStartTime() > pFlight->GetTime() || pTimeItem->GetEndTime() < pFlight->GetTime())
				continue;

			int nRwyCount = pTimeItem->m_Data.size();
			for (int k =0; k < nRwyCount; k++)
			{
				CFollowMeConnectionRunway* pRwyItem = pTimeItem->m_Data.at(k);
				if (pRwyItem->GetRwyID() == pRunway->GetRunwayInSim()->GetRunwayID() || pRwyItem->GetRwyMark() == pRunway->getLogicRunwayType())
				{
					pRwyData = pRwyItem;
					break;
				}
			}
		}
		
	}

	if (pRwyData != NULL)
	{
		int nStandCount = pRwyData->m_Data.size();
		for (int i =0; i < nStandCount; i++)
		{
			CFollowMeConnectionStand* pStandItem = pRwyData->m_Data.at(i);
			CString strName = pStandItem->m_StandInSim;

			if (strName.CompareNoCase("All") ==0)
				return pStandItem;
			
			ALTObjectID objName(strName);
			if (pDest->GetStandInput()->GetObjectName().idFits(objName))
				return pStandItem;
		}
	}
	
	return NULL;
}