#include "StdAfx.h"
#include ".\takeoffsequencinginsim.h"
#include "AirsideFlightInSim.h"
#include "RunwayInSim.h"
#include "QueueToTakeoffPos.h"
#include "../../common/FileOutPut.h"
#include "AirspaceResourceManager.h"
#include "FlightRunwayTakeOffEvent.h"

const int NO_MATCH = -1;
TakeoffSequencingInSim::TakeoffSequencingInSim()
{
}

TakeoffSequencingInSim::~TakeoffSequencingInSim(void)
{
}

void TakeoffSequencingInSim::Init(int nPrjID, const AirRouteNetworkInSim* pAirRouteNetwork, CAirportDatabase* _airportDB)
{
	m_pAirRouteNetwork = const_cast<AirRouteNetworkInSim*>(pAirRouteNetwork);
	m_pTakeoffSequencing = new TakeoffSequencingList(nPrjID,_airportDB);
	m_pTakeoffSequencing->ReadData();
	m_pPreviousTakeoffQueue = NULL;
}

bool TakeoffSequencingInSim::IsWaitingInQueue(AirsideFlightInSim* pFlight, ElapsedTime tCurTime)
{
	LogicRunwayInSim* pLogicRunway = pFlight->GetAndAssignTakeoffRunway();
	RunwayExitInSim* pTakeoffPos = pFlight->GetAndAssignTakeoffPosition();
	QueueToTakeoffPosList& QueueList = pLogicRunway->GetQueueList();

	if (QueueList.GetAllFlightCount() <= 1)
		return false;

	std::vector<RunwayExitInSim*> vTakeoffPosList;
	vTakeoffPosList.clear();

	int nSeqIdx = NO_MATCH;
	TakeOffSequenceInfo* pSequence = GetRelativeTakeoffSequenceAndIndex(pFlight,nSeqIdx);

	if (pSequence != NULL && nSeqIdx > NO_MATCH)
	{
		TakeoffSequencing* pItem = m_pTakeoffSequencing->GetItem(nSeqIdx);
		int nCount = pItem->GetTakeOffPositionCount();
		for (int m = 0; m < nCount; m++)
		{
			int nID = pItem->GetTakeOffPositionInfo(m)->m_nTakeOffPositionID;

			if (nID >=0)
			{
				int nExit = pLogicRunway->GetExitCount();
				for (int n = 0; n < nExit; n++)
				{
					if (pLogicRunway->GetExitAt(n)->GetExitInput().GetID() == nID)
					{
						vTakeoffPosList.push_back(pLogicRunway->GetExitAt(n));
						break;
					}
				}

			}
		}
	}
	else
	{
		const int nQueueCount = QueueList.GetQueueCount();
		for (int n = 0; n < nQueueCount; n++)
		{
			RunwayExitInSim* pExit = QueueList.GetQueueByIdx(n)->GetTakeoffPos();
			vTakeoffPosList.push_back(pExit);
		}
		
	}

	if(pSequence == NULL)
	{			
		
		QueueToTakeoffPos* pFligthQueue = QueueList.GetAddQueue(pFlight->GetAndAssignTakeoffPosition());
		if(!pFligthQueue->IsFlightInQueue(pFlight))
			return false;

		ElapsedTime minQueueTime = ElapsedTime::Max();
		for(int qidx =0;qidx< QueueList.GetQueueCount(); qidx++)
		{
			QueueToTakeoffPos* pQueue = (QueueToTakeoffPos* )QueueList.GetQueueByIdx(qidx);
			if(pQueue == pFligthQueue)
				continue;
			if(pQueue->GetTakeoffPos()->IsLeft() == pFligthQueue->GetTakeoffPos()->IsLeft()) //exam queue for other side
				continue;

			if(!pQueue->GetHeadFlight(tCurTime))
				continue;
			
			ElapsedTime queueTime;
			if( (queueTime = pQueue->GetQueueTime()) < minQueueTime ) //if flight queue below other , wait
			{
				minQueueTime = queueTime;
			}
		}
		pFligthQueue->SetHeadFlight(pFlight);

		
		ElapsedTime fltQueueTime = pFligthQueue->GetQueueTime();
		//CFileOutput fileDebug1("C:\\flgihtqueue.txt");
		//fileDebug1.LogLine("");
		//fileDebug1.LogLine(pFlight->GetCurrentFlightID() + _T("  ")+ fltQueueTime.printTime() + _T("  ") + minQueueTime.printTime() );
		
		if(fltQueueTime <= minQueueTime)
		{			
			return false;
		}
		return true;
		////get runway head flight except pFlight and flight at the same side
		//AirsideFlightInSim* pHeadFlight = NULL;
		//ElapsedTime HeadfltEnterQT;
		//for(int qidx =0;qidx< QueueList.GetQueueCount(); qidx++)
		//{
		//	QueueToTakeoffPos* pQueue = (QueueToTakeoffPos* )QueueList.GetQueueByIdx(qidx);

		//	if(pQueue->GetTakeoffPos()->IsLeft() == pFligthQueue->GetTakeoffPos()->IsLeft()) //exam queue for other side
		//		continue;

		//					
		//	AirsideFlightInSim* pQheadFlight = pQueue->GetHeadFlight();
		//	ElapsedTime qheadFlightT=  pQueue->GetQueueTime();

		//	if(!pHeadFlight && pQheadFlight!=pFlight)
		//	{
		//		pHeadFlight = pQheadFlight;
		//		HeadfltEnterQT = qheadFlightT;
		//		continue;
		//	}
		//	else
		//	{				
		//		if(pQheadFlight && pQheadFlight!=pFlight)
		//		{					
		//			if(qheadFlightT < HeadfltEnterQT)
		//			{
		//				pHeadFlight = pQheadFlight;
		//				HeadfltEnterQT = qheadFlightT;
		//			}
		//		}
		//	}

		//		
		//}
	
		//ElapsedTime fltEnterQT = pLogicRunway->GetFlightEnterQueueTime(pFlight);		
		//QueueToTakeoffPos* pQueue = QueueList.GetAddQueue(pTakeoffPos);
		//
		//CFileOutput fileDebug("C:\\flgihtqueue.txt");
		//fileDebug.LogLine("");
		//fileDebug.LogLine(pFlight->GetCurrentFlightID() + fltEnterQT.printTime() );
		//if(pHeadFlight)
		//	fileDebug.LogLine(pHeadFlight->GetCurrentFlightID() + HeadfltEnterQT.printTime() );
		//
		//if(pHeadFlight && pHeadFlight!=pFlight && HeadfltEnterQT < fltEnterQT )
		//{			
		//	pQueue->SetHeadFlight(pFlight);
		//	return true;
		//}
		//else
		//{			
		//	pQueue->SetHeadFlight(pFlight);
		//	return false;
		//}			
	}

	std::vector<AirsideFlightInSim*> vSatisfiedFlights;

	for (int i = 0; i < PriorityCount; i++)
	{
		GetSatisfiedTakeoffFlight(pSequence->m_priority[i], vTakeoffPosList, QueueList,tCurTime, vSatisfiedFlights);

		size_t nCount = vSatisfiedFlights.size();
		for (size_t j =0; j < nCount; j++)
		{
			bool bNeedDelay = false;
			AirsideFlightInSim* pSatisfiedFlight = vSatisfiedFlights.at(j);

			if(pSequence->m_priority[i].m_Type != PriorityFlightType)
			{
				BOOL bDelayCrossPoint = pSequence->m_priority[i].m_CrossPoint;
				if (bDelayCrossPoint && m_pAirRouteNetwork->GetFirstDelayTimeAtWaypoint(pSatisfiedFlight, tCurTime) >0L)
					bNeedDelay = true;

				if (!bNeedDelay)
				{
					BOOL bDelayAtSID = pSequence->m_priority[i].m_CanEncroach;
					if (bDelayAtSID && m_pAirRouteNetwork->GetDelayTimeByEncroachTrailSeperationOnSameSID(pSatisfiedFlight, tCurTime) >0L)
						bNeedDelay = true;
				}

				if(!bNeedDelay)
				{
					BOOL bDelayByFlight = pSequence->m_priority[i].m_CanTakeOff;
					if (bDelayByFlight)
					{
						std::vector<AirsideFlightInSim*> vPriorityFlights;
						vPriorityFlights.clear();

						for (int k =0; k < PriorityCount; k++)
						{
							if (pSequence->m_priority[k].m_Type != PriorityFlightType)
								continue;

							GetPriorityFlights(pSequence->m_priority[k], vTakeoffPosList, QueueList, tCurTime, vPriorityFlights);
						}

						if (std::find(vPriorityFlights.begin(), vPriorityFlights.end(), pSatisfiedFlight) == vPriorityFlights.end() )
							bNeedDelay = true;

						//AirsideFlightInSim* pFirstFlight = GetEarliestEnterFlightInQueue(QueueList, vPriorityFlights);

						//CFlightTakeOffOnRwyEvent* pEvent = new CFlightTakeOffOnRwyEvent(pFirstFlight);
						//pEvent->setTime(tCurTime);
						//pEvent->addEvent();

						//return true;

					}
				}
			}


			//if (bNeedDelay && pSatisfiedFlight == pFlight)
			//	return true;

			if (!bNeedDelay)
			{
				if (pSatisfiedFlight == pFlight)
					return false;

				CFlightTakeOffOnRwyEvent* pEvent = new CFlightTakeOffOnRwyEvent(pSatisfiedFlight);
				pEvent->setTime(tCurTime);
				pEvent->addEvent();

				return true;
			}
				
		}
		return false;
	}
	
	QueueToTakeoffPos* pQueue = QueueList.GetAddQueue(pFlight->GetAndAssignTakeoffPosition());
	ElapsedTime tEarlyEnter = pQueue->GetFlightEnterTime(pFlight);
	AirsideFlightInSim* pTakeoffFlight = pFlight;

	size_t nQueue = vTakeoffPosList.size();
	for (size_t i =0; i < nQueue; i++)
	{
		if (vTakeoffPosList.at(i) == pFlight->GetAndAssignTakeoffPosition())
			continue;

		pQueue = QueueList.GetAddQueue(vTakeoffPosList.at(i));
		AirsideFlightInSim* pHeadFlight = pQueue->GetHeadFlight(tCurTime);

		ElapsedTime tEnter = pQueue->GetFlightEnterTime(pHeadFlight);
		if ( tEarlyEnter > tEnter)
		{
			tEarlyEnter = tEnter;
			pTakeoffFlight = pHeadFlight;
		}
	}

	if (pTakeoffFlight != pFlight)
	{
		CFlightTakeOffOnRwyEvent* pEvent = new CFlightTakeOffOnRwyEvent(pTakeoffFlight);
		pEvent->setTime(tCurTime);
		pEvent->addEvent();

		return true;
	}

	return false;
}

