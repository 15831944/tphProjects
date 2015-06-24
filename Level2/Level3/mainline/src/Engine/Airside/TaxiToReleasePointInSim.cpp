#include "StdAfx.h"
#include ".\taxitoreleasepointinsim.h"
#include "DynamicConflictGraph.h"
#include "AirsideFlightInSim.h"
#include "AirTrafficController.h"


TaxiToReleasePointRouteInSim::TaxiToReleasePointRouteInSim(AirsideMobileElementMode mode,AirsideResource* pSrcRes, AirsideResource* pDestRes)
:TaxiRouteInSim(mode,pSrcRes,pDestRes)
{
}

TaxiToReleasePointRouteInSim::~TaxiToReleasePointRouteInSim(void)
{
}

bool TaxiToReleasePointRouteInSim::FindClearanceInConcern(AirsideFlightInSim * pFlight,ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance)
{
	return TaxiRouteInSim::FindClearanceInConcern(pFlight,lastItem,radius,newClearance);
}

bool TaxiToReleasePointRouteInSim::IsLockDirectionOfWholeRoute()
{
	return true;
}

void TaxiToReleasePointRouteInSim::PlanRoute( AirsideFlightInSim* pFlt,const ElapsedTime& t )
{
	RouteDirPathList routeList = getNextPathList(-1); //all route	
	if(!routeList.empty() && !m_postSegs.empty())
	{
		RouteDirPath* pPath = *routeList.rbegin();
		RouteDirPath* pNextPath = *m_postSegs.begin();
		if(pPath== pNextPath)
		{
			routeList.pop_back();//erase last seg
		}
	}
	routeList.insert(routeList.end(),m_postSegs.begin(),m_postSegs.end());

	pFlt->GetAirTrafficController()->getConflictGraph()->OnMobilePlaneRoute(pFlt,routeList,t);
}

FlightGroundRouteDirectSegInSim* TaxiToReleasePointRouteInSim::getLastSeg()
{
	if(!m_vItems.empty())
		return (FlightGroundRouteDirectSegInSim*)m_vItems.rbegin()->GetResource();
	return NULL;
}

DistanceUnit TaxiToReleasePointRouteInSim::GetExitRouteDist( AirsideFlightInSim* pFlight )
{
 	if(mpResDest && mpResDest->GetType()==AirsideResource::ResType_StartPosition)
 	{
 		DistanceUnit dDefault = GetEndDist() -100;
//  		HoldInTaxiRoute* pLastEntry = GetlastEntryHold();
//  		if(pLastEntry)
//  		{
//  			dDefault = min(dDefault, pLastEntry->m_dDistInRoute-1);
//  		}
 		return dDefault;
 	}
	return TaxiRouteInSim::GetExitRouteDist(pFlight);
}