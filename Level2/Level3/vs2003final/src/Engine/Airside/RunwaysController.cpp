#include "StdAfx.h"
#include ".\runwayscontroller.h"
#include <fstream>
#include "AirsideResourceManager.h"
#include "AirportResourceManager.h"
#include "RunwayResourceManager.h"
#include "AirsideFlightInSim.h"
#include "FlightPerformanceManager.h"
#include "ApproachSeparationInSim.h"
#include "TakeoffSeparationInSim.h"
#include "AirTrafficController.h"
#include "../../Common/ARCUnit.h"
#include "TaxiwayConflictResolutionInEngine.h"
#include "SimulationErrorShow.h"
#include "../SimulationDiagnoseDelivery.h"
#include "Common/ARCExceptionEx.h"
#include "RunwayTakeoffPositionAssignInSim.h"
#include "RunwayExitAssignmentStrategiesInSim.h"
#include "../../common/DynamicMovement.h"
#include "RunwaySegInSim.h"
#include "../../Results/AirsideFllightRunwayDelayLog.h"
#include "../MobileDynamics.h"
#include "AirspaceResourceManager.h"
#include "RunwayRelatedIntersectionNodeInSim.h"
#include "FlightPerformancesInSim.h"
#include "../../InputAirside/TakeOffTimeRangeRunwayAssignItem.h"

#define _DEBUGLOG 1

CRunwaySystem::CRunwaySystem(void)
{
	m_nProjID = -1;
	//m_pRunwayResManager = NULL;
	m_pApproachSeparationInSim = NULL;
	m_pTakeOffSeparationInSim = NULL;
	m_pOutput = NULL;
}

CRunwaySystem::~CRunwaySystem(void)
{
	size_t nSize = m_vRunwayRelatedIntersectionNodes.size();
	for (size_t i = 0; i < nSize; i++)
	{
		RunwayRelatedIntersectionNodeInSim* pNode = m_vRunwayRelatedIntersectionNodes.at(i);
		delete pNode;
		pNode = NULL;
	}

}
void CRunwaySystem::Initlization(int nProjID,
									AirTrafficController *pATC,
									CRunwayExitAssignmentStrategiesInSim* pExitStrategy, 
									CRunwayTakeoffPositionAssignInSim* pTakeoffStrategy,
									OutputAirside *pOutputAirside)

{
	m_nProjID = nProjID;
	RunwayResourceManager *pRunwayResManager = pATC->GetAirsideResourceManager()->GetAirportResource()->getRunwayResource();
	m_pAirRouteNetwork = pATC->GetAirsideResourceManager()->GetAirspaceResource();
	//init runway in simulation list
	m_vRunwayInSim.clear();
	int nRunwayCount = pRunwayResManager->GetRunwayCount();
	for(int nRunway = 0; nRunway < nRunwayCount; ++nRunway)
	{
		m_vRunwayInSim.push_back(pRunwayResManager->GetRunwayByIndex(nRunway)->GetLogicRunway1());
		m_vRunwayInSim.push_back(pRunwayResManager->GetRunwayByIndex(nRunway)->GetLogicRunway2());
	}

	m_pTakeOffSeparationInSim = pRunwayResManager->GetTakeoffSeparation();
	m_pApproachSeparationInSim = pRunwayResManager->GetApproachSeparation();


	long lBufferTime = pATC->GetTaxiwayConflictResolution()->GetConflictResolution()->GetRunwayCrossBuffer();
	m_eTimeCrossRunwayBuffer = ElapsedTime(lBufferTime);

	m_pRunwayExitStrategy = pExitStrategy;
	m_pRunwayTakeoffPosStrategy = pTakeoffStrategy;

	ASSERT(pOutputAirside != NULL);
	m_pOutput = pOutputAirside;

	InitlizationRelatedUnintersectedRunways();
}

bool CRunwaySystem::IsLandingFlightCrossingRunwayIntersectionNodes(AirsideFlightInSim* pFlight,LogicRunwayInSim* pLandingRunway, ElapsedTime& timeToLanding, const std::vector<IntersectionNodeInSim*>& vIntNodes)
{
	double dMinLandingDist = pFlight->GetPerformance()->getMinLandingDist(pFlight);
	int nSize = vIntNodes.size();
	for(int idx =0; idx < nSize; idx++)
	{
		IntersectionNodeInSim* pIntNode = vIntNodes.at(idx);

		CPoint2008 intPoint = pIntNode->GetNodeInput().GetPosition();
		//have condition to do LASHO operation
		bool bHaveConditionLasho = false;
		double dExitDistance = pFlight->GetRunwayExit()->GetExitRunwayPos();
		double dIntDistance = pLandingRunway->GetPointDist(intPoint);

		if (dIntDistance > dExitDistance)//the landing flight not cross the intersection node
			continue;

		if (dIntDistance > dMinLandingDist)
			bHaveConditionLasho = true;

		///////////////////temp disabled/////////////////////////
		//if (bHaveConditionLasho)
		//{
		//	//port1
		//	LogicRunwayInSim *pRunwayPortLeft = pIntRunway->GetLogicRunway1();
		//	TimeRange timeRange1;
		//	ElapsedTime eLASHOTime1; 
		//	BOOL bLASHO = PerformanceApproachSeparationLASHO(pFlight,pRunwayPortLeft,pIntNode,eTimeStayOnRunway,timeToLanding,timeRange1,eLASHOTime1);

		//	if (CheckTimeRangeAvaiable(timeRange,timeRange1) == false)
		//	{
		//		timeToLanding =  timeRange.GetStartTime()> timeRange1.GetStartTime()? timeRange.GetStartTime():timeRange1.GetStartTime() ;
		//		continue;
		//	}
		//	if (bLASHO)
		//	{
		//		vRunwayArrangeInfo.push_back(RunwayOccupyInfo(pRunwayPortLeft,timeRange1,pIntNode,true,eLASHOTime1));
		//	}
		//}
		////////////////////////////////////////////////////
		//else
		//{
		double dRollingDist;
		double landspd = pFlight->GetPerformance()->getLandingSpeed(pFlight);
		double dTouchDownDist = pFlight->GetTouchDownDistAtRuwnay(pLandingRunway);//pFlight->GetPerformance()->getDistThresToTouch(pFlight);
		double dMaxDecel = m_pRunwayExitStrategy->GetFlightMaxDecelOnRunway(pFlight);

		double dMinLandingDistance = (landspd*landspd - pFlight->GetExitSpeed()*pFlight->GetExitSpeed())/(2.0* dMaxDecel);
		if(dMinLandingDistance > dExitDistance - dTouchDownDist)
			dRollingDist = dMinLandingDistance;
		else
			dRollingDist = dExitDistance - dTouchDownDist;

		double dDistToNode = dIntDistance - dTouchDownDist;
		DynamicMovement flightmove(landspd, pFlight->GetExitSpeed(),dRollingDist);
		ElapsedTime tToNode = flightmove.GetDistTime(dDistToNode);	
		TimeRange timeRange1;	//the time range is node available time range
		ElapsedTime tTimeAtNode = timeToLanding + tToNode;
		PerformanceApproachSeparation(pFlight,pLandingRunway,pIntNode,1L,tTimeAtNode,timeRange1);	//judge node time, assume staying time 1s
		if (timeToLanding < (tTimeAtNode- tToNode))		//the node with conflict and need delay
		{
			timeToLanding = tTimeAtNode- tToNode;
			return false;
		}
	}

	return true;
}

bool CRunwaySystem::ApplyForLandingTime(AirsideFlightInSim* pFlight,ElapsedTime timeToLanding, TimeRange& landingTimeRange,std::vector<RunwayOccupyInfo>& vRunwayArrangeInfo)
{

	//ASSERT(pFlight->GetUID() != 130);
	vRunwayArrangeInfo.clear();
	LogicRunwayInSim *pLandingRunway = pFlight->GetLandingRunway();		

	if (pLandingRunway == NULL)
		return false;

	RunwayInSim *pRunwayInSim = pLandingRunway->GetRunwayInSim();
	if(pRunwayInSim == NULL)
		return false;

	if(pRunwayInSim->GetRunwayState() == RunwayInSim::RunwayState_WaveCross)
	{
		pRunwayInSim->AddWaitingFlight(pFlight);
		return false;
	}

	OccupancyInstance _instance = pLandingRunway->GetOccupyInstance(pFlight);
	if (_instance.GetFlight() == pFlight && _instance.GetEnterTime() == timeToLanding)
	{
		landingTimeRange.SetStartTime(timeToLanding);
		landingTimeRange.SetEndTime(_instance.GetExitTime());
		return true;
	}

	//The time need to landing
	ElapsedTime eTimeStayOnRunway = GetFlightMinTimeInRunway(pFlight);

	std::vector<IntersectionNodeInSim*> vIntNodes;
	vIntNodes.clear();
	GetLogicRunwayIntersectionNodeList(pLandingRunway, vIntNodes);	

	double dRwyDist = pFlight->GetPerformance()->getDistThresToTouch(pFlight);

	TimeRange timeRange;
	while (true)
	{
		ElapsedTime tRelatedNodeDelay = GetFlightDelayTimeAtRunwayRelatedIntersections(pFlight,timeToLanding, dRwyDist);

		if (tRelatedNodeDelay > 0L)
			timeToLanding += tRelatedNodeDelay;

		timeRange.SetStartTime(ElapsedTime(0L));
		timeRange.SetEndTime(ElapsedTime(0L));

		//handle the current runway	
		{
			PerformanceApproachSeparation(pFlight,pLandingRunway,NULL,eTimeStayOnRunway,timeToLanding,timeRange);
			if(timeRange.GetStartTime() < timeToLanding && timeRange.GetEndTime() >= timeToLanding)
				timeRange.SetStartTime(timeToLanding);

			TimeRange timeRangeOpp;
			PerformanceApproachSeparation(pFlight,pLandingRunway->GetOtherPort(),NULL,eTimeStayOnRunway,timeToLanding,timeRangeOpp);
			if (CheckTimeRangeAvaiable(timeRange,timeRangeOpp) == false)
			{
				timeToLanding = timeRange.GetStartTime()> timeRangeOpp.GetStartTime()? timeRange.GetStartTime():timeRangeOpp.GetStartTime() ;
				continue;
			}
		}

		if (!vIntNodes.empty())
		{		
			if (!IsLandingFlightCrossingRunwayIntersectionNodes(pFlight,pLandingRunway,timeToLanding,vIntNodes))
				continue;
		}

		//user defined relate runways
		std::vector<int > vConcernRunway;
		std::vector<std::pair<int,RUNWAY_MARK> > vNotConcernLandRunway;
		std::vector<std::pair<int,RUNWAY_MARK> > vNotConcernTakeOffRunway;
		m_pApproachSeparationInSim->GetRelatedRunwayNotConcern(pLandingRunway,vConcernRunway,vNotConcernLandRunway,vNotConcernTakeOffRunway);
	
		//if p			m_pTakeOffSeparationInSim->GetRelatedRunwayNotConcern(pTakeOffRunway,vConcernRunway,vNotConcernLandRunway,vNotConcernTakeOffRunway);
		for (int nLand =0; nLand < (int)vNotConcernLandRunway.size(); ++ nLand )
		{
			TimeRange timeRange1;
			LogicRunwayInSim *pNotConcernPort = GetLogicRunwayResource(vNotConcernLandRunway[nLand].first,vNotConcernLandRunway[nLand].second);
			PerformanceApproachSeparation(pFlight,pNotConcernPort,NULL,eTimeStayOnRunway,timeToLanding,timeRange1);
			if (CheckTimeRangeAvaiable(timeRange,timeRange1) == false)
			{
				timeToLanding = timeRange.GetStartTime()> timeRange1.GetStartTime()? timeRange.GetStartTime():timeRange1.GetStartTime() ;
				continue;
			}
		}

		for (int nTakeOff =0; nTakeOff < (int)vNotConcernTakeOffRunway.size(); ++ nTakeOff )
		{	
			TimeRange timeRange1;
			LogicRunwayInSim *pNotConcernPort = GetLogicRunwayResource(vNotConcernLandRunway[nLand].first,vNotConcernLandRunway[nLand].second);
			PerformanceApproachSeparation(pFlight,pNotConcernPort,NULL,eTimeStayOnRunway,timeToLanding,timeRange1);
			if (CheckTimeRangeAvaiable(timeRange,timeRange1) == false)
			{
				timeToLanding = timeRange.GetStartTime()> timeRange1.GetStartTime()? timeRange.GetStartTime():timeRange1.GetStartTime() ;
				continue;
			}
		}
		//if proceed to this line , the time is available
		break;
	}
	landingTimeRange = timeRange;

	//pLandingRunway->AddFlightOccupyRunwayInfo(pFlight,vRunwayArrangeInfo);
	pLandingRunway->SetEnterTime(pFlight,landingTimeRange.GetStartTime(),OnLanding);
	pLandingRunway->SetExitTime(pFlight,landingTimeRange.GetEndTime());

	return true;
}

bool CRunwaySystem::CheckTimeRangeAvaiable(TimeRange& timeRange, TimeRange compareTimeRange)
{
	if (timeRange.GetEndTime() >= 0L && timeRange.GetEndTime() <= compareTimeRange.GetStartTime())
		return false;

	if (compareTimeRange.GetEndTime() >=0L && compareTimeRange.GetEndTime() <= timeRange.GetStartTime())
		return false;

	ElapsedTime tStart = timeRange.GetStartTime() > compareTimeRange.GetStartTime() ? timeRange.GetStartTime():compareTimeRange.GetStartTime();
	ElapsedTime tEnd = timeRange.GetEndTime() < compareTimeRange.GetEndTime()? timeRange.GetEndTime():compareTimeRange.GetEndTime();

	timeRange.SetStartTime(tStart);
	timeRange.SetEndTime(tEnd);

	return true;
	//if (timeRange.GetStartTime()<compareTimeRange.GetStartTime())
	//	timeRange.SetStartTime(compareTimeRange.GetStartTime());

	//if (timeRange.GetEndTime() == ElapsedTime(-1L))
	//	timeRange.SetEndTime(compareTimeRange.GetEndTime());
	//else 
	//{
	//	if (compareTimeRange.GetEndTime() == ElapsedTime(-1L))
	//	{

	//	}
	//	else
	//	{
	//		if(timeRange.GetEndTime() >=compareTimeRange.GetEndTime())
	//		{
	//			timeRange.SetEndTime(compareTimeRange.GetEndTime());
	//		}
	//		else
	//		{
	//			return false;
	//		}

	//	}
	//}
	//return true;
}
//return the time range

//get the RunwayInSims  from runway ids
void CRunwaySystem::GetRunwayIntersectionNodes(LogicRunwayInSim *pCurLandingRunway ,std::vector<std::pair<RunwayInSim*, IntersectionNodeInSim * > >& vRunways)
{
	vRunways.clear();

	std::vector<std::pair<int,IntersectionNodeInSim *> >  vRunwayIDs;
	pCurLandingRunway->GetIntersectRunways(vRunwayIDs);
	
	size_t nRunwayCount = vRunwayIDs.size();
	for (size_t nRunway = 0; nRunway < nRunwayCount; ++ nRunway)
	{
		RunwayInSim *pRunwayInSim = GetRunwayResource(vRunwayIDs[nRunway].first);
		
		vRunways.push_back(std::make_pair(pRunwayInSim,vRunwayIDs[nRunway].second));
	}
}

ElapsedTime CRunwaySystem::GetFlightMinTimeInRunway(AirsideFlightInSim *pFlight)
{
	double dTouchDownDistance = pFlight->GetTouchDownDistAtRuwnay(pFlight->GetLandingRunway());//pFlight->GetPerformance()->getDistThresToTouch(pFlight);
	double dExitDistance = pFlight->GetRunwayExit()->GetExitRunwayPos();

	//average speed in runway 
	double dSpeed = (pFlight->GetPerformance()->getLandingSpeed(pFlight) + pFlight->GetPerformance()->getExitSpeed(pFlight))/2.0;
	
	ElapsedTime elapTime = ElapsedTime((dExitDistance - dTouchDownDistance)/dSpeed);

	return elapTime;
}

void CRunwaySystem::PerformanceApproachSeparation( AirsideFlightInSim *pFlight,
												  const LogicRunwayInSim *pLogicRunway,
												  const IntersectionNodeInSim *pNode,
												  ElapsedTime needTimeStayInRunway,
												  ElapsedTime& timelanding,
												  TimeRange& timeRange)
{
	OccupancyTable occTable;
	if (pNode == NULL)
	{
		occTable = pLogicRunway->GetOccupancyTable();//the runway itself
	}
	else
	{
		occTable = pNode->GetOccupancyTable();//intersected runway
	}

	//print occupancy table
	//{
	//	//OccupancyTable tempOccupancyTable = pLogicRunway->GetOccupancyTable();

	//	size_t ntempCount = occTable.size();

	//	for (size_t tempi = 0; tempi < ntempCount; ++ tempi)
	//	{
	//		OccupancyInstance temInstance = occTable.at(tempi);
	//		TRACE("Flight ID: %d     %d  -  %d \n",temInstance.GetFlight()->GetUID(),(long)temInstance.GetEnterTime(),(long)temInstance.GetExitTime());
	//	}
	//	TRACE("\n");

	//}	  
	int nIdx = GetFrontFlightOccupancyInstanceIndex(occTable,timelanding);
	ElapsedTime tAvailLandingTime = timelanding;
	ElapsedTime tEndTime = -1L;

	int nCount = occTable.size();
	for (int i = nIdx; i < nCount; i++)
	{
		OccupancyInstance frontIns;
		OccupancyInstance afterIns;

		if (i >=0)
			frontIns = occTable.at(i);
		if (i < nCount-1)
			afterIns = occTable.at(i+1);

		AirsideFlightInSim* pFrontFlight = frontIns.GetFlight();
		AirsideFlightInSim* pAfterFlight = afterIns.GetFlight();

		if (pFrontFlight && pFrontFlight != pFlight)
		{
			double dSepTime = 60;
			m_pApproachSeparationInSim->GetApproachSeparationTime(pFlight,pFrontFlight,(AirsideMobileElementMode)frontIns.GetOccupyType(),dSepTime);

			if (frontIns.GetOccupyType() == OnTakeoff && pNode == NULL)
			{
				if (pFrontFlight->getRealOpTimeOnRunway() + ElapsedTime(dSepTime) > tAvailLandingTime)
				{
					tAvailLandingTime = pFrontFlight->getRealOpTimeOnRunway() + ElapsedTime(dSepTime);
				}
			}
			else
			{
				if (pNode != NULL)		//if node separation neglect the same runway flight operation
				{
					if ((frontIns.GetOccupyType() == OnTakeoff && pLogicRunway != pFrontFlight->GetAndAssignTakeoffRunway())
						|| (frontIns.GetOccupyType() == OnLanding && pLogicRunway != pFrontFlight->GetLandingRunway()))
					{
						if (frontIns.GetEnterTime() + ElapsedTime(dSepTime) > tAvailLandingTime)			//can not keep separation to front flight at intersection node
						{
							tAvailLandingTime = frontIns.GetEnterTime() + ElapsedTime(dSepTime);
						}
					}
				}
				else
				{
					if (frontIns.GetEnterTime() + ElapsedTime(dSepTime) > tAvailLandingTime)			//can not keep separation to front flight at intersection node
					{
						tAvailLandingTime = frontIns.GetEnterTime() + ElapsedTime(dSepTime);
					}
				}

			}		
		}

		if (pAfterFlight && pAfterFlight != pFlight)
		{
			double dSepTime = 60;
			if(afterIns.GetOccupyType() == OnLanding)		//landing behind landing
			{
				if ((pNode != NULL && pLogicRunway != pAfterFlight->GetLandingRunway())	//if node separation neglect the same runway flight operation
					|| pNode == NULL)
				{
					m_pApproachSeparationInSim->GetApproachSeparationTime(pAfterFlight,pFlight,OnLanding,dSepTime);
					tEndTime = afterIns.GetEnterTime();
				}

			}
			else			//takeoff behind landing
			{
				if ((pNode != NULL && pLogicRunway != pAfterFlight->GetAndAssignTakeoffRunway())	//if node separation neglect the same runway flight operation
					|| pNode == NULL)
				{
					m_pTakeOffSeparationInSim->GetTakeoffSeparationTime(pAfterFlight,pFlight,OnLanding,dSepTime);
					tEndTime = afterIns.GetEnterTime();
				}
			}

			if (afterIns.GetEnterTime() - ElapsedTime(dSepTime) < tAvailLandingTime)			//can not keep separation to front flight
				continue;
		}

		if (pAfterFlight == NULL)		//no after flight, needn't end time
		{
			tEndTime = -1L;
		}

		if (tEndTime > 0L && (tEndTime - tAvailLandingTime) < needTimeStayInRunway)		//not enough take off time
			continue;

		break;
	}

	if (timelanding < tAvailLandingTime)
		timelanding = tAvailLandingTime;


	timeRange.SetStartTime(tAvailLandingTime);
	timeRange.SetEndTime(tEndTime);
}


int CRunwaySystem::GetFrontFlightOccupancyInstanceIndex(const OccupancyTable& occupancyTable,ElapsedTime elapTime)
{
	size_t nOccCount = occupancyTable.size();

	for (int i = (int)nOccCount - 1; i >= 0; --i)
	{
		if (occupancyTable[i].GetEnterTime() <= elapTime)
		{
			return i;
		}
	}

	return -1;			//no occupancy instance before the time
}

