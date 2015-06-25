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
		ElapsedTime tDelayTime = 0L;
		int nMaxQueue = 0;
		CString strTakePos;
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
		if(pFltItem->GetTriggerType() == AirsideArrivalDelayTrigger::CFlightTypeItem::Independently)
		{
			if(tDelayTime> ElapsedTime(0L))
                return tDelayTime;
		}
		//for enroute delay
		
		ElapsedTime tDelayEnRoute = 0L;
		int nMaxQueueEnRoute = 0;
		CString strTakePosEnRoute;
		AirsideArrivalDelayTrigger::CTriggerConditionList& enRouteTriggerList = pFltItem->GetEnRouteConditionList();
		for(size_t j=0; j< enRouteTriggerList.GetElementCount(); ++j)
		{
			AirsideArrivalDelayTrigger::CTriggerCondition* pTrigger = enRouteTriggerList.GetItem(j);

			int QueueLen = pTrigger->GetQueueLength();
			int nExitID = pTrigger->GetTakeOffPosition()->GetID();
			if(nExitID < 0)
				continue;

			LogicRunwayInSim* pRwyPort = m_pAirportres->getRunwayResource()->GetLogicRunway(pTrigger->GetTakeOffPosition()->GetRunwayID(),pTrigger->GetTakeOffPosition()->GetRunwayMark());
			if(!pRwyPort)
				continue;
			RunwayExitInSim* pRwyExit = pRwyPort->GetExitByID(nExitID); 
			if(!pRwyExit)
				continue;

			HoldPositionInSim* pHold = pRwyExit->getHoldPosition();
			if(!pHold)
				continue;


			int nQueueCount =  pHold->GetQueueLength();

			if (QueueLen < nQueueCount)
			{
				ElapsedTime tDelay = ElapsedTime((nQueueCount - QueueLen)* pTrigger->GetMinsPerAircraft()*60);
				tDelayEnRoute = max(tDelay,tDelayEnRoute);
				if (nMaxQueueEnRoute < nQueueCount)
				{
					nMaxQueueEnRoute = nQueueCount;
					strTakePos = pTrigger->GetTakeOffPosition()->GetName();
				}
			}
		}
		
		if(pFltItem->GetTriggerType() == AirsideArrivalDelayTrigger::CFlightTypeItem::Independently)
		{
			if(tDelayEnRoute>ElapsedTime(0L))
			{
				strReason.Format("Exceed %d flights waiting for departure at takeoff position: %s", nMaxQueueEnRoute, strTakePos);
				return tDelayEnRoute;
			}
		}
		else
		{
			if(tDelayEnRoute>ElapsedTime(0L) && tDelayTime > ElapsedTime(0L))
			{
				strReason.Format("Arrival Delay Trigger for EnRoute Queue at %s, Takeoff Queue at %s", strTakePosEnRoute, strTakePos);
				return MIN(tDelayEnRoute, tDelayTime);
			}
		}
	}

	return 0L;
}
