#include "StdAfx.h"
#include ".\taxiwaycontroller.h"
#include "Inputs/flight.h"
#include "FlightInAirsideSimulation.h"
#include "TaxiwayResource.h"
#include "AirportTrafficController.h"
#include "StandTrafficController.h"
#include "RunwayController.h"
#include "../../Common/ARCUnit.h"
#include "InboundRouteInSimList.h"
#include "StandInSim.h"
#include "FlightMovementsOnResource.h"
#include "../../InputAirside/InboundRouteAssignment.h"
#include "../../InputAirside/outboundRouteAssignment.h"
#include "OutboundRouteInSimList.h"


TaxiwayTrafficController::TaxiwayTrafficController( AirportTrafficController * pAirportController )
{
	m_pAirportController = pAirportController;

	m_pInboundRouteInSimList = new CInboundRouteInSimList;
	m_pOutboundRouteInSimList = new COutboundRouteInSimList;
}

TaxiwayTrafficController::~TaxiwayTrafficController()
{
	if (NULL != m_pInboundRouteInSimList)
	{
		delete m_pInboundRouteInSimList;
	}

	if (NULL != m_pOutboundRouteInSimList)
	{
		delete m_pOutboundRouteInSimList;
	}
}

Clearance* TaxiwayTrafficController::AssignNextClearance( FlightInAirsideSimulation * pFlight )
{
	if(pFlight->GetFlightMode() == OnBirth)
	{
		ASSERT(false);
		return NULL;
	}
	//check there is other flight
	if(pFlight->GetCurrentResource()&& pFlight->GetCurrentResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
	{
		TaxiwayDirectSegInSim * pDirSeg = (TaxiwayDirectSegInSim *) pFlight->GetCurrentResource();
		DistanceUnit endDist = pDirSeg->GetEndDist();
		
		Clearance * newClearance = new Clearance;
		if(pFlight->GetDistInResource() < endDist)  // still not ready to leave this segment
		{
			newClearance->m_dDist = endDist;
			newClearance->m_pResource = pDirSeg;
			newClearance->m_nMode = pFlight->GetFlightMode();
			newClearance->m_tTime = pFlight->GetCurrentTime();
		}
		else                                                //ready to leave this segment
		{
			AirsideResource * nextResource = GetNextResource(pFlight);
			if(!nextResource)
				return NULL;
			newClearance->m_dDist = 0;
			if(nextResource->GetType() == AirsideResource::ResType_RunwayPort)
			{
				RunwayPortInSim * pRunway = (RunwayPortInSim*) nextResource;
				
				newClearance->m_nMode = OnPreTakeoff;	
				newClearance->m_dDist = pRunway->GetPointDist(pDirSeg->GetExitNode()->GetDistancePoint(0));
			
			}
			else if(nextResource->GetType() == AirsideResource::ResType_Stand)
				newClearance->m_nMode = OnHeldAtStand;

			else { 
				newClearance->m_nMode = pFlight->GetFlightMode(); 
			}
			newClearance->m_tTime = pFlight->GetCurrentTime();
			newClearance->m_pResource = nextResource;			
		}
		m_pAirportController->AmendClearance(pFlight,newClearance);
		return newClearance;
	}

	return NULL;
	//return GetConflictClearance(pFlight);
	
}


bool TaxiwayTrafficController::Init( int nPrjId, TaxiwayResourceManager * taxiwayRes )
{
	m_pTaxiwayRes = taxiwayRes;
	m_pInboundRouteInSimList->Init(nPrjId, nPrjId, taxiwayRes);
	m_pOutboundRouteInSimList->Init(nPrjId, nPrjId, taxiwayRes);

	return true;
}

//call once if you don't want to change route
AirsideResource * TaxiwayTrafficController::GetNextResource( FlightInAirsideSimulation * pFlight ,bool bOtherChoose)
{
	AirsideResource * pCurRes = pFlight->GetCurrentResource();

	IntersectionNodeInSim * exitNode = NULL;
	IntersectionNodeInSim * destNode = NULL;

	AirsideResource * pNextRes = NULL;
	
	if( pCurRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg ) // flight in taxiway segment
	{
		TaxiwayDirectSegInSim * pTaxiSeg = (TaxiwayDirectSegInSim*) pCurRes;	
		exitNode = pTaxiSeg->GetExitNode();
		//return exitNode;

	}
	if( pCurRes->GetType() == AirsideResource::ResType_IntersectionNode)
	{
		exitNode = (IntersectionNodeInSim* )pCurRes;
	}
	
	//
	//check to see whether the flight is about to end taxiing  
	if( pFlight->GetFlightMode() == OnTaxiToStand )  // 
	{
		//use InboundRoute
		StandInSim * pStand = m_pAirportController->GetStandTrafficController()->AssignStand(pFlight);
		ASSERT(pStand);
		destNode = pStand->GetInNode(); 
		if( destNode == exitNode ) return pStand;	
		else
		{
			if( !pFlight->GetPlanningFlow().GetInBoundRoute().GetVersion()  ) // init InBound Route
			{	
				TaxiwayDirectSegList theRoute;

				switch(m_pInboundRouteInSimList->GetInboundRouteType())
				{
				case ROUTEASSIGNMENT_SHORTPATHTOSTAND:
					m_pTaxiwayRes->GetRoute(exitNode,destNode,pFlight,0,theRoute);
					break;
				case ROUTEASSIGNMENT_TIMESHORTPATHTOSTAND:
					ASSERT(FALSE);
				    break;
				case ROUTEASSIGNMENT_PREFERREDROUTE:
					{
						BOOL bExist = FALSE;
						bExist = m_pInboundRouteInSimList->GetInboundRoute(pFlight->GetRunwayExit(), exitNode, pStand, theRoute);

						//can't fine prefer route
						if (!bExist)
						{
							m_pTaxiwayRes->GetRoute(exitNode,destNode,pFlight,0,theRoute);
						}
					}
					break;
				default:
				    break;
				}

				pFlight->GetPlanningFlow().GetInBoundRoute().SetRoute(theRoute);
			}
			PlanGroundRouteItem * pNextItem  =  pFlight->GetPlanningFlow().GetInBoundRoute().GetNextItem();
			if(pNextItem)
			{
				pNextRes = pNextItem->GetTaxiwaySeg();				
			}
		}
	}
	if(pFlight->GetFlightMode() == OnTaxiToRunway ) //taxi to runway
	{
		//Use OutboundRoute
		//get take off position of runway
		RunwayPortInSim * pTakeoffRunway = m_pAirportController->GetRunwayTrafficController()->GetTakeoffRunway(pFlight);
		IntersectionNodeInSim* pTakeoffPosition = m_pAirportController->GetRunwayTrafficController()->GetTakeoffPosition(pFlight,pTakeoffRunway);
		ASSERT(pTakeoffPosition);
		destNode = pTakeoffPosition;
		if(destNode == exitNode ) return pTakeoffRunway;
		else
		{
			if( !pFlight->GetPlanningFlow().GetOutBoundRoute().GetVersion()  ) // init InBound Route
			{			
				TaxiwayDirectSegList theRoute;

				switch(m_pOutboundRouteInSimList->GetOutboundRouteType())
				{
				case OUTBOUNDROUTEASSIGNMENT_SHORTPATHTOSTAND:
					m_pTaxiwayRes->GetRoute(exitNode,destNode,pFlight,0,theRoute);
					break;

				case OUTBOUNDROUTEASSIGNMENT_TIMESHORTPATHTOSTAND:
					ASSERT(FALSE);
				    break;

				case OUTBOUNDROUTEASSIGNMENT_PREFERREDROUTE:
					{
						StandInSim * pStand = m_pAirportController->GetStandTrafficController()->AssignStand(pFlight);
						//just for test
						exitNode = pStand->GetOutNode();
						BOOL bExist = m_pOutboundRouteInSimList->GetOutboundRoute(pFlight->GetRunwayExit(), exitNode, pStand, theRoute);

						//can't fine prefer route
						if (!bExist)
						{
							m_pTaxiwayRes->GetRoute(exitNode,destNode,pFlight,0,theRoute);
						}
					}
					break;

				default:
				    break;
				}
				
				pFlight->GetPlanningFlow().GetOutBoundRoute().SetRoute(theRoute);				
			}

			PlanGroundRouteItem * pNextItem  =  pFlight->GetPlanningFlow().GetOutBoundRoute().GetNextItem();
			if(pNextItem)
			{
				pNextRes = pNextItem->GetTaxiwaySeg();				
			}
		}
		
	}
	if(pFlight->GetFlightMode() == OnTaxiToDeice) //taxi to deice
	{
		// do check arrived the dest
	}
	if(pFlight->GetFlightMode() == OnTaxiToHoldArea)
	{
		// do check arrived the dest
	}

	//if(destNode!=exitNode) pNextRes = EstimateNextTaxiwaySegment(exitNode,destNode,pFlight,bOtherChoose);
	
	return pNextRes;
}
//
TaxiwayDirectSegInSim * TaxiwayTrafficController::EstimateNextTaxiwaySegment(IntersectionNodeInSim * pFrom , IntersectionNodeInSim * pTo, FlightInAirsideSimulation * pFlight, bool bOtherChoose)
{
	//Check to see 
	TaxiwayDirectSegList theRoute;
	
	//if( m_RoutePriorityMap.end() == m_RoutePriorityMap.find(pFlight) ) m_RoutePriorityMap[pFlight] = 0;	


	m_pTaxiwayRes->GetRoute(pFrom,pTo,pFlight,0,theRoute);
	
	ASSERT(!theRoute.empty());
	if(theRoute.empty()) return NULL;
	
	return theRoute.at(0).get();

}

ElapsedTime TaxiwayTrafficController::GetTaxiwaySeparationTime(FlightInAirsideSimulation* pLeadFlight, FlightInAirsideSimulation* pFollowFlight)const
{
	ElapsedTime tSeperation(1L);

	return tSeperation;
}

DistanceUnit TaxiwayTrafficController::GetTaxiwaySeperationDistance( FlightInAirsideSimulation * pLeadFlight, FlightInAirsideSimulation * pFollowFlight ) const
{
	return 10000;  // 100meters
}

bool TaxiwayTrafficController::AmendClearance( FlightInAirsideSimulation * pFlight, Clearance* pClearance )
{
	double taxispd = pFlight->getFlightPerformanceManager()->getTaxiInSpeed(pFlight->GetCFlight());
	TaxiwayDirectSegInSim * pNextSegment = NULL;
	DistanceUnit m_nextDist = pClearance->m_dDist;
	if(!pClearance->m_pResource)
	{
		if(pFlight->GetCurrentResource()&& pFlight->GetCurrentResource()->GetType()== AirsideResource::ResType_Stand)
		{
			StandInSim * pStand = (StandInSim * )pFlight->GetCurrentResource();
			RunwayPortInSim * pDestRunway = m_pAirportController->GetRunwayTrafficController()->GetTakeoffRunway(pFlight);
			IntersectionNodeInSim * pNode = m_pAirportController->GetRunwayTrafficController()->GetTakeoffPosition(pFlight,pDestRunway);
			
			pNextSegment = EstimateNextTaxiwaySegment(pStand->GetOutNode(),pNode,pFlight);
			m_nextDist = 0;
		}

	}
	else if(pClearance->m_pResource && pClearance->m_pResource->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
	{
		pNextSegment = (TaxiwayDirectSegInSim *) pClearance->m_pResource;
	}
	pClearance->m_pResource = pNextSegment;
	pClearance->m_dDist = m_nextDist;
	//check conflict ion
	if(pNextSegment == pFlight->GetCurrentResource()) // on the same taxiway segment
	{
		OccupancyInstance prefltOcy = pNextSegment->GetPreviousFlightOccupancy(pFlight);
		if(prefltOcy.IsValid()){
			FlightInAirsideSimulation * pLeadFlight = prefltOcy.GetFlight();
			DistanceUnit prefltDist = 0;
			ElapsedTime prefltTimeAtDist = pLeadFlight->GetCurrentTime();
			DistanceUnit sepDist = GetTaxiwaySeperationDistance(pLeadFlight,pFlight);
			ElapsedTime sepTime = GetTaxiwaySeparationTime(pLeadFlight,pFlight);

			if(pLeadFlight->GetCurrentResource()== pNextSegment) //still on the segment
			{
				prefltDist = pLeadFlight->GetDistInResource();
				prefltTimeAtDist = pLeadFlight->GetCurrentTime();
			}
			else if(prefltOcy.IsExitTimeValid())                       //already left
			{
				prefltDist = pNextSegment->GetEndDist();
				prefltTimeAtDist = prefltOcy.GetExitTime();
			}
			//
			//DistanceUnit dFlightDist = prefltDist - pFlight->GetDistInResource();
			//ElapsedTime dFlightSepT  = pFlight->GetCurrentTime() - prefltTimeAtDist;
			DistanceUnit saftDist = prefltDist - sepDist;
			ElapsedTime saftTime = prefltTimeAtDist + sepTime;

			DistanceUnit fltDist = pFlight->GetDistInResource();
			ElapsedTime fltTime = pFlight->GetCurrentTime();

			if(fltDist >= saftDist) // wait to the flight time 
			{				
				if(fltTime < saftTime)
				{
					pClearance->m_tTime = saftTime;	
					pClearance->m_dDist = fltDist;
				}
				
			}
			else{
				ElapsedTime timeToSafeDist = ElapsedTime( ( saftDist - fltDist )/taxispd) + fltTime;
				if(timeToSafeDist > saftTime)
				{	
					pClearance->m_dDist = saftDist;
				}
				
			}
					
			return true;
		}
	}
	else             //going to enter another resource
	{
		//first try to active route 
		{
			pFlight->GetCurrentRoute().Activate();
			
			PlanGroundRouteItem * pItem = pFlight->GetCurrentRoute().GetNextItem();
			if(pItem && (pItem->GetTaxiwaySeg() == pNextSegment) )
			{
				if( !pNextSegment->IsActive() )
				{				
					pClearance->m_tTime += ElapsedTime(10L);
					pClearance->m_dDist = pFlight->GetDistInResource();
					pClearance->m_pResource = pFlight->GetCurrentResource();
					pClearance->m_nMode = pFlight->GetFlightMode();		
					return true;
				}
			}		
			
		}
		
		IntersectionNodeInSim * pEnterNode = pNextSegment->GetEntryNode();  //check node first
		OccupancyInstance prefltOcy = pEnterNode->GetLastOccupyInstance();
		if(prefltOcy.IsValid() && prefltOcy.IsExitTimeValid())
		{
			FlightInAirsideSimulation * pLeadFlight = prefltOcy.GetFlight();
			DistanceUnit sepDist = GetTaxiwaySeperationDistance(pLeadFlight,pFlight);
			ElapsedTime sepTime = GetTaxiwaySeparationTime(pLeadFlight,pFlight);
			ElapsedTime dFlightSepT = pClearance->m_tTime - prefltOcy.GetExitTime();
			
			if(dFlightSepT < sepTime){
				pClearance->m_tTime += sepTime - dFlightSepT;
			}
		}
		//check segment
		OccupancyInstance presegOcy = pNextSegment->GetLastOccupyInstance();
		if(presegOcy.IsValid())
		{
			
			
			FlightInAirsideSimulation *pLeadflight = presegOcy.GetFlight();			
			
			DistanceUnit prefltDist = 0;
			ElapsedTime prefltTimeAtDist = pLeadflight->GetCurrentTime();
			DistanceUnit sepDist = GetTaxiwaySeperationDistance(pLeadflight,pFlight);
			ElapsedTime sepTime = GetTaxiwaySeparationTime(pLeadflight,pFlight);

			if(pLeadflight->GetCurrentResource()== pNextSegment) //still on the segment
			{
				prefltDist = pLeadflight->GetDistInResource();
				prefltTimeAtDist = pLeadflight->GetCurrentTime();
			}
			else if(presegOcy.IsExitTimeValid())                    //already left
			{
				prefltDist = pNextSegment->GetEndDist();
				prefltTimeAtDist = presegOcy.GetExitTime();
			}
			
			DistanceUnit dFlightDist = prefltDist - pClearance->m_dDist ;
			if(dFlightDist < sepDist) // wait to the flight time 
			{
				/*pClearance->m_pResource = pFlight->GetCurrentResource();
				pClearance->m_dDist = pFlight->GetDistInResource();
				pClearance->m_nMode = pFlight->GetFlightMode();*/
				ElapsedTime dFlightSepT  = pClearance->m_tTime - prefltTimeAtDist;
				if(dFlightSepT < sepTime){
					pClearance->m_tTime += sepTime - dFlightSepT;		
				}else {
					pClearance->m_tTime += ElapsedTime( (sepDist -dFlightDist)/taxispd);
				}
			}			
		}
		//check the other segment
		{
			TaxiwayDirectSegInSim * pOtherSegment = pNextSegment->GetOppositeSegment();
			OccupancyInstance otherOcy = pOtherSegment->GetLastOccupyInstance();
			if(otherOcy.IsValid() )
			{
				FlightInAirsideSimulation *pLeadflight = otherOcy.GetFlight();	
				ElapsedTime sepTime = GetTaxiwaySeparationTime(pLeadflight,pFlight);
				if(otherOcy.IsExitTimeValid())
				{
					ElapsedTime prefltTimeAtDist = otherOcy.GetExitTime();
					ElapsedTime dFlightSepT  = pClearance->m_tTime - prefltTimeAtDist;			

					if(dFlightSepT < sepTime){
						pClearance->m_tTime += sepTime - dFlightSepT;		
					}		
				}else
				{
					pClearance->m_tTime += sepTime;
					pClearance->m_dDist = pFlight->GetDistInResource();
					pClearance->m_pResource = pFlight->GetCurrentResource();
					pClearance->m_nMode = pFlight->GetFlightMode();
					return true;
				}

			}
		}
		

		
	}
	
	PlanGroundRouteItem * pItem = pFlight->GetCurrentRoute().GetNextItem();
	if(pItem && (pItem->GetTaxiwaySeg() == pClearance->m_pResource) )
	{
		pItem->SetFinished(true);
	}
	return true;	
}