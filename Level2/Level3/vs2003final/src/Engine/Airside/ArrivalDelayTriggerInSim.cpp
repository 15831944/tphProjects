#include "StdAfx.h"
#include ".\arrivaldelaytriggerinsim.h"
#include "AirsideFlightInSim.h"
#include "RunwayInSim.h"
#include "../../InputAirside/ArrivalDelayTriggers.h"
#include "AirportResourceManager.h"
#include "RunwayResourceManager.h"
#include "QueueToTakeoffPos.h"

const int MAX_QUEUELENGTH = 1000;
ArrivalDelayTriggerInSim::ArrivalDelayTriggerInSim(void)
{
	m_pAirportres = NULL;
	m_pdelayTriggerInput = NULL;
}

ArrivalDelayTriggerInSim::~ArrivalDelayTriggerInSim(void)
{
	delete m_pdelayTriggerInput;
	m_pdelayTriggerInput = NULL;
}

void ArrivalDelayTriggerInSim::Init( int nPrjId, AirportResourceManager * pAirportRes, CAirportDatabase* pAirportDB)
{
	m_pdelayTriggerInput = new CAirsideArrivalDelayTrigger(pAirportDB);
	m_pdelayTriggerInput->ReadData();
	m_pAirportres = pAirportRes;
}

ElapsedTime ArrivalDelayTriggerInSim::GetDelayTime(AirsideFlightInSim* pFlight,CString& strReason)
{
	LogicRunwayInSim * pLandRwy = pFlight->GetLandingRunway();
	int nRwyID = pLandRwy->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK nRwyMark = pLandRwy->getLogicRunwayType();

	ElapsedTime tDelayTime = 0L;
	int nMaxQueue = 0;
	CString strTakePos = "";

	size_t nCount = m_pdelayTriggerInput->GetElementCount();
	for(size_t i=0 ;i< nCount;i++)
	{
		AirsideArrivalDelayTrigger::CFlightTypeItem* pFltItem = m_pdelayTriggerInput->GetItem(i);
		if( !pFlight->fits(pFltItem->GetFltType()) )
			continue;

		if (!pFltItem->GetTimeRangeList().IsTimeInTimeRangeList(pFlight->GetTime()))
			continue;

		if (!pFltItem->GetRunwayList().IsLogicRunwayInRunwayList(nRwyID,nRwyMark))
			continue;

		AirsideArrivalDelayTrigger::CTriggerConditionList& triggerList = pFltItem->GetTriggerConditionList();
		size_t nPosCount = triggerList.GetElementCount();
		for(size_t j=0; j < nPosCount; j++)
		{
			AirsideArrivalDelayTrigger::CTriggerCondition* pTrigger = triggerList.GetItem(j);

			int QueueLen = pTrigger->GetQueueLength();
			int nExitID = pTrigger->GetTakeOffPosition()->GetID();
			if(nExitID < 0)
				continue;

			LogicRunwayInSim* pRwyPort = m_pAirportres->getRunwayResource()->GetLogicRunway(pTrigger->GetTakeOffPosition()->GetRunwayID(),pTrigger->GetTakeOffPosition()->GetRunwayMark());
			RunwayExitInSim* pRwyExit = pRwyPort->GetExitByID(nExitID); 

			int nQueueCount = pRwyPort->GetTakeoffPostionQueueLength(pRwyExit);

			if (QueueLen < nQueueCount)
			{
				ElapsedTime tDelay = ElapsedTime((nQueueCount - QueueLen)* pTrigger->GetMinsPerAircraft()*60);
				tDelayTime = max(tDelayTime,tDelay);
				if (nMaxQueue < nQueueCount)
				{
					nMaxQueue = nQueueCount;
					strTakePos = pTrigger->GetTakeOffPosition()->GetName();
				}
			}
		
		}
		strReason.Format("Exceed %d flights waiting for departure at takeoff position: %s", nMaxQueue, strTakePos);
		return tDelayTime;

	}

	return 0L;
}
