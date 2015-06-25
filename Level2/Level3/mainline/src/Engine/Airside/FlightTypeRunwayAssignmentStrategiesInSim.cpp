#include "StdAfx.h"
#include ".\flighttyperunwayassignmentstrategiesinsim.h"
#include "./InputAirside/TakeoffStandRunwayAssignItem.h"
#include "./InputAirside/FlightTypeRunwayAssignmentStrategyItem.h"
#include "./InputAirside/TakeOffTimeRangeRunwayAssignItem.h"
#include "./InputAirside/LandingRunwayAssignmentStrategies.h"
#include "./Common/AirportDatabase.h"
#include "./InputAirside/FlightTypeRunwayAssignmentStrategyItem.h"
#include "./InputAirside/TimeRangeRunwayAssignmentStrategyItem.h"
#include "./InputAirside/Stand.h"
#include "./Common/ALTObjectID.h"
#include "./Common/elaptime.h"
#include "AirsideFlightInSim.h"
#include "./InputAirside/StrategyType.h"
#include "ManagedRunwayStrategyInSim.h"
#include "RunwayResourceManager.h"
#include "ClosetRunwayStrategyInSim.h"
#include "RunwayInSim.h"
#include "RunwayResourceManager.h"
#include "FlightPlanedRunwayStrategyInSim.h"
#include "AirTrafficController.h"
#include "QueueToTakeoffPos.h"
#include "StandInSim.h"

CFlightTypeRunwayAssignmentStrategiesInSim::CFlightTypeRunwayAssignmentStrategiesInSim(void)
{
	m_pFlightTypeLandingRunwayAssignmentStrategies = NULL;
	m_pTakeoffRunwayAssignmentStrategies = NULL;
}

CFlightTypeRunwayAssignmentStrategiesInSim::~CFlightTypeRunwayAssignmentStrategiesInSim(void)
{
	delete m_pFlightTypeLandingRunwayAssignmentStrategies;
	m_pFlightTypeLandingRunwayAssignmentStrategies = NULL;
	delete m_pTakeoffRunwayAssignmentStrategies;
	m_pTakeoffRunwayAssignmentStrategies = NULL;
}

void CFlightTypeRunwayAssignmentStrategiesInSim::Init( int nPrj,CAirportDatabase *pAirportDatabase)
{
	m_pFlightTypeLandingRunwayAssignmentStrategies = new LandingRunwayAssignmentStrategies(nPrj);
	m_pFlightTypeLandingRunwayAssignmentStrategies->SetAirportDatabase(pAirportDatabase);
	m_pFlightTypeLandingRunwayAssignmentStrategies->ReadData();
	m_pFlightTypeLandingRunwayAssignmentStrategies->Sort();
	m_pTakeoffRunwayAssignmentStrategies = new CTakeoffStandRunwayAssign(nPrj, pAirportDatabase);
	m_pTakeoffRunwayAssignmentStrategies->ReadData();
	m_pTakeoffRunwayAssignmentStrategies->Sort();

}

FlightTypeRunwayAssignmentStrategyItem* CFlightTypeRunwayAssignmentStrategiesInSim::GetFlightTypeLandingRunwayAssignmentStrategyItem(AirsideFlightInSim* pFlight)
{
	int nItemCount = m_pFlightTypeLandingRunwayAssignmentStrategies->GetStrategyCount();
	for (int i =0; i< nItemCount; i++)
	{
		FlightConstraint fltCnst; 
		FlightTypeRunwayAssignmentStrategyItem* pItem = m_pFlightTypeLandingRunwayAssignmentStrategies->GetStrategyItem(i) ;
		fltCnst = pItem->GetFlightType();
		if(	pFlight->fits(fltCnst))
		{
			return pItem;
		}
	}

	return NULL;

}

CTakeOffTimeRangeRunwayAssignItem* getTimeItem(AirsideFlightInSim* pFlight,CTakeoffFlightTypeRunwayAssignment* pItem )
{
	ElapsedTime tTime = pFlight->GetTime();
	size_t nItemCount = pItem->m_TimeRange.size() ;
	for (size_t i =0; i< nItemCount; i++)
	{
		CTakeOffTimeRangeRunwayAssignItem* pTimeItem = pItem->m_TimeRange.at(i) ;
		if (tTime >= pTimeItem->GetStartTime() && tTime <= pTimeItem->GetEndTime())
		{
			return pTimeItem;
		}

	}
	return NULL;

}