AirsideFlightInSim* TakeoffSequencingInSim::GetEarliestEnterFlightInQueue(QueueToTakeoffPosList& QueueList, const std::vector<AirsideFlightInSim*>& vFlights)
{
	AirsideFlightInSim* pEarlyFlight = NULL;
	size_t nSize = vFlights.size();
	ElapsedTime tEarlyTime = -1L;
	for (size_t j =0; j< nSize; j++)
	{
		AirsideFlightInSim* pFlight = vFlights.at(j);

		QueueToTakeoffPos* pQueue = QueueList.GetAddQueue(pFlight->GetAndAssignTakeoffPosition());

		if (pEarlyFlight == NULL)
		{
			tEarlyTime = pQueue->GetFlightEnterTime(pFlight) ;
			pEarlyFlight = pFlight;
		}
		else
		{
			if (pQueue->GetFlightEnterTime(pFlight) < tEarlyTime)
			{
				tEarlyTime = pQueue->GetFlightEnterTime(pFlight) ;
				pEarlyFlight = pFlight;
			}
		}		
	}

	return pEarlyFlight;
}

TakeOffSequenceInfo* TakeoffSequencingInSim::GetRelativeTakeoffSequenceAndIndex(AirsideFlightInSim* pFlight,int& nSeqIdx)
{
	if (pFlight == NULL)
		return NULL;

	//LogicRunwayInSim* pLogicRunway = pFlight->GetTakeoffRunway();
	int nTakeoffPos = pFlight->GetAndAssignTakeoffPosition()->GetID();
	
	int nSequence = m_pTakeoffSequencing->GetTakeoffSequencingCount();
	for (int i =0; i< nSequence; i++)
	{
		TakeoffSequencing* pSequence = m_pTakeoffSequencing->GetItem(i);
		int nPosition = pSequence->GetTakeOffPositionCount();
		for (int j =0; j < nPosition ; j++)
		{
			if (pSequence->GetTakeOffPositionInfo(j)->m_nTakeOffPositionID > nTakeoffPos 
				|| pSequence->GetTakeOffPositionInfo(j)->m_nTakeOffPositionID < nTakeoffPos)
			{
				continue;
			}

			int nPriority = pSequence->GetTakeOffSequenceInfoCount();
			for (int m =0 ; m < nPriority; m++)
			{
				TakeOffSequenceInfo* pInfo = pSequence->GetTakeOffSequenceInfo(m);
				if ( pInfo->m_startTime <= pFlight->GetTime() && pInfo->m_endTime >= pFlight->GetTime())
				{
					nSeqIdx = i; 
					return pInfo;
				}
			}
		}
	}

	nSeqIdx = NO_MATCH;
	return NULL;
}

