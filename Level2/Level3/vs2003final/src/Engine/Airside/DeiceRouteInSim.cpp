#include "StdAfx.h"
#include "DeiceRouteInSim.h"

#include "DynamicConflictGraph.h"
#include "Airsideflightinsim.h"
#include "AirTrafficController.h"


void DeiceRouteInSim::setAfterRoute( FlightGroundRouteDirectSegList& routes ,bool isCyclicRoute)
{
	return mAfterDeiceRoute.AddTaxiwaySegList(routes,isCyclicRoute);
}

void DeiceRouteInSim::PlanRoute( AirsideFlightInSim* pFlt,const ElapsedTime& t )
{
	RouteDirPathList routeList = getNextPathList(-1); //all route
	RouteDirPathList leftRoutelist = mAfterDeiceRoute.getNextPathList(-1);
	routeList.insert(routeList.end(),leftRoutelist.begin(),leftRoutelist.end());

	pFlt->GetAirTrafficController()->getConflictGraph()->OnMobilePlaneRoute(pFlt,routeList,t);

}

DeiceRouteInSim::DeiceRouteInSim(AirsideResource* pOrign, AirsideResource* pDest):
TaxiRouteInSim(OnTaxiToDeice, pOrign,pDest),
mAfterDeiceRoute(OnTaxiToRunway,pOrign,pDest)
{

}

void DeiceRouteInSim::FlightExitRoute( AirsideFlightInSim* pFlight, const ElapsedTime& releaseTime )
{
	TaxiRouteInSim::FlightExitRoute(pFlight,releaseTime);

	RouteDirPathList leftRoutelist = mAfterDeiceRoute.getNextPathList(-1);
	pFlight->GetAirTrafficController()->getConflictGraph()->OnMobilePlaneRoute(pFlight,leftRoutelist,releaseTime,true);

}