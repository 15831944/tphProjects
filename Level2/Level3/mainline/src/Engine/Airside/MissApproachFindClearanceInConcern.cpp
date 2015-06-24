#include "StdAfx.h"
#include "missapproachfindclearanceinconcern.h"
#include "AirspaceResource.h"
#include "AirsideFlightInSim.h"
#include "Clearance.h"
#include "FlightPerformanceManager.h"
#include "AirTrafficController.h"
#include "AirspaceResourceManager.h"

CMissApproachFindClearanceInConcern::CMissApproachFindClearanceInConcern(void)
{
	m_pAirspaceRes = NULL;
}

CMissApproachFindClearanceInConcern::~CMissApproachFindClearanceInConcern(void)
{
}



bool CMissApproachFindClearanceInConcern::FindClearanceInConcern(AirsideFlightInSim * pFlight,  ClearanceItem& lastItem,  Clearance& newClearance)
{
	RunwayPortInSim * pLandRunway = pFlight->GetLandingRunway();
	FlightRouteInSim route = m_pAirspaceRes->GetMisApproachRoute(pFlight);

	//
	if( !m_pAirspaceRes->FindClearanceInConcern(pFlight,&route,lastItem, newClearance) )
	{ //terminate or go back to star
		
		AirWayPointInSim * lastWayPtInRoute = NULL;
		if( route.GetItemCount() ){
			
			AirsideResource* pLastResource = route.GetItem(route.GetItemCount()-1);
			if(pLastResource && pLastResource->GetType() == AirsideResource::ResType_WayPoint)
			{
				lastWayPtInRoute = (AirWayPointInSim *) pLastResource;	
			}
		}

        FlightRouteInSim * pSTAR = pFlight->GetSTAR();
		if( pSTAR && lastWayPtInRoute)
		{			
			FlightPlanPropertyInSim property = pSTAR->GetRoutePointProperty(lastWayPtInRoute);
			//back to Item
			ClearanceItem backStarItem(lastWayPtInRoute,property.GetMode(),0);
			backStarItem.SetSpeed(property.GetSpeed());
			backStarItem.SetAltitude(property.GetAlt());
			ElapsedTime dT = pFlight->GetTimeBetween(lastItem,backStarItem);
			backStarItem.SetTime( dT + lastItem.GetTime() );
			lastItem = backStarItem;
			newClearance.AddItem(lastItem);
			return true;
		}

	}
	//terminate item
	
	return true;
}

void CMissApproachFindClearanceInConcern::Init( int nPrjId, AirRouteNetworkInSim * pAirspaceRes )
{
	m_pAirspaceRes = pAirspaceRes;
}