void CRunwaySystem::GetLogicRunwayIntersectionNodeList(LogicRunwayInSim* pLogicRunway, std::vector<IntersectionNodeInSim*>& vNodes)
{
	std::vector<std::pair<RunwayInSim*, IntersectionNodeInSim * > > vIntRunways;
	GetRunwayIntersectionNodes(pLogicRunway,vIntRunways);
	size_t nIntRunwayCount = vIntRunways.size();
	IntersectionNodeInSim* pFirstNode = NULL;
	double dMinDistToNode = pLogicRunway->GetEndDist();
	for (size_t nIntRunway = 0; nIntRunway < nIntRunwayCount; ++ nIntRunway )
	{
		std::pair<RunwayInSim *,IntersectionNodeInSim *> pairIntRunway = vIntRunways[nIntRunway];
		IntersectionNodeInSim *pIntNode = pairIntRunway.second;
		//double dDist = pLogicRunway->GetPointDist(pIntNode->GetNodeInput().GetPosition());
		//if (dDist < dMinDistToNode)
		//{
		//	dMinDistToNode = dDist;
		//	pFirstNode = pIntNode;
		//}
		vNodes.push_back(pIntNode);
	}

	//return pFirstNode;
}

AirsideFlightInSim*  CRunwaySystem::GetFirstFlightBeforeTime(const OccupancyTable& occupancyTable,ElapsedTime elapTime,OccupancyInstance& occuInstance)
{
	AirsideFlightInSim *pFlight = NULL;
	size_t nOccCount = occupancyTable.size();
	
	for (int i = (int)nOccCount - 1; i >= 0; --i)
	{
		if (occupancyTable[i].GetEnterTime() <= elapTime)
		{
			pFlight = occupancyTable[i].GetFlight();
			occuInstance = occupancyTable[i];
			break;
		}
	}

	return pFlight;
}
AirsideFlightInSim*  CRunwaySystem::GetFirstLandingFlightBeforeTime(const OccupancyTable& occupancyTable,ElapsedTime elapTime,OccupancyInstance& occuInstance)
{
	AirsideFlightInSim *pFlight = NULL;
	size_t nOccCount = occupancyTable.size();

	for (int i = (int)nOccCount - 1; i >= 0; --i)
	{
		if (occupancyTable[i].GetEnterTime() <= elapTime&&occupancyTable[i].GetOccupyType() == OnLanding)
		{
			pFlight = occupancyTable[i].GetFlight();
			occuInstance = occupancyTable[i];
			break;
		}
	}

	return pFlight;
}
AirsideFlightInSim*  CRunwaySystem::GetFirstTakeOffFlightBeforeTime(const OccupancyTable& occupancyTable,ElapsedTime elapTime,OccupancyInstance& occuInstance)
{
	AirsideFlightInSim *pFlight = NULL;
	size_t nOccCount = occupancyTable.size();

	for (int i = (int)nOccCount - 1; i >= 0; --i)
	{
		if (occupancyTable[i].GetEnterTime() <= elapTime && occupancyTable[i].GetOccupyType() == OnTakeoff)
		{
			pFlight = occupancyTable[i].GetFlight();
			occuInstance = occupancyTable[i];
			break;
		}
	}

	return pFlight;
}

AirsideFlightInSim*  CRunwaySystem::GetFirstFlightAfterTime(const OccupancyTable& occupancyTable,ElapsedTime elapTime,OccupancyInstance& occuInstance)
{
	AirsideFlightInSim *pFlight = NULL;
	size_t nOccCount = occupancyTable.size();

	for (size_t i = 0; i < nOccCount; ++i)
	{
		if (occupancyTable[i].GetEnterTime() >= elapTime)
		{
			pFlight = occupancyTable[i].GetFlight();
			occuInstance = occupancyTable[i];
			break;
		}
	}

	return pFlight;
}
AirsideFlightInSim*  CRunwaySystem::GetFirstTakeOffFlightAfterTime(const OccupancyTable& occupancyTable,ElapsedTime elapTime,OccupancyInstance& occuInstance)
{
	AirsideFlightInSim *pFlight = NULL;
	size_t nOccCount = occupancyTable.size();

	for (size_t i = 0; i < nOccCount; ++i)
	{
		if (occupancyTable[i].GetEnterTime() >= elapTime&&occupancyTable[i].GetOccupyType() == OnTakeoff)
		{
			pFlight = occupancyTable[i].GetFlight();
			occuInstance = occupancyTable[i];
			break;
		}
	}

	return pFlight;
}
AirsideFlightInSim*  CRunwaySystem::GetFirstLandingFlightAfterTime(const OccupancyTable& occupancyTable,ElapsedTime elapTime,OccupancyInstance& occuInstance)
{
	AirsideFlightInSim *pFlight = NULL;
	size_t nOccCount = occupancyTable.size();

	for (size_t i = 0; i < nOccCount; ++i)
	{
		if (occupancyTable[i].GetEnterTime() >= elapTime&&occupancyTable[i].GetOccupyType() == OnLanding)
		{
			pFlight = occupancyTable[i].GetFlight();
			occuInstance = occupancyTable[i];
			break;
		}
	}

	return pFlight;
}

LogicRunwayInSim *CRunwaySystem::GetLogicRunwayResource(int nRunwayID,RUNWAY_MARK runwayMark)
{
	size_t nRunwaySize = m_vRunwayInSim.size();

	for (size_t i = 0; i < nRunwaySize; ++ i)
	{
		LogicRunwayInSim *pPort = m_vRunwayInSim.at(i);
		if (pPort->GetRunwayInSim()->GetRunwayInput()->getID() == nRunwayID && pPort->getLogicRunwayType() == runwayMark )
		{
			return pPort;
		}
	}
	
	return NULL;
}
RunwayInSim *CRunwaySystem::GetRunwayResource(int nRunwayID)
{
	RunwayInSim *pRunwayInSim = NULL;
	size_t nRunwaySize = m_vRunwayInSim.size();

		for (size_t i = 0; i < nRunwaySize; ++ i)
		{
			RunwayInSim *pTempRunwayInSim = m_vRunwayInSim.at(i)->GetRunwayInSim();
			if (pTempRunwayInSim->GetRunwayInput()->getID() == nRunwayID)
			{

				pRunwayInSim = pTempRunwayInSim;
				break;
			}
		}
	
	return pRunwayInSim;
}

//CApproachRunwayNode *CRunwaySystem::GetApproachRules(std::vector<CApproachRunwayNode *>& runwayRules, int nRunwayID, RUNWAY_MARK runwaymark)
//{
//	CApproachRunwayNode *pReturnRunwayNode = NULL;
//
//	size_t nSize = runwayRules.size();
//	for (size_t i = 0; i < nSize ;++ i)
//	{
//		CApproachRunwayNode *pRunwayNode = runwayRules.at(i);
//		if (pRunwayNode->m_nSecondRunwayID == nRunwayID &&
//			pRunwayNode->m_emSecondRunwayMark == runwaymark	)
//		{
//			pReturnRunwayNode = pRunwayNode;
//			//remove 
//			runwayRules.erase(runwayRules.begin() + i);
//			break;
//		}
//	}
//	return pReturnRunwayNode;
//}

bool CRunwaySystem::GetRunwayOccupancyInstance(LogicRunwayInSim* pRunwayInSim,ElapsedTime elpTimeBegin,OccupancyTable orderOccupancyTable)
{

	//std::vector<TimeRange > vOccupancyTimeRange;
	const OccupancyTable& occupancyTable = pRunwayInSim->GetOccupancyTable();
//	OccupancyTable::const_iterator const_iter = OccupancyTable.begin();
	size_t nInstanceCount = occupancyTable.size();
	
	//for (;const_iter != occupancyTable.end(); ++const_iter)
	for(size_t nInstance = 0; nInstance < nInstanceCount; ++nInstance)
	{
		OccupancyInstance instance = occupancyTable.at(nInstance);

		if (instance.GetExitTime() >= elpTimeBegin )
		{

			//add in order
			if (orderOccupancyTable.size() == 0)
			{
				orderOccupancyTable.push_back(instance);
			}
			else
			{
				bool bInsert = false;
				for (size_t i =0; i< orderOccupancyTable.size(); ++i)
				{
					if (orderOccupancyTable.at(i).GetEnterTime() >= instance.GetEnterTime())
					{
						orderOccupancyTable.insert(orderOccupancyTable.begin() + i,instance);
						bInsert = true;
					}
				}

				if (bInsert == false)
				{
					orderOccupancyTable.push_back(instance);
				}
			}
		}
	}
	
	return true;
}

bool CRunwaySystem::IsTakeoffFlightCrossingRunwayIntersectionNodes(AirsideFlightInSim* pFlight,LogicRunwayInSim* pTakeOffRunway,ElapsedTime tTimeToRwy, ElapsedTime& timeToTakeOff, 
																   ElapsedTime& eTempEnterRunwayTime, const std::vector<IntersectionNodeInSim*>& vIntNodes)
{
	int nSize = vIntNodes.size();
	for (int idx =0; idx < nSize; idx++)
	{
		IntersectionNodeInSim* pIntNode = vIntNodes.at(idx);

		double dDistToNode = pTakeOffRunway->GetPointDist(pIntNode->GetNodeInput().GetPosition()) -pFlight->GetAndAssignTakeoffPosition()->GetPosAtRunway();
		DynamicMovement flightmove(0,pFlight->GetLiftoffSpeed(),pFlight->GetTakeoffRoll());
		ElapsedTime tToNode = 0L;
		if (dDistToNode <= pFlight->GetTakeoffRoll())
		{
			tToNode = flightmove.GetDistTime(dDistToNode);
		}
		else
		{
			tToNode = flightmove.GetDistTime(pFlight->GetTakeoffRoll());

			double dDistLiftToNode = dDistToNode - pFlight->GetTakeoffRoll() - pFlight->GetAndAssignTakeoffPosition()->GetPosAtRunway();
			//double dAcc = (pFlight->GetPerformance()->getMaxHorizontalAccel(pFlight)+ pFlight->GetPerformance()->getMinHorizontalAccel(pFlight))/2;
			//double dSpdAtNode = sqrt(2*dAcc*dDistLiftToNode + pFlight->GetLiftoffSpeed()* pFlight->GetLiftoffSpeed());
			//tToNode += ElapsedTime(abs(dSpdAtNode-pFlight->GetLiftoffSpeed())/dAcc);
			tToNode += ElapsedTime((long)dDistLiftToNode/pFlight->GetLiftoffSpeed());
		}

		TimeRange timeRange1;	//the time range is node available time range
		ElapsedTime tTimeAtNode = timeToTakeOff + tToNode;
		PerformanceTakeOffSeparation(pFlight,pTakeOffRunway,pIntNode,tTimeToRwy,1L,tTimeAtNode,timeRange1,false,eTempEnterRunwayTime);		//judge node time, assume node staying time 1s
		if (timeToTakeOff < (tTimeAtNode- tToNode))		//the node with conflict and need delay
		{
			ElapsedTime tDelay = tTimeAtNode - tToNode - timeToTakeOff;
			timeToTakeOff += tDelay;
			//eEnterRunwayTime += tDelay;
			return false;
		}

	}

	return true;
}

bool CRunwaySystem::ApplyForTakeoffTime(AirsideFlightInSim* pFlight,
										ElapsedTime tTimeToRwy,
										ElapsedTime timeToTakeOff, 
										TimeRange& takeoffTimeRange,
										std::vector<RunwayOccupyInfo>& vRunwayArrangeInfo)
{

	//ASSERT(pFlight->GetUID() != 36);
	//it is the time that flight after enter runway, how much time need to prepare for take off
	//at the time, it include the TakeoffPositionTime primary, if the flight was serviced by tow truck,need to include the unhook time
	ElapsedTime eFlightPrepareTimeToTakeOff = ElapsedTime(0L);

	//get time in runway	
	RunwayExitInSim *pTakeOffPos = pFlight->GetAndAssignTakeoffPosition();
	double dSpeed = pFlight->GetLiftoffSpeed();
	ElapsedTime needTimeToTakeOff(300L) ;
	if (dSpeed > 0)
	{
		double dTime = (pFlight->GetTakeoffRoll()*2)/dSpeed;
		needTimeToTakeOff = ElapsedTime(dTime + pFlight->GetTakeoffPositionTime());
		eFlightPrepareTimeToTakeOff += ElapsedTime(pFlight->GetTakeoffPositionTime());//prepare time - add take off position time
		if (pFlight->GetTowingServiceVehicle() && pFlight->GetTowingServiceVehicle()->GetReleasePoint() == pFlight->GetAndAssignTakeoffRunway())		//towing to runway
		{
			needTimeToTakeOff += pFlight->GetUnhookTime();
			//prepare time + unhook time
			eFlightPrepareTimeToTakeOff += pFlight->GetUnhookTime();
		}
	}

	LogicRunwayInSim *pTakeOffRunway = pFlight->GetAndAssignTakeoffRunway();
	
	if(pTakeOffRunway)
	{
		RunwayInSim *pRunwayInSim = pTakeOffRunway->GetRunwayInSim();
		if(pRunwayInSim == NULL)
			return false;
			
		if(pRunwayInSim->GetRunwayState() == RunwayInSim::RunwayState_WaveCross)
		{
			pRunwayInSim->AddWaitingFlight(pFlight);		
			return false;
		}

	}

	//only take effort while the pre-flight is take off, it means after the pre-flight moves for x seconds, the flight moves to runway
	ElapsedTime eEnterRunwayTime = timeToTakeOff;	
	ElapsedTime eTempEnterRunwayTime = timeToTakeOff;//this only for parameter use
	ElapsedTime tStart = timeToTakeOff;
	AirsideFlightRunwayDelayLog* pLog = NULL;
	std::vector<IntersectionNodeInSim*> vIntNodes;
	vIntNodes.clear();
	GetLogicRunwayIntersectionNodeList(pTakeOffRunway, vIntNodes);	

	while (true)
	{

		//handle the current runway
		ElapsedTime eTimeStayOnRunway = needTimeToTakeOff;
		TimeRange& timeRange = takeoffTimeRange;

		PerformanceTakeOffSeparation(pFlight,pTakeOffRunway,NULL,tTimeToRwy,eTimeStayOnRunway,timeToTakeOff,timeRange,true,eEnterRunwayTime);

		TimeRange timeRangeOpp;	
		PerformanceTakeOffSeparation(pFlight,pTakeOffRunway->GetOtherPort(),NULL,tTimeToRwy,eTimeStayOnRunway,timeToTakeOff,timeRangeOpp,true,eTempEnterRunwayTime);
		if (CheckTimeRangeAvaiable(timeRange,timeRangeOpp) == false)
		{
			timeToTakeOff = timeRange.GetStartTime()> timeRangeOpp.GetStartTime()? timeRange.GetStartTime():timeRangeOpp.GetStartTime() ;
			continue;
		}

		if (!vIntNodes.empty())
		{
			if (!IsTakeoffFlightCrossingRunwayIntersectionNodes(pFlight,pTakeOffRunway,tTimeToRwy,timeToTakeOff,eTempEnterRunwayTime, vIntNodes))
				continue;
		}

		//user defined relate runways
		std::vector<int > vConcernRunway;
// 		for (int nConvern = 0;nConvern < (int)m_vRunwayInSim.size(); nConvern++)
// 		{
// 			LogicRunwayInSim* pLogicRunway = m_vRunwayInSim[nConvern];
// 			if (pLogicRunway->GetRunwayInSim() != pTakeOffRunway->GetRunwayInSim())
// 			{
// 				int nConcernID = pLogicRunway->GetRunwayInSim()->GetRunwayID();
// 				if (std::find(vConcernRunway.begin(),vConcernRunway.end(),nConcernID) == vConcernRunway.end())
// 				{
// 					vConcernRunway.push_back(nConcernID);
// 				}
// 			}
// 		}

		int nConcernID = pTakeOffRunway->GetRunwayInSim()->GetRunwayID();
		vConcernRunway.push_back(nConcernID);
		std::vector<std::pair<int,RUNWAY_MARK> > vNotConcernLandRunway;
		std::vector<std::pair<int,RUNWAY_MARK> > vNotConcernTakeOffRunway;
		m_pTakeOffSeparationInSim->GetRelatedRunwayNotConcern(pTakeOffRunway,vConcernRunway,vNotConcernLandRunway,vNotConcernTakeOffRunway);
		for (int nLand =0; nLand < (int)vNotConcernLandRunway.size(); ++ nLand )
		{
			TimeRange timeRange1;	//the time range is node available time range
			LogicRunwayInSim *pNotConcernPort = GetLogicRunwayResource(vNotConcernLandRunway[nLand].first,vNotConcernLandRunway[nLand].second);
			PerformanceTakeOffSeparation(pFlight,pNotConcernPort,NULL,tTimeToRwy,eTimeStayOnRunway,timeToTakeOff,timeRange1,false,eTempEnterRunwayTime);
			if (CheckTimeRangeAvaiable(timeRange,timeRange1) == false)
			{
				timeToTakeOff = timeRange.GetStartTime()> timeRange1.GetStartTime()? timeRange.GetStartTime():timeRange1.GetStartTime() ;
				continue;
			}
		}

		for (int nTakeOff =0; nTakeOff < (int)vNotConcernTakeOffRunway.size(); ++ nTakeOff )
		{
			TimeRange timeRange1;	//the time range is node available time range
			LogicRunwayInSim *pNotConcernPort = GetLogicRunwayResource(vNotConcernTakeOffRunway[nTakeOff].first,vNotConcernTakeOffRunway[nTakeOff].second);
			PerformanceTakeOffSeparation(pFlight,pNotConcernPort,NULL,tTimeToRwy,eTimeStayOnRunway,timeToTakeOff,timeRange1,false,eTempEnterRunwayTime);
			if (CheckTimeRangeAvaiable(timeRange,timeRange1) == false)
			{
				timeToTakeOff = timeRange.GetStartTime()> timeRange1.GetStartTime()? timeRange.GetStartTime():timeRange1.GetStartTime() ;
				continue;
			}
		}
	
		//}
		//if proceed to this line , the time is available
		break;
	}

	if (eEnterRunwayTime - tStart > 0l)	//hold short delay
	{
		ElapsedTime tDelay = eEnterRunwayTime - tStart;
		ResourceDesc resDesc;
		pFlight->GetAndAssignTakeoffRunway()->getDesc(resDesc);
		pLog = new AirsideFlightRunwayDelayLog(resDesc, tStart.asSeconds(), OnTakeoff, eEnterRunwayTime.asSeconds(), tDelay.asSeconds(), FlightRunwayDelay::AtHoldShort);
		pLog->sReasonDetail = "Runway occupied";
		pFlight->LogEventItem(pLog);
	}

	pFlight->setTOStartPrepareTime(0L);			//reset the start prepare time
	if(eEnterRunwayTime > ElapsedTime(0L) && eEnterRunwayTime < timeToTakeOff)			//if the enter runway time earlier than take off time
	{
		ASSERT(eEnterRunwayTime >= pFlight->GetTime());		//error

		if(eEnterRunwayTime < pFlight->GetTime())			//if enter runway time, less than current flight time, use flight current time
			eEnterRunwayTime = pFlight->GetTime();

		pFlight->setTOStartPrepareTime(timeToTakeOff - eEnterRunwayTime);
		takeoffTimeRange.SetStartTime(eEnterRunwayTime);

	}
	pFlight->setRealOpTimeOnRunway(timeToTakeOff);
	return true;
}

bool CRunwaySystem::GetRunwayExitFilletTime(AirsideFlightInSim* pFlight,const LogicRunwayInSim *pLogicRunway,const ElapsedTime& timeTakeoff,ElapsedTime& costTime)
{
	OccupancyTable occTable;
	occTable = pLogicRunway->GetOccupancyTable();

	int nIdx = GetFrontFlightOccupancyInstanceIndex(occTable,timeTakeoff);
	if (nIdx == -1)
		return false;
	
	LogicRunwayInSim* pRunwayInSim = pFlight->GetAndAssignTakeoffRunway();
	RUNWAY_MARK rwyMark = pRunwayInSim->getLogicRunwayType();

	OccupancyInstance occIns = occTable.at(nIdx);
	AirsideFlightInSim* pLandingFlight = occIns.GetFlight();
	if (occIns.GetOccupyType() == OnLanding)
	{
		RunwayExitInSim* pRunwayExit = pLandingFlight->GetRunwayExit();
		CPath2008 filletPath;
		pRunwayExit->getFilletPath(filletPath,rwyMark);
		double dBeginSpeed = pLandingFlight->GetExitSpeed();
		double dEndSpeed = pLandingFlight->GetPerformance()->getTaxiInNormalSpeed(pLandingFlight);
		filletPath.invertList();
		MobileTravelTrace travelTrace(pLandingFlight->mGroundPerform,filletPath.GetTotalLength(),dBeginSpeed,dEndSpeed);
		travelTrace.BeginUse();
		costTime = travelTrace.getEndTime();
		return true;
	}	

	return false;
}

