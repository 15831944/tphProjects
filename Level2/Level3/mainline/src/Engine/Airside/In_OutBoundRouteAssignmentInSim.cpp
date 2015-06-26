#include "StdAfx.h"
#include ".\in_outboundrouteassignmentinsim.h"
#include "AirsideFlightInSim.h"
#include "TakeoffQueueInSim.h"
#include "StandInSim.h"
#include "RunwayInSim.h"
#include "IntersectionNodeInSim.h"
#include "../../InputAirside/BoundRouteAssignment.h"
#include "../../InputAirside/TaxiRoute.h"
#include "../../Common/AirportDatabase.h"
#include "../../InputAirside/GroundRoute.h"
#include "StandLeadOutLineInSim.h"
#include "SimulationErrorShow.h"
#include "FlightGroundRouteResourceManager.h"
#include "RunwayResourceManager.h"
#include ".\deiceresourcemanager.h"
#include "./StandLeadInLineInSim.h"
#include "./StandLeadOutLineInSim.h"
#include "./StartPositionInSim.h"
#include "./HoldShortLineInSim.h"
#include "FlightPerformanceManager.h"
#include "../../InputAirside/PostDeice_BoundRoute.h"
#include "../../InputAirside/HoldShortLines.h"

#include <Common/ARCTracker.h>
#include <Common/ARCTracker.h>
#include <Common/ARCProbPlacer.h>
#include <limits>
#include "DeicepadGroup.h"
#include "InputAirside/OccupiedAssignedStandAction.h"

In_OutBoundRouteAssignmentInSim::In_OutBoundRouteAssignmentInSim(void)
:m_pInboundRouteAssignment(NULL)
,m_pOutboundRouteAssignment(NULL)
,m_pDeiceRouteAssigment(NULL)
,m_pPostDeiceRouteAssigment(NULL)
,m_pTaxiResManager(NULL)
,m_pIntersectionNodeManager(NULL)
,m_pGroundResManager(NULL)
,m_pRunwayResManager(NULL)
{

}

In_OutBoundRouteAssignmentInSim::~In_OutBoundRouteAssignmentInSim(void)
{
	PLACE_METHOD_TRACK_STRING();
	if (m_pInboundRouteAssignment)
	{
		delete m_pInboundRouteAssignment;
		m_pInboundRouteAssignment = NULL;
	}

	if (m_pOutboundRouteAssignment)
	{
		delete m_pOutboundRouteAssignment;
		m_pOutboundRouteAssignment = NULL;
	}

	delete m_pDeiceRouteAssigment;
	m_pDeiceRouteAssigment = NULL;

	if (m_pPostDeiceRouteAssigment)
	{
		delete m_pPostDeiceRouteAssigment;
		m_pPostDeiceRouteAssigment = NULL;
	}
}

void In_OutBoundRouteAssignmentInSim::Init( int nPrjID, CAirportDatabase* pAirportDB, TaxiwayResourceManager* pResManager,RunwayResourceManager * pRunwayResManager,FlightGroundRouteResourceManager* pGoundRouteResManager,IntersectionNodeInSimList* pNodeManager )
{
	PLACE_METHOD_TRACK_STRING();
	m_pInboundRouteAssignment = new CBoundRouteAssignment(INBOUND,pAirportDB);
	m_pInboundRouteAssignment->ReadData(nPrjID);

	m_pOutboundRouteAssignment = new CBoundRouteAssignment(OUTBOUND, pAirportDB);
	m_pOutboundRouteAssignment->ReadData(nPrjID);

	m_pDeiceRouteAssigment = new CBoundRouteAssignment(DEICING, pAirportDB);
	m_pDeiceRouteAssigment->ReadData(nPrjID);

	m_pPostDeiceRouteAssigment = new CBoundRouteAssignment(POSTDEICING, pAirportDB);
	m_pPostDeiceRouteAssigment->ReadData(nPrjID);

	m_pTaxiResManager = pResManager;
	m_pIntersectionNodeManager = pNodeManager;
	m_pGroundResManager = pGoundRouteResManager;
	m_pRunwayResManager = pRunwayResManager;
	

}

CTimeRangeRouteAssignment* In_OutBoundRouteAssignmentInSim::GetTimeRangeRouteAssignment(AirsideFlightInSim* pFlight,BOUNDROUTETYPE eType)
{
	PLACE_METHOD_TRACK_STRING();

	if (!pFlight)
	{
		ASSERT(FALSE);
		return NULL;
	}

	CBoundRouteAssignment* pUsedAss = NULL;
	switch (eType)
	{
	case INBOUND:
		{
			pUsedAss = m_pInboundRouteAssignment;
		}
		break;
	case OUTBOUND:
		{
			pUsedAss = m_pOutboundRouteAssignment;
		}
		break;
	case DEICING:
		{
			pUsedAss = m_pDeiceRouteAssigment;
		}
		break;
	case POSTDEICING:
		{
			pUsedAss = m_pPostDeiceRouteAssigment;
		}
		break;
	default:
		{
			ASSERT(FALSE);
			return NULL;
		}
		break;
	}
	int nCount = pUsedAss->GetElementCount();
	for (int i =0; i < nCount; i++)
	{
		CFlightTypeRouteAssignment* pData = pUsedAss->GetItem(i);
		if (pFlight->fits(pData->GetFltType()))
		{
			int nTimeRange = pData->GetElementCount();
			for (int j =0; j < nTimeRange; j++)
			{
				CTimeRangeRouteAssignment* pTimeData = pData->GetItem(j);
				if (pTimeData->IsInTimeRange(pFlight->GetTime()))
					return pTimeData;
			}
		}
	}
	return NULL;
}