CTakeOffTimeRangeRunwayAssignItem* CFlightTypeRunwayAssignmentStrategiesInSim::GetFlightTypeTakeoffRunwayAssignmentStrategyItem(AirsideFlightInSim* pFlight )const
{
	std::vector<CTakeoffStandRunwayAssignItem*> vFitStandData;
	vFitStandData.clear();

	StandInSim* pStandInSim = pFlight->GetOperationParkingStand();
	if (pStandInSim == NULL)
		return NULL;

	Stand* pStandInput = pStandInSim->GetStandInput();
	if (pStandInput == NULL)
		return NULL;
	
	m_pTakeoffRunwayAssignmentStrategies->GeStandRunwayAssignData(pStandInput->GetObjectName(), vFitStandData);


	size_t nItemCount = vFitStandData.size();
	for (size_t i =0; i< nItemCount; i++)
	{
		CTakeoffStandRunwayAssignItem* pData = vFitStandData.at(i);

		size_t nFltCount = pData->m_FlightData.size();
		for (size_t j =0; j < nFltCount; j++)
		{
			CTakeoffFlightTypeRunwayAssignment* pStrategyItem = pData->m_FlightData.at(j);

			if(pFlight->fits(pStrategyItem->GetFlightType()))
			{
				return getTimeItem(pFlight,pStrategyItem);				
			}
		}
	}
 
	return NULL;
}

LogicRunwayInSim* CFlightTypeRunwayAssignmentStrategiesInSim::GetArrivalRunway(AirsideFlightInSim* pFlight, RunwayResourceManager* pRunwayRes)
{
	LogicRunwayInSim* pLogicRunway = NULL;
	FlightTypeRunwayAssignmentStrategyItem* pItem = NULL;
	int nItemCount = m_pFlightTypeLandingRunwayAssignmentStrategies->GetStrategyCount();
	for (int i =0; i< nItemCount; i++)
	{
		FlightConstraint fltCnst; 
		fltCnst = m_pFlightTypeLandingRunwayAssignmentStrategies->GetStrategyItem(i)->GetFlightType();
		if(	pFlight->fits(fltCnst))
		{
			pItem = m_pFlightTypeLandingRunwayAssignmentStrategies->GetStrategyItem(i);
			break;
		}
	}
	if (pItem)
		pLogicRunway =GetRunway(pFlight,pRunwayRes,pItem);

	return pLogicRunway;
}

//LogicRunwayInSim* CFlightTypeRunwayAssignmentStrategiesInSim::GetDepartureRunway(AirsideFlightInSim* pFlight, RunwayResourceManager* pRunwayRes)
//{
//	LogicRunwayInSim* pLogicRunway = NULL;
//	CTakeoffFlightTypeRunwayAssignment* pItem = GetFlightTypeTakeoffRunwayAssignmentStrategyItem(pFlight);
//
//	if (pItem)
//	{
//		RunwayExitInSim * pTakeoffPos = GetTakeoffPosition(pFlight,pRunwayRes,pItem);
//		if(pTakeoffPos)
//		{
//			pLogicRunway = pTakeoffPos->GetLogicRunway();//pFlight,pRunwayRes,pItem);
//		}
//	}
//
//	return pLogicRunway;
//}