void CRunwaySystem::PerformanceTakeOffSeparation( AirsideFlightInSim *pFlight,
														const LogicRunwayInSim *pLogicRunway,
														const IntersectionNodeInSim *pNode,
														ElapsedTime tTimeToRwy,
														ElapsedTime needTimeStayInRunway,
														ElapsedTime& timeTakeoff,
														TimeRange& timeRange,
														bool bSameRunway,
														ElapsedTime& eEnterRunwayTime)
{	


	OccupancyTable occTable;
	if (pNode == NULL)
	{
		occTable= pLogicRunway->GetOccupancyTable();//runway
	}
	else
	{
		occTable= pNode->GetOccupancyTable();//runway intersection node
	}


	int nIdx = GetFrontFlightOccupancyInstanceIndex(occTable,timeTakeoff);
	ElapsedTime tAvailTakeoffTime = timeTakeoff;
	ElapsedTime tEndTime = -1L;

	AirsideFlightRunwayDelayLog* pLog = NULL;
	ResourceDesc resDesc;
	pFlight->GetAndAssignTakeoffRunway()->getDesc(resDesc);


	int nCount = occTable.size();
	for (int i = nIdx; i < nCount; i++)
	{
		OccupancyInstance frontIns;
		OccupancyInstance afterIns;

		if (i >=0)
			frontIns = occTable.at(i);
		if (i < nCount-1)
			afterIns = occTable.at(i+1);

		AirsideFlightInSim* pFrontFlight = frontIns.GetFlight();
		AirsideFlightInSim* pAfterFlight = afterIns.GetFlight();

		if (pFrontFlight && pFrontFlight != pFlight)
		{
			ElapsedTime tClearPosSepTime = 60L;
			m_pTakeOffSeparationInSim->GetTakeoffClearToPositionSeparationTime(pFlight,pFrontFlight,(AirsideMobileElementMode)frontIns.GetOccupyType(),tClearPosSepTime);
			tClearPosSepTime += tTimeToRwy;
			if (eEnterRunwayTime <  pFrontFlight->getRealOpTimeOnRunway() + tClearPosSepTime)
				eEnterRunwayTime =  pFrontFlight->getRealOpTimeOnRunway() + tClearPosSepTime;

			AirsideMobileElementMode frontMode = (AirsideMobileElementMode)frontIns.GetOccupyType();
			if(bSameRunway )//same runway
			{
				IntersectionNodeInSim* pNode = pFlight->GetAndAssignTakeoffPosition()->GetRouteSeg()->GetEntryNode();
				if (pNode)
				{
					OccupancyInstance frontInstance = pNode->GetOccupyInstance(pFrontFlight);
					ElapsedTime tFrontNode;
					tFrontNode.setPrecisely((long(frontInstance.GetEnterTime() + frontInstance.GetExitTime()))/2);
					if (eEnterRunwayTime < tFrontNode+ tClearPosSepTime)
						eEnterRunwayTime = tFrontNode + tClearPosSepTime;
				}

				double dSepTime = 60;
				//get the separation with front flight
				m_pTakeOffSeparationInSim->GetTakeoffSeparationTime(pFlight,pFrontFlight,frontMode, dSepTime);

				ElapsedTime tSepTime = tClearPosSepTime > ElapsedTime(dSepTime)? tClearPosSepTime:ElapsedTime(dSepTime);

				if (pFrontFlight->getRealOpTimeOnRunway() + tSepTime > tAvailTakeoffTime)			//can not keep separation to front flight
				{
					ElapsedTime tDelay = pFrontFlight->getRealOpTimeOnRunway() + tSepTime - tAvailTakeoffTime;
					////////////////////////runway delay log
					pLog = new AirsideFlightRunwayDelayLog(resDesc, tAvailTakeoffTime.asSeconds(), OnTakeoff, (tAvailTakeoffTime+tDelay).asSeconds(), tDelay.asSeconds(), FlightRunwayDelay::UNKNOWNPOS);
					pLog->sReasonDetail = "Runway occupied";
					pFlight->LogEventItem(pLog);
					////////////////////////
					tAvailTakeoffTime += tDelay ;
				}
			}
			else//intersection node or related runway
			{	
				double dSepTime = 60;
				m_pTakeOffSeparationInSim->GetTakeoffSeparationTime(pFlight,pFrontFlight,frontMode , dSepTime);





				if (pNode != NULL)
				{
					if ((frontIns.GetOccupyType() == OnTakeoff && pLogicRunway != pFrontFlight->GetAndAssignTakeoffRunway())
						|| (frontIns.GetOccupyType() == OnLanding && pLogicRunway != pFrontFlight->GetLandingRunway()))		
					{
						double dSepTime = 60;
						m_pTakeOffSeparationInSim->GetTakeoffSeparationTime(pFlight,pFrontFlight,(AirsideMobileElementMode)frontIns.GetOccupyType() , dSepTime);

 						if (frontIns.GetEnterTime() + ElapsedTime(dSepTime) > tAvailTakeoffTime)			//can not keep separation to front flight at intersection node
 						{
 							tAvailTakeoffTime = frontIns.GetEnterTime() + ElapsedTime(dSepTime);
 						}
					}
				}
				else
				{
					double dSepTime = 60;
					m_pTakeOffSeparationInSim->GetTakeoffSeparationTime(pFlight,pFrontFlight,(AirsideMobileElementMode)frontIns.GetOccupyType() , dSepTime);

// 					if (frontIns.GetEnterTime() + ElapsedTime(dSepTime) > tAvailTakeoffTime)			//can not keep separation to front flight at intersection node
// 					{
// 						tAvailTakeoffTime = frontIns.GetEnterTime() + ElapsedTime(dSepTime);
// 					}
					tAvailTakeoffTime = max(tAvailTakeoffTime,pFrontFlight->getRealOpTimeOnRunway() + ElapsedTime(dSepTime));
				//	ElapsedTime tDelay = frontIns.GetEnterTime() + ElapsedTime(dSepTime) - tAvailTakeoffTime;
					ElapsedTime tDelay = pFrontFlight->getRealOpTimeOnRunway()  + ElapsedTime(dSepTime) - tAvailTakeoffTime;
					////////////////////////runway delay log
					pLog = new AirsideFlightRunwayDelayLog(resDesc, tAvailTakeoffTime.asSeconds(), OnTakeoff, (tAvailTakeoffTime+tDelay).asSeconds(), tDelay.asSeconds(), FlightRunwayDelay::UNKNOWNPOS);
					CString strDtail;
					if (frontMode == OnLanding)
						strDtail.Format("Separated with landing AC %s on runway %s", pFrontFlight->GetCurrentFlightID(),pFrontFlight->GetLandingRunway()->PrintResource());
					else
						strDtail.Format("Separated with takeoff AC %s on runway %s", pFrontFlight->GetCurrentFlightID(),pFrontFlight->GetAndAssignTakeoffRunway()->PrintResource());
					pLog->sReasonDetail = strDtail.GetString();
					pFlight->LogEventItem(pLog);
					////////////////////////
					tAvailTakeoffTime += tDelay ;
				}


			}
		}
		
		if (pAfterFlight && pAfterFlight != pFlight)
		{
			double dSepTime = 60;
			if(afterIns.GetOccupyType() == OnLanding )		//landing behind takeoff
			{
				if ((pNode != NULL && pLogicRunway != pAfterFlight->GetLandingRunway()) || pNode == NULL)
				{
					m_pApproachSeparationInSim->GetApproachSeparationTime(pAfterFlight,pFlight,OnTakeoff,dSepTime);

					if (afterIns.GetEnterTime() - ElapsedTime(dSepTime) < tAvailTakeoffTime)			//can not keep separation to front flight
						continue;
					
					tEndTime = afterIns.GetEnterTime();
				}

			}
			else			//takeoff behind takeoff
			{
				ElapsedTime tClearPosSepTime = 60L;
				m_pTakeOffSeparationInSim->GetTakeoffClearToPositionSeparationTime(pAfterFlight,pFlight,OnTakeoff,tClearPosSepTime);
				tClearPosSepTime += tTimeToRwy;
				m_pTakeOffSeparationInSim->GetTakeoffSeparationTime(pAfterFlight,pFlight,OnTakeoff,dSepTime);

				ElapsedTime tSepTime = tClearPosSepTime > ElapsedTime(dSepTime)? tClearPosSepTime:ElapsedTime(dSepTime);


				if(bSameRunway)			//if on same runway when the after flight real take off the runway must be empty
				{
					if (pAfterFlight->getRealOpTimeOnRunway() - tSepTime < tAvailTakeoffTime)			//can not keep separation to front flight
						continue;
					tEndTime = pAfterFlight->getRealOpTimeOnRunway();
				}
				else
				{
					if (pNode == NULL || (pNode != NULL && pLogicRunway != pAfterFlight->GetAndAssignTakeoffRunway()))
					{
						if (afterIns.GetEnterTime() - tSepTime < tAvailTakeoffTime)			//can not keep separation to front flight
							continue;
					}

					tEndTime = afterIns.GetEnterTime();
				}
			}
		} 
		
		if (pAfterFlight == NULL)		//no after flight, needn't end time
		{
			tEndTime = -1L;
		}

		if (tEndTime > 0L && (tEndTime - tAvailTakeoffTime) < needTimeStayInRunway)		//not enough take off time
			continue;

		//set the flight's pre flight's leave runway time.
		if(pFrontFlight != NULL && bSameRunway)
		{
			pFlight->setPreFlightExitRunwayTime(frontIns.GetExitTime());
		}

		break;
	}

	if (timeTakeoff < tAvailTakeoffTime)
		timeTakeoff = tAvailTakeoffTime;

	if (bSameRunway)
		timeRange.SetStartTime(eEnterRunwayTime);
	else
		timeRange.SetStartTime(tAvailTakeoffTime);
	timeRange.SetEndTime(tEndTime);
}

ClearanceItem CRunwaySystem::GetRunwayTouchdownClearanceItem(AirsideFlightInSim *pFlight, ElapsedTime eLandingTime,ClearanceItem& lastItem)
{
	LogicRunwayInSim* pPort = pFlight->GetLandingRunway();
	double landspd = pFlight->GetPerformance()->getLandingSpeed(pFlight);	
	DistanceUnit touchDist = pFlight->GetTouchDownDistAtRuwnay(pPort);// pFlight->GetPerformance()->getDistThresToTouch(pFlight);

	//landing item
	ClearanceItem landingItem(pPort,OnLanding,touchDist);		
	landingItem.SetSpeed(landspd);
	landingItem.SetPosition(pPort->GetFlightTouchdownPoint(pFlight));
	landingItem.SetDistInResource(touchDist);
	ElapsedTime dT = pFlight->GetTimeBetween(lastItem,landingItem);
	ElapsedTime tEnter = pPort->GetOccupyInstance(pFlight).GetEnterTime();
	landingItem.SetTime(lastItem.GetTime()+ dT);

	return landingItem;
}

//bool CRunwaySystem::GenerateRunwayToCircuitRouteClearance(AirsideCircuitFlightInSim * pFlight, FlightRouteInSim* pCircuitRoute,ClearanceItem& lastItem, Clearance& newClearance)
//{
//	AirRoutePointInSim* pRoutePoint = ((AirRouteSegInSim*)pCircuitRoute->GetItem(0))->GetFirstConcernIntersection()->getInputPoint();
//
//	ClearanceItem climbout(pRoutePoint,OnClimbout,0);		
//	climbout.SetSpeed(lastItem.GetSpeed());
//	double dAlt = pCircuitRoute->GetRoutePointProperty(pRoutePoint).GetAlt();
//	climbout.SetAltitude(dAlt);
//
//	CPoint2008 curPos = lastItem.GetPosition();
//	curPos.setZ(lastItem.GetAltitude());
//	CPoint2008 nextPos = pRoutePoint->GetPosition();
//	nextPos.setZ(dAlt);
//	climbout.SetPosition(nextPos);
//	ElapsedTime intervalTime;
//	if (lastItem.GetSpeed())
//	{
//		intervalTime = ElapsedTime(curPos.distance3D(nextPos)/lastItem.GetSpeed());
//	}
//	climbout.SetTime(lastItem.GetTime() + intervalTime);
//	lastItem = climbout;
//	newClearance.AddItem(lastItem);
//	return true;
//}
//bool CRunwaySystem::GetCircuitFlightRunwayLandingClearance( AirsideCircuitFlightInSim * pFlight, ClearanceItem& lastItem, Clearance& newClearance,AirsideCircuitFlightInSim::LandingOperation emType )
//{
//	switch (emType)
//	{
//	case AirsideCircuitFlightInSim::LowAndOver_Operation:
//		{
//			FlightRouteInSim* pCircuitRoute = pFlight->GetLandingCircuit();
//			GenerateLowAndOverClearance(pFlight,pCircuitRoute,lastItem,newClearance);
//		}
//		break;
//	case AirsideCircuitFlightInSim::TouchAndGo_Operaiton:
//		{
//			GenerateTouchAndGoClearance(pFlight,lastItem,newClearance);
//		}
//		break;
//	case AirsideCircuitFlightInSim::StopAndGo_Operaiton:
//		{
//			GenerateStopAndGoClearance(pFlight,lastItem,newClearance);
//		}
//		break;
//	case AirsideCircuitFlightInSim::ForceAndGo_Operation:
//		{
//			ASSERT(FALSE);
//			//GenerateForceAndGoClearance(pFlight,lastItem,newClearance);
//		}
//		break;
//	case  AirsideCircuitFlightInSim::Normal_Operation:
//		{
//			//GenerateForceAndGoClearance(pFlight,lastItem,newClearance);
//		}
//		break;
//	default:
//		break;
//	}
//	return true;
//}
//
bool CRunwaySystem::GenerateLowAndOverClearance( AirsideCircuitFlightInSim * pFlight,LogicRunwayInSim* pPort, FlightRouteInSim* pCircuitRoute,ClearanceItem& lastItem, Clearance& newClearance )
{
	const ALTAirport& airportInfo = pFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource()->GetAirportInfo();
	//land item
	ClearanceItem landItem(pPort,OnApproach,0.0);		
	landItem.SetSpeed(lastItem.GetSpeed());
	CPoint2008 ptPos = pPort->GetDistancePoint(0.0);
	double dAlt = ptPos.getZ() + pFlight->GetLowHigh() + airportInfo.getElevation();
	landItem.SetPosition(ptPos);
	landItem.SetAltitude(dAlt);
	ElapsedTime dT = pFlight->GetTimeBetween(lastItem,landItem);
	landItem.SetTime(lastItem.GetTime()+ dT);
	lastItem = landItem;
	newClearance.AddItem(lastItem);

	//takeoff item
	ClearanceItem takeoffItem(pPort,OnApproach, pPort->GetEndDist() );		
	takeoffItem.SetSpeed(lastItem.GetSpeed());
	ptPos = pPort->GetDistancePoint(1000);
	takeoffItem.SetPosition(ptPos);
	takeoffItem.SetAltitude(dAlt);
	dT = pFlight->GetTimeBetween(lastItem,takeoffItem);
	takeoffItem.SetTime(lastItem.GetTime()+ dT);
	lastItem = takeoffItem;
	newClearance.AddItem(lastItem);	

	//GetTakeoffFirstWaypointItem

	ClearanceItem clibItem = lastItem;
	if(_GenTakeoffFirstWaypointItem(pFlight,pCircuitRoute,clibItem)){
		lastItem = clibItem;
		newClearance.AddItem(lastItem);
	}	
	return true;
}

void CRunwaySystem::_WriteRunwayNodeTime(AirsideFlightInSim* pFlight , LogicRunwayInSim* pRunway , const ClearanceItem& itemFrom, const ClearanceItem& itemTo)
{
	OccupancyInstance ocyInst = pRunway->GetOccupyInstance(pFlight);

	std::vector<IntersectionNodeInSim *> vAfterIntNodes;
	pRunway->GetIntersectNodeInRange( itemFrom.GetDistInResource() + 1, itemTo.GetDistInResource() ,vAfterIntNodes);
	DynamicMovement dynMove(itemFrom.GetSpeed(), itemTo.GetSpeed(), itemTo.GetDistInResource()-itemFrom.GetDistInResource() );
	for (size_t nNode = 0; nNode < vAfterIntNodes.size(); ++ nNode)
	{
		IntersectionNodeInSim *pIntNode = vAfterIntNodes[nNode];
		if(!pIntNode)continue;

		DistanceUnit nodedistInRw = pRunway->GetPointDist(pIntNode->GetNodeInput().GetPosition() );
		ElapsedTime nodeTime = dynMove.GetDistTime(nodedistInRw-itemFrom.GetDistInResource())+itemFrom.GetTime();
		if(!pIntNode->GetNodeInput().IsRunwaysIntersection())
		{
			pIntNode->SetEnterTime(pFlight, nodeTime -m_eTimeCrossRunwayBuffer  ,OnLanding);
			pIntNode->SetExitTime(pFlight, nodeTime +m_eTimeCrossRunwayBuffer);
		}
		else	//intersection node of runway
		{
			pIntNode->SetEnterTime(pFlight,ocyInst.GetEnterTime() ,OnLanding);
			pIntNode->SetExitTime(pFlight,ocyInst.GetExitTime() );
		}
	}
}


bool CRunwaySystem::GenerateTouchAndGoClearance( AirsideCircuitFlightInSim * pFlight,LogicRunwayInSim* plandingRunway, FlightRouteInSim* pCircuitRoute,ClearanceItem& lastItem, Clearance& newClearance , bool bOccupyRunway)
{
	//LogicRunwayInSim *plandingRunway = pFlight->GetLandingRunway();
	ClearanceItem touchdownItem = GetRunwayTouchdownClearanceItem(pFlight,lastItem.GetTime(),lastItem);
	lastItem = touchdownItem;
	newClearance.AddItem(lastItem);

	ElapsedTime eLandingTime = lastItem.GetTime();	
	pFlight->setRealOpTimeOnRunway(eLandingTime);//why we need this
	double landspd = lastItem.GetSpeed();

	//double dExitDistance = plandingRunway->GetEndDist();
	//calculate rolling distance and time
	//decl to the runway circute touch speed
	double dTouchDownDistance = pFlight->GetTouchDownDistAtRuwnay(plandingRunway);
	double dTouchSpeed = MIN( pFlight->GetTouchSpeed(), landspd );
	double dMaxDecel = m_pRunwayExitStrategy->GetFlightMaxDecelOnRunway(pFlight);

	double dMinLandingDistance = (landspd*landspd - dTouchSpeed*dTouchSpeed)/(2.0* dMaxDecel);
	dMinLandingDistance = MAX(0,dMinLandingDistance);
	
	//touch speed item
	ClearanceItem endDecItem = lastItem;
	if(dMinLandingDistance>0){ //dec to touch speed
		endDecItem.SetDistInResource(dTouchDownDistance+dMinLandingDistance);
		endDecItem.SetPosition( plandingRunway->GetDistancePoint(endDecItem.GetDistInResource()) );
		endDecItem.SetSpeed(dTouchSpeed);
		endDecItem.SetTime(lastItem.GetTime() + pFlight->GetTimeBetween(lastItem,endDecItem) );
		lastItem = endDecItem;
		newClearance.AddItem(lastItem);
	}

	// lift off item
	double dlifeOffSpd = pFlight->GetPerformance()->getLiftoffSpeed(pFlight);
	DistanceUnit rolldist = pFlight->GetTakeoffRoll();
	DistanceUnit dDistToLift = ( 1.0 - (dTouchSpeed/dlifeOffSpd)*(dTouchSpeed/dlifeOffSpd) )*rolldist;
	ClearanceItem liftItem = lastItem;
	liftItem.SetDistInResource( lastItem.GetDistInResource() + dDistToLift );
	liftItem.SetPosition( plandingRunway->GetDistancePoint(liftItem.GetDistInResource()) );
	liftItem.SetSpeed(dlifeOffSpd);
	liftItem.SetTime(lastItem.GetTime()+pFlight->GetTimeBetween(lastItem,liftItem) );
	lastItem = liftItem;
	newClearance.AddItem(lastItem);


	//climb to first waypoint 
	ClearanceItem climbItem = lastItem;
	if(_GenTakeoffFirstWaypointItem(pFlight,pCircuitRoute,climbItem)){
		lastItem = climbItem;
		newClearance.AddItem(lastItem);
	}
	
	//write runway occupy Instance
	if(bOccupyRunway)
	{		
		//set runway time table
		plandingRunway->SetEnterTime(pFlight,touchdownItem.GetTime(),OnLanding);
		plandingRunway->SetExitTime(pFlight,liftItem.GetTime());

		plandingRunway->GetOtherPort()->SetEnterTime(pFlight,touchdownItem.GetTime(),OnLanding);
		plandingRunway->GetOtherPort()->SetExitTime(pFlight,liftItem.GetTime());		
		//node dec to 
		_WritePreNodesTime(pFlight,plandingRunway,touchdownItem);
		_WriteRunwayNodeTime(pFlight,plandingRunway, touchdownItem,endDecItem);
		_WriteRunwayNodeTime(pFlight,plandingRunway, endDecItem,liftItem);
		_WriteAfterNodesTime(pFlight,plandingRunway, liftItem);
		
	}
	return true;
}

bool CRunwaySystem::GenerateStopAndGoClearance( AirsideCircuitFlightInSim * pFlight,LogicRunwayInSim* plandingRunway, FlightRouteInSim* pCircuitRoute,ClearanceItem& lastItem, Clearance& newClearance, bool bOccupyRunway )
{
	//touch down
	ClearanceItem touchdownItem = GetRunwayTouchdownClearanceItem(pFlight,lastItem.GetTime(),lastItem);
	lastItem = touchdownItem;
	newClearance.AddItem(lastItem);

	//???
	pFlight->setRealOpTimeOnRunway(touchdownItem.GetTime());


	//dec to speed 0
	double dTouchDownDistance = touchdownItem.GetDistInResource();	
	double dMaxDecel = m_pRunwayExitStrategy->GetFlightMaxDecelOnRunway(pFlight);
	double landspd = touchdownItem.GetSpeed();
	double dMinLandingDistance = (landspd*landspd)/(2.0* dMaxDecel);
	dMinLandingDistance = MAX(0,dMinLandingDistance);
	//stop item
	ClearanceItem stopItem = lastItem;	
	stopItem.SetDistInResource(dTouchDownDistance+dMinLandingDistance);
	stopItem.SetPosition( plandingRunway->GetDistancePoint(stopItem.GetDistInResource()) );
	stopItem.SetSpeed(0);
	stopItem.SetTime(lastItem.GetTime() + pFlight->GetTimeBetween(lastItem,stopItem) );
	lastItem = stopItem;
	newClearance.AddItem(lastItem);
	//wait item
	ClearanceItem waitItem = stopItem;
	waitItem.SetTime(stopItem.GetTime()+pFlight->GetStopTime());
	lastItem = waitItem ;
	newClearance.AddItem(lastItem);
	//life off
	double dlifeOffSpd = pFlight->GetPerformance()->getLiftoffSpeed(pFlight);
	DistanceUnit dDistToLift = pFlight->GetTakeoffRoll();
	ClearanceItem liftItem = lastItem;
	liftItem.SetDistInResource( lastItem.GetDistInResource() + dDistToLift );
	liftItem.SetPosition( plandingRunway->GetDistancePoint(liftItem.GetDistInResource()) );
	liftItem.SetSpeed(dlifeOffSpd);
	liftItem.SetTime(lastItem.GetTime()+pFlight->GetTimeBetween(lastItem,liftItem) );
	lastItem = liftItem;
	newClearance.AddItem(lastItem);

	//climb to first waypoint 
	ClearanceItem climbItem = lastItem;
	if(_GenTakeoffFirstWaypointItem(pFlight,pCircuitRoute,climbItem)){
		lastItem = climbItem;
		newClearance.AddItem(lastItem);
	}

	
	if(bOccupyRunway)
	{		
		//set runway time table
		plandingRunway->SetEnterTime(pFlight,touchdownItem.GetTime(),OnLanding);
		plandingRunway->SetExitTime(pFlight,liftItem.GetTime());

		plandingRunway->GetOtherPort()->SetEnterTime(pFlight,touchdownItem.GetTime(),OnLanding);
		plandingRunway->GetOtherPort()->SetExitTime(pFlight,liftItem.GetTime());		
		//node dec to 
		_WritePreNodesTime(pFlight,plandingRunway,touchdownItem);
		_WriteRunwayNodeTime(pFlight,plandingRunway, touchdownItem,stopItem);
		_WriteRunwayNodeTime(pFlight,plandingRunway, waitItem,liftItem);
		_WriteAfterNodesTime(pFlight,plandingRunway, liftItem);

	}
	

	return true;
}

//bool CRunwaySystem::GenerateForceAndGoClearance( AirsideCircuitFlightInSim * pFlight, ClearanceItem& lastItem, Clearance& newClearance )
//{
//	LogicRunwayInSim *plandingRunway = pFlight->GetLandingRunway();
//
//	std::vector<IntersectionNodeInSim *> vRunwayIntNodes; //Runway intersection nodes, remember it, after the landing operation, write entry and exit node time
//
//	ElapsedTime eLandingTime = lastItem.GetTime();
//	pFlight->setRealOpTimeOnRunway(eLandingTime);
//	double landspd = lastItem.GetSpeed();
//
//	//calculate rolling distance and time
//	double dTouchDownDistance = pFlight->GetTouchDownDistAtRuwnay(plandingRunway);
//	double dExitDistance = pFlight->GetRunwayExit()->GetExitRunwayPos();
//	double dExitSpeed = pFlight->GetExitSpeed();
//	double dRollingDist;
//	double dMaxDecel = m_pRunwayExitStrategy->GetFlightMaxDecelOnRunway(pFlight);
//
//	double dMinLandingDistance = (landspd*landspd - dExitSpeed*dExitSpeed)/(2.0* dMaxDecel);
//	if(dMinLandingDistance > dExitDistance - dTouchDownDistance)
//		dRollingDist = dMinLandingDistance;
//	else
//		dRollingDist = dExitDistance - dTouchDownDistance;
//
//	double dDecel = (landspd*landspd - dExitSpeed*dExitSpeed)/(2.0* dRollingDist);
//
//
//	//Deceleration
//	std::vector<RunwayOccupyInfo> vIntRunwayNodeNeedWriteOccupancyTable;
//	DistanceUnit dExitDistInRunway = dTouchDownDistance + dRollingDist;
//	{
//		std::vector<IntersectionNodeInSim *> vDecelNodes;
//		plandingRunway->GetIntersectNodeInRange(dTouchDownDistance,dExitDistInRunway + 1,vDecelNodes);
//
//		int nDecelNode =0;
//		for (; nDecelNode< (int)vDecelNodes.size(); ++nDecelNode)
//		{
//			IntersectionNodeInSim *pCurNode = vDecelNodes[nDecelNode];
//			bool bIntRunwayNode = false;
//			int nIntRunwayNode = 0;
//			double dNodePosition = plandingRunway->GetPointDist(pCurNode->GetNodeInput().GetPosition()) ;
//
//			DynamicMovement flightmove(landspd,dExitSpeed,dExitDistInRunway- dTouchDownDistance);
//			ElapsedTime eNodeTime = flightmove.GetDistTime(dNodePosition- dTouchDownDistance);				
//			ElapsedTime tDifference = 0L;			//calculation difference
//			if (pCurNode->GetNodeInput().IsRunwaysIntersection())		//if node is runways intersection, needn't taxi cross buffer
//			{
//				pCurNode->SetEnterTime(pFlight, lastItem.GetTime() + eNodeTime - tDifference,OnLanding);
//				pCurNode->SetExitTime(pFlight, lastItem.GetTime()+ eNodeTime + tDifference);
//			}
//			else
//			{
//				ElapsedTime eT =  lastItem.GetTime() + eNodeTime - m_eTimeCrossRunwayBuffer;
//				ElapsedTime exT = lastItem.GetTime()+ eNodeTime + m_eTimeCrossRunwayBuffer;
//				pCurNode->SetEnterTime(pFlight,eT,OnLanding);
//				pCurNode->SetExitTime(pFlight,exT);
//			}
//		}
//	}
//
//	//backtrack clearance	
//	if( dRollingDist >  dExitDistance - dTouchDownDistance)
//	{		
//		CString strTime = lastItem.GetTime().printTime();
//		char fltstr[255];
//		pFlight->getFullID(fltstr);
//		CString strID;
//		strID.Format("A:%s",fltstr);
//		AirsideDiagnose* pNewDiagnose = new AirsideDiagnose(strTime, strID);		
//		CString strDetals;
//		strDetals.Format("NOT ENOUGH LANDING DISTANCE ON RUNWAY %s", pFlight->GetLandingRunway()->PrintResource() );
//		pNewDiagnose->SetDetails(strDetals);
//		throw new AirsideSystemError(pNewDiagnose);	
//	}
//	//takeoff operation
//	else
//	{
//		ClearanceItem runwayExitItem(plandingRunway,OnExitRunway,dExitDistance);		
//		runwayExitItem.SetSpeed(dExitSpeed);
//		runwayExitItem.SetPosition(plandingRunway->GetDistancePoint(dExitDistance));
//		DynamicMovement flightmove(landspd,dExitSpeed,dRollingDist);
//		ElapsedTime eNodeTime = flightmove.GetDistTime(dRollingDist);				
//		ElapsedTime nextTimeExit = eLandingTime + eNodeTime;
//		runwayExitItem.SetTime(nextTimeExit);
//		newClearance.AddItem(runwayExitItem);
//		lastItem = runwayExitItem;
//	}
//
//
//
//	ElapsedTime nextTimeExit = lastItem.GetTime();
//
//	//only write the runway itself, other runways don't write 
//	for(size_t nRunway = 0; nRunway< 2; ++nRunway)
//	{
//		LogicRunwayInSim* pCurPort = NULL;
//
//		if (nRunway == 0)
//		{
//			pCurPort = pFlight->GetLandingRunway();
//		}
//		else
//		{
//			pCurPort = pFlight->GetLandingRunway()->GetOtherPort();
//		}
//		pCurPort->SetEnterTime(pFlight,eLandingTime,OnLanding);
//		pCurPort->SetExitTime(pFlight,nextTimeExit);
//		//////////////////////////////////////////////////////////////////////////
//
//	}
//	//write intersection runways' time
//
//	for (size_t nNode = 0; nNode < vRunwayIntNodes.size(); ++ nNode)
//	{
//		IntersectionNodeInSim *pIntNode = vRunwayIntNodes[nNode];
//		if(pIntNode)
//		{
//			pIntNode->SetEnterTime(pFlight,eLandingTime,OnLanding);
//			pIntNode->SetExitTime(pFlight,nextTimeExit);
//		}
//	}
//	//write the entry time and exit time to node
//	std::vector<IntersectionNodeInSim *> vPreIntNodes;
//	pFlight->GetLandingRunway()->GetIntersectNodeInRange(0,dTouchDownDistance,vPreIntNodes);
//	for (size_t nNode = 0; nNode < vPreIntNodes.size(); ++ nNode)
//	{
//		IntersectionNodeInSim *pIntNode = vPreIntNodes[nNode];
//		if(pIntNode)
//		{
//			pIntNode->SetEnterTime(pFlight,eLandingTime - m_eTimeCrossRunwayBuffer,OnLanding);
//			pIntNode->SetExitTime(pFlight,eLandingTime + m_eTimeCrossRunwayBuffer);
//		}
//
//	}
//
//	double dLandingRunwayDistance = dExitDistance;
//	if(pFlight->GetLandingRunway())
//		dLandingRunwayDistance = pFlight->GetLandingRunway()->GetEndDist();
//
//	//from exit to runway end
//	//write the entry time and exit time to node
//	std::vector<IntersectionNodeInSim *> vAfterIntNodes;
//	pFlight->GetLandingRunway()->GetIntersectNodeInRange(dExitDistance + 1,dLandingRunwayDistance,vAfterIntNodes);
//	for (size_t nNode = 0; nNode < vAfterIntNodes.size(); ++ nNode)
//	{
//		IntersectionNodeInSim *pIntNode = vAfterIntNodes[nNode];
//		if(pIntNode && !pIntNode->GetNodeInput().IsRunwaysIntersection())
//		{
//			pIntNode->SetEnterTime(pFlight,eLandingTime,OnLanding);
//			pIntNode->SetExitTime(pFlight,nextTimeExit);
//		}
//
//		if(pIntNode && pIntNode->GetNodeInput().IsRunwaysIntersection())		//intersection node of runway
//		{
//			pIntNode->SetEnterTime(pFlight,lastItem.GetTime(),OnLanding);
//			pIntNode->SetExitTime(pFlight,lastItem.GetTime()+1L);
//		}
//
//	}
//
//	for (size_t nIntRunway =0; nIntRunway < vIntRunwayNodeNeedWriteOccupancyTable.size();++nIntRunway)
//	{
//		if (vIntRunwayNodeNeedWriteOccupancyTable[nIntRunway].bLASHO )
//		{
//			vIntRunwayNodeNeedWriteOccupancyTable[nIntRunway].pIntNode->SetExitTime(pFlight,nextTimeExit);
//		}
//		else
//		{
//			vIntRunwayNodeNeedWriteOccupancyTable[nIntRunway].pIntNode->SetExitTime(pFlight,nextTimeExit);
//		}
//	}
//	return true;
//}
//
bool CRunwaySystem::GetRunwayLandingClearance(AirsideFlightInSim * pFlight, ClearanceItem& lastItem, Clearance& newClearance)
{
	RunwayExitInSim* pExit = pFlight->GetRunwayExit();
	LogicRunwayInSim* pLogicRunway = pFlight->GetLandingRunway();

	if (!pLogicRunway->IsRunwayClear(pFlight))		//runway be occupied
	{
		CString strError = "Runway not available to landing!";
		CString strErrorType = "AIRCRAFT TERMINATE";
		AirsideSimErrorShown::SimWarning(pFlight,strError,strErrorType);

		ClearanceItem newItem(NULL,OnTerminate,0);
		newItem.SetTime(lastItem.GetTime());
		newClearance.AddItem(newItem);	
		pFlight->PerformClearance(newClearance);
		return true;
	}

	//CRunwaySystem *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();

	if(pExit && pExit->CanHoldFlight(pFlight))
	{
		bool bBacktrackOp = m_pRunwayExitStrategy->IsBackTrackOp(pFlight);
		WriteRunwayLogs(pFlight,bBacktrackOp,lastItem,newClearance);
	}
	else
	{

		pFlight->GetAirTrafficController()->AssignAvaibleRunwayExit(pFlight);
		//pExit = pLogicRunway->GetAvailableExit(pFlight);
		//if (pExit == NULL)
		//{
		//	CString strError = "No available runway exit!";
		//	CString strErrorType = "AIRCRAFT TERMINATE";
		//	AirsideSimErrorShown::SimWarning(pFlight,strError,strErrorType);

		//	ClearanceItem newItem(NULL,OnTerminate,0);
		//	newItem.SetTime(lastItem.GetTime());
		//	newClearance.AddItem(newItem);	
		//	return;
		//}
		//if (pExit != NULL)
			//pFlight->SetRunwayExit(pExit);
//  		ASSERT(pFlight->GetRunwayExit());

		if (pFlight->GetRunwayExit() == NULL)		// no exit
		{
			CString strWarn;
			strWarn.Format("No available runway exit for the flight on %s!", pFlight->GetLandingRunway()->PrintResource()) ;
			CString strError = _T("FLLIGHT TERMINATE");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);	
			pFlight->GetLandingRunway()->RemoveElementOccupancyInfo(pFlight);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastItem.GetTime());
			newClearance.AddItem(newItem);
			pFlight->PerformClearance(newClearance);
			return true;

		}

		bool bBacktrackOp = m_pRunwayExitStrategy->IsBackTrackOp(pFlight);
		WriteRunwayLogs(pFlight,bBacktrackOp,lastItem,newClearance);
	}
	return true;
}

bool  CRunwaySystem::GenerateLandingCleaerance(AirsideFlightInSim * pFlight,LogicRunwayInSim* plandingRunway,RunwayExitInSim* pRunwayExit,  bool bBackTrack, ClearanceItem& lastItem, Clearance& newClearance,bool OccupyRunway)
{	
	ClearanceItem touchdownItem = GetRunwayTouchdownClearanceItem(pFlight,lastItem.GetTime(),lastItem);
	lastItem = touchdownItem;
	newClearance.AddItem(lastItem);


	ElapsedTime eLandingTime = lastItem.GetTime();
	pFlight->setRealOpTimeOnRunway(eLandingTime);
	double landspd = lastItem.GetSpeed();
	//calculate rolling distance and time
	double dTouchDownDistance = pFlight->GetTouchDownDistAtRuwnay(plandingRunway);
	double dExitDistance = pRunwayExit->GetExitRunwayPos();
	double dExitSpeed = pFlight->GetExitSpeed();
	double dRollingDist;
	double dMaxDecel = m_pRunwayExitStrategy->GetFlightMaxDecelOnRunway(pFlight);

	double dMinLandingDistance = (landspd*landspd - dExitSpeed*dExitSpeed)/(2.0* dMaxDecel);
	if(dMinLandingDistance > dExitDistance - dTouchDownDistance)
		dRollingDist = dMinLandingDistance;
	else
		dRollingDist = dExitDistance - dTouchDownDistance;

	double dDecel = (landspd*landspd - dExitSpeed*dExitSpeed)/(2.0* dRollingDist);

	DistanceUnit dExitDistInRunway = pRunwayExit->GetPosAtRunway();
	

	if( dRollingDist >  dExitDistance - dTouchDownDistance)
	{		
		if (!bBackTrack)
		{
			CString strTime = lastItem.GetTime().printTime();
			char fltstr[255];
			pFlight->getFullID(fltstr);
			CString strID;
			strID.Format("A:%s",fltstr);
			AirsideDiagnose* pNewDiagnose = new AirsideDiagnose(strTime, strID);		
			CString strDetals;
			strDetals.Format("NOT ENOUGH LANDING DISTANCE ON RUNWAY %s", pFlight->GetLandingRunway()->PrintResource() );
			pNewDiagnose->SetDetails(strDetals);
			throw new AirsideSystemError(pNewDiagnose);	
		}

		ClearanceItem BacktrackItem(plandingRunway,OnLanding,dTouchDownDistance + dRollingDist);		
		BacktrackItem.SetSpeed(dExitSpeed);
		BacktrackItem.SetPosition(plandingRunway->GetDistancePoint(dTouchDownDistance + dRollingDist));
		ElapsedTime dTExit = pFlight->GetTimeBetween(lastItem,BacktrackItem);
		BacktrackItem.SetTime(lastItem.GetTime() + dTExit);
		newClearance.AddItem(BacktrackItem);
		lastItem = BacktrackItem;

		//add speed 0 item
		double dDec = pFlight->GetPerformance()->getNormalLandingDecel(pFlight);
		ElapsedTime DT = ElapsedTime(dExitSpeed/dDec);
		lastItem.SetSpeed(0);
		lastItem.SetTime(DT + lastItem.GetTime());
		DistanceUnit dDist = dExitSpeed * 0.5 * DT.asSeconds();
		lastItem.SetDistInResource(lastItem.GetDistInResource() + dDist);
		newClearance.AddItem(lastItem);


		//turn a 180 angle
		ElapsedTime backtrackTime = ElapsedTime(20L);
		ClearanceItem backItem(	plandingRunway->GetOtherPort(), OnLanding, plandingRunway->GetEndDist() - lastItem.GetDistInResource() );
		backItem.SetSpeed(dExitSpeed);
		backItem.SetTime(lastItem.GetTime() + backtrackTime);
		newClearance.AddItem(backItem);
		lastItem = backItem;
		//go to opposite runway exit;
		RunwayExitInSim * pOppsiteExit = plandingRunway->GetOtherPort()->GetSameNodeRunwayExitWith(pFlight->GetRunwayExit());
		DistanceUnit dBackDist = 0;
		if(pOppsiteExit)
		{
			DistanceUnit dExitDist = pOppsiteExit->GetExitRunwayPos();		
			ClearanceItem runwayExitItem( plandingRunway->GetOtherPort(),OnExitRunway,dExitDist);		
			runwayExitItem.SetSpeed(dExitSpeed);			
			ElapsedTime dTExit = pFlight->GetTimeBetween(lastItem,runwayExitItem);
			ElapsedTime nextTimeExit = lastItem.GetTime() + dTExit;
			runwayExitItem.SetTime(nextTimeExit);
			newClearance.AddItem(runwayExitItem);
			lastItem = runwayExitItem;
			dBackDist = abs(dRollingDist - dExitDist);
		}
		else
		{
			//runway exit clearance
			ClearanceItem runwayExitItem(plandingRunway,OnExitRunway,dExitDistance);		
			runwayExitItem.SetSpeed(dExitSpeed);
			runwayExitItem.SetPosition(plandingRunway->GetDistancePoint(dExitDistance));
			ElapsedTime dTExit = pFlight->GetTimeBetween(lastItem,runwayExitItem);
			ElapsedTime nextTimeExit = lastItem.GetTime() + dTExit;
			runwayExitItem.SetTime(nextTimeExit);
			newClearance.AddItem(runwayExitItem);
			lastItem = runwayExitItem;
			dBackDist = abs(dRollingDist - dExitDistance);
		}

		if(pFlight->GetMode() == OnLanding)		//already touch down runway write delay log
		{
			ElapsedTime tAvailableTime = lastItem.GetTime();
			ElapsedTime tExpectOp = ElapsedTime((2.0*dRollingDist)/(landspd + dExitSpeed));
			ElapsedTime tDelay = tAvailableTime - (pFlight->GetTime()+tExpectOp);
			ResourceDesc resDesc;
			plandingRunway->getDesc(resDesc);
			AirsideFlightRunwayDelayLog* pLog = new AirsideFlightRunwayDelayLog(resDesc,pFlight->GetTime().asSeconds(), OnLanding, tAvailableTime.asSeconds(), tDelay.asSeconds(), FlightRunwayDelay::LandingRoll);
			CString strReason;
			strReason.Format("Require back track %.2f m", dBackDist/100);
			pLog->sReasonDetail = strReason.GetString(); 
			pFlight->LogEventItem(pLog);
		}

	}
	else
	{
		//runway exit clearance
		ClearanceItem runwayExitItem(plandingRunway,OnExitRunway,dExitDistance);		
		runwayExitItem.SetSpeed(dExitSpeed);
		runwayExitItem.SetPosition(plandingRunway->GetDistancePoint(dExitDistance));
		DynamicMovement flightmove(landspd,dExitSpeed,dRollingDist);
		ElapsedTime eNodeTime = flightmove.GetDistTime(dRollingDist);				
		ElapsedTime nextTimeExit = eLandingTime + eNodeTime;
		runwayExitItem.SetTime(nextTimeExit);
		newClearance.AddItem(runwayExitItem);
		lastItem = runwayExitItem;

		double dDownline =  dRollingDist - dMinLandingDistance; 
		if(pFlight->GetMode() == OnLanding && (int)dDownline >0)		//already touch down runway write delay log
		{
			ElapsedTime tAvailableTime = lastItem.GetTime();
			ElapsedTime tExpectOp = ElapsedTime((2.0*dMinLandingDistance)/(landspd + dExitSpeed));
			ElapsedTime tDelay = tAvailableTime - (pFlight->GetTime()+tExpectOp);
			ResourceDesc resDesc;
			plandingRunway->getDesc(resDesc);
			AirsideFlightRunwayDelayLog* pLog = new AirsideFlightRunwayDelayLog(resDesc,pFlight->GetTime().asSeconds(), OnLanding, tAvailableTime.asSeconds(), tDelay.asSeconds(), FlightRunwayDelay::LandingRoll);
			CString strReason;
			strReason.Format("First exit downline %.2f m", (dDownline/100));
			pLog->sReasonDetail = strReason.GetString(); 
			pFlight->LogEventItem(pLog);
		}
	}
	//write the entry time and exit time to node
	if(OccupyRunway)
	{		
		//set runway time table
		plandingRunway->SetEnterTime(pFlight,touchdownItem.GetTime(),OnLanding);
		plandingRunway->SetExitTime(pFlight,lastItem.GetTime());

		plandingRunway->GetOtherPort()->SetEnterTime(pFlight,touchdownItem.GetTime(),OnLanding);
		plandingRunway->GetOtherPort()->SetExitTime(pFlight,lastItem.GetTime());		
		//node dec to 
		_WritePreNodesTime(pFlight,plandingRunway,touchdownItem);
		_WriteRunwayNodeTime(pFlight,plandingRunway, touchdownItem,lastItem);
	}

	return true;
}