void In_OutBoundRouteAssignmentInSim::GetInboundRoute(AirsideFlightInSim* pFlight,RunwayExitInSim* pExit, 
													  StandInSim* pStand, FlightGroundRouteDirectSegList& vSegments)
{
	PLACE_METHOD_TRACK_STRING();
	ASSERT(m_pTaxiResManager);
	//ASSERT(pFlight->GetUID() != 606);

	if (pFlight == NULL || pExit == NULL || pStand == NULL)
		return;

#ifdef _DEBUG
	char cTmp[1024];
	pFlight->GetFlightInput()->getAirline(cTmp);
	pFlight->GetFlightInput()->getArrID(cTmp + strlen(cTmp));
	static ARCProbPlacer sCheckArr("C:\\InoutBound\\inbound_route_AC.txt");
	ASSERT(!sCheckArr.Check(cTmp));
#endif

	IntersectionNodeInSim* pNodeIn = pExit->GetRouteSeg()->GetExitNode();
	IntersectionNodeInSim * pNodeOut = pStand->GetMinDistInNode();
	if( !pNodeOut )
	{
		CString strWarn;
		strWarn.Format("There is no Entry Node for flight enter the Stand %s", pStand->PrintResource() );
		CString strError = _T("FLIGHT ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
		return ;
	}

	CTimeRangeRouteAssignment* pTimeRangeData = GetTimeRangeRouteAssignment(pFlight,INBOUND);
	CIn_OutBoundRoute* pIn_Route = NULL;

	FlightGroundRouteFinderStrategy* pUsingStrategy = NULL;
	SpeedConstraintFindStrategy refStrategy(pFlight->GetPerformance()->getTaxiInNormalSpeed(pFlight));

	if (pTimeRangeData)
	{
		switch (pTimeRangeData->GetRouteAssignmentStatus())
		{
		case BOUNDROUTEASSIGNMENT_SHORTPATH:
			{
				// do not do anything
			}
			break;
		case BOUNDROUTEASSIGNMENT_TIMESHORTPATH:
			{
				pUsingStrategy = &refStrategy;
			}
			break;
		case BOUNDROUTEASSIGNMENT_PREFERREDROUTE:
			{
				int nRouteCount = pTimeRangeData->GetElementCount();
				for (int i =0; i < nRouteCount; i++)
				{
					CIn_OutBoundRoute* pRoute = pTimeRangeData->GetItem(i);
					if (pRoute->IsFitRoute(pStand->GetStandInput()->GetObjectName() ,pExit->GetID()))
					{
						pIn_Route = pRoute;
						break;
					}
				}
			}
			break;
		default:
			{
				ASSERT(FALSE);
			}
			break;
		}
	}
	
	if (pIn_Route)// if route found
	{
		size_t nPriorityCount = pIn_Route->GetElementCount(); // to find first priority item suitable
		for(size_t i=0;i<nPriorityCount;i++)
		{
			CRoutePriority* pPriority = pIn_Route->GetItem(i);

			FlightGroundRouteDirectSegList vRouteItems;
			if (!GetEntireRouteFromPriority(pPriority, pFlight, pNodeIn, pNodeOut, vRouteItems))
				continue;

			// insert the exit item
			vRouteItems.insert(vRouteItems.begin(),pExit->GetRouteSeg());

			// check the change condition
			if (CheckRoutePriorityChangeCondition(pFlight, pPriority, vRouteItems))
				continue;

			// Change condition fits failed, return the result
			vSegments.insert(vSegments.end(),vRouteItems.begin(),vRouteItems.end());
			pFlight->SetPreferredRoutePriority(pPriority);

			//if (IsValidRoute(vSegments, pNodeIn->GetID(), pNodeOut->GetID()))
			return;
			//else
			//	vSegments.clear();
		}
	}

	//no proper route, find shortest path
	double dPathWeight = (std::numeric_limits<double>::max)();
	if( m_pGroundResManager->GetRoute(pExit->GetRouteSeg(),pNodeOut,pFlight,vSegments, dPathWeight,pUsingStrategy) )
	{
		vSegments.insert(vSegments.begin(),pExit->GetRouteSeg());
	}

	/*if(vSegments.empty())
		return;*/	
}

static IntersectionNodeInSim* GetResourceOutNode( AirsideResource* pStartRes, AirsideResource* pSubStartRes, AirsideFlightInSim* pFlight )
{
	PLACE_METHOD_TRACK_STRING();
	switch (pStartRes->GetType())
	{
	case AirsideResource::ResType_Stand:
		{
			StandLeadOutLineInSim* pLeadOutLine = ((StandInSim*)pStartRes)->AssignLeadOutLine(pFlight);
			if (pLeadOutLine && pLeadOutLine->GetIntersectNodeCount()>0)
				return pLeadOutLine->GetEndNode();
		}
		break;
	case AirsideResource::ResType_StandLeadOutLine:
		{
			StandLeadOutLineInSim* pLeadOutLine = (StandLeadOutLineInSim*)pStartRes;
			return pLeadOutLine->GetEndNode();
		}
		break;
	case AirsideResource::ResType_DeiceStation:
		{
			return ((DeicePadInSim*)pStartRes)->m_pExitNode;
		}
		break;
	case AirsideResource::ResType_StartPosition:
	case AirsideResource::ResType_InterruptLine:
		{
			if (pSubStartRes && pSubStartRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
				return ((TaxiwayDirectSegInSim*)pSubStartRes)->GetExitNode();
		}
		break;
	case AirsideResource::ResType_TaxiwayDirSeg:
		{
			TaxiwayDirectSegInSim* pSeg = (TaxiwayDirectSegInSim*)pStartRes;
			return pSeg->GetExitNode();
		}
		break;
	default:
		{
			ASSERT(FALSE);
			return NULL;
		}
		break;
	}
	return NULL;
}

static bool IsRouteFitForOrgDst( COut_BoundRoute* pRoute, AirsideResource* pStartRes, RunwayExitInSim* pTakeoffPos )
{
	PLACE_METHOD_TRACK_STRING();
	switch (pStartRes->GetType())
	{
	case AirsideResource::ResType_Stand:
		return pRoute->IsFitRouteStand(((StandInSim*)pStartRes)->GetStandInput()->GetObjectName(),pTakeoffPos->GetID());
		break;
	case AirsideResource::ResType_DeiceStation:
		return pRoute->IsFitRouteDeicepad(((DeicePadInSim*)pStartRes)->GetInput()->GetObjectName(),pTakeoffPos->GetID());
		break;
	case AirsideResource::ResType_StartPosition:
		return pRoute->IsFitRouteStartPosition(((StartPositionInSim*)pStartRes)->GetStartPosition()->GetObjectName(),pTakeoffPos->GetID());
		break;
	case AirsideResource::ResType_InterruptLine:
		return pRoute->IsFitRouteTaxiInterruptLine(((TaxiInterruptLineInSim*)pStartRes)->GetInterruptLineInput()->GetID(),pTakeoffPos->GetID());
		break;
	default:
		{
			//ASSERT(FALSE);
			return false;
		}
		break;
	}
	return false;
}

void In_OutBoundRouteAssignmentInSim::GetCircuitBoundRoute( AirsideFlightInSim* pFlight,TakeoffQueueInSim * pQueue, RunwayExitInSim* pExit, RunwayExitInSim* pTakeoffPos, FlightGroundRouteDirectSegList& vSegments )
{
	PLACE_METHOD_TRACK_STRING();
	ASSERT(m_pTaxiResManager);

	if (pFlight ==NULL || pExit == NULL || pTakeoffPos == NULL)
		return;

	IntersectionNodeInSim* pNodeFrom = pExit->GetRouteSeg()->GetExitNode();
	IntersectionNodeInSim * pNodeTo = pTakeoffPos->GetRouteSeg()->GetExitNode();
	FlightGroundRouteDirectSegInSim* pTakeoffPosSeg =pTakeoffPos->GetRouteSeg()->GetOppositeSegment();
	bool bHaveQueue = false;
	if(pQueue)
	{
		for(int i=0;i<pQueue->GetEntryNodeCount();i++)
		{
			FlightGroundRouteDirectSegList segments;
			double dPathWeight = (std::numeric_limits<double>::max)();
			m_pGroundResManager->GetRoute(pNodeFrom,pQueue->GetEntryNode(i),pFlight,0,segments,dPathWeight, NULL);
			if(!segments.empty())
			{
				pNodeTo = pQueue->GetEntryNode(i);
				bHaveQueue = true;
				break;
			}
		}
	}

	// simply return route (pNodeFrom -> pNodeTo)
	double dPathWeight = (std::numeric_limits<double>::max)();
	if(m_pGroundResManager->GetRoute(pExit->GetRouteSeg(),pNodeTo,pFlight,vSegments,dPathWeight, NULL))
	{
		vSegments.insert(vSegments.begin(),pExit->GetRouteSeg());
	}
	if(!bHaveQueue)
		vSegments.push_back(pTakeoffPosSeg);
}
void In_OutBoundRouteAssignmentInSim::GetOutboundRoute(AirsideFlightInSim* pFlight,TakeoffQueueInSim* pQueue, 
													   AirsideResource* pStartRes, FlightGroundRouteDirectSegInSim* pStartSeg, RunwayExitInSim* pTakeoffPos,
													   FlightGroundRouteDirectSegList& vSegments)
{
	PLACE_METHOD_TRACK_STRING();
	ASSERT(m_pTaxiResManager);

	if (pFlight ==NULL || pStartRes == NULL || pTakeoffPos == NULL)
		return;

//	IntersectionNodeInSim * pNodeFrom = pStartRes->GetOutNode();
#ifdef _DEBUG
	char cTmp[1024];
	pFlight->GetFlightInput()->getAirline(cTmp);
	pFlight->GetFlightInput()->getDepID(cTmp + strlen(cTmp));
	static ARCProbPlacer sCheckDep("C:\\InoutBound\\outbound_route_AC.txt");
	ASSERT(!sCheckDep.Check(cTmp));
#endif

	IntersectionNodeInSim * pNodeFrom = GetResourceOutNode(pStartRes, pStartSeg, pFlight);
	if(!pNodeFrom)
	{
		CString strWarn;
		strWarn.Format("There is no Exit Node for flight to exit the %s: %s", pStartRes->GetTypeName(), pStartRes->PrintResource() );
		CString strError = _T("FLIGHT ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
		return ;
	}

	CTimeRangeRouteAssignment* pTimeRangeData = GetTimeRangeRouteAssignment(pFlight,OUTBOUND);

	FlightGroundRouteFinderStrategy* pUsingStrategy = NULL;
	SegExcludedFinderStrategy segExcludedStrategy;
	if (pStartSeg)
	{
		segExcludedStrategy.AddExcludedSeg(pStartSeg->GetOppositeSegment()); // exclude opposite segment when finding route
		pUsingStrategy = &segExcludedStrategy;
	}
// 	SpeedConstraintFindStrategy refStrategy(pFlight->GetPerformance()->getTaxiOutNormalSpeed(pFlight));

	if (pTimeRangeData)
	{
		int nRouteCount = pTimeRangeData->GetElementCount();
		for (int i =0; i < nRouteCount; i++)
		{
			COut_BoundRoute* pRoute = (COut_BoundRoute*)pTimeRangeData->GetItem(i);
			if (IsRouteFitForOrgDst(pRoute, pStartRes, pTakeoffPos))
			{
				switch (pRoute->GetBoundRouteChoice())
				{
				case RouteChoice_ShortestPath:
				case RouteChoice_TimeShortestPath:
					break;
				case RouteChoice_Preferred:
					{
						GetOutboundRouteFromRouteItem(pRoute, pFlight, pNodeFrom, pTakeoffPos, pQueue, vSegments, pUsingStrategy);
						if (vSegments.size())
							return;
					}
					break;
				default:
					{
						ASSERT(FALSE);
					}
					break;
				}
				break;
			}
		}
	}

	IntersectionNodeInSim * pNodeTo = pTakeoffPos->GetRouteSeg()->GetExitNode();
	FlightGroundRouteDirectSegInSim* pTakeoffPosSeg =pTakeoffPos->GetRouteSeg()->GetOppositeSegment();

	bool bHaveQueue = false;
	if(pQueue)
	{
		for(int i=0;i<pQueue->GetEntryNodeCount();i++)
		{
			FlightGroundRouteDirectSegList segments;
			double dPathWeight = (std::numeric_limits<double>::max)();
			m_pGroundResManager->GetRoute(pNodeFrom,pQueue->GetEntryNode(i),pFlight,0,segments,dPathWeight, pUsingStrategy);
			if(!segments.empty())
			{
				pNodeTo = pQueue->GetEntryNode(i);
				bHaveQueue = true;
				break;
			}
		}
	}

	// simply return route (pNodeFrom -> pNodeTo)
	double dPathWeight = (std::numeric_limits<double>::max)();
	m_pGroundResManager->GetRoute(pNodeFrom,pNodeTo,pFlight,0,vSegments,dPathWeight, pUsingStrategy);
	if(!bHaveQueue)
		vSegments.push_back(pTakeoffPosSeg);

#ifdef _DEBUG
	for (FlightGroundRouteDirectSegList::iterator ite = vSegments.begin();ite!=vSegments.end();ite++)
	{
		FlightGroundRouteDirectSegInSim* pSeg = *ite;
		pSeg = NULL;
	}
#endif

}

// void In_OutBoundRouteAssignmentInSim::GetRouteFStandTDeicepad( AirsideFlightInSim* pFlight, StandLeadOutLineInSim* pStandOut, DeicePadInSim* pDeicePad,FlightGroundRouteDirectSegList& vSegments )
// {
// 	if(!pStandOut || !pDeicePad)
// 		return;
// 
// 	IntersectionNodeInSim* pNodeFrom = /*pStandOut->GetIntersectNode(0)*/pStandOut->GetEndNode();
// 	IntersectionNodeInSim* pNodeTo = pDeicePad->m_pEntryNode;
// 
// 	CTimeRangeRouteAssignment* pTimeRangeData = GetTimeRangeRouteAssignment(pFlight,DEICING);
// 	CIn_OutBoundRoute* pOut_Route = NULL;
// 
// 	FlightGroundRouteFinderStrategy* pUsingStrategy = NULL;
// 	SpeedConstraintFindStrategy refStrategy(pFlight->GetPerformance()->getTaxiOutNormalSpeed(pFlight));
// 
// 	if (pTimeRangeData)
// 	{
// 		switch (pTimeRangeData->GetRouteAssignmentStatus())
// 		{
// 		case BOUNDROUTEASSIGNMENT_SHORTPATH:
// 			{
// 				// do not do anything
// 			}
// 			break;
// 		case BOUNDROUTEASSIGNMENT_TIMESHORTPATH:
// 			{
// 				pUsingStrategy = &refStrategy;
// 			}
// 			break;
// 		case BOUNDROUTEASSIGNMENT_PREFERREDROUTE:
// 			{
// 				int nRouteCount = pTimeRangeData->GetElementCount();
// 				for (int i =0; i < nRouteCount; i++)
// 				{
// 					CIn_OutBoundRoute* pRoute = pTimeRangeData->GetItem(i);
// 					CDeice_BoundRoute* pDeiceRoute = (CDeice_BoundRoute*)pRoute;
// 					if (pDeiceRoute->IsFitRoute(pStandOut->GetStandInSim()->GetStandInput()->GetObjectName(),pDeicePad->GetInput()->GetObjectName()))
// 					{
// 						pOut_Route = pRoute;
// 						break;
// 					}
// 				}
// 			}
// 			break;
// 		default:
// 			{
// 				ASSERT(FALSE);
// 			}
// 			break;
// 		}
// 	}
// 
// 	if (pOut_Route)
// 	{
// 		size_t nPriorityCount = pOut_Route->GetElementCount();
// 		for(size_t i=0;i<nPriorityCount;i++)
// 		{
// 			CRoutePriority* pPriority = pOut_Route->GetItem(i);
// 
// 			FlightGroundRouteDirectSegList vRouteItems;
// 			if (!GetEntireRouteFromPriority(pPriority, pFlight, pNodeFrom, pNodeTo, vRouteItems))
// 				continue;
// 
// 			// check change condition
// 			if (CheckRoutePriorityChangeCondition(pFlight, pPriority, vRouteItems))
// 				continue;
// 
// 			// Change condition fits failed, return the result
// 			vSegments.insert(vSegments.end(),vRouteItems.begin(),vRouteItems.end());
// 			pFlight->SetPreferredRoutePriority(pPriority); 
// 
// 			//if (IsValidRoute(vSegments,pNodeFrom->GetID(),pNodeTo->GetID()))
// 			return;
// 		}
// 		
// 
// 	}
// 
// 	if(pNodeTo && pNodeFrom)
// 	{
// 		double dPathWeight = (std::numeric_limits<double>::max)();
// 		m_pGroundResManager->GetRoute(pNodeFrom,pNodeTo,pFlight,0,vSegments,dPathWeight, pUsingStrategy);
// 	}
// }

DeicepadGroupEntry* In_OutBoundRouteAssignmentInSim::GetRouteFStandTDeicepadGroup( AirsideFlightInSim* pFlight, StandLeadOutLineInSim* pStandOut, DeicepadGroup* pGroup,FlightGroundRouteDirectSegList& vSegments )
{
	PLACE_METHOD_TRACK_STRING();
	if(!pStandOut || !pGroup)
		return NULL;

	IntersectionNodeInSim* pNodeFrom = pStandOut->GetEndNode();
	DeicepadGroup::EntryList concernEntries = pGroup->GetEntryList();
	if (0 == concernEntries.size())
		return NULL;

	CTimeRangeRouteAssignment* pTimeRangeData = GetTimeRangeRouteAssignment(pFlight,DEICING);
	CIn_OutBoundRoute* pOut_Route = NULL;

	FlightGroundRouteFinderStrategy* pUsingStrategy = NULL;
	SpeedConstraintFindStrategy refStrategy(pFlight->GetPerformance()->getTaxiOutNormalSpeed(pFlight));

	if (pTimeRangeData)
	{
		switch (pTimeRangeData->GetRouteAssignmentStatus())
		{
		case BOUNDROUTEASSIGNMENT_SHORTPATH:
			{
				// do not do anything
			}
			break;
		case BOUNDROUTEASSIGNMENT_TIMESHORTPATH:
			{
				pUsingStrategy = &refStrategy;
			}
			break;
		case BOUNDROUTEASSIGNMENT_PREFERREDROUTE:
			{
				int nRouteCount = pTimeRangeData->GetElementCount();
				for (int i =0; i < nRouteCount; i++)
				{
					CIn_OutBoundRoute* pRoute = pTimeRangeData->GetItem(i);
					CDeice_BoundRoute* pDeiceRoute = (CDeice_BoundRoute*)pRoute;
					if (pDeiceRoute->IsFitRoute(pStandOut->GetStandInSim()->GetStandInput()->GetObjectName(),pGroup->GetGroupID()))
					{
						pOut_Route = pRoute;
						break;
					}
				}
			}
			break;
		default:
			{
				ASSERT(FALSE);
			}
			break;
		}
	}

	IntersectionNodeInSim* pNodeTo = NULL;

	if (pOut_Route)
	{
		// let's test every entry of DeicepadGroup
		for (DeicepadGroup::EntryList::iterator ite = concernEntries.begin();ite!=concernEntries.end();ite++)
		{
			// fetch one entry
			pNodeTo = (*ite)->GetIntersectionNode();

			size_t nPriorityCount = pOut_Route->GetElementCount();
			for(size_t i=0;i<nPriorityCount;i++)
			{
				CRoutePriority* pPriority = pOut_Route->GetItem(i);
	
				FlightGroundRouteDirectSegList vRouteItems;
				if (!GetEntireRouteFromPriority(pPriority, pFlight, pNodeFrom, pNodeTo, vRouteItems))
					continue;
	
				// check change condition
				if (CheckRoutePriorityChangeCondition(pFlight, pPriority, vRouteItems))
					continue;

				DeicepadGroupEntry* pEntry = *ite;
				FlightGroundRouteDirectSegList::iterator iteFound = std::find_first_of(
					vRouteItems.begin(), vRouteItems.end(), concernEntries.begin(), concernEntries.end(), SegEntrySuitGourpEntry(pEntry));
				vRouteItems.erase(iteFound, vRouteItems.end());

				// Change condition fits failed, return the result
				vSegments.insert(vSegments.end(),vRouteItems.begin(),vRouteItems.end());

				//
				pFlight->SetPreferredRoutePriority(pPriority); 
	
				//if (IsValidRoute(vSegments,pNodeFrom->GetID(),pNodeTo->GetID()))
				return pEntry;
			}
		}
	}

	// let's test every entry of DeicepadGroup
	for (DeicepadGroup::EntryList::iterator ite = concernEntries.begin();ite!=concernEntries.end();ite++)
	{
		// fetch one entry
		pNodeTo = (*ite)->GetIntersectionNode();

		FlightGroundRouteDirectSegList vRouteItems;
		double dPathWeight = (std::numeric_limits<double>::max)();
		m_pGroundResManager->GetRoute(pNodeFrom,pNodeTo,pFlight,0,vRouteItems,dPathWeight,pUsingStrategy);

		DeicepadGroupEntry* pEntry = *ite;
		FlightGroundRouteDirectSegList::iterator iteFound = std::find_first_of(
			vRouteItems.begin(), vRouteItems.end(), concernEntries.begin(), concernEntries.end(), SegEntrySuitGourpEntry(pEntry));
		vRouteItems.erase(iteFound, vRouteItems.end());

		vSegments.insert(vSegments.end(),vRouteItems.begin(),vRouteItems.end());
		return pEntry;
	}
	return NULL;
}

void In_OutBoundRouteAssignmentInSim::GetRouteFStandTStand( AirsideFlightInSim* pFlight, StandLeadOutLineInSim* pStandOut, StandLeadInLineInSim* pStandIn,FlightGroundRouteDirectSegList& vSegments )
{
	PLACE_METHOD_TRACK_STRING();
	if( pStandOut && pStandOut->GetIntersectNodeCount() && pStandIn && pStandIn->GetIntersectNodeCount() )
	{
		IntersectionNodeInSim* pNodeFrom = /*pStandOut->GetIntersectNode(0)*/pStandOut->GetEndNode();
		IntersectionNodeInSim* pNodeTo = /*pStandIn->GetIntersectNode(0)*/pStandIn->GetEndNode();
		if(pNodeFrom && pNodeTo)
		{
			double dPathWeight = (std::numeric_limits<double>::max)();
			m_pGroundResManager->GetRoute(pNodeFrom,pNodeTo,pFlight,0,vSegments,dPathWeight);
		}
	}
}

void In_OutBoundRouteAssignmentInSim::GetRouteFDeiceTTakoff( AirsideFlightInSim* pFlight, DeicePadInSim* pDeicePad,RunwayExitInSim* pTakeoffPos, FlightGroundRouteDirectSegList& vSegments )
{
	PLACE_METHOD_TRACK_STRING();
	if(!pTakeoffPos || !pDeicePad)
		return;

	IntersectionNodeInSim* pNodeFrom = pDeicePad->m_pExitNode;
	IntersectionNodeInSim* pNodeTo = pTakeoffPos->GetRouteSeg()->GetEntryNode();

	CTimeRangeRouteAssignment* pTimeRangeData = GetTimeRangeRouteAssignment(pFlight,POSTDEICING);
	CIn_OutBoundRoute* pOut_Route = NULL;

	FlightGroundRouteFinderStrategy* pUsingStrategy = NULL;
	SpeedConstraintFindStrategy refStrategy(pFlight->GetPerformance()->getTaxiOutNormalSpeed(pFlight));

	if (pTimeRangeData)
	{
		switch (pTimeRangeData->GetRouteAssignmentStatus())
		{
		case BOUNDROUTEASSIGNMENT_SHORTPATH:
			{
				// do not do anything
			}
			break;
		case BOUNDROUTEASSIGNMENT_TIMESHORTPATH:
			{
				pUsingStrategy = &refStrategy;
			}
			break;
		case BOUNDROUTEASSIGNMENT_PREFERREDROUTE:
			{
				int nRouteCount = pTimeRangeData->GetElementCount();
				for (int i =0; i < nRouteCount; i++)
				{
					CIn_OutBoundRoute* pRoute = pTimeRangeData->GetItem(i);
					CPostDeice_BoundRoute* pPostDeicingRoute = (CPostDeice_BoundRoute*)pRoute;
					if (pPostDeicingRoute->IsAvailableRoute(pDeicePad->GetInput()->GetObjectName(), 
															pFlight->GetAndAssignTakeoffRunway()->GetRunwayInSim()->GetRunwayID(),pFlight->GetAndAssignTakeoffRunway()->getLogicRunwayType()))
					{
						pOut_Route = pRoute;
						break;
					}
				}
			}
			break;
		default:
			{
				ASSERT(FALSE);
			}
			break;
		}
	}

	if (pOut_Route)
	{
		size_t nPriorityCount = pOut_Route->GetElementCount();
		for(size_t i=0;i<nPriorityCount;i++)
		{
			CRoutePriority* pPriority = pOut_Route->GetItem(i);

			FlightGroundRouteDirectSegList vRouteItems;
			if (!GetEntireRouteFromPriority(pPriority, pFlight, pNodeFrom, pNodeTo, vRouteItems))
				continue;

			// check change condition
			if (CheckRoutePriorityChangeCondition(pFlight, pPriority, vRouteItems))
				continue;

			// Change condition fits failed, return the result
			vSegments.insert(vSegments.end(),vRouteItems.begin(),vRouteItems.end());
			pFlight->SetPreferredRoutePriority(pPriority); 

			return;
		}


	}

	if(pNodeFrom && pNodeTo)
	{
		double dPahtWeight = (std::numeric_limits<double>::max)();
		m_pGroundResManager->GetRoute(pNodeFrom,pNodeTo,pFlight,0,vSegments, dPahtWeight, pUsingStrategy);
	}
}

// throws CString which identify the error
// get route items
// return NULL if found
// return the unique intersection node if no segment found and one intersection exists
IntersectionNodeInSim* In_OutBoundRouteAssignmentInSim::GetRouteItemListInPriority(
	CRoutePriority* pPriority,
	FlightGroundRouteDirectSegList& vRouteItems,
// 	IntersectionNodeInSim*& pTailNode, // [out]
	int& nHeadTaxiwayID,                // [out]
	int& nTailTaxiwayID                 // [out]
	) const
{
	PLACE_METHOD_TRACK_STRING();
// 	pTailNode = NULL;
	nHeadTaxiwayID = -1;
	nTailTaxiwayID = -1;

	if (pPriority == NULL)
	{
		ASSERT(FALSE);
		return NULL;
	}

	int nBranchCount = pPriority->GetElementCount();
	if (nBranchCount <= 0)
		return NULL;

	// get a random start route item
	int nRandomIdx = 0;
	if (nBranchCount > 1)
		nRandomIdx = rand()%nBranchCount;
	CRouteItem* pThisRouteItem = pPriority->GetItem(nRandomIdx);

	int nPreIntersectionID = -1;

	bool bTailIsRunway = false;
	while(pThisRouteItem)
	{
		// Retrieve and analyze the alt object
		ALTObject *pALTObj = ALTObject::ReadObjectByID(pThisRouteItem->GetALTObjectID());
		if(pALTObj == NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		ALTOBJECT_TYPE objectType = pALTObj->GetType();
		if (objectType == ALT_GROUNDROUTE)
		{
			FlightGroundRouteDirectSegList vSegments;
			int nHeadTmp = -1;
			GetRouteItemsFromGroundRoute((GroundRoute*)pALTObj, nTailTaxiwayID, vSegments, nPreIntersectionID,
				-1 == nHeadTaxiwayID ? nHeadTaxiwayID : nHeadTmp, nTailTaxiwayID, bTailIsRunway);
			vRouteItems.insert(vRouteItems.end(),vSegments.begin(),vSegments.end());
		}
		//else if (objectType == ALT_RUNWAY)
		//{
		//	bTailIsRunway = true;
		//}
		else
		{
			int nThisTaxiwayID = pALTObj->getID();
			ASSERT(nThisTaxiwayID>0);
			if (nTailTaxiwayID>0)
			{
				IntersectionNodeInSim* pNode = m_pIntersectionNodeManager->GetIntersectionNode(nTailTaxiwayID,nThisTaxiwayID,0);
				if (NULL == pNode)
				{
					//ASSERT(FALSE); // node finding error, should not be here
					vRouteItems.clear();
					TaxiwayInSim* pPreTaxiway = m_pTaxiResManager->GetTaxiwayByID(nTailTaxiwayID);
					TaxiwayInSim* pThisTaxiway = m_pTaxiResManager->GetTaxiwayByID(nThisTaxiwayID);
					ASSERT(pPreTaxiway && pThisTaxiway);
					CString strExceptionString;
					strExceptionString.Format(_T("There defines a route from taxiway %s to %s, but the two taxiways have no intersection node."),
						pPreTaxiway->GetTaxiwayInput()->GetMarking().c_str(), pThisTaxiway->GetTaxiwayInput()->GetMarking().c_str());
					throw strExceptionString;
				}
// 				int nThisIntersectionID = pThisRouteItem->GetIntersectNodeID(); // caution: this method is unreliable, discarded
				int nThisIntersectionID = pNode->GetID();
				if (nPreIntersectionID>0 && nThisIntersectionID>0)
				{
					FlightGroundRouteDirectSegList vSegmentList;

					if (m_pRunwayResManager->GetRunwayByID(nTailTaxiwayID) != NULL)		//rwy item
						m_pRunwayResManager->GetRunwaySegments(nTailTaxiwayID,nPreIntersectionID,nThisIntersectionID,vSegmentList);
					else
						m_pTaxiResManager->GetTaxiwaySegments(nTailTaxiwayID,nPreIntersectionID,nThisIntersectionID,vSegmentList);

					vRouteItems.insert(vRouteItems.end(),vSegmentList.begin(),vSegmentList.end());
				}
				nPreIntersectionID = nThisIntersectionID;
			}
			if (-1 == nHeadTaxiwayID)
			{
				nHeadTaxiwayID = nThisTaxiwayID;
			}
			nTailTaxiwayID = nThisTaxiwayID;
		}
	
		delete pALTObj;
		pALTObj = NULL;

		nBranchCount = pThisRouteItem->GetElementCount();
		if (nBranchCount <= 0 || bTailIsRunway)
		{
			if (nPreIntersectionID >0 && vRouteItems.empty())	//only defined one entry node
			{
				return m_pIntersectionNodeManager->GetNodeByID(nPreIntersectionID);
			}
			return NULL;
		}

		// get the next item
		if (nBranchCount > 1)
			nRandomIdx = rand()%nBranchCount;
		else
			nRandomIdx = 0;
		pThisRouteItem = pThisRouteItem->GetItem(nRandomIdx);
	}
	return NULL;
}

// retrieve segments from ground route, return the head intersection node ID
int In_OutBoundRouteAssignmentInSim::GetRouteItemsFromGroundRoute(
	GroundRoute* pGroundRoute,
	int nPreTaxiwayID, // only taxiway, -1 if not exists
	FlightGroundRouteDirectSegList& vRouteItems, // [out]
	int& nTailIntersetionNodeID, // [out]
	int& nHeadTaxiwayID,          // [out]
	int& nTailTaxiwayID,          // [out]
	bool& bTailIsRunway          // [out]
	) const
{
	PLACE_METHOD_TRACK_STRING();
	ASSERT(m_pIntersectionNodeManager);

	if (pGroundRoute == NULL)
	{
		ASSERT(FALSE);
		return -1;
	}
	int nHeadIntersectionNodeID = -1;
	nTailIntersetionNodeID = -1;
	nHeadTaxiwayID = -1;
	nTailTaxiwayID = -1;
	bTailIsRunway = false;

	GroundRouteItem* pPreItem = pGroundRoute->GetFirstItem();
	while(pPreItem)
	{
		if (pPreItem->GetRunwayMarking()>=0
			&& NULL == m_pTaxiResManager->GetTaxiwayByID(pPreItem->GetALTObjectID())
			&& pPreItem->GetChildCount()<=0
			)	//last item is runway port
		{
			bTailIsRunway = true;
			break;
		}

		if (nHeadTaxiwayID == -1)
		{
			nHeadTaxiwayID = pPreItem->GetALTObjectID(); // record the first way
		}
		nTailTaxiwayID = pPreItem->GetALTObjectID(); // record until the tail

		if (nPreTaxiwayID>0)
		{
			IntersectionNodeInSim* pNode = m_pIntersectionNodeManager->GetIntersectionNode(nPreTaxiwayID,nTailTaxiwayID,0);
			if (pNode == NULL)
			{
				ASSERT(FALSE);
				TaxiwayInSim* pFromTaxiway = m_pTaxiResManager->GetTaxiwayByID(nPreTaxiwayID);
				TaxiwayInSim* pToTaxiway = m_pTaxiResManager->GetTaxiwayByID(nTailTaxiwayID);
				if (pFromTaxiway && pToTaxiway)
				{
					AirsideSimErrorShown::SimpleSimWarning(
						pGroundRoute->GetObjectName().GetIDString(),
						_T("Error getting intersection node between taxiways: ") +
						pFromTaxiway->GetTaxiwayInput()->GetObjectName().GetIDString() + _T(" <-> ") +
						pToTaxiway->GetTaxiwayInput()->GetObjectName().GetIDString(),
						_T("Taxi Route Invalid")
						);
				}

				return -1;
			}
			if (nTailIntersetionNodeID>0)
			{
				FlightGroundRouteDirectSegList vSegments;
				if (m_pRunwayResManager->GetRunwayByID(nPreTaxiwayID) != NULL)		//rwy item
					m_pRunwayResManager->GetRunwaySegments(nPreTaxiwayID,nTailIntersetionNodeID,pNode->GetID(),vSegments);
				else
					m_pTaxiResManager->GetTaxiwaySegments(nPreTaxiwayID,nTailIntersetionNodeID,pNode->GetID(),vSegments);
				vRouteItems.insert(vRouteItems.end(), vSegments.begin(), vSegments.end());
			}
			if (-1 == nHeadIntersectionNodeID)
			{
				nHeadIntersectionNodeID = pNode->GetID(); // record the head intersection node
			}
			nTailIntersetionNodeID = pNode->GetID(); // record until the tail
		}

		nPreTaxiwayID = nTailTaxiwayID;
		int nChildCount = pPreItem->GetChildCount();
		if (nChildCount <= 0)	//no child branch
		{
			break;
		}

		int nChildIdx = 0;
		if (nChildCount >1)
			nChildIdx = rand()%nChildCount;
		pPreItem = pPreItem->GetChildItem(nChildIdx);
	}

	return nHeadIntersectionNodeID;
}

bool In_OutBoundRouteAssignmentInSim::IsValidRoute(FlightGroundRouteDirectSegList& vSegments, int nFromID, int nToID)
{
	PLACE_METHOD_TRACK_STRING();
	int nSize = vSegments.size();
	if (nSize <=0)
		return false;

	FlightGroundRouteDirectSegInSim* pDirSeg = vSegments.at(0);
	if (pDirSeg->GetEntryNode()->GetID() != nFromID )
		return false;

	int nPreExitID = pDirSeg->GetExitNode()->GetID();
	if (nSize == 1 && nPreExitID != nToID)
		return false;
	
	for (int i = 1; i < nSize; i++)
	{
		FlightGroundRouteDirectSegInSim* pNextDirSeg = vSegments.at(i);
		if (pDirSeg->GetOppositeSegment() == pNextDirSeg)
			return false;

		if (pNextDirSeg->GetEntryNode()->GetID() != nPreExitID )
			return false;
		nPreExitID =  pNextDirSeg->GetExitNode()->GetID();
		pDirSeg = pNextDirSeg;
	}

	if (nPreExitID != nToID)
		return false;

	return true;
}

bool In_OutBoundRouteAssignmentInSim::CheckRoutePriorityChangeCondition( AirsideFlightInSim* pFlight, CRoutePriority* pPriority, const FlightGroundRouteDirectSegList &vSegList )
{
	PLACE_METHOD_TRACK_STRING();
	if (vSegList.size())
	{
		//------------------------------------------------------------
		// let's check the change condition
		// 1. get all flights on route and flights with different preferred route
		if (!pPriority->m_bHasChangeCondition)// if no change condition need does not check it
		{
			return false;
		}

		std::vector<AirsideFlightInSim* > allFlightsOnRoute;
		FlightGroundRouteDirectSegList::const_iterator iteSeg = vSegList.begin();
		FlightGroundRouteDirectSegList::const_iterator iteSegEn = vSegList.end();
		for (;iteSeg!=iteSegEn;iteSeg++)
		{
			(*iteSeg)->GetInPathAirsideFlightList(allFlightsOnRoute);
		}
		std::vector<AirsideFlightInSim* > flightsDiffPreferredRoute = allFlightsOnRoute;// pFlight->FindFlightsOnDiffPreferredRoute(allFlightsOnRoute, pPriority);
		int nFlightDiffCount = allFlightsOnRoute.size(); //int)flightsDiffPreferredRoute.size();
		// 2.1 Number of AC on route > N
		if (nFlightDiffCount>pPriority->m_RepConData.m_nAcOnRouteCount)
			return true;
		// 2.2 AC on route stopped > T Secs
		ElapsedTime timeSum(0L);
		ElapsedTime curTime = pFlight->getCurTime();
		int nAcSlowCount = 0;
		std::vector<AirsideFlightInSim* >::iterator iteDiff = flightsDiffPreferredRoute.begin();
		std::vector<AirsideFlightInSim* >::iterator iteDiffEn = flightsDiffPreferredRoute.end();
		for (;iteDiff!=iteDiffEn;iteDiff++)
		{
			ElapsedTime timeStop;
			if ((*iteDiff)->GetStoppedStartTime(timeStop))
			{
				timeSum += (curTime - timeStop);
			}
			if ((*iteDiff)->IsSpeedConstrainedByOtherFlight())
			{
				nAcSlowCount++;
			}
		}
		if (timeSum.asSeconds()>nFlightDiffCount*pPriority->m_RepConData.m_nAcOnRouteStopTime)
			return true;
		// 2.3 Preceding on route slower than P%
		if (nAcSlowCount*100>nFlightDiffCount*pPriority->m_RepConData.m_nPrecdingAcSlowerPercent)
			return true;
		//------------------------------------------------------------
	}
	return false;
}

bool In_OutBoundRouteAssignmentInSim::GetPreRoute(
	IntersectionNodeInSim*& pStartNode,
	IntersectionNodeInSim* pNodeIn,
	int nHeadTaxiwayID,
	bool bHeadTaxiwayOK,
	AirsideFlightInSim* pFlight,
	FlightGroundRouteDirectSegList& vRouteItems,
	FlightGroundRouteFinderStrategy* pUsingStrategy /*= NULL*/
	)
{
	PLACE_METHOD_TRACK_STRING();
	if (pStartNode)
	{
		if (pNodeIn->GetID() == pStartNode->GetID())
		{
			bHeadTaxiwayOK = true;
		}
		else
		{
			FlightGroundRouteDirectSegList vPreItems;
			if (m_pTaxiResManager->IsNodeOnTaxiway(nHeadTaxiwayID, pNodeIn->GetID())) // if both in-node and start-node are on the taxiway
			{
				// just retrieve segments from this taxiway
				m_pTaxiResManager->GetTaxiwaySegments(nHeadTaxiwayID, pNodeIn->GetID(), pStartNode->GetID(), vPreItems);
				bHeadTaxiwayOK = true;
			}
			else if(m_pRunwayResManager->IsNodeOnRunway(nHeadTaxiwayID, pNodeIn->GetID()))
			{
				m_pRunwayResManager->GetRunwaySegments(nHeadTaxiwayID, pNodeIn->GetID(), pStartNode->GetID(), vPreItems);
				bHeadTaxiwayOK = true;
			}
			else
			{
				double dPathWeight = (std::numeric_limits<double>::max)();
				m_pGroundResManager->GetRoute(pNodeIn,pStartNode,pFlight,0,vPreItems,dPathWeight, pUsingStrategy);
				if (!bHeadTaxiwayOK && vPreItems.size()>0)
				{
					FlightGroundRouteDirectSegInSim* pTailSeg = vPreItems.back();
					if (pTailSeg->GetObjectID() == nHeadTaxiwayID)
					{
						bHeadTaxiwayOK = true;
					}
				}
			}
			if (bHeadTaxiwayOK)
			{
				vRouteItems.insert(vRouteItems.begin(),vPreItems.begin(),vPreItems.end());
			}
		}
	}
	if (!bHeadTaxiwayOK)
	{
		// have to match the head taxiway
		TaxiwayInSim* pTaxiway = m_pTaxiResManager->GetTaxiwayByID(nHeadTaxiwayID);
		if (pTaxiway)
		{
			FlightGroundRouteDirectSegList vPreSeg;
			double dPathWeight = (std::numeric_limits<double>::max)();
			IntersectionNodeInSimList vNodeList = pTaxiway->GetIntersectionNodeList();
			IntersectionNodeInSim* pExceptNode = pStartNode;
			IntersectionNodeInSim* pStartNodeTmp = pStartNode;

			int nCount = vNodeList.GetNodeCount();
			for(int i=0;i<nCount;i++)
			{
				IntersectionNodeInSim* pNode = vNodeList.GetNodeByIndex(i);
				if (pNode != pExceptNode)
				{
					FlightGroundRouteDirectSegList vPreSegTmp;
					double dPathWeightTmp = (std::numeric_limits<double>::max)();
					if (m_pGroundResManager->GetRoute(pNodeIn, pNode, pFlight, 0, vPreSegTmp, dPathWeightTmp, pUsingStrategy)
						&& dPathWeightTmp < dPathWeight)
					{
						// get linking segments
						if (pExceptNode)
						{
							FlightGroundRouteDirectSegList vPreSegLink;
							m_pTaxiResManager->GetTaxiwaySegments(nHeadTaxiwayID, pNode->GetID(), pExceptNode->GetID(), vPreSegLink);
							vPreSegTmp.insert(vPreSegTmp.end(), vPreSegLink.begin(), vPreSegLink.end());
						}

						// record
						dPathWeight = dPathWeightTmp;
						vPreSeg = vPreSegTmp;
						bHeadTaxiwayOK = true;

						if (NULL == pStartNode)
						{
							pStartNodeTmp = pNode;
						}
					}
				}
			}
			if (bHeadTaxiwayOK)
			{
				pStartNode = pStartNodeTmp;
				if (vPreSeg.size()>=2) // if last two segments make a U turn, erase them
				{
					FlightGroundRouteDirectSegList::iterator ite2 = vPreSeg.end() - 1;
					FlightGroundRouteDirectSegList::iterator ite1 = ite2 - 1;
					if ((*ite1)->GetOppositeSegment() == *ite2)
					{
						vPreSeg.erase(ite1, vPreSeg.end());
						pStartNode = pExceptNode; // restore start point, as the excepted node
					}
				}
				vRouteItems.insert(vRouteItems.begin(),vPreSeg.begin(),vPreSeg.end());
			}
			else if (pStartNode)
			{
				// if finding no link route but start node is OK, get route direct to start node
				// it means just to pass by the node, not any segment of the specified taxiway
				double dPathWeight = (std::numeric_limits<double>::max)();
				if (m_pGroundResManager->GetRoute(pNodeIn, pStartNode, pFlight, 0, vPreSeg, dPathWeight, pUsingStrategy))
				{
					vRouteItems.insert(vRouteItems.begin(),vPreSeg.begin(),vPreSeg.end());
					bHeadTaxiwayOK = true;
				}
			}
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	return bHeadTaxiwayOK;
}

bool In_OutBoundRouteAssignmentInSim::GetPostRoute(
	IntersectionNodeInSim* pStartNode,
	IntersectionNodeInSim* pEndNode,
	IntersectionNodeInSim* pNodeOut,
	int nTailTaxiwayID,
	bool bTailTaxiwayOK,
	AirsideFlightInSim* pFlight,
	FlightGroundRouteDirectSegList& vRouteItems,
	FlightGroundRouteFinderStrategy* pUsingStrategy /*= NULL*/
	)
{
	PLACE_METHOD_TRACK_STRING();
	ASSERT(pStartNode);
	if (NULL == pEndNode)
	{
		pEndNode = pStartNode;
	}
	if (pEndNode->GetID() == pNodeOut->GetID())
	{
		bTailTaxiwayOK = true;
	}
	else
	{
		FlightGroundRouteDirectSegList vPostItems;
		if (m_pTaxiResManager->IsNodeOnTaxiway(nTailTaxiwayID, pNodeOut->GetID())) // if both end-node and out-node are on the taxiway
		{
			// just retrieve segments from this taxiway
			m_pTaxiResManager->GetTaxiwaySegments(nTailTaxiwayID, pEndNode->GetID(), pNodeOut->GetID(), vPostItems);
			bTailTaxiwayOK = true;
		}
		else
		{
			double dPathWeight = (std::numeric_limits<double>::max)();
			if (m_pGroundResManager->GetRoute(pEndNode,pNodeOut,pFlight,0,vPostItems,dPathWeight, pUsingStrategy)
				&& !bTailTaxiwayOK && vPostItems.size())
			{
				FlightGroundRouteDirectSegInSim* pHeadSeg = vPostItems[0];
				if (pHeadSeg->GetObjectID() == nTailTaxiwayID)
				{
					bTailTaxiwayOK = true;
				}
			}
		}
		if (bTailTaxiwayOK)
		{
			vRouteItems.insert(vRouteItems.end(),vPostItems.begin(),vPostItems.end());
		}
	}
	if (!bTailTaxiwayOK)
	{
		// have to match the tail taxiway
		TaxiwayInSim* pTaxiway = m_pTaxiResManager->GetTaxiwayByID(nTailTaxiwayID);
		if (pTaxiway)
		{
			FlightGroundRouteDirectSegList vPostSeg;
			double dPathWeight = (std::numeric_limits<double>::max)();
			IntersectionNodeInSimList vNodeList = pTaxiway->GetIntersectionNodeList();
			IntersectionNodeInSim* pExceptNode = pEndNode;
			IntersectionNodeInSim* pEndNodeTmp = pEndNode;

			int nCount = vNodeList.GetNodeCount();
			for(int i=0;i<nCount;i++)
			{
				IntersectionNodeInSim* pNode = vNodeList.GetNodeByIndex(i);
				if (pNode != pExceptNode)
				{
					FlightGroundRouteDirectSegList vPostSegTmp;
					double dPathWeightTmp = (std::numeric_limits<double>::max)();
					if (m_pGroundResManager->GetRoute(pNode, pNodeOut, pFlight, 0, vPostSegTmp, dPathWeightTmp, pUsingStrategy)
						&& dPathWeightTmp < dPathWeight)
					{
						// get linking segments
						FlightGroundRouteDirectSegList vPostSegLink;
						m_pTaxiResManager->GetTaxiwaySegments(nTailTaxiwayID, pExceptNode->GetID(), pNode->GetID(), vPostSegLink);
						vPostSegTmp.insert(vPostSegTmp.begin(), vPostSegLink.begin(), vPostSegLink.end());

						// record
						dPathWeight = dPathWeightTmp;
						vPostSeg = vPostSegTmp;
						bTailTaxiwayOK = true;

						if (NULL == pEndNode)
						{
							pEndNodeTmp = pNode;
						}
					}
				}
			}
			if (bTailTaxiwayOK)
			{
				pEndNode = pEndNodeTmp;
				if (vPostSeg.size()>=2) // if first two segments make a U turn, erase them
				{
					FlightGroundRouteDirectSegList::iterator ite1 = vPostSeg.begin();
					FlightGroundRouteDirectSegList::iterator ite2 = ite1 + 1;
					if ((*ite1)->GetOppositeSegment() == *ite2)
					{
						vPostSeg.erase(ite1, ite1 + 2);
						pEndNode = pExceptNode; // restore end point, as the excepted node
					}
				}
				vRouteItems.insert(vRouteItems.end(),vPostSeg.begin(),vPostSeg.end());
			}
			else if (pEndNode)
			{
				// if finding no link route but end node is OK, get route direct from end node
				// it means just to pass by the node, not any segment of the specified taxiway
				double dPathWeight = (std::numeric_limits<double>::max)();
				if (m_pGroundResManager->GetRoute(pEndNode, pNodeOut, pFlight, 0, vPostSeg, dPathWeight, pUsingStrategy))
				{
					vRouteItems.insert(vRouteItems.end(),vPostSeg.begin(),vPostSeg.end());
					bTailTaxiwayOK = true;
				}
			}
		}
		else
		{
			ASSERT(FALSE);
		}
	}

	return bTailTaxiwayOK;
}

bool In_OutBoundRouteAssignmentInSim::GetEntireRouteFromPriority(
	CRoutePriority* pPriority,
	AirsideFlightInSim* pFlight,
	IntersectionNodeInSim* pNodeIn,
	IntersectionNodeInSim* pNodeOut,
	FlightGroundRouteDirectSegList& vRouteItems,
	FlightGroundRouteFinderStrategy* pUsingStrategy /*= NULL*/
	)
{
	PLACE_METHOD_TRACK_STRING();
	// find partial route vPostItems specified in the priority item by user
	int nHeadTaxiwayID = -1;
	int nTailTaxiwayID = -1;
	IntersectionNodeInSim* pStartNode = NULL;
	IntersectionNodeInSim* pEndNode = NULL;
	try
	{
		pEndNode = pStartNode = GetRouteItemListInPriority(pPriority, vRouteItems, nHeadTaxiwayID, nTailTaxiwayID);
	}
	catch (CString& e)
	{
		AirsideSimErrorShown::SimWarning(pFlight,CString(_T("")),e);
		vRouteItems.clear();
		return false;
	}
	if (nHeadTaxiwayID<0 || nTailTaxiwayID<0)
	{
		return false;
	}

	bool bHeadTaxiwayOK = false; // 
	bool bTailTaxiwayOK = false; // 
	if (vRouteItems.size() > 0)
	{
		FlightGroundRouteDirectSegInSim* pHeadSeg = vRouteItems[0];
		FlightGroundRouteDirectSegInSim* pTailSeg = vRouteItems.back();
		if (pHeadSeg->GetObjectID() == nHeadTaxiwayID)
		{
			bHeadTaxiwayOK = true;
		}
		if (pTailSeg->GetObjectID() == nTailTaxiwayID)
		{
			bTailTaxiwayOK = true;
		}
		pStartNode = pHeadSeg->GetEntryNode();
		pEndNode = pTailSeg->GetExitNode();
	}

	// find pre route to the partial route vPostItems
	bHeadTaxiwayOK = GetPreRoute(pStartNode, pNodeIn, nHeadTaxiwayID, bHeadTaxiwayOK, pFlight, vRouteItems, pUsingStrategy);

	if (!bHeadTaxiwayOK || NULL == pStartNode)
	{
		ASSERT(FALSE);
		vRouteItems.clear();
		return false;
	}

	// find post route from the partial route vPostItems
	bTailTaxiwayOK = GetPostRoute(pStartNode, pEndNode, pNodeOut, nTailTaxiwayID, bTailTaxiwayOK, pFlight, vRouteItems, pUsingStrategy);

	if (!bTailTaxiwayOK)
	{
		ASSERT(FALSE);
		vRouteItems.clear();
		return false;
	}

#ifdef _DEBUG
	for (FlightGroundRouteDirectSegList::iterator ite = vRouteItems.begin();ite!=vRouteItems.end();ite++)
	{
		FlightGroundRouteDirectSegInSim* pSeg = *ite;
		pSeg = NULL;
	}
#endif

	return true;
}

bool In_OutBoundRouteAssignmentInSim::GetEntireOutboundRouteFromPriority( CRoutePriority* pPriority, AirsideFlightInSim* pFlight,
																		 IntersectionNodeInSim * pNodeFrom, IntersectionNodeInSim * pNodeTo, TakeoffQueueInSim* pQueue,
																		 FlightGroundRouteDirectSegList &vRouteItems, FlightGroundRouteFinderStrategy* pUsingStrategy /*= NULL*/ )
{
	PLACE_METHOD_TRACK_STRING();
	int nHeadTaxiwayID = -1;
	int nTailTaxiwayID = -1;
	IntersectionNodeInSim* pStartNode = NULL;
	IntersectionNodeInSim* pEndNode = NULL;
	try
	{
		pEndNode = pStartNode = GetRouteItemListInPriority(pPriority, vRouteItems, nHeadTaxiwayID, nTailTaxiwayID);
	}
	catch (CString& e)
	{
		AirsideSimErrorShown::SimWarning(pFlight,e,CString(_T("DEFINE ERROR")));
		vRouteItems.clear();
		return false;
	}
	if (nHeadTaxiwayID<0 || nTailTaxiwayID<0)
	{
		return false;
	}

	bool bHeadTaxiwayOK = false; // 
	bool bTailTaxiwayOK = false; // 
	if (vRouteItems.size() > 0)
	{
		FlightGroundRouteDirectSegInSim* pHeadSeg = vRouteItems[0];
		FlightGroundRouteDirectSegInSim* pTailSeg = vRouteItems.back();
		if (pHeadSeg->GetObjectID() == nHeadTaxiwayID)
		{
			bHeadTaxiwayOK = true;
		}
		if (pTailSeg->GetObjectID() == nTailTaxiwayID)
		{
			bTailTaxiwayOK = true;
		}
		pStartNode = pHeadSeg->GetEntryNode();
		pEndNode = pTailSeg->GetExitNode();
	}

	// find pre route to the partial route vPostItems
	bHeadTaxiwayOK = GetPreRoute(pStartNode, pNodeFrom, nHeadTaxiwayID, bHeadTaxiwayOK, pFlight, vRouteItems);

	if (!bHeadTaxiwayOK || NULL == pStartNode)
	{
		ASSERT(FALSE);
		vRouteItems.clear();
		return false;
	}

	// find pNodeTo for post route finding
	if (NULL == pEndNode)
	{
		pEndNode = pStartNode;
	}
	if (pQueue)
	{
		for(int i=0;i<pQueue->GetEntryNodeCount();i++)
		{
			if (pQueue->GetEntryNode(i)->GetID() == pEndNode->GetID())
			{
				pNodeTo = pEndNode;
				break; 
			}
			else
			{
				FlightGroundRouteDirectSegList segments;
				double dPathWeight = (std::numeric_limits<double>::max)();
				m_pGroundResManager->GetRoute(pEndNode,pQueue->GetEntryNode(i),pFlight,0,segments, dPathWeight, pUsingStrategy);
				if(!segments.empty())
				{
					int nCount = segments.size();
					for (int j =0; j < nCount; j++)
					{
						FlightGroundRouteDirectSegInSim* pSegment = segments[j]->GetOppositeSegment();
						if (pQueue->IsHaveSegment(pSegment))
						{
							pNodeTo = segments[j]->GetEntryNode();
							break;
						}
					}
					pNodeTo = pQueue->GetEntryNode(i);
					break;
				}
			}
		}
	}

	// find post route from the partial route vPostItems
	bTailTaxiwayOK = GetPostRoute(pStartNode, pEndNode, pNodeTo, nTailTaxiwayID, bTailTaxiwayOK, pFlight, vRouteItems);
	if (!bTailTaxiwayOK)
	{
		ASSERT(FALSE);
		vRouteItems.clear();
		return false;
	}

#ifdef _DEBUG
	for (FlightGroundRouteDirectSegList::iterator ite = vRouteItems.begin();ite!=vRouteItems.end();ite++)
	{
		FlightGroundRouteDirectSegInSim* pSeg = *ite;
		pSeg = NULL;
	}
#endif

	return true;
}

void In_OutBoundRouteAssignmentInSim::GetOutboundRouteFromRouteItem( COut_BoundRoute* pRoute, AirsideFlightInSim* pFlight,
																	IntersectionNodeInSim * pNodeFrom, RunwayExitInSim* pTakeoffPos, TakeoffQueueInSim* pQueue,
																	FlightGroundRouteDirectSegList &vSegments, FlightGroundRouteFinderStrategy* pUsingStrategy /*= NULL*/ )
{
	PLACE_METHOD_TRACK_STRING();
	IntersectionNodeInSim * pNodeTo = pTakeoffPos->GetRouteSeg()->GetExitNode();
	FlightGroundRouteDirectSegInSim* pTakeoffPosSeg =pTakeoffPos->GetRouteSeg()->GetOppositeSegment();

	size_t nPriorityCount = pRoute->GetElementCount();
	for(size_t i=0;i<nPriorityCount;i++)
	{
		CRoutePriority* pPriority = pRoute->GetItem(i);


		FlightGroundRouteDirectSegList vRouteItems;
		if (!GetEntireOutboundRouteFromPriority(pPriority, pFlight, pNodeFrom, pNodeTo, pQueue, vRouteItems, pUsingStrategy))
			continue;

		// check the change condition
		if (CheckRoutePriorityChangeCondition(pFlight, pPriority, vRouteItems))
			continue;

		// Change condition fits failed, return the result
		vSegments.insert(vSegments.end(),vRouteItems.begin(),vRouteItems.end());
		pFlight->SetPreferredRoutePriority(pPriority);

		//if (IsValidRoute(vSegments,pNodeFrom->GetID(),pNodeTo->GetID()))
		vSegments.push_back(pTakeoffPosSeg);
		return;
	} // end of for
}

DeicepadGroupEntry* In_OutBoundRouteAssignmentInSim::GetRouteToDeiceGroup( AirsideFlightInSim* pFlight, TaxiwayDirectSegInSim* pSeg, DeicepadGroup* pGroup,FlightGroundRouteDirectSegList& vSegments )
{
	PLACE_METHOD_TRACK_STRING();
	if(!pSeg || !pGroup)
		return NULL;

	//IntersectionNodeInSim* pNodeFrom = pSeg->GetExitNode();
	DeicepadGroup::EntryList concernEntries = pGroup->GetEntryList();
	if (0 == concernEntries.size())
		return NULL;

	CTimeRangeRouteAssignment* pTimeRangeData = GetTimeRangeRouteAssignment(pFlight,DEICING);
	CIn_OutBoundRoute* pOut_Route = NULL;

	

	IntersectionNodeInSim* pNodeTo = NULL;
	
	// let's test every entry of DeicepadGroup
	for (DeicepadGroup::EntryList::iterator ite = concernEntries.begin();ite!=concernEntries.end();ite++)
	{
		// fetch one entry
		pNodeTo = (*ite)->GetIntersectionNode();

		FlightGroundRouteDirectSegList vRouteItems;
		double dPathWeight = (std::numeric_limits<double>::max)();
		m_pGroundResManager->GetRoute(pSeg,pNodeTo,pFlight,vRouteItems,dPathWeight);

		DeicepadGroupEntry* pEntry = *ite;
		FlightGroundRouteDirectSegList::iterator iteFound = std::find_first_of(
			vRouteItems.begin(), vRouteItems.end(), concernEntries.begin(), concernEntries.end(), SegEntrySuitGourpEntry(pEntry));
		vRouteItems.erase(iteFound, vRouteItems.end());

		vSegments.insert(vSegments.end(),vRouteItems.begin(),vRouteItems.end());
		return pEntry;
	}
	return NULL;

}

IntersectionNodeInSim* In_OutBoundRouteAssignmentInSim::_getCirculateRouteItems( CirculateRoute* pPriority,FlightGroundRouteDirectSegList& vRouteItems, int& nHeadTaxiwayID, int& nTailTaxiwayID )
{
	// 	pTailNode = NULL;
	nHeadTaxiwayID = -1;
	nTailTaxiwayID = -1;

	int nBranchCount = pPriority->GetElementCount();
	if (nBranchCount <= 0)
		return NULL;

	// get a random start route item
	int nRandomIdx = 0;
	if (nBranchCount > 1)
		nRandomIdx = rand()%nBranchCount;
	CirculateRoute* pThisRouteItem = pPriority->GetItem(nRandomIdx);

	int nPreIntersectionID = -1;

	bool bTailIsRunway = false;
	while(pThisRouteItem)
	{
		// Retrieve and analyze the alt object
		ALTObject *pALTObj = ALTObject::ReadObjectByID(pThisRouteItem->GetALTObjectID());
		if(pALTObj == NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		ALTOBJECT_TYPE objectType = pALTObj->GetType();

		{
			int nThisTaxiwayID = pALTObj->getID();
			ASSERT(nThisTaxiwayID>0);
			if (nTailTaxiwayID>0)
			{
				IntersectionNodeInSim* pNode = m_pIntersectionNodeManager->GetIntersectionNode(nTailTaxiwayID,nThisTaxiwayID,0);
				if (NULL == pNode)
				{
					//ASSERT(FALSE); // node finding error, should not be here
					vRouteItems.clear();
					TaxiwayInSim* pPreTaxiway = m_pTaxiResManager->GetTaxiwayByID(nTailTaxiwayID);
					TaxiwayInSim* pThisTaxiway = m_pTaxiResManager->GetTaxiwayByID(nThisTaxiwayID);
					ASSERT(pPreTaxiway && pThisTaxiway);
					CString strExceptionString;
					strExceptionString.Format(_T("There defines a route from taxiway %s to %s, but the two taxiways have no intersection node."),
						pPreTaxiway->GetTaxiwayInput()->GetMarking().c_str(), pThisTaxiway->GetTaxiwayInput()->GetMarking().c_str());
					throw strExceptionString;
				}
				// 				int nThisIntersectionID = pThisRouteItem->GetIntersectNodeID(); // caution: this method is unreliable, discarded
				int nThisIntersectionID = pNode->GetID();
				if (nPreIntersectionID>0 && nThisIntersectionID>0)
				{
					FlightGroundRouteDirectSegList vSegmentList;

					if (m_pRunwayResManager->GetRunwayByID(nTailTaxiwayID) != NULL)		//rwy item
						m_pRunwayResManager->GetRunwaySegments(nTailTaxiwayID,nPreIntersectionID,nThisIntersectionID,vSegmentList);
					else
						m_pTaxiResManager->GetTaxiwaySegments(nTailTaxiwayID,nPreIntersectionID,nThisIntersectionID,vSegmentList);

					vRouteItems.insert(vRouteItems.end(),vSegmentList.begin(),vSegmentList.end());
				}
				nPreIntersectionID = nThisIntersectionID;
			}
			if (-1 == nHeadTaxiwayID)
			{
				nHeadTaxiwayID = nThisTaxiwayID;
			}
			nTailTaxiwayID = nThisTaxiwayID;
		}

		delete pALTObj;
		pALTObj = NULL;

		nBranchCount = pThisRouteItem->GetElementCount();
		if (nBranchCount <= 0 || bTailIsRunway)
		{
			if (nPreIntersectionID >0 && vRouteItems.empty())	//only defined one entry node
			{
				return m_pIntersectionNodeManager->GetNodeByID(nPreIntersectionID);
			}
			return NULL;
		}

		// get the next item
		if (nBranchCount > 1)
			nRandomIdx = rand()%nBranchCount;
		else
			nRandomIdx = 0;
		pThisRouteItem = pThisRouteItem->GetItem(nRandomIdx);
	}
	return NULL;
}

bool In_OutBoundRouteAssignmentInSim::getCirculateRoute( AirsideFlightInSim* pFlight, IntersectionNodeInSim * pNodeFrom, IntersectionNodeInSim * pNodeTo,CirculateRoute* pPriority
									,  FlightGroundRouteDirectSegList& vRouteItems )
{
	PLACE_METHOD_TRACK_STRING();
	int nHeadTaxiwayID = -1;
	int nTailTaxiwayID = -1;
	IntersectionNodeInSim* pStartNode = NULL;
	IntersectionNodeInSim* pEndNode = NULL;
	try
	{
		pEndNode = pStartNode = _getCirculateRouteItems(pPriority, vRouteItems, nHeadTaxiwayID, nTailTaxiwayID);
	}
	catch (CString& e)
	{
		AirsideSimErrorShown::SimWarning(pFlight,e,CString(_T("DEFINE ERROR")));
		vRouteItems.clear();
		return false;
	}
	if (nHeadTaxiwayID<0 || nTailTaxiwayID<0)
	{
		return false;
	}

	bool bHeadTaxiwayOK = false; // 
	bool bTailTaxiwayOK = false; // 
	if (vRouteItems.size() > 0)
	{
		FlightGroundRouteDirectSegInSim* pHeadSeg = vRouteItems[0];
		FlightGroundRouteDirectSegInSim* pTailSeg = vRouteItems.back();
		if (pHeadSeg->GetObjectID() == nHeadTaxiwayID)
		{
			bHeadTaxiwayOK = true;
		}
		if (pTailSeg->GetObjectID() == nTailTaxiwayID)
		{
			bTailTaxiwayOK = true;
		}
		pStartNode = pHeadSeg->GetEntryNode();
		pEndNode = pTailSeg->GetExitNode();
	}

	// find pre route to the partial route vPostItems
	bHeadTaxiwayOK = GetPreRoute(pStartNode, pNodeFrom, nHeadTaxiwayID, bHeadTaxiwayOK, pFlight, vRouteItems);

	if (!bHeadTaxiwayOK || NULL == pStartNode)
	{
		ASSERT(FALSE);
		vRouteItems.clear();
		return false;
	}

	// find pNodeTo for post route finding
	if (NULL == pEndNode)
	{
		pEndNode = pStartNode;
	}

	// find post route from the partial route vPostItems
	bTailTaxiwayOK = GetPostRoute(pStartNode, pEndNode, pNodeTo, nTailTaxiwayID, bTailTaxiwayOK, pFlight, vRouteItems);
	if (!bTailTaxiwayOK)
	{
		ASSERT(FALSE);
		vRouteItems.clear();
		return false;
	}

#ifdef _DEBUG
	for (FlightGroundRouteDirectSegList::iterator ite = vRouteItems.begin();ite!=vRouteItems.end();ite++)
	{
		FlightGroundRouteDirectSegInSim* pSeg = *ite;
		pSeg = NULL;
	}
#endif

	return true;
}


In_OutBoundRouteAssignmentInSim::SegEntrySuitGourpEntry::SegEntrySuitGourpEntry(DeicepadGroupEntry*& pEntry)
	: m_pEntry(pEntry)
{

}

bool In_OutBoundRouteAssignmentInSim::SegEntrySuitGourpEntry::operator()( FlightGroundRouteDirectSegInSim* pSeg, DeicepadGroupEntry* pEntry )
{
	bool b = pSeg->GetEntryNode() == pEntry->GetIntersectionNode();
	if (b)
	{
		m_pEntry = pEntry;
	}
	return b;
}