LogicRunwayInSim* CFlightTypeRunwayAssignmentStrategiesInSim::GetRunway(AirsideFlightInSim* pFlight,RunwayResourceManager * pRunwayRes,FlightTypeRunwayAssignmentStrategyItem* pStrategyItem)
{
	ElapsedTime tTime = pFlight->GetTime();
	int nItemCount = pStrategyItem->GetTimeRangeStrategyItemCount();
	LogicRunwayInSim* pLogicRunway = NULL;
	for (int i =0; i< nItemCount; i++)
	{
		TimeRangeRunwayAssignmentStrategyItem* pItem = pStrategyItem->GetTimeRangeStrategyItem( i);
		if (tTime >= pItem->getStartTime()&& tTime <= pItem->getEndTime())
		{
			RunwayAssignmentStrategyType StrateType = pItem->getStrategyType();
			switch(StrateType)
			{
			case FlightPlannedRunway:
				{
					CFlightPlanedRunwayStrategyInSim StrategyInSim;
					StrategyInSim.SetFlight(pFlight);
					pLogicRunway = StrategyInSim.GetRunway(pRunwayRes);
					break;
				}
			case ClosestRunway:
				{		
					CClosetRunwayStrategyInSim StrategyInSim;
					StrategyInSim.SetFlight(pFlight) ;
					pLogicRunway = StrategyInSim.GetRunway(pRunwayRes);
					break;
				}
			case ManagedAssignment:
				{		
					CManagedRunwayStrategyInSim StrategyInSim;
					StrategyInSim.SetRunwayAssignmentPercentItems(pItem->GetManagedAssignmentRWAssignPercent());
					pLogicRunway = StrategyInSim.GetRunway(pRunwayRes);
					break;
				}

			default:
				break;
			}
			break;
		}

	}
	return pLogicRunway;
}

bool CFlightTypeRunwayAssignmentStrategiesInSim::IsArrivalRunway(AirsideFlightInSim* pFlight, int nRwyID, int nMark)
{
	FlightTypeRunwayAssignmentStrategyItem* pItem = GetFlightTypeLandingRunwayAssignmentStrategyItem(pFlight);
	if (pItem == NULL)
		return true;

	ElapsedTime tTime = pFlight->GetTime();
	int nItemCount = pItem->GetTimeRangeStrategyItemCount();
	for (int i =0; i< nItemCount; i++)
	{
		TimeRangeRunwayAssignmentStrategyItem* pTimeItem = pItem->GetTimeRangeStrategyItem( i);
		if (tTime >= pTimeItem->getStartTime()&& tTime <= pTimeItem->getEndTime())
		{
			RunwayAssignmentStrategyType StrateType = pTimeItem->getStrategyType();
			switch(StrateType)
			{
			case FlightPlannedRunway:
				{
					CFlightPlanedRunwayStrategyInSim StrategyInSim;
					StrategyInSim.SetFlight(pFlight);
					LogicRunwayInSim* pLogicRunway = StrategyInSim.GetRunway(NULL);
					if (pLogicRunway == NULL)
						return true;

					if (pLogicRunway->getLogicRunwayType() == nMark && pLogicRunway->GetRunwayInSim()->GetRunwayInput()->getID() == nRwyID)
						return true;

					return false;
				}
			case ClosestRunway:
				{		
					return true;
				}
			case ManagedAssignment:
				{		
					CManagedRunwayStrategyInSim StrategyInSim;
					StrategyInSim.SetRunwayAssignmentPercentItems(pTimeItem->GetManagedAssignmentRWAssignPercent());
					return StrategyInSim.IsAssignedRunway(nRwyID,nMark);
				}

			default:
				break;
			}
			break;
		}

	}
	return true;

}

bool CFlightTypeRunwayAssignmentStrategiesInSim::IsDepartureRunway(AirsideFlightInSim* pFlight,int nRwyID, int nMark)
{
	CTakeOffTimeRangeRunwayAssignItem* pTimeItem = GetFlightTypeTakeoffRunwayAssignmentStrategyItem(pFlight);
	if (pTimeItem == NULL)
		return true;

	RunwayAssignmentStrategyType StrateType = pTimeItem->GetTimeRangeType();
	switch(StrateType)
	{
	case FlightPlannedRunway:
		{
			CFlightPlanedRunwayStrategyInSim StrategyInSim;
			StrategyInSim.SetFlight(pFlight);
			LogicRunwayInSim* pLogicRunway = StrategyInSim.GetRunway(NULL);
			if (pLogicRunway == NULL)
				return true;

			if (pLogicRunway->getLogicRunwayType() == nMark && pLogicRunway->GetRunwayInSim()->GetRunwayInput()->getID() == nRwyID)
				return true;

			return false;
		}
	case ClosestRunway:
		{		
			return true;
		}
	case ManagedAssignment:
		{		
			return pTimeItem->IsAssignedRunway(nRwyID,nMark);
		}

	default:
		break;
	}
		
	return true;
}