void CRunwaySystem::WriteRunwayLogs(AirsideFlightInSim *pFlight,bool bBackup,ClearanceItem& lastItem,Clearance& newClearance)
{	
	LogicRunwayInSim *plandingRunway = pFlight->GetLandingRunway();

	////touch down 
	//double landspd = pFlight->GetPerformance()->getLandingSpeed(pFlight);	

	////landing item 
	//DistanceUnit touchDist = pFlight->GetPerformance()->getDistThresToTouch(pFlight);
	//ClearanceItem landingItem(plandingRunway,OnLanding,touchDist);		
	//landingItem.SetSpeed(landspd);
	//ElapsedTime dTLand = pFlight->GetTimeBetween(lastItem,landingItem);
	//ElapsedTime nextTimeLand = lastItem.GetTime() + dTLand;
	//landingItem.SetTime(nextTimeLand);


	//ClearanceItem landingItem = GetRunwayTouchdownClearanceItem(pFlight,eLandingTime,lastItem);
	//lastItem = landingItem;
	//newClearance.AddItem(lastItem);
	
	std::vector<IntersectionNodeInSim *> vRunwayIntNodes; //Runway intersection nodes, remember it, after the landing operation, write entry and exit node time

	ElapsedTime eLandingTime = lastItem.GetTime();
	pFlight->setRealOpTimeOnRunway(eLandingTime);
	//RunwayOccupyInfoList vRunwayArrangeInfo = plandingRunway->GetFlightOccupyRunwayInfo(pFlight);
	double landspd = lastItem.GetSpeed();

	//std::vector<std::pair<RunwayInSim*, IntersectionNodeInSim * > > vIntRunways;
	//GetRunwayIntersectionNodes(plandingRunway,vIntRunways);

	//calculate rolling distance and time
	double dTouchDownDistance = pFlight->GetTouchDownDistAtRuwnay(plandingRunway);
	double dExitDistance = pFlight->GetRunwayExit()->GetExitRunwayPos();
	double dExitSpeed = pFlight->GetExitSpeed();
	double dRollingDist;
	double dMaxDecel = m_pRunwayExitStrategy->GetFlightMaxDecelOnRunway(pFlight);

	double dMinLandingDistance = (landspd*landspd - dExitSpeed*dExitSpeed)/(2.0* dMaxDecel);
	if(dMinLandingDistance > dExitDistance - dTouchDownDistance)
		dRollingDist = dMinLandingDistance;
	else
		dRollingDist = dExitDistance - dTouchDownDistance;

	double dDecel = (landspd*landspd - dExitSpeed*dExitSpeed)/(2.0* dRollingDist);


	//Deceleration
	std::vector<RunwayOccupyInfo> vIntRunwayNodeNeedWriteOccupancyTable;

	//std::vector<RunwayOccupyInfo> vOtherIntRunwayOccupanyInfo;
	//vOtherIntRunwayOccupanyInfo.insert(vOtherIntRunwayOccupanyInfo.begin(),vRunwayArrangeInfo.begin(),vRunwayArrangeInfo.end());
	DistanceUnit dExitDistInRunway = pFlight->GetRunwayExit()->GetPosAtRunway();
	{
		std::vector<IntersectionNodeInSim *> vDecelNodes;
		plandingRunway->GetIntersectNodeInRange(dTouchDownDistance,dExitDistInRunway + 1,vDecelNodes);

		int nDecelNode =0;
		for (; nDecelNode< (int)vDecelNodes.size(); ++nDecelNode)
		{
			IntersectionNodeInSim *pCurNode = vDecelNodes[nDecelNode];

			//check the node is intersection node with runway
			bool bIntRunwayNode = false;
			//int nIntRunwayCount = vOtherIntRunwayOccupanyInfo.size();
			int nIntRunwayNode = 0;
			//for (; nIntRunwayNode < nIntRunwayCount; ++ nIntRunwayNode)
			//{
			//	if (vOtherIntRunwayOccupanyInfo[nIntRunwayNode].pIntNode == pCurNode)
			//	{
			//		bIntRunwayNode = true;
			//		break;
			//	}
			//}

			//if(bIntRunwayNode)
			//{
			//	vIntRunwayNodeNeedWriteOccupancyTable.push_back(vOtherIntRunwayOccupanyInfo[nIntRunwayNode]);
			//	//calculate the time that flight pass the intersection
			//	double dNodePosition = plandingRunway->GetPointDist(pCurNode->GetNodeInput().GetPosition());
			//	double dNodeSpeed = (1-(dNodePosition - dTouchDownDistance)/dRollingDist)*(landspd-dExitSpeed);
			//	double dNodeTime = (landspd - dNodeSpeed)/dDecel;
			//	ElapsedTime eNodeTime = ElapsedTime(dNodeTime);
			//	pCurNode->SetEnterTime(pFlight, lastItem.GetTime() + eNodeTime -m_eTimeCrossRunwayBuffer,OnLanding);

			//}
			//else
			{
				double dNodePosition = plandingRunway->GetPointDist(pCurNode->GetNodeInput().GetPosition()) ;

				DynamicMovement flightmove(landspd,dExitSpeed,dExitDistInRunway- dTouchDownDistance);
				ElapsedTime eNodeTime = flightmove.GetDistTime(dNodePosition- dTouchDownDistance);				
				ElapsedTime tDifference = 0L;			//calculation difference
				if (pCurNode->GetNodeInput().IsRunwaysIntersection())		//if node is runways intersection, needn't taxi cross buffer
				{
					pCurNode->SetEnterTime(pFlight, lastItem.GetTime() + eNodeTime - tDifference,OnLanding);
					pCurNode->SetExitTime(pFlight, lastItem.GetTime()+ eNodeTime + tDifference);

					//vRunwayIntNodes.push_back(pCurNode);

					//ClearanceItem nodeItem = lastItem;
					//nodeItem.SetTime(eLandingTime + eNodeTime);
					//nodeItem.SetDistInResource(dNodePosition + dTouchDownDistance);
					//nodeItem.SetPosition(pCurNode->GetNodeInput().GetPosition());
					//double dSpd = landspd - dDecel* double(eNodeTime);
					//nodeItem.SetSpeed(dSpd);
					//lastItem = nodeItem;
					//newClearance.AddItem(lastItem);
#if _DEBUGLOG
					{
					CString debugfileName;
					debugfileName.Format( "C:\\RunwayIntNode.txt" );

					std::ofstream outfile;
					outfile.open(debugfileName,std::ios::app);
					char fltstr[255];
					pFlight->getFullID(fltstr);
					CString strNodeName = pCurNode->GetNodeInput().GetName();
					outfile <<"Landing   " <<pFlight->GetUID()<<"    " << fltstr << "      "<< pFlight->GetTime().printTime() <<"      "<<strNodeName <<"    " << (lastItem.GetTime() + eNodeTime).printTime()<<"        "<< (long)eLandingTime<<"     " << eLandingTime.printTime() <<"     "/*<< (long)nextTimeExit<<"    "<< nextTimeExit.printTime() <<"     "*/<< std::endl;
					outfile.close();
					}
#endif

				}
				else
				{
					ElapsedTime eT =  lastItem.GetTime() + eNodeTime - m_eTimeCrossRunwayBuffer;
					ElapsedTime exT = lastItem.GetTime()+ eNodeTime + m_eTimeCrossRunwayBuffer;
#if _DEBUGLOG
					{
						CString debugfileName;
						debugfileName.Format( "C:\\RunwayIntNode.txt" );

						std::ofstream outfile;
						outfile.open(debugfileName,std::ios::app);
						char fltstr[255];
						pFlight->getFullID(fltstr);
						CString strNodeName = pCurNode->GetNodeInput().GetName();
						outfile <<"Landing   " <<pFlight->GetUID()<<"    " << fltstr 
							<< "      "<< pFlight->GetTime().printTime()
							<<"      "<<strNodeName <<"    " << 
							(lastItem.GetTime() + eNodeTime).printTime()<<"        "<<
							(long)eLandingTime<<"     " << eLandingTime.printTime() << std::endl;
						outfile.close();
					}
#endif
					pCurNode->SetEnterTime(pFlight,eT,OnLanding);
					pCurNode->SetExitTime(pFlight,exT);
				}
			}
		}
	}

	//	ElapsedTime eTotalTimeDecel = (landspd - dExitSpeed)/dDecel;
	//	ClearanceItem decelEndItem = lastItem;

	//	decelEndItem.SetTime(lastItem.GetTime() + eTotalTimeDecel);
	//	decelEndItem.SetSpeed(dExitSpeed);

	//	//if(dExitDistance >= dRollingDist + dTouchDownDistance)
	//	//	decelEndItem.SetDistInResource(dExitDistance);
	//	//else
	//	decelEndItem.SetDistInResource(dRollingDist + dTouchDownDistance);

	//	ElapsedTime dDecelEndTime = pFlight->GetTimeBetween(lastItem,decelEndItem);

	//	decelEndItem.SetTime(lastItem.GetTime() + dDecelEndTime);

	//	//decel end
	//	lastItem = decelEndItem;
	//	newClearance.AddItem(lastItem);
	//}
	////Decelerate end to runway exit

	//{
	//	ElapsedTime eCurTime = lastItem.GetTime();
	//	CPoint2008 pPreNodePosition = lastItem.GetPosition();
	//	std::vector<IntersectionNodeInSim *> vNormalNodes;
	//	DistanceUnit dExitDistInRunway = pFlight->GetRunwayExit()->GetPosAtRunway();
	//	plandingRunway->GetIntersectNodeInRange(dTouchDownDistance+dRollingDist,dExitDistInRunway,vNormalNodes);

	//	for (int nNormalNode =0; nNormalNode< (int)vNormalNodes.size(); ++nNormalNode)
	//	{
	//		IntersectionNodeInSim *pCurNode = vNormalNodes[nNormalNode];

	//		double dNodeDistance = plandingRunway->GetPointDist(pCurNode->GetNodeInput().GetPosition());
	//		double dPreDistance = plandingRunway->GetPointDist(pPreNodePosition);
	//		double dNodeTime = (dNodeDistance - dPreDistance)/dExitSpeed;
	//		ElapsedTime eNodeTime = ElapsedTime(dNodeTime);	

	//		//check the node is intersection node with runway
	//		bool bIntRunwayNode = false;
	//		int nIntRunwayCount = vOtherIntRunwayOccupanyInfo.size();
	//		int nIntRunwayNode = 0;
	//		for (; nIntRunwayNode < nIntRunwayCount; ++ nIntRunwayNode)
	//		{
	//			if (vOtherIntRunwayOccupanyInfo[nIntRunwayNode].pIntNode == pCurNode)
	//			{
	//				bIntRunwayNode = true;
	//				break;
	//			}
	//		}

	//		if(bIntRunwayNode)
	//		{
	//			RunwayOccupyInfo runwayOccupyInfo = vOtherIntRunwayOccupanyInfo[nNormalNode];
	//			if (runwayOccupyInfo.bLASHO)
	//			{

	//				//stop
	//				ClearanceItem nodeItem = lastItem;
	//				nodeItem.SetTime(eCurTime + eNodeTime);
	//				nodeItem.SetPosition(pCurNode->GetNodeInput().GetPosition());
	//				nodeItem.SetSpeed(0);
	//				lastItem = nodeItem;
	//				newClearance.AddItem(lastItem);


	//				nodeItem.SetTime(runwayOccupyInfo.eLASHOTime);
	//				nodeItem.SetSpeed(dExitSpeed);
	//				nodeItem.SetPosition(pCurNode->GetNodeInput().GetPosition());
	//				lastItem = nodeItem;
	//				newClearance.AddItem(lastItem);

	//				pCurNode->SetEnterTime(pFlight,runwayOccupyInfo.eLASHOTime,OnLanding,dExitSpeed);

	//				eCurTime = runwayOccupyInfo.eLASHOTime;

	//			}
	//			else
	//			{
	//				if (pCurNode->GetNodeInput().IsRunwaysIntersection())
	//					pCurNode->SetEnterTime(pFlight, eCurTime + eNodeTime ,OnLanding);
	//				else
	//					pCurNode->SetEnterTime(pFlight, eCurTime + eNodeTime -m_eTimeCrossRunwayBuffer,OnLanding);

	//				vIntRunwayNodeNeedWriteOccupancyTable.push_back(vOtherIntRunwayOccupanyInfo[nIntRunwayNode]);
	//				eCurTime += eNodeTime;

	//			}
	//		}
	//		else
	//		{
	//			if (pCurNode->GetNodeInput().IsRunwaysIntersection())
	//			{
	//				pCurNode->SetEnterTime(pFlight, eCurTime + eNodeTime,OnLanding);
	//				pCurNode->SetExitTime(pFlight, eCurTime+ eNodeTime);
	//			}
	//			else
	//			{
	//				pCurNode->SetEnterTime(pFlight, eCurTime + eNodeTime -m_eTimeCrossRunwayBuffer,OnLanding);
	//				pCurNode->SetExitTime(pFlight, eCurTime+ eNodeTime + m_eTimeCrossRunwayBuffer);
	//			}

	//			eCurTime += eNodeTime;
	//		}

	//		pPreNodePosition = pCurNode->GetNodeInput().GetPosition();
	//	}

	//}


	//backtrack clearance	
	if( dRollingDist >  dExitDistance - dTouchDownDistance)
	{		
		if (!bBackup)
		{
			CString strTime = lastItem.GetTime().printTime();
			char fltstr[255];
			pFlight->getFullID(fltstr);
			CString strID;
			strID.Format("A:%s",fltstr);
			AirsideDiagnose* pNewDiagnose = new AirsideDiagnose(strTime, strID);		
			CString strDetals;
			strDetals.Format("NOT ENOUGH LANDING DISTANCE ON RUNWAY %s", pFlight->GetLandingRunway()->PrintResource() );
			pNewDiagnose->SetDetails(strDetals);
			throw new AirsideSystemError(pNewDiagnose);	
		}

		ClearanceItem BacktrackItem(plandingRunway,OnLanding,dTouchDownDistance + dRollingDist);		
		BacktrackItem.SetSpeed(dExitSpeed);
		BacktrackItem.SetPosition(plandingRunway->GetDistancePoint(dTouchDownDistance + dRollingDist));
		ElapsedTime dTExit = pFlight->GetTimeBetween(lastItem,BacktrackItem);
		BacktrackItem.SetTime(lastItem.GetTime() + dTExit);
		newClearance.AddItem(BacktrackItem);
		lastItem = BacktrackItem;

		//add speed 0 item
		double dDec = pFlight->GetPerformance()->getNormalLandingDecel(pFlight);
		ElapsedTime DT = ElapsedTime(dExitSpeed/dDec);
		lastItem.SetSpeed(0);
		lastItem.SetTime(DT + lastItem.GetTime());
		DistanceUnit dDist = dExitSpeed * 0.5 * DT.asSeconds();
		lastItem.SetDistInResource(lastItem.GetDistInResource() + dDist);
		newClearance.AddItem(lastItem);


		//turn a 180 angle
		ElapsedTime backtrackTime = ElapsedTime(20L);
		ClearanceItem backItem(	plandingRunway->GetOtherPort(), OnLanding, plandingRunway->GetEndDist() - lastItem.GetDistInResource() );
		backItem.SetSpeed(dExitSpeed);
		backItem.SetTime(lastItem.GetTime() + backtrackTime);
		newClearance.AddItem(backItem);
		lastItem = backItem;
		//go to opposite runway exit;
		RunwayExitInSim * pOppsiteExit = plandingRunway->GetOtherPort()->GetSameNodeRunwayExitWith(pFlight->GetRunwayExit());
		DistanceUnit dBackDist = 0;
		if(pOppsiteExit)
		{
			DistanceUnit dExitDist = pOppsiteExit->GetExitRunwayPos();		
			ClearanceItem runwayExitItem( plandingRunway->GetOtherPort(),OnExitRunway,dExitDist);		
			runwayExitItem.SetSpeed(dExitSpeed);			
			ElapsedTime dTExit = pFlight->GetTimeBetween(lastItem,runwayExitItem);
			ElapsedTime nextTimeExit = lastItem.GetTime() + dTExit;
			runwayExitItem.SetTime(nextTimeExit);
			newClearance.AddItem(runwayExitItem);
			lastItem = runwayExitItem;
			dBackDist = abs(dRollingDist - dExitDist);
		}
		else
		{
			//runway exit clearance
			ClearanceItem runwayExitItem(plandingRunway,OnExitRunway,dExitDistance);		
			runwayExitItem.SetSpeed(dExitSpeed);
			runwayExitItem.SetPosition(plandingRunway->GetDistancePoint(dExitDistance));
			ElapsedTime dTExit = pFlight->GetTimeBetween(lastItem,runwayExitItem);
			ElapsedTime nextTimeExit = lastItem.GetTime() + dTExit;
			runwayExitItem.SetTime(nextTimeExit);
			newClearance.AddItem(runwayExitItem);
			lastItem = runwayExitItem;
			dBackDist = abs(dRollingDist - dExitDistance);
		}

		if(pFlight->GetMode() == OnLanding)		//already touch down runway write delay log
		{
			ElapsedTime tAvailableTime = lastItem.GetTime();
			ElapsedTime tExpectOp = ElapsedTime((2.0*dRollingDist)/(landspd + dExitSpeed));
			ElapsedTime tDelay = tAvailableTime - (pFlight->GetTime()+tExpectOp);
			ResourceDesc resDesc;
			plandingRunway->getDesc(resDesc);
			AirsideFlightRunwayDelayLog* pLog = new AirsideFlightRunwayDelayLog(resDesc,pFlight->GetTime().asSeconds(), OnLanding, tAvailableTime.asSeconds(), tDelay.asSeconds(), FlightRunwayDelay::LandingRoll);
			CString strReason;
			strReason.Format("Require back track %.2f m", dBackDist/100);
			pLog->sReasonDetail = strReason.GetString(); 
			pFlight->LogEventItem(pLog);
		}

	}
	else
	{
		//runway exit clearance
		ClearanceItem runwayExitItem(plandingRunway,OnExitRunway,dExitDistance);		
		runwayExitItem.SetSpeed(dExitSpeed);
		runwayExitItem.SetPosition(plandingRunway->GetDistancePoint(dExitDistance));
		DynamicMovement flightmove(landspd,dExitSpeed,dRollingDist);
		ElapsedTime eNodeTime = flightmove.GetDistTime(dRollingDist);				
		ElapsedTime nextTimeExit = eLandingTime + eNodeTime;
		runwayExitItem.SetTime(nextTimeExit);
		newClearance.AddItem(runwayExitItem);
		lastItem = runwayExitItem;

		double dDownline =  dRollingDist - dMinLandingDistance; 
		if(pFlight->GetMode() == OnLanding && (int)dDownline >0)		//already touch down runway write delay log
		{
			ElapsedTime tAvailableTime = lastItem.GetTime();
			ElapsedTime tExpectOp = ElapsedTime((2.0*dMinLandingDistance)/(landspd + dExitSpeed));
			ElapsedTime tDelay = tAvailableTime - (pFlight->GetTime()+tExpectOp);
			ResourceDesc resDesc;
			plandingRunway->getDesc(resDesc);
			AirsideFlightRunwayDelayLog* pLog = new AirsideFlightRunwayDelayLog(resDesc,pFlight->GetTime().asSeconds(), OnLanding, tAvailableTime.asSeconds(), tDelay.asSeconds(), FlightRunwayDelay::LandingRoll);
			CString strReason;
			strReason.Format("First exit downline %.2f m", (dDownline/100));
			pLog->sReasonDetail = strReason.GetString(); 
			pFlight->LogEventItem(pLog);
		}
	}


	
	ElapsedTime nextTimeExit = lastItem.GetTime();

	//only write the runway itself, other runways don't write 
	for(size_t nRunway = 0; nRunway< 2; ++nRunway)
	{
		LogicRunwayInSim* pCurPort = NULL;
		
		if (nRunway == 0)
		{
			pCurPort = pFlight->GetLandingRunway();
		}
		else
		{
			pCurPort = pFlight->GetLandingRunway()->GetOtherPort();
		}
		pCurPort->SetEnterTime(pFlight,eLandingTime,OnLanding);
		pCurPort->SetExitTime(pFlight,nextTimeExit);


		//////////////////////////////////////////////////////////////////////////
		#if _DEBUGLOG
		{
			CString debugfileName;
			if (pCurPort->getLogicRunwayType() == RUNWAYMARK_FIRST)
			{	
				debugfileName.Format( "C:\\RunwayDebug\\%s(mark1).txt",pCurPort->GetRunwayInSim()->GetRunwayInput()->GetObjectName().GetIDString() );
			}
			else
			{
				debugfileName.Format( "C:\\RunwayDebug\\%s(mark2).txt",pCurPort->GetRunwayInSim()->GetRunwayInput()->GetObjectName().GetIDString() );

			}

			std::ofstream outfile;
			outfile.open(debugfileName,std::ios::app);
			char fltstr[255];
			pFlight->getFullID(fltstr);
			outfile << (long)eLandingTime<<"     " << (long)nextTimeExit<<"       "<<pFlight->GetUID()<<"    " <<fltstr <<std::endl;
			outfile.close();
		}
		#endif
		//////////////////////////////////////////////////////////////////////////

	}
	//write intersection runways' time
	
	for (size_t nNode = 0; nNode < vRunwayIntNodes.size(); ++ nNode)
	{
		IntersectionNodeInSim *pIntNode = vRunwayIntNodes[nNode];
		if(pIntNode)
		{
			pIntNode->SetEnterTime(pFlight,eLandingTime,OnLanding);
			pIntNode->SetExitTime(pFlight,nextTimeExit);
#if _DEBUGLOG
			CString debugfileName;
			debugfileName.Format( "C:\\RunwayIntNode.txt" );

			std::ofstream outfile;
			outfile.open(debugfileName,std::ios::app);
			char fltstr[255];
			pFlight->getFullID(fltstr);
			CString strNodeName = pIntNode->GetNodeInput().GetName();
			outfile <<"Landing   " <<pFlight->GetUID()<<"    " << fltstr << "      "<< pFlight->GetTime().printTime() <<"      "<<strNodeName <<"    " <<(long)eLandingTime<<"     " << eLandingTime.printTime() <<"     "<< (long)nextTimeExit<<"    "<< nextTimeExit.printTime() <<"     "<< std::endl;
			outfile.close();
#endif		
		}
	}
	//write the entry time and exit time to node
	std::vector<IntersectionNodeInSim *> vPreIntNodes;
	pFlight->GetLandingRunway()->GetIntersectNodeInRange(0,dTouchDownDistance,vPreIntNodes);
	for (size_t nNode = 0; nNode < vPreIntNodes.size(); ++ nNode)
	{
		IntersectionNodeInSim *pIntNode = vPreIntNodes[nNode];
		if(pIntNode)
		{
			pIntNode->SetEnterTime(pFlight,eLandingTime - m_eTimeCrossRunwayBuffer,OnLanding);
			pIntNode->SetExitTime(pFlight,eLandingTime + m_eTimeCrossRunwayBuffer);
		}

	}

	double dLandingRunwayDistance = dExitDistance;
	if(pFlight->GetLandingRunway())
		dLandingRunwayDistance = pFlight->GetLandingRunway()->GetEndDist();

	//from exit to runway end
	//write the entry time and exit time to node
	std::vector<IntersectionNodeInSim *> vAfterIntNodes;
	pFlight->GetLandingRunway()->GetIntersectNodeInRange(dExitDistance + 1,dLandingRunwayDistance,vAfterIntNodes);
	for (size_t nNode = 0; nNode < vAfterIntNodes.size(); ++ nNode)
	{
		IntersectionNodeInSim *pIntNode = vAfterIntNodes[nNode];
		if(pIntNode && !pIntNode->GetNodeInput().IsRunwaysIntersection())
		{
			pIntNode->SetEnterTime(pFlight,eLandingTime,OnLanding);
			pIntNode->SetExitTime(pFlight,nextTimeExit);
		}

		if(pIntNode && pIntNode->GetNodeInput().IsRunwaysIntersection())		//intersection node of runway
		{
			pIntNode->SetEnterTime(pFlight,lastItem.GetTime(),OnLanding);
			pIntNode->SetExitTime(pFlight,lastItem.GetTime()+1L);
		}

	}

	for (size_t nIntRunway =0; nIntRunway < vIntRunwayNodeNeedWriteOccupancyTable.size();++nIntRunway)
	{
		if (vIntRunwayNodeNeedWriteOccupancyTable[nIntRunway].bLASHO )
		{
			vIntRunwayNodeNeedWriteOccupancyTable[nIntRunway].pIntNode->SetExitTime(pFlight,nextTimeExit);
		}
		else
		{
			vIntRunwayNodeNeedWriteOccupancyTable[nIntRunway].pIntNode->SetExitTime(pFlight,nextTimeExit);
		}
	}

	////intersection node
	//size_t nIntNodeCount = vIntRunways.size();
	//for (int nIntNode = 0; nIntNode < (int)nIntNodeCount; ++ nIntNode )
	//{
	//	IntersectionNodeInSim *pIntNode = vIntRunways[nIntNode].second;
	//	pIntNode->SetEnterTime(pFlight,eLandingTime,OnLanding);
	//	ElapsedTime eIntNodeExitTime = ElapsedTime((nextTimeExit.asSeconds() -eLandingTime.asSeconds())/2.0) + eLandingTime;

	//	pIntNode->SetExitTime(pFlight,eIntNodeExitTime);

	//}
	
	//write runway log
	
	//RunwayExitInSim* pRunwayExit = pFlight->GetRunwayExit();
	//
	//FlightGroundRouteDirectSegInSim *pGroundRouteSeg = pRunwayExit->GetRouteSeg();
	//if(pGroundRouteSeg->GetType() == AirsideResource::ResType_RunwayDirSeg)
	//{
	//	RunwayDirectSegInSim *pRunwayDirectSeg = (RunwayDirectSegInSim *)pGroundRouteSeg;

	//	//from runway exit to cross runway
	//	//ClearanceItem leaveLandingRunwayItem(pRunwayDirectSeg,OnTaxiToStand,0);
	//	//{

	//	//	leaveLandingRunwayItem.SetSpeed(dExitSpeed);
	//	//	leaveLandingRunwayItem.SetPosition(plandingRunway->GetDistancePoint(dExitDistance));
	//	//	ElapsedTime dTExit = pFlight->GetTimeBetween(lastItem,leaveLandingRunwayItem);
	//	//	ElapsedTime nextTimeExit = lastItem.GetTime() + dTExit;
	//	//	leaveLandingRunwayItem.SetTime(nextTimeExit);
	//	//	newClearance.AddItem(leaveLandingRunwayItem);
	//	//	lastItem = leaveLandingRunwayItem;
	//	//}

	//	double dTaxiwaySpeed = pFlight->GetPerformance()->getTaxiInNormalSpeed(pFlight);

	//	ClearanceItem entryRunwaySegItem(pRunwayDirectSeg,OnExitRunway,pRunwayDirectSeg->GetEntryHoldDist());
	//	{

	//		entryRunwaySegItem.SetSpeed(dTaxiwaySpeed);
	//		entryRunwaySegItem.SetPosition(plandingRunway->GetDistancePoint(pRunwayDirectSeg->GetEntryHoldDist()));
	//		ElapsedTime dTExit = pFlight->GetTimeBetween(lastItem,entryRunwaySegItem);
	//		ElapsedTime nextTimeExit = lastItem.GetTime() + dTExit;
	//		entryRunwaySegItem.SetTime(nextTimeExit);
	//		newClearance.AddItem(entryRunwaySegItem);
	//		lastItem = entryRunwaySegItem;
	//	}

	//	//to cross runway exit
	//	ClearanceItem crossrunwayexitItem = entryRunwaySegItem;
	//	{
	//		crossrunwayexitItem.SetDistInResource(pRunwayDirectSeg->GetExitHoldDist());
	//		crossrunwayexitItem.SetSpeed(dTaxiwaySpeed);
	//		
	//		crossrunwayexitItem.SetPosition(pRunwayDirectSeg->GetDistancePoint(pRunwayDirectSeg->GetExitHoldDist()));
	//		ElapsedTime dTExit = pFlight->GetTimeBetween(lastItem,crossrunwayexitItem);
	//		ElapsedTime nextTimeExit = lastItem.GetTime() + dTExit;
	//		crossrunwayexitItem.SetTime(nextTimeExit);
	//		crossrunwayexitItem.SetMode(OnTaxiToStand);
	//		newClearance.AddItem(crossrunwayexitItem);
	//		

	//		lastItem = crossrunwayexitItem;
	//	}

	//}


}

void CRunwaySystem::WriteCircuitFlightRunwayTakeoffLogs(AirsideCircuitFlightInSim *pFlight, std::vector<RunwayOccupyInfo>& vRunwayArrangeInfo,  Clearance& newClearance)
{

	ElapsedTime eTimeBegin;
	ElapsedTime eTimeEnd;
	if (newClearance.GetItemCount() > 0)
	{
		eTimeBegin= newClearance.ItemAt(0).GetTime();
		eTimeEnd = newClearance.ItemAt(newClearance.GetItemCount() -1).GetTime();
	}

	//only write the runway itself, other runways don't write 
	for(size_t nRunway = 0; nRunway< 2; ++nRunway)
	{
		LogicRunwayInSim* pCurPort = NULL;
		if(nRunway == 0)
			pCurPort = pFlight->GetAndAssignTakeoffRunway();
		else
			pCurPort = pFlight->GetAndAssignTakeoffRunway()->GetOtherPort();

		pCurPort->SetEnterTime(pFlight,eTimeBegin,OnTakeoff);
		pCurPort->SetExitTime(pFlight,eTimeEnd);

	}

	//get Take off time
	ElapsedTime ePreTakeOffTime;
	double dPreTakeOffDistance = 0;
	int nClearanceCount = newClearance.GetItemCount();
	for (int nClearance = 0; nClearance < nClearanceCount; ++nClearance)
	{
		ClearanceItem clearItem = newClearance.GetItem(nClearance);
		if (clearItem.GetMode() == OnTakeoff)
		{
			ePreTakeOffTime = clearItem.GetTime();
			dPreTakeOffDistance = clearItem.GetDistInResource();
			break;
		}
	}

	//lift off item
	ClearanceItem lastClearItem(pFlight->GetLandingRunway(),OnTakeoff);
	if(nClearanceCount >0)
	{
		lastClearItem = newClearance.ItemAt(nClearanceCount -1);
	}

	DistanceUnit dExitDistance = pFlight->GetDistInResource();

	DistanceUnit dRunwayDistance = pFlight->GetAndAssignTakeoffRunway()->GetEndDist();

	std::vector<IntersectionNodeInSim *> vIntNodes;
	pFlight->GetAndAssignTakeoffRunway()->GetIntersectNodeInRange(dExitDistance,dRunwayDistance,vIntNodes);

	double dLiftSpeed = lastClearItem.GetSpeed();
	double dLastDistance = lastClearItem.GetDistInResource();

	double dTakeoffRoll = pFlight->GetTakeoffRoll();
	double dTakeoffRollTime = (2.0 *dTakeoffRoll)/dLiftSpeed;
	double dAccelSpeed = dLiftSpeed/dTakeoffRollTime;


	for (size_t nNode = 0; nNode < vIntNodes.size(); ++ nNode)
	{
		IntersectionNodeInSim *pIntNode = vIntNodes[nNode];
		double dNodeDistance = pFlight->GetAndAssignTakeoffRunway()->GetPointDist(pIntNode->GetNodeInput().GetPosition()) - pFlight->GetAndAssignTakeoffPosition()->GetPosAtRunway();

		DynamicMovement flightmove(0,pFlight->GetLiftoffSpeed(),dTakeoffRoll);

		ElapsedTime tToNode = 0L;
		if (dNodeDistance <= pFlight->GetTakeoffRoll())
		{
			tToNode = flightmove.GetDistTime(dNodeDistance);
		}
		else
		{
			tToNode = flightmove.GetDistTime(pFlight->GetTakeoffRoll());

			double dDistLiftToNode = dNodeDistance - pFlight->GetTakeoffRoll()-pFlight->GetAndAssignTakeoffPosition()->GetPosAtRunway();

			tToNode += ElapsedTime((long)dDistLiftToNode/pFlight->GetLiftoffSpeed());

		}

		if (pIntNode->GetNodeInput().IsRunwaysIntersection())		//runways intersection needn't taxi cross buffer time 
		{
			pIntNode->SetEnterTime(pFlight,pFlight->getRealOpTimeOnRunway() + tToNode,OnTakeoff);
			pIntNode->SetExitTime(pFlight,pFlight->getRealOpTimeOnRunway() + tToNode);

		}
		else
		{
			pIntNode->SetEnterTime(pFlight,pFlight->getRealOpTimeOnRunway() + tToNode - m_eTimeCrossRunwayBuffer,OnTakeoff);
			pIntNode->SetExitTime(pFlight,pFlight->getRealOpTimeOnRunway() + tToNode + m_eTimeCrossRunwayBuffer);
		}
	}

	//distance from 0 to exit position
	std::vector<IntersectionNodeInSim *> vPreIntNodes;
	pFlight->GetAndAssignTakeoffRunway()->GetIntersectNodeInRange(0,dExitDistance,vPreIntNodes);

	for (size_t nNode = 0; nNode < vPreIntNodes.size(); ++ nNode)
	{
		IntersectionNodeInSim *pIntNode = vPreIntNodes[nNode];
		if(pIntNode)
		{
			pIntNode->SetEnterTime(pFlight,eTimeBegin,OnTakeoff);
			pIntNode->SetExitTime(pFlight,eTimeEnd);
		}
	}
}

void CRunwaySystem::WriteRunwayTakeOffLogs(AirsideFlightInSim *pFlight, std::vector<RunwayOccupyInfo>& vRunwayArrangeInfo, ElapsedTime eTakeOffTime, ElapsedTime tTimeToRwy, ClearanceItem& lastItem,Clearance& newClearance)
{
	//ASSERT(pFlight->GetUID() != 109);
	GetFlightTakeOffClearance(pFlight, tTimeToRwy, lastItem,newClearance);

	ElapsedTime eTimeBegin;
	ElapsedTime eTimeEnd;
	if (newClearance.GetItemCount() > 0)
	{
		eTimeBegin= newClearance.ItemAt(0).GetTime();
		eTimeEnd = newClearance.ItemAt(newClearance.GetItemCount() -1).GetTime();
	}

	//only write the runway itself, other runways don't write 
	for(size_t nRunway = 0; nRunway< 2; ++nRunway)
	{
		LogicRunwayInSim* pCurPort = NULL;
		if(nRunway == 0)
			pCurPort = pFlight->GetAndAssignTakeoffRunway();
		else
			pCurPort = pFlight->GetAndAssignTakeoffRunway()->GetOtherPort();

		pCurPort->SetEnterTime(pFlight,eTimeBegin,OnTakeoff);
		pCurPort->SetExitTime(pFlight,eTimeEnd);

		//////////////////////////////////////////////////////////////////////////
		//write log
		#if _DEBUGLOG
		{
			CString debugfileName;
			if (pCurPort->getLogicRunwayType() == RUNWAYMARK_FIRST)
			{	
				debugfileName.Format( "C:\\RunwayDebug\\%s(mark1).txt",pCurPort->GetRunwayInSim()->GetRunwayInput()->GetObjectName().GetIDString() );
			}
			else
			{
				debugfileName.Format( "C:\\RunwayDebug\\%s(mark2).txt",pCurPort->GetRunwayInSim()->GetRunwayInput()->GetObjectName().GetIDString() );

			}

			std::ofstream outfile;
			outfile.open(debugfileName,std::ios::app);
			char fltstr[255];
			pFlight->getFullID(fltstr);
			outfile << (long)eTimeBegin<<"     " << (long)eTimeEnd<<"       "<<pFlight->GetUID()<<"    " <<fltstr <<std::endl;
			outfile.close();
		}
		#endif
		//////////////////////////////////////////////////////////////////////////

	}

	//get Take off time
	ElapsedTime ePreTakeOffTime;
	double dPreTakeOffDistance = 0;
	int nClearanceCount = newClearance.GetItemCount();
	for (int nClearance = 0; nClearance < nClearanceCount; ++nClearance)
	{
		ClearanceItem clearItem = newClearance.GetItem(nClearance);
		if (clearItem.GetMode() == OnTakeoff)
		{
			ePreTakeOffTime = clearItem.GetTime();
			dPreTakeOffDistance = clearItem.GetDistInResource();
			break;
		}
	}

	//lift off item
	ClearanceItem lastClearItem(pFlight->GetLandingRunway(),OnTakeoff);
	if(nClearanceCount >0)
	{
		lastClearItem = newClearance.ItemAt(nClearanceCount -1);
	}

	DistanceUnit dExitDistance = pFlight->GetAndAssignTakeoffPosition()->GetEnterRunwayPos();

	DistanceUnit dRunwayDistance = pFlight->GetAndAssignTakeoffRunway()->GetEndDist();

	std::vector<IntersectionNodeInSim *> vIntNodes;
	pFlight->GetAndAssignTakeoffRunway()->GetIntersectNodeInRange(dExitDistance,dRunwayDistance,vIntNodes);

	double dLiftSpeed = lastClearItem.GetSpeed();
	double dLastDistance = lastClearItem.GetDistInResource();

	double dTakeoffRoll = pFlight->GetTakeoffRoll();
	double dTakeoffRollTime = (2.0 *dTakeoffRoll)/dLiftSpeed;
	double dAccelSpeed = dLiftSpeed/dTakeoffRollTime;


	for (size_t nNode = 0; nNode < vIntNodes.size(); ++ nNode)
	{
		IntersectionNodeInSim *pIntNode = vIntNodes[nNode];
		double dNodeDistance = pFlight->GetAndAssignTakeoffRunway()->GetPointDist(pIntNode->GetNodeInput().GetPosition()) - pFlight->GetAndAssignTakeoffPosition()->GetPosAtRunway();
		////speed
		//double dSpeed = dLiftSpeed *((dNodeDistance-dExitDistance)/(dLastDistance-dExitDistance));

		//double dTempValue = 2*(dNodeDistance-dExitDistance)/dAccelSpeed;
		//double dTime = 0.0;
		//if(dTempValue > 0)
		//	dTime = sqrt(dTempValue);
		DynamicMovement flightmove(0,pFlight->GetLiftoffSpeed(),dTakeoffRoll);
		//ElapsedTime dTime = flightmove.GetDistTime(dNodeDistance);

		ElapsedTime tToNode = 0L;
		if (dNodeDistance <= pFlight->GetTakeoffRoll())
		{
			tToNode = flightmove.GetDistTime(dNodeDistance);
		}
		else
		{
			tToNode = flightmove.GetDistTime(pFlight->GetTakeoffRoll());

			double dDistLiftToNode = dNodeDistance - pFlight->GetTakeoffRoll()-pFlight->GetAndAssignTakeoffPosition()->GetPosAtRunway();
			//double dAcc = (pFlight->GetPerformance()->getMaxHorizontalAccel(pFlight)+ pFlight->GetPerformance()->getMinHorizontalAccel(pFlight))/2;
			//double dSpdAtNode = sqrt(2*dAcc*dDistLiftToNode + pFlight->GetLiftoffSpeed()* pFlight->GetLiftoffSpeed());
			//tToNode += ElapsedTime(abs(dSpdAtNode-pFlight->GetLiftoffSpeed())/dAcc);
			tToNode += ElapsedTime((long)dDistLiftToNode/pFlight->GetLiftoffSpeed());

		}

		if (pIntNode->GetNodeInput().IsRunwaysIntersection())		//runways intersection needn't taxi cross buffer time 
		{
			pIntNode->SetEnterTime(pFlight,pFlight->getRealOpTimeOnRunway() + tToNode,OnTakeoff);
			pIntNode->SetExitTime(pFlight,pFlight->getRealOpTimeOnRunway() + tToNode);

			//pIntNode->SetEnterTime(pFlight,eTimeBegin,OnTakeoff);
			//pIntNode->SetExitTime(pFlight,eTimeEnd);
#if _DEBUGLOG
			{
			CString debugfileName;
			debugfileName.Format( "C:\\RunwayIntNode.txt" );

			std::ofstream outfile;
			outfile.open(debugfileName,std::ios::app);
			char fltstr[255];
			pFlight->getFullID(fltstr);
			CString strNodeName = pIntNode->GetNodeInput().GetName();
			outfile <<"Take Off   "<<pFlight->GetUID()<<"    " << fltstr << "      "<< pFlight->GetTime().printTime() <<"      "<<strNodeName  <<"      "<<(pFlight->getRealOpTimeOnRunway() + tToNode).printTime()<< "        "<< (long)eTimeBegin<<"    "<<eTimeBegin.printTime()<<"     "<< (long)eTimeEnd<<"     "<<eTimeEnd.printTime() <<"     "<< std::endl;
			outfile.close();
			}
#endif

		}
		else
		{
			pIntNode->SetEnterTime(pFlight,pFlight->getRealOpTimeOnRunway() + tToNode - m_eTimeCrossRunwayBuffer,OnTakeoff);
			pIntNode->SetExitTime(pFlight,pFlight->getRealOpTimeOnRunway() + tToNode + m_eTimeCrossRunwayBuffer);
		}
	}
	
	//distance from 0 to exit position
	std::vector<IntersectionNodeInSim *> vPreIntNodes;
	pFlight->GetAndAssignTakeoffRunway()->GetIntersectNodeInRange(0,dExitDistance,vPreIntNodes);

	for (size_t nNode = 0; nNode < vPreIntNodes.size(); ++ nNode)
	{
		IntersectionNodeInSim *pIntNode = vPreIntNodes[nNode];
		if(pIntNode)
		{
			pIntNode->SetEnterTime(pFlight,eTimeBegin,OnTakeoff);
			pIntNode->SetExitTime(pFlight,eTimeEnd);
		}
	}


	////intersection node
	//std::vector<std::pair<RunwayInSim*, IntersectionNodeInSim * > > vIntRunways;			//canceled by emily,the runway's node occupancy tabale already writen
	//GetRunwayIntersectionNodes(pFlight->GetTakeoffRunway(),vIntRunways);
	//size_t nIntRunwayCount = vIntRunways.size();
	//for (size_t nIntRunway = 0; nIntRunway < nIntRunwayCount; ++ nIntRunway )
	//{
	//	IntersectionNodeInSim *pIntNode = vIntRunways[nIntRunway].second;
	//	//pIntNode->SetEnterTime(pFlight, eTimeBegin,OnExitRunway);
	//	//ElapsedTime eIntNodeExitTime = ElapsedTime((eTimeEnd.asSeconds() -eTimeBegin.asSeconds())/2.0) + eTimeBegin;

	//	//pIntNode->SetExitTime(pFlight,eIntNodeExitTime);

	//	double dNodeDistance = pFlight->GetTakeoffRunway()->GetPointDist(pIntNode->GetNodeInput().GetPosition());
	//	//speed
	//	double dSpeed = dLiftSpeed *((dNodeDistance-dExitDistance)/(dLastDistance-dExitDistance));

	//	double dTempValue = 2*(dNodeDistance-dExitDistance)/dAccelSpeed;
	//	double dTime = 0.0;
	//	if(dTempValue > 0)
	//		dTime = sqrt(dTempValue);

	//	pIntNode->SetEnterTime(pFlight,ePreTakeOffTime + ElapsedTime(dTime) - m_eTimeCrossRunwayBuffer,OnTakeoff);
	//	pIntNode->SetExitTime(pFlight,ePreTakeOffTime + m_eTimeCrossRunwayBuffer);

	//}

}

//void CRunwaySystem::GetCircuitFlightTakeoffClearance(AirsideCircuitFlightInSim *pFlight,ElapsedTime tTimeToRwy, ClearanceItem& lastItem,Clearance& newClearance,AirsideCircuitFlightInSim::LandingOperation emType)
//{
//	RunwayExitInSim * pTakeoffPos = pFlight->GetTakeoffPosition();		
//	double takeoffPos = 0.0;
//	AirsideResource * pRes = lastItem.GetResource(); // add at the end of the taxiway item
//	if (pRes)
//	{
//		if (pRes->GetType() == AirsideResource::ResType_AirRouteSegment)
//		{
//			//landing
//			if (emType != AirsideCircuitFlightInSim::LowAndOver_Operation)
//			{
//				ClearanceItem landingItem = GetRunwayTouchdownClearanceItem(pFlight,lastItem.GetTime(),lastItem);
//				lastItem = landingItem;
//				newClearance.AddItem(lastItem);
//			}
//			tTimeToRwy = 0L;
//			GetCircuitFlightRunwayLandingClearance(pFlight,lastItem,newClearance,emType);
//			takeoffPos = lastItem.GetDistInResource();
//		}
//		else if(pRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg&& pTakeoffPos)
//		{
//			FlightGroundRouteDirectSegInSim * pLastSeg  = (TaxiwayDirectSegInSim*)pRes;
//			ClearanceItem lastInSegItem = lastItem;
//			lastInSegItem.SetMode(OnTakeOffEnterRunway);
//
//			newClearance.AddItem(lastInSegItem);
//			lastItem = lastInSegItem;
//
//			takeoffPos = pTakeoffPos->GetEnterRunwayPos();
//		}
//	}
//
//
//	double dRwyLength = pFlight->GetAndAssignTakeoffRunway()->GetEndDist() ;
//	double dMinTakeoffRoll = pFlight->GetPerformance()->getMinTakeoffRoll(pFlight);
//	double dTakeoffRoll = pFlight->GetTakeoffRoll();
//	double dDistToRwyport = dRwyLength - takeoffPos;
//
//
//	BACKTRACKTYPE backtrack = NoBacktrack;
//	AirsideFlightRunwayDelayLog* pLog = NULL;	
//
//	if (dDistToRwyport < dMinTakeoffRoll)
//	{
//		CString strTime = lastItem.GetTime().printTime();
//		char fltstr[255];
//		pFlight->getFullID(fltstr);
//		CString strID;
//		strID.Format("A:%s",fltstr);
//		AirsideDiagnose* pNewDiagnose = new AirsideDiagnose(strTime, strID);		
//		CString strDetals;
//		strDetals.Format("NOT ENOUGH TAKEOFF ROLLING DISTANCE ON RUNWAY %s",pFlight->GetAndAssignTakeoffRunway()->PrintResource() );
//		pNewDiagnose->SetDetails(strDetals);
//		throw new AirsideSystemError(pNewDiagnose);	
//	}
//	else
//	{
//		dTakeoffRoll = dMinTakeoffRoll;
//
//		ClearanceItem preTakeoffItem(pFlight->GetAndAssignTakeoffRunway(),OnPreTakeoff,takeoffPos);
//		preTakeoffItem.SetSpeed(lastItem.GetSpeed());
//		preTakeoffItem.SetTime(lastItem.GetTime() +tTimeToRwy);
//		lastItem = preTakeoffItem;	
//		newClearance.AddItem(lastItem);
//	}
//
//
//	//add at take off pos item
//	ClearanceItem atPosItem = lastItem;
//	atPosItem.SetMode(OnTakeoff);
//
//	atPosItem.SetDistInResource(lastItem.GetDistInResource() + 1);
//	ElapsedTime dT = pFlight->GetTimeBetween(lastItem,atPosItem);
//	atPosItem.SetTime(lastItem.GetTime() + dT);
//	lastItem = atPosItem;
//	newClearance.AddItem(lastItem);
//
//	
//
//	////////////////runway delay
//	if (pLog)
//	{
//		pLog->mAvailableTime = lastItem.GetTime().asSeconds();
//		pFlight->LogEventItem(pLog);	
//	}
//
//	//////////////////////////////////
//
//	if (emType == AirsideCircuitFlightInSim::Normal_Operation)
//	{
//		//check the flight need to waiting the separation with pre-flight & take off position time
//		{
//			//take off position time has twp types
//			//1. if the flight has to waiting a few seconds to keep the separation with pre-fight, if the time larger than take off position time
//			//, use the larger one
//			//2. if the flight does not need to wait the pre-flight, or has no pre flight, using take off position time.
//
//			if(pFlight->getTOStartPrepareTime() > ElapsedTime(0L))//need to wait, the value was set in apply for take off time
//			{
//				ElapsedTime dWaitingSeparationTime = pFlight->getTOStartPrepareTime();
//				double dPositionTime = pFlight->GetTakeoffPositionTime();
//				ElapsedTime estPositionTime(dPositionTime);
//				if(dWaitingSeparationTime > estPositionTime)//use the larger one
//					estPositionTime = dWaitingSeparationTime;
//
//				ClearanceItem waitingSeparationItem = lastItem;
//
//				waitingSeparationItem.SetTime(lastItem.GetTime() + estPositionTime);
//				waitingSeparationItem.SetSpeed(0.0);
//				lastItem = waitingSeparationItem;
//				newClearance.AddItem(waitingSeparationItem);
//			}
//			else//need take off position time, 
//			{
//				//takeoff position time
//			/*	double dPositionTime = pFlight->GetTakeoffPositionTime();
//				ClearanceItem positionTimeItem = lastItem;
//				ElapsedTime estPositionTime(dPositionTime);
//				positionTimeItem.SetTime(lastItem.GetTime() + estPositionTime);
//				positionTimeItem.SetSpeed(0.0);
//				lastItem = positionTimeItem;
//				newClearance.AddItem(positionTimeItem);*/
//			}
//			//check the pre-flight has been went out runway or not
//			if(pFlight->getPreFlightExitRunwayTime() > ElapsedTime(0L))
//			{
//				ElapsedTime filletTime;//default value from runway exit fillet time
//				ASSERT(pFlight->GetAndAssignTakeoffRunway());
//				GetRunwayExitFilletTime(pFlight,pFlight->GetAndAssignTakeoffRunway(),lastItem.GetTime() +tTimeToRwy,filletTime);
//
//				ElapsedTime exitTime = pFlight->getPreFlightExitRunwayTime() + filletTime;
//				//waiting the pre flight exit runway
//				if(exitTime > lastItem.GetTime())
//				{
//
//					ClearanceItem waitingSeparationItem = lastItem;
//					waitingSeparationItem.SetTime(/*pFlight->getPreFlightExitRunwayTime()*/exitTime);
//					waitingSeparationItem.SetSpeed(0.0);
//					lastItem = waitingSeparationItem;
//					newClearance.AddItem(waitingSeparationItem);
//				}
//			}
//		}
//
//	}
//
//	pFlight->setRealOpTimeOnRunway(lastItem.GetTime());
//
//	//take off item
//	{
//		ClearanceItem takeoffItem = lastItem;
//		takeoffItem.SetMode(OnTakeoff);
//		ElapsedTime dT = pFlight->GetTimeBetween(lastItem,takeoffItem);
//		takeoffItem.SetTime(lastItem.GetTime() + dT);
//		lastItem = takeoffItem;
//		newClearance.AddItem(lastItem);
//	}
//
//	// roll item
//
//	double dLiftOffSpeed = pFlight->GetLiftoffSpeed();
//	{
//		ClearanceItem rollitem = lastItem;
//		if (lastItem.GetPosition().distance(pFlight->GetAndAssignTakeoffRunway()->GetDistancePoint(0)) + dTakeoffRoll > dRwyLength)
//			rollitem.SetDistInResource( rollitem.GetDistInResource()+ dMinTakeoffRoll );
//		else
//			rollitem.SetDistInResource( rollitem.GetDistInResource()+ dTakeoffRoll );
//		rollitem.SetSpeed(dLiftOffSpeed);
//		ElapsedTime dT = pFlight->GetTimeBetween(lastItem,rollitem);
//		rollitem.SetTime(rollitem.GetTime()+ dT);
//		rollitem.SetMode(OnTakeoff);
//		lastItem = rollitem;
//		newClearance.AddItem(lastItem);
//	}
//
//}
//
void CRunwaySystem::GetFlightTakeOffClearance(AirsideFlightInSim *pFlight,/*ElapsedTime eTakeOffTime,*/ElapsedTime tTimeToRwy, ClearanceItem& lastItem,Clearance& newClearance)
{
	RunwayExitInSim * pTakeoffPos = pFlight->GetAndAssignTakeoffPosition();		
	DistanceUnit takeoffPos = 0; 
	if(pTakeoffPos)
	{
		AirsideResource * pRes = lastItem.GetResource(); // add at the end of the taxiway item
		if(pRes && pRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
		{
			FlightGroundRouteDirectSegInSim * pLastSeg  = (TaxiwayDirectSegInSim*)pRes;
			ClearanceItem lastInSegItem = lastItem;
			lastInSegItem.SetMode(OnTakeOffEnterRunway);
			//lastInSegItem.SetDistInResource(pLastSeg->GetExitHoldDist());
			//ElapsedTime dT = pFlight->GetTimeBetween(lastItem, lastInSegItem);
			//lastInSegItem.SetTime(dT + lastItem.GetTime());
			//ClearanceItem lastInSegItem(pLastSeg,OnTakeOffEnterRunway,);
			////lastInSegItem.SetSpeed(pFlight->GetPerformance()->getTaxiOutNormalSpeed(pFlight));
			
			//lastInSegItem.SetSpeed(lastItem.GetSpeed());			
			newClearance.AddItem(lastInSegItem);
			lastItem = lastInSegItem;
		}
		takeoffPos = pTakeoffPos->GetEnterRunwayPos();
	}
 

	double dRwyLength = pFlight->GetAndAssignTakeoffRunway()->GetEndDist() ;
	double dMinTakeoffRoll = pFlight->GetPerformance()->getMinTakeoffRoll(pFlight);
	double dTakeoffRoll = pFlight->GetTakeoffRoll();
	double dDistToRwyport = dRwyLength - takeoffPos;
	

//	TakeOffBackTrack backtrack = BT_NoBacktrack;
	RunwayResourceManager * pRunwayRes = pFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource()->getRunwayResource();
	TakeOffBackTrack backtrack = pFlight->GetAirTrafficController()->GetRunwayAssignmentStrategy()->GetTakeoffBacktrack(pFlight,pRunwayRes);
	AirsideFlightRunwayDelayLog* pLog = NULL;	
	bool bShort = dDistToRwyport < dTakeoffRoll ? true : false;
	if (!bShort)	//the distance to runway port less than rolling distance
	{
		/*RunwayResourceManager * pRunwayRes = pFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource()->getRunwayResource();
		backtrack = pFlight->GetAirTrafficController()->GetRunwayAssignmentStrategy()->GetTakeoffBacktrack(pFlight,pRunwayRes);*/
		if (backtrack == BT_MinBacktrack)
		{
			backtrack = BT_NoBacktrack;
		}
	}
	if (backtrack == BT_NoBacktrack)
	{
		if (dDistToRwyport < dMinTakeoffRoll)
		{
			CString strTime = lastItem.GetTime().printTime();
			char fltstr[255];
			pFlight->getFullID(fltstr);
			CString strID;
			strID.Format("A:%s",fltstr);
			AirsideDiagnose* pNewDiagnose = new AirsideDiagnose(strTime, strID);		
			CString strDetals;
			strDetals.Format("NOT ENOUGH TAKEOFF ROLLING DISTANCE ON RUNWAY %s",pFlight->GetAndAssignTakeoffRunway()->PrintResource() );
			pNewDiagnose->SetDetails(strDetals);
			throw new AirsideSystemError(pNewDiagnose);	
		}
		else
		{
			dTakeoffRoll = dMinTakeoffRoll;

			//wait for landing flight cross runway exit completely
// 			DistanceUnit takeoffPos = 0;
// 			ClearanceItem startItem(pFlight->GetTakeoffRunway(),OnPreTakeoff,takeoffPos);
// 			startItem.SetSpeed(0);
// 			startItem.SetTime(lastItem.GetTime() +tTimeToRwy);
// 			newClearance.AddItem(startItem);

			ClearanceItem preTakeoffItem(pFlight->GetAndAssignTakeoffRunway(),OnPreTakeoff,takeoffPos);
			preTakeoffItem.SetSpeed(0);
			preTakeoffItem.SetTime(lastItem.GetTime() +tTimeToRwy);
			lastItem = preTakeoffItem;	
			newClearance.AddItem(lastItem);
		}

	}
	else		//back track
	{
		//enter runway pos
		LogicRunwayInSim* pOtherPort = pFlight->GetAndAssignTakeoffRunway()->GetOtherPort();
		RunwayExitInSim* pOtherExit = pOtherPort->GetSameNodeRunwayExitWith(pTakeoffPos);
		if(pOtherExit)
		{
			ClearanceItem RollingOnRwyItem( pOtherPort,OnPreTakeoff,pOtherExit->GetEnterRunwayPos() );
			RollingOnRwyItem.SetSpeed(pFlight->GetPerformance()->getTaxiOutNormalSpeed(pFlight));
			ElapsedTime dT = pFlight->GetTimeBetween(lastItem,RollingOnRwyItem);
			RollingOnRwyItem.SetTime(lastItem.GetTime() +dT);
			lastItem = RollingOnRwyItem;	
			newClearance.AddItem(lastItem);
		}
		ElapsedTime tEnter = lastItem.GetTime();

		if (backtrack == BT_MaxBacktrack)
		{
			takeoffPos = pFlight->GetLength();
		}
		if (backtrack == BT_MinBacktrack)
		{
			if (dRwyLength >= dTakeoffRoll)
			{	
				takeoffPos = dRwyLength - dTakeoffRoll;
			}
			else
			{
				takeoffPos = dRwyLength - dMinTakeoffRoll;
			}

		}		
		//other port position
		DistanceUnit dOtherPortPos =  pOtherPort->GetEndDist() - takeoffPos;
		//if(pOtherExit && pOtherExit->GetEnterRunwayPos() < takeoffPos )
		//	dOtherPortPos = pOtherExit->GetEnterRunwayPos();

		ClearanceItem RollingOnRwyItem( pOtherPort,OnPreTakeoff,dOtherPortPos);
		RollingOnRwyItem.SetSpeed(pFlight->GetPerformance()->getTaxiOutNormalSpeed(pFlight));
		ElapsedTime dT = pFlight->GetTimeBetween(lastItem,RollingOnRwyItem);
		RollingOnRwyItem.SetTime(lastItem.GetTime() +dT);
		lastItem = RollingOnRwyItem;	
		newClearance.AddItem(lastItem);	

		ElapsedTime uTuneTime = ElapsedTime(20L);
		ClearanceItem preTakeoffItem(pFlight->GetAndAssignTakeoffRunway(),OnPreTakeoff,takeoffPos);
		preTakeoffItem.SetSpeed(lastItem.GetSpeed());
		preTakeoffItem.SetTime(lastItem.GetTime() +uTuneTime);
		lastItem = preTakeoffItem;	
		newClearance.AddItem(lastItem);

		///////back track delay
		ResourceDesc resDesc;
		pFlight->GetAndAssignTakeoffRunway()->getDesc(resDesc);
		pLog = new AirsideFlightRunwayDelayLog(resDesc, lastItem.GetTime().asSeconds(), OnTakeoff, 0l, 0l, FlightRunwayDelay::UNKNOWNPOS);
		pLog->mDelayTime = (lastItem.GetTime() - tEnter).asSeconds();
		CString strDetail;
		strDetail.Format("Require back track %.2f m",abs((dTakeoffRoll - takeoffPos)/100));
		pLog->sReasonDetail = strDetail.GetString();
		//////////////////

		//if (pFlight->GetMode() == OnTowToDestination)
		//{
		//	dT = pFlight->GetUnhookTime();
		//	preTakeoffItem.SetSpeed(lastItem.GetSpeed());
		//	preTakeoffItem.SetTime(lastItem.GetTime() +dT);
		//	preTakeoffItem.SetDistInResource(lastItem.GetDistInResource() +1);
		//	lastItem = preTakeoffItem;	
		//	newClearance.AddItem(lastItem);
		//}
	}
		



	//add at take off pos item
	ClearanceItem atPosItem = lastItem;
	atPosItem.SetMode(OnTakeoff);
	if (pFlight->GetMode() == OnTowToDestination)
	{
		ElapsedTime dT = pFlight->GetUnhookTime();
		atPosItem.SetTime(lastItem.GetTime() +dT);
		atPosItem.SetDistInResource(lastItem.GetDistInResource() +1);
		lastItem = atPosItem;	
		newClearance.AddItem(lastItem);
	}
	else
	{
		atPosItem.SetDistInResource(lastItem.GetDistInResource() + 1);
		ElapsedTime dT = pFlight->GetTimeBetween(lastItem,atPosItem);
		atPosItem.SetTime(lastItem.GetTime() + dT);
		lastItem = atPosItem;
		newClearance.AddItem(lastItem);

	}

	////////////////runway delay
	if (pLog)
	{
		pLog->mAvailableTime = lastItem.GetTime().asSeconds();
		pFlight->LogEventItem(pLog);	
	}

	//////////////////////////////////

	//check the flight need to waiting the separation with pre-flight & take off position time
	{
		//take off position time has twp types
		//1. if the flight has to waiting a few seconds to keep the separation with pre-fight, if the time larger than take off position time
		//, use the larger one
		//2. if the flight does not need to wait the pre-flight, or has no pre flight, using take off position time.

		if(pFlight->getTOStartPrepareTime() > ElapsedTime(0L))//need to wait, the value was set in apply for take off time
		{
			ElapsedTime dWaitingSeparationTime = pFlight->getTOStartPrepareTime();
			double dPositionTime = pFlight->GetTakeoffPositionTime();
			ElapsedTime estPositionTime(dPositionTime);
			if(dWaitingSeparationTime > estPositionTime)//use the larger one
				estPositionTime = dWaitingSeparationTime;

			ClearanceItem waitingSeparationItem = lastItem;

			waitingSeparationItem.SetTime(lastItem.GetTime() + estPositionTime);
			waitingSeparationItem.SetSpeed(0.0);
			lastItem = waitingSeparationItem;
			newClearance.AddItem(waitingSeparationItem);
		}
		else//need take off position time, 
		{
			//takeoff position time
			double dPositionTime = pFlight->GetTakeoffPositionTime();
			ClearanceItem positionTimeItem = lastItem;
			ElapsedTime estPositionTime(dPositionTime);
			positionTimeItem.SetTime(lastItem.GetTime() + estPositionTime);
			positionTimeItem.SetSpeed(0.0);
			lastItem = positionTimeItem;
			newClearance.AddItem(positionTimeItem);
		}
		//check the pre-flight has been went out runway or not
		if(pFlight->getPreFlightExitRunwayTime() > ElapsedTime(0L))
		{
			ElapsedTime filletTime;//default value from runway exit fillet time
			ASSERT(pFlight->GetAndAssignTakeoffRunway());
			GetRunwayExitFilletTime(pFlight,pFlight->GetAndAssignTakeoffRunway(),lastItem.GetTime() +tTimeToRwy,filletTime);
			
			ElapsedTime exitTime = pFlight->getPreFlightExitRunwayTime() + filletTime;
			//waiting the pre flight exit runway
			if(/*pFlight->getPreFlightExitRunwayTime()*/ exitTime > lastItem.GetTime())
			{
				
				ClearanceItem waitingSeparationItem = lastItem;
				waitingSeparationItem.SetTime(/*pFlight->getPreFlightExitRunwayTime()*/exitTime);
				waitingSeparationItem.SetSpeed(0.0);
				lastItem = waitingSeparationItem;
				newClearance.AddItem(waitingSeparationItem);
			}
		}
	}

	pFlight->setRealOpTimeOnRunway(lastItem.GetTime());
	
	{

		////check pre flight is moving out the runway or not, if not,waiting
		//{
		//	////get the runway's last occupancy flight, check the flight has been moved out or not
		//	//OccupancyInstance preFlightInstance;
		//	//GetFirstFlightBeforeTime(pFlight->GetTakeoffRunway()->GetOccupancyTable(),lastItem.GetTime(),preFlightInstance);
		//	//if(preFlightInstance.IsValid() &&  preFlightInstance.GetExitTime() > lastItem.GetTime())
		//	//{
		//		//waiting pre-flight out of runway
		//		ClearanceItem waitingPreFlightTimeItem = lastItem;
		//		//after the pre-flight get out the runway 2 seconds, the flight take off
		//		waitingPreFlightTimeItem.SetTime(pFlight->getRealOpTimeOnRunway());
		//		waitingPreFlightTimeItem.SetSpeed(0.0);
		//		lastItem = waitingPreFlightTimeItem;
		//		newClearance.AddItem(waitingPreFlightTimeItem);
		//	//}
		//}
		////set the flight start move time

	}

	//take off item
	{
		ClearanceItem takeoffItem = lastItem;
		takeoffItem.SetMode(OnTakeoff);
		ElapsedTime dT = pFlight->GetTimeBetween(lastItem,takeoffItem);
		takeoffItem.SetTime(lastItem.GetTime() + dT);
		lastItem = takeoffItem;
		newClearance.AddItem(lastItem);
	}

	// roll item

	double dLiftOffSpeed = pFlight->GetLiftoffSpeed();
	{
		ClearanceItem rollitem = lastItem;
		if (lastItem.GetPosition().distance(pFlight->GetAndAssignTakeoffRunway()->GetDistancePoint(0)) + dTakeoffRoll > dRwyLength)
			rollitem.SetDistInResource( rollitem.GetDistInResource()+ dMinTakeoffRoll );
		else
			rollitem.SetDistInResource( rollitem.GetDistInResource()+ dTakeoffRoll );
		rollitem.SetSpeed(dLiftOffSpeed);
		ElapsedTime dT = pFlight->GetTimeBetween(lastItem,rollitem);
		rollitem.SetTime(rollitem.GetTime()+ dT);
		rollitem.SetMode(OnTakeoff);
		lastItem = rollitem;
		newClearance.AddItem(lastItem);
	}

}

void CRunwaySystem::InitlizationRelatedUnintersectedRunways()
{
	std::vector<std::pair<LogicRunwayInSim*,LogicRunwayInSim*> > vRelatedRunways;

	CRunwaySeparationRelation* pRwyRelation = m_pApproachSeparationInSim->GetApproachSepRwyRelation();

	vRelatedRunways.clear();
	m_pApproachSeparationInSim->GetRelatedRunwayListByACOpType(m_vRunwayInSim,AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED ,vRelatedRunways);

	std::vector<std::pair<LogicRunwayInSim*,LogicRunwayInSim*> >::iterator iter = vRelatedRunways.begin();
	for (;iter != vRelatedRunways.end(); iter++)
	{
		LogicRunwayInSim* pRwy1 = iter->first;
		LogicRunwayInSim* pRwy2 = iter->second;

		AddNewRelatedRunwayIntersectionNode(pRwy1,pRwy2,pRwyRelation,AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED);

		//RunwayIntersectionType eInterType;
		//CPoint2008 InterP;
		//double dDist1, dDist2;

		//RunwayRelationType eRelation = pRwyRelation->GetLogicRunwayRelationship(AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED,
		//																		pRwy1->GetRunwayInSim()->GetRunwayID(),
		//																		pRwy1->getLogicRunwayType(),
		//																		pRwy2->GetRunwayInSim()->GetRunwayID(),
		//																		pRwy2->getLogicRunwayType(),
		//																		eInterType, InterP, dDist1, dDist2);

		//if (eRelation == RRT_INTERSECT || eInterType == RIT_NONE)		// intersected runways or parallel runways
		//	continue;

		//RunwayRelatedIntersectionNodeInSim* pNode = new RunwayRelatedIntersectionNodeInSim(this, pRwy1,pRwy2, AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED);
		//pNode->SetPosition(InterP);
		//pNode->SetIntersectType(eInterType);

		//m_vRunwayRelatedIntersectionNodes.push_back(pNode);
	}

	vRelatedRunways.clear();
	m_pApproachSeparationInSim->GetRelatedRunwayListByACOpType(m_vRunwayInSim,AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF ,vRelatedRunways);

	iter = vRelatedRunways.begin();
	for (;iter != vRelatedRunways.end(); iter++)
	{
		LogicRunwayInSim* pRwy1 = iter->first;
		LogicRunwayInSim* pRwy2 = iter->second;

		AddNewRelatedRunwayIntersectionNode(pRwy1,pRwy2,pRwyRelation,AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF);

		//RunwayIntersectionType eInterType;
		//CPoint2008 InterP;
		//double dDist1, dDist2;

		//RunwayRelationType eRelation = pRwyRelation->GetLogicRunwayRelationship(AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF,
		//	pRwy1->GetRunwayInSim()->GetRunwayID(),
		//	pRwy1->getLogicRunwayType(),
		//	pRwy2->GetRunwayInSim()->GetRunwayID(),
		//	pRwy2->getLogicRunwayType(),
		//	eInterType, InterP, dDist1, dDist2);

		//if (eRelation == RRT_INTERSECT || eInterType == RIT_NONE)		// intersected runways or parallel runways
		//	continue;

		//RunwayRelatedIntersectionNodeInSim* pNode = new RunwayRelatedIntersectionNodeInSim(this,pRwy1,pRwy2, AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF);
		//pNode->SetPosition(InterP);
		//pNode->SetIntersectType(eInterType);

		//m_vRunwayRelatedIntersectionNodes.push_back(pNode);
	}


	pRwyRelation = m_pTakeOffSeparationInSim->GetTakeSepRwyRelation();

	vRelatedRunways.clear();
	m_pTakeOffSeparationInSim->GetRelatedRunwayListByAcOpType(m_vRunwayInSim, AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED, vRelatedRunways);
	iter = vRelatedRunways.begin();
	for (;iter != vRelatedRunways.end(); iter++)
	{
		LogicRunwayInSim* pRwy1 = iter->first;
		LogicRunwayInSim* pRwy2 = iter->second;

		AddNewRelatedRunwayIntersectionNode(pRwy1,pRwy2,pRwyRelation,AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED);

		//RunwayIntersectionType eInterType;
		//CPoint2008 InterP;
		//double dDist1, dDist2;

		//RunwayRelationType eRelation = pRwyRelation->GetLogicRunwayRelationship(AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED,
		//	pRwy1->GetRunwayInSim()->GetRunwayID(),
		//	pRwy1->getLogicRunwayType(),
		//	pRwy2->GetRunwayInSim()->GetRunwayID(),
		//	pRwy2->getLogicRunwayType(),
		//	eInterType, InterP, dDist1, dDist2);

		//if (eRelation == RRT_INTERSECT || eInterType == RIT_NONE)		// intersected runways or parallel runways
		//	continue;

		//RunwayRelatedIntersectionNodeInSim* pNode = new RunwayRelatedIntersectionNodeInSim(this,pRwy1,pRwy2, AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED);
		//pNode->SetPosition(InterP);
		//pNode->SetIntersectType(eInterType);

		//m_vRunwayRelatedIntersectionNodes.push_back(pNode);
	}

	vRelatedRunways.clear();
	m_pTakeOffSeparationInSim->GetRelatedRunwayListByAcOpType(m_vRunwayInSim, AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF, vRelatedRunways);
	iter = vRelatedRunways.begin();
	for (;iter != vRelatedRunways.end(); iter++)
	{
		LogicRunwayInSim* pRwy1 = iter->first;
		LogicRunwayInSim* pRwy2 = iter->second;
		
		AddNewRelatedRunwayIntersectionNode(pRwy1,pRwy2,pRwyRelation,AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF);

	}


}

void CRunwaySystem::AddNewRelatedRunwayIntersectionNode( LogicRunwayInSim* pRwy1,LogicRunwayInSim* pRwy2, CRunwaySeparationRelation* pRwyRelation, AIRCRAFTOPERATIONTYPE eType )
{
	RunwayIntersectionType eInterType;
	CPoint2008 InterP;
	double dDist1, dDist2;

	RunwayRelationType eRelation = pRwyRelation->GetLogicRunwayRelationship(eType,
		pRwy1->GetRunwayInSim()->GetRunwayID(),
		pRwy1->getLogicRunwayType(),
		pRwy2->GetRunwayInSim()->GetRunwayID(),
		pRwy2->getLogicRunwayType(),
		eInterType, InterP, dDist1, dDist2);

	if (eRelation == RRT_INTERSECT || eInterType == RIT_NONE)		// intersected runways or parallel runways
		return;

	size_t nSize = m_vRunwayRelatedIntersectionNodes.size();
	for (size_t i =0; i <nSize; i++)
	{
		RunwayRelatedIntersectionNodeInSim* pIntNode = m_vRunwayRelatedIntersectionNodes.at(i);
		if (pIntNode->IsSameNode(pRwy1,pRwy2,InterP,eType))
			return;
	}

	RunwayRelatedIntersectionNodeInSim* pNode = new RunwayRelatedIntersectionNodeInSim(this,pRwy1,pRwy2, eType);
	pNode->SetPosition(InterP);
	pNode->SetIntersectType(eInterType);

	m_vRunwayRelatedIntersectionNodes.push_back(pNode);
}

void CRunwaySystem::SetOutputAirside( OutputAirside *pOutput )
{
	ASSERT(pOutput != NULL);
	m_pOutput = pOutput;
}

ElapsedTime CRunwaySystem::GetSeperationTime( AirsideFlightInSim* pTrailFlight, AirsideFlightInSim* pLeadFlight, AIRCRAFTOPERATIONTYPE eOpType )
{
	double dSepTime = 0.0;
	if (eOpType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED)
		m_pApproachSeparationInSim->GetApproachSeparationTime(pTrailFlight,pLeadFlight,OnLanding, dSepTime);
	
	if (eOpType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF)
		m_pApproachSeparationInSim->GetApproachSeparationTime(pTrailFlight,pLeadFlight,OnTakeoff, dSepTime);	

	if (eOpType == AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED)
		m_pTakeOffSeparationInSim->GetTakeoffSeparationTime(pTrailFlight, pLeadFlight, OnLanding, dSepTime);

	if (eOpType == AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF)
		m_pTakeOffSeparationInSim->GetTakeoffSeparationTime(pTrailFlight, pLeadFlight, OnTakeoff, dSepTime);

	return ElapsedTime(dSepTime);
}

ElapsedTime CRunwaySystem::GetFlightDelayTimeAtRunwayRelatedIntersections( AirsideFlightInSim* pFlight,ElapsedTime tRwyTime, double dDistToRwyPort )
{
	size_t nSize = m_vRunwayRelatedIntersectionNodes.size();
	for (size_t i = 0; i < nSize; i++)
	{
		RunwayRelatedIntersectionNodeInSim* pNode = m_vRunwayRelatedIntersectionNodes.at(i);
		ElapsedTime tTimeAtNode = 0L;

		if(pFlight->IsArrivingOperation())
		{
			if (pFlight->GetLandingRunway() != pNode->GetOpRunway1() && pFlight->GetLandingRunway() != pNode->GetOpRunway2())
				continue;

			CPath2008 path = pFlight->GetLandingRunway()->GetRunwayInSim()->GetPath();
			if(pNode->GetIntersectionType() == RIT_RUNWAY && path.bPointInPath (pNode->GetPosition()))		//node at runway
			{
				double dNodeDistance = pFlight->GetLandingRunway()->GetPointDist(pNode->GetPosition());
				double dExitDistance = pFlight->GetRunwayExit()->GetExitRunwayPos();

				DynamicMovement flightmove(pFlight->GetPerformance()->getLandingSpeed(pFlight), pFlight->GetExitSpeed(),dExitDistance);
				if (dNodeDistance > dExitDistance)
					dNodeDistance = dExitDistance;
				
				ElapsedTime tTimeAtRwy = flightmove.GetDistTime(dNodeDistance);
				tTimeAtNode = tRwyTime + tTimeAtRwy;
			}
			else
			{
				ElapsedTime tPointToRwy = m_pAirRouteNetwork->GetLandingFlightTimeFromPointToRwy(pFlight, pFlight->GetSTAR(),pNode->GetPosition());
				tTimeAtNode = tRwyTime - tPointToRwy;
			}

		}
		else
		{
			if (pFlight->GetAndAssignTakeoffRunway() != pNode->GetOpRunway1() && pFlight->GetAndAssignTakeoffRunway() != pNode->GetOpRunway2())
				continue;

			CPath2008 path = pFlight->GetAndAssignTakeoffRunway()->GetRunwayInSim()->GetPath();
			if(pNode->GetIntersectionType() == RIT_RUNWAY && path.bPointInPath(pNode->GetPosition()))		//node at runway
			{
				double dNodeDistance = pFlight->GetAndAssignTakeoffRunway()->GetPointDist(pNode->GetPosition());

				DynamicMovement flightmove(0,pFlight->GetLiftoffSpeed(),dDistToRwyPort);
				if (dNodeDistance > dDistToRwyPort)
					dNodeDistance = dDistToRwyPort;

				ElapsedTime tTimeAtRwy = flightmove.GetDistTime(dDistToRwyPort) - flightmove.GetDistTime(dNodeDistance);
				tTimeAtNode = tRwyTime - tTimeAtRwy;

			}
			else
			{
				ElapsedTime tRwyToPoint = m_pAirRouteNetwork->GetTakeoffFlightTimeFromRwyToPoint(pFlight, pFlight->GetSID(), pNode->GetPosition());
				tTimeAtNode = tRwyTime + tRwyToPoint;
			}

		}

		ElapsedTime tDelay = pNode->GetFlightDelayTime(pFlight,tTimeAtNode);

		if (tDelay >0L)
			return tDelay;

	}

	return 0L;

}

ElapsedTime CRunwaySystem::getFlightTakeoffTime( AirsideFlightInSim* pFlight )
{

	RunwayExitInSim * pTakeoffPos = pFlight->GetAndAssignTakeoffPosition();		
	DistanceUnit takeoffPos = 0; 
	if(pTakeoffPos)
		takeoffPos = pTakeoffPos->GetEnterRunwayPos();

	double dRwyLength = pFlight->GetAndAssignTakeoffRunway()->GetEndDist() ;
	double dMinTakeoffRoll = pFlight->GetPerformance()->getMinTakeoffRoll(pFlight);
	double dTakeoffRoll = pFlight->GetTakeoffRoll();
	double dDistToRwyport = dRwyLength - takeoffPos;
	ElapsedTime tTakeoffTime = 0L;

	BACKTRACKTYPE backtrack = NoBacktrack;

	if (dDistToRwyport < dTakeoffRoll)	//the distance to runway port less than rolling distance
		backtrack = m_pRunwayTakeoffPosStrategy->GetBacktrackTypeOfTakeoffPos(pFlight);

	if(backtrack)		//back track
	{
		LogicRunwayInSim* pOtherPort = pFlight->GetAndAssignTakeoffRunway()->GetOtherPort();
		RunwayExitInSim* pOtherExit = pOtherPort->GetSameNodeRunwayExitWith(pTakeoffPos);

		if (backtrack == MaxBacktrack)
			takeoffPos = pFlight->GetLength();

		if (backtrack == MinBacktrack)
		{
			if (dRwyLength >= dTakeoffRoll)	
				takeoffPos = dRwyLength - dTakeoffRoll;
			else
				takeoffPos = dRwyLength - dMinTakeoffRoll;
		}		

		//other port position
		DistanceUnit dOtherPortPos =  pOtherPort->GetEndDist() - takeoffPos;
		if(pOtherExit && pOtherExit->GetEnterRunwayPos() < takeoffPos )
			dOtherPortPos = pOtherExit->GetEnterRunwayPos();

		tTakeoffTime += ElapsedTime(2.0*dOtherPortPos/pFlight->GetPerformance()->getTaxiOutNormalSpeed(pFlight));
	}

	if (pFlight->GetMode() == OnTowToDestination)
		tTakeoffTime += pFlight->GetUnhookTime();
	else
		tTakeoffTime += ElapsedTime(pFlight->GetTakeoffPositionTime());

	double dLiftOffSpeed = pFlight->GetLiftoffSpeed();
	tTakeoffTime += ElapsedTime( 2.0*dTakeoffRoll/dLiftOffSpeed );
	
	return tTakeoffTime;
}

bool CRunwaySystem::_GenTakeoffFirstWaypointItem( AirsideCircuitFlightInSim * pFlight,FlightRouteInSim* pCircuitRoute,ClearanceItem& lastItem )
{
	//climbout to the point
	if(pCircuitRoute->GetItemCount()>1){
		if( AirsideResource* pRes=  pCircuitRoute->GetItem(1) )
		{
			if(pRes->GetType()== AirsideResource::ResType_AirRouteSegment){
				if( AirRoutePointInSim* pRoutePoint = ((AirRouteSegInSim*)pRes)->GetFirstConcernIntersection()->getInputPoint() )
				{
					ClearanceItem pointItem(pRoutePoint,OnClimbout,0);		
					FlightPlanPropertyInSim prop =  pCircuitRoute->GetRoutePointProperty(pRoutePoint);
					pointItem.SetSpeed(lastItem.GetSpeed());
					pointItem.SetAltitude(prop.GetAlt());
					pointItem.SetPosition(pRoutePoint->GetPosition());
					//CPoint2008 curPos = lastItem.GetPosition();
					ElapsedTime moveTime = pFlight->GetTimeBetween(lastItem,pointItem);
					pointItem.SetTime(moveTime + lastItem.GetTime() );
					lastItem  = pointItem;					
					return true;
				}
			}
		}
	}	
	return false;
}

void CRunwaySystem::_WriteAfterNodesTime( AirsideFlightInSim* pFlight, LogicRunwayInSim* pRunway, const ClearanceItem& endItem )
{
	OccupancyInstance ocyInst = pRunway->GetOccupyInstance(pFlight);
	std::vector<IntersectionNodeInSim *> vAfterIntNodes;
	pFlight->GetLandingRunway()->GetIntersectNodeInRange(endItem.GetDistInResource() + 1,pRunway->GetEndDist(),vAfterIntNodes);
	for (size_t nNode = 0; nNode < vAfterIntNodes.size(); ++ nNode)
	{
		IntersectionNodeInSim *pIntNode = vAfterIntNodes[nNode];
		if(!pIntNode)continue;

		if(!pIntNode->GetNodeInput().IsRunwaysIntersection())
		{
			pIntNode->SetEnterTime(pFlight,ocyInst.GetEnterTime(),OnLanding);
			pIntNode->SetExitTime(pFlight,ocyInst.GetExitTime() );
		}
		else
		{
			pIntNode->SetEnterTime(pFlight,endItem.GetTime(),OnLanding);
			pIntNode->SetExitTime(pFlight,endItem.GetTime()+1L);
		}
	}

}

void CRunwaySystem::_WritePreNodesTime( AirsideFlightInSim* pFlight , LogicRunwayInSim* pRunway , const ClearanceItem& beginItem )
{
	std::vector<IntersectionNodeInSim *> vPreIntNodes;
	pRunway->GetIntersectNodeInRange(0,beginItem.GetDistInResource(),vPreIntNodes);
	for (size_t nNode = 0; nNode < vPreIntNodes.size(); ++ nNode)
	{
		IntersectionNodeInSim *pIntNode = vPreIntNodes[nNode];
		if(pIntNode)
		{
			pIntNode->SetEnterTime(pFlight,beginItem.GetTime() - m_eTimeCrossRunwayBuffer,OnLanding);
			pIntNode->SetExitTime(pFlight,beginItem.GetTime()  + m_eTimeCrossRunwayBuffer);
		}
	}
}