const CPriorityData& TakeoffSequencingInSim::GetNextPriorityData( TakeOffSequenceInfo* pSequenceInfo, int nNextIdx )
{
	return pSequenceInfo->m_priority[nNextIdx];
}

void TakeoffSequencingInSim::GetSatisfiedTakeoffFlight(const CPriorityData& _priorityData,
														const std::vector<RunwayExitInSim*>& vTakeoffPosList,
														QueueToTakeoffPosList& QueueList,
														const ElapsedTime& tCurTime,
														std::vector<AirsideFlightInSim*>& vSatisfiedFlights)
{
	AirsideFlightInSim* pSuitFlight = NULL;
	vSatisfiedFlights.clear();

	size_t nQueue = vTakeoffPosList.size();
	switch(_priorityData.m_Type)
	{
	case FIFO:
		{
			QueueToTakeoffPos* pQueue = QueueList.GetAddQueue(vTakeoffPosList.at(0));
			ElapsedTime tMaxTime = pQueue->GetFlightEnterTime(pQueue->GetHeadFlight(tCurTime));

			for (size_t j = 1; j < nQueue; j++)
			{
				RunwayExitInSim* pPos = vTakeoffPosList.at(j);
				pQueue = QueueList.GetAddQueue(pPos);
				ElapsedTime tTime = pQueue->GetFlightEnterTime(pQueue->GetHeadFlight(tCurTime));

				if ( tTime > tMaxTime )
					tMaxTime = tTime;
			}
			
			for (size_t i =0; i < nQueue; i++)
			{
				pQueue = QueueList.GetAddQueue(vTakeoffPosList.at(i));
				pSuitFlight = pQueue->GetHeadFlight(tCurTime);
				if (pSuitFlight && pQueue->GetFlightEnterTime(pSuitFlight) == tMaxTime)
					vSatisfiedFlights.push_back(pSuitFlight);
			}
							
		}
		break;
	case LongestQFirst:
		{
			QueueToTakeoffPos* pQueue = QueueList.GetAddQueue(vTakeoffPosList.at(0));
			//pSuitFlight = pQueue->GetHeadFlight(tCurTime);
			int nMaxQueueLength = pQueue->GetItemCount();

			for (size_t j = 1; j < nQueue; j++)
			{
				RunwayExitInSim* pPos = vTakeoffPosList.at(j);
				pQueue = QueueList.GetAddQueue(pPos);
				int nOtherQueueLength = pQueue->GetItemCount();

				if ( nOtherQueueLength > nMaxQueueLength)
					nMaxQueueLength = nOtherQueueLength;
			}

			for (size_t i =0; i < nQueue; i++)
			{
				pQueue = QueueList.GetAddQueue(vTakeoffPosList.at(i));
				pSuitFlight = pQueue->GetHeadFlight(tCurTime);
				if (pSuitFlight && pQueue->GetItemCount() == nMaxQueueLength)
					vSatisfiedFlights.push_back(pSuitFlight);
			}	
		}
		break;
	case RoundRobin:
		{
			int nPreIdx = 0;
			for (size_t j =0; j < nQueue; j++)
			{
				RunwayExitInSim* pPos = vTakeoffPosList[j];
				if (m_pPreviousTakeoffQueue == NULL || QueueList.GetAddQueue(pPos) == m_pPreviousTakeoffQueue)
				{
					nPreIdx = j;
					break;
				}
			}
			for (size_t j =1; j <= nQueue; j++)
			{
				RunwayExitInSim* pPos = vTakeoffPosList[j];

				int nNext = (nPreIdx +j) % nQueue;
				RunwayExitInSim* pNextPos = vTakeoffPosList.at(nNext);
				QueueToTakeoffPos* pNextQueue = QueueList.GetAddQueue(pNextPos);

				if (pNextQueue->GetItemCount() <= 0)
					continue;
				
				if (pNextQueue->GetHeadFlight(tCurTime))
				{
					vSatisfiedFlights.push_back(pNextQueue->GetHeadFlight(tCurTime));
					m_pPreviousTakeoffQueue = pNextQueue;
					break;
				}			
			}			
		}
		break;
	case DepartureSlot:
		break;
	case PriorityFlightType:
		{
			GetPriorityFlights(_priorityData, vTakeoffPosList, QueueList, tCurTime, vSatisfiedFlights);
		}
		break;
	default:
		break;
	}
}

void TakeoffSequencingInSim::GetPriorityFlights( const CPriorityData& _priorityData,
												const std::vector<RunwayExitInSim*>& vTakeoffPosList,
												QueueToTakeoffPosList& QueueList,
												const ElapsedTime& tCurTime,
												std::vector<AirsideFlightInSim*>& vPriorityFlights )
{
	size_t nQueue = vTakeoffPosList.size();

	for (size_t j = 0; j < nQueue; j++)
	{				
		RunwayExitInSim* pPos = vTakeoffPosList.at(j);
		QueueToTakeoffPos* pQueue = QueueList.GetAddQueue(pPos);
		AirsideFlightInSim* pFlight = pQueue->GetHeadFlight(tCurTime);

		if (pFlight && pFlight->fits(_priorityData.m_FltTy))
			vPriorityFlights.push_back(pFlight);
	}
	
}