//LogicRunwayInSim* CFlightTypeRunwayAssignmentStrategiesInSim::AssignDepartureRunway( AirsideFlightInSim* pFlight, RunwayResourceManager * pRunwayRes,CTakeoffFlightTypeRunwayAssignment* pStrategyItem )
//{
//	LogicRunwayInSim* pLogicRunway = NULL;
//	size_t nItemCount = pStrategyItem->m_TimeRange.size() ;
//	for (size_t i =0; i< nItemCount; i++)
//	{
//		CTakeOffTimeRangeRunwayAssignItem* pTimeItem = pStrategyItem->m_TimeRange.at(i) ;
//		if (pFlight->GetTime() >= pTimeItem->GetStartTime() && pFlight->GetTime() <= pTimeItem->GetEndTime())
//		{
//			RunwayAssignmentStrategyType StrateType = pTimeItem->GetTimeRangeType();
//			switch(StrateType)
//			{
//			case FlightPlannedRunway:
//				{
//					CFlightPlanedRunwayStrategyInSim StrategyInSim;
//					StrategyInSim.SetFlight(pFlight);
//					pLogicRunway = StrategyInSim.GetRunway(pRunwayRes);
//					
//					break;
//				}
//			case ClosestRunway:
//				{		
//					CClosetRunwayStrategyInSim StrategyInSim;
//					StrategyInSim.SetFlight(pFlight) ;
//					pLogicRunway = StrategyInSim.GetRunway(pRunwayRes);
//					break;
//				}
//			case ManagedAssignment:
//				{		
//					return GetManagedDepartureRunway(pFlight, pRunwayRes, pTimeItem);
//				}
//
//			default:
//				break;
//			}
//		}
//
//	}
//	return pLogicRunway;
//}

LogicRunwayInSim* CFlightTypeRunwayAssignmentStrategiesInSim::GetManagedDepartureRunway( AirsideFlightInSim* pFlight, RunwayResourceManager * pRunwayRes,CTakeOffTimeRangeRunwayAssignItem* pTimeItem )
{
	RunwayExitInSim* pTakeoffPos = GetManagedTakeoffPos(pFlight,pRunwayRes,pTimeItem);
	if(pTakeoffPos)
	{
		pFlight->SetTakeoffPosition(pTakeoffPos);
		return pTakeoffPos->GetLogicRunway();
	}
	return NULL;
}

bool IsAtTheDecisionPoint(AirsideFlightInSim* pFlight,CTakeOffPriorityRunwayItem* pItem )
{

	ASSERT(pItem);
	AirsideResource* pRes = pFlight->GetResource();
	if(pItem && pRes)
	{
		bool bAtStand = pRes->IsStandResource()!=NULL;
		if(pItem->m_eDecision==DP_DepStand && bAtStand)
		{
			return true;
		}

		//
		int nIntersectionID = -1;
		if(pRes->GetType()==AirsideResource::ResType_TaxiwayDirSeg)
		{
			TaxiwayDirectSegInSim* pSeg = (TaxiwayDirectSegInSim*)pRes;
			nIntersectionID = pSeg->GetExitNode()->GetID();
		}
		if(pItem->m_eDecision==DP_Intersection && pItem->HasIntersecion(nIntersectionID))
		{
			return true;
		}

	}
	return false;
}


RunwayExitInSim* getRandomTakeoffPos(CTakeOffPriorityRunwayItem* priortItem,RunwayResourceManager * pRunwayRes)
{
	const std::vector<CTakeOffPosition*>& vTakeoffPos = priortItem->m_TakeOffPosition;
	
	std::vector<double> percentAcc;
	double percentIt = 0;
	for(size_t j =0; j < vTakeoffPos.size(); j++)
	{
		CTakeOffPosition* pPosData = vTakeoffPos.at(j);
		percentIt += pPosData->m_dPercent;
		percentAcc.push_back(percentIt);		
	}
	if(percentIt>0)
	{
		double dRandPercent = double(rand()% int(percentIt*100))/100;		
		size_t iPos = std::lower_bound(percentAcc.begin(),percentAcc.end(),dRandPercent) - percentAcc.begin();
		if(iPos<vTakeoffPos.size())
		{
			CTakeOffPosition* pPos = vTakeoffPos.at(iPos);
			return pRunwayRes->GetExitByRunwayExitDescription(pPos->m_runwayExitDescription);
		}
		ASSERT(false);

	}
	return NULL;
	
}

int getNextChangeIndex(CTakeOffTimeRangeRunwayAssignItem* pTimeItem,CManageAssignPriorityData* thepData )
{
	size_t nCount = pTimeItem->m_Data.size();
	for (size_t i =0; i < nCount; i++)
	{
		CManageAssignPriorityData* pData = pTimeItem->m_Data.at(i);
		if(pData == thepData )
			return i;
	}
	return 0;
}

bool isPriortySatisfied(AirsideFlightInSim* pFlight,RunwayExitInSim* pExit ,CManageAssignPriorityData* pData)
{
	if( pData->m_bHasChangeCondition)		
	{
		QueueToTakeoffPos* pQueue =  pExit->getTakeoffQueue();
		int nQLength = pQueue->GetItemCount();
		if (nQLength > pData->m_AircraftNum)
			return false;
		
		ElapsedTime tMaxTime = ElapsedTime(pData->m_time*60L);

		if (nQLength > 0)
		{
			ElapsedTime tEnter = pQueue->ItemAt(nQLength -1).GetEnterTime();
			if (pFlight->GetTime() - tEnter > tMaxTime)
			{
				return false;
			}			
		}	
	
	}
	return true;
}

RunwayExitInSim* CFlightTypeRunwayAssignmentStrategiesInSim::GetManagedTakeoffPos( AirsideFlightInSim* pFlight, RunwayResourceManager * pRunwayRes,CTakeOffTimeRangeRunwayAssignItem* pTimeItem )
{
	size_t nCount = pTimeItem->m_Data.size();

	RunwayExitInSim* pTakeoffPos = NULL;
	ASSERT(nCount>0);//should define at least one
	int nNextIndex =0;// getNextChangeIndex(pTimeItem, getFlightLastPriority(pFlight) );

	for (size_t i =nNextIndex; i < nCount; i++)
	{
		CManageAssignPriorityData* pData = pTimeItem->m_Data.at(i);		
		pTakeoffPos = getRandomTakeoffPos(pData->m_pRunwayItem,pRunwayRes);
		
		
		//ASSERT(pTakeoffPos);
		if(!pTakeoffPos)
			continue;

	
		//check to see if the change condition fulfilled if then continue 	
		
		if(isPriortySatisfied(pFlight,pTakeoffPos,pData))
		{		
			m_assignPriority[pFlight] = pData;
			break;
		}
		
	}

	return pTakeoffPos;
}

TakeOffBackTrack CFlightTypeRunwayAssignmentStrategiesInSim::GetTakeoffBacktrack(AirsideFlightInSim* pFlight,RunwayResourceManager * pRunwayRes)
{
	CTakeOffTimeRangeRunwayAssignItem* pItem = GetFlightTypeTakeoffRunwayAssignmentStrategyItem(pFlight);

	if (pItem)
		return GetFlightTakeoffBacktrack(pFlight,pRunwayRes,pItem);

	return BT_NoBacktrack;
}

RunwayExitInSim* CFlightTypeRunwayAssignmentStrategiesInSim::GetTakeoffPosition( AirsideFlightInSim* pFlight, RunwayResourceManager* pRunwayRes )
{
	
	CTakeOffTimeRangeRunwayAssignItem* pItem = GetFlightTypeTakeoffRunwayAssignmentStrategyItem(pFlight);

	if (pItem)
		return GetTakeoffPosition(pFlight,pRunwayRes,pItem);

	return NULL;
}

TakeOffBackTrack CFlightTypeRunwayAssignmentStrategiesInSim::GetFlightTakeoffBacktrack(AirsideFlightInSim* pFlight,RunwayResourceManager * pRunwayRes,CTakeOffTimeRangeRunwayAssignItem* pStrategyItem)
{
	if (pFlight->GetTime() >= pStrategyItem->GetStartTime() && pFlight->GetTime() <= pStrategyItem->GetEndTime())
	{
		size_t nCount = pStrategyItem->m_Data.size();

		RunwayExitInSim* pTakeoffPos = NULL;
		ASSERT(nCount>0);//should define at least one
		int nNextIndex =0;// getNextChangeIndex(pTimeItem, getFlightLastPriority(pFlight) );

		for (size_t i =nNextIndex; i < nCount; i++)
		{
			CManageAssignPriorityData* pData = pStrategyItem->m_Data.at(i);		
			pTakeoffPos = getRandomTakeoffPos(pData->m_pRunwayItem,pRunwayRes);


			//ASSERT(pTakeoffPos);
			if(!pTakeoffPos)
				continue;


			//check to see if the change condition fulfilled if then continue 	

			if(isPriortySatisfied(pFlight,pTakeoffPos,pData))
			{		
				if (pData->m_pRunwayItem)
				{
					return pData->m_pRunwayItem->GetTakeOffBackTrack();
				}
			}

		}
	}
	return BT_NoBacktrack;
}

RunwayExitInSim* CFlightTypeRunwayAssignmentStrategiesInSim::GetTakeoffPosition( AirsideFlightInSim* pFlight, RunwayResourceManager * pRunwayRes,CTakeOffTimeRangeRunwayAssignItem* pTimeItem )
{
	RunwayExitInSim* pTakeoffPos = NULL;
	if (pFlight->GetTime() >= pTimeItem->GetStartTime() && pFlight->GetTime() <= pTimeItem->GetEndTime())
	{
		RunwayAssignmentStrategyType StrateType = pTimeItem->GetTimeRangeType();
		switch(StrateType)
		{
		case FlightPlannedRunway:
			{
				CFlightPlanedRunwayStrategyInSim StrategyInSim;
				StrategyInSim.SetFlight(pFlight);
				LogicRunwayInSim* pLogicRunway = StrategyInSim.GetRunway(pRunwayRes);
				if(pLogicRunway)
				{
					pTakeoffPos = pFlight->GetAirTrafficController()->getTakeoffPositionAssignment().GetTakeoffPositionByFlightType(pLogicRunway,pFlight);
				}
				break;
			}
		case ClosestRunway:
			{		
				CClosetRunwayStrategyInSim StrategyInSim;
				StrategyInSim.SetFlight(pFlight) ;
				LogicRunwayInSim* pLogicRunway = StrategyInSim.GetRunway(pRunwayRes);
				if(pLogicRunway)
				{
					pTakeoffPos = pFlight->GetAirTrafficController()->getTakeoffPositionAssignment().GetTakeoffPositionByFlightType(pLogicRunway,pFlight);
				}
				break;
			}
		case ManagedAssignment:
			{		
				pTakeoffPos =  GetManagedTakeoffPos(pFlight, pRunwayRes, pTimeItem);
			}

		default:
			break;
		}
	}

	return pTakeoffPos;
}

//bool CFlightTypeRunwayAssignmentStrategiesInSim::IsManageAssignTakeoffPosition( AirsideFlightInSim* pFlight ) const
//{
//	CTakeOffTimeRangeRunwayAssignItem* pItem = GetFlightTypeTakeoffRunwayAssignmentStrategyItem(pFlight);
//	if(pItem )
//	{
//		if(pItem->GetTimeRangeType() == ManagedAssignment)
//			return true;
//	}
//	return false;
//}

CManageAssignPriorityData* CFlightTypeRunwayAssignmentStrategiesInSim::getFlightLastPriority( AirsideFlightInSim* pFlight ) const
{
	PriorityMap::const_iterator itr = m_assignPriority.find(pFlight);
	if(itr!=m_assignPriority.end())
		return itr->second;
	return NULL;
}

bool CFlightTypeRunwayAssignmentStrategiesInSim::IsAbleToChangeTakeoffPos( AirsideFlightInSim* pFlight ) const
{
	CManageAssignPriorityData* pData = getFlightLastPriority(pFlight);
	if(pData)
	{
		if( IsAtTheDecisionPoint(pFlight,pData->m_pRunwayItem) )
		{
			RunwayExitInSim* pExit = pFlight->GetTakeoffPosition();
			if(pExit)
			{
				if(!isPriortySatisfied(pFlight,pExit,pData))
				{
					return true;
				}
			}
		}
	}
	return false;
}
