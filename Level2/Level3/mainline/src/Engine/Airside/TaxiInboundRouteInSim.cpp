#include "StdAfx.h"
#include ".\taxiinboundrouteinsim.h"
#include "AirsideFlightInSim.h"

TaxiInboundRouteInSim::TaxiInboundRouteInSim(AirsideMobileElementMode mode,AirsideResource* pOrign,AirsideResource* pDest)
:TaxiRouteInSim(mode,pOrign,pDest)
{
}

TaxiInboundRouteInSim::~TaxiInboundRouteInSim(void)
{
}

bool TaxiInboundRouteInSim::FindClearanceInConcern(AirsideFlightInSim * pFlight,ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance)
{
	return TaxiRouteInSim::FindClearanceInConcern(pFlight,lastItem,radius,newClearance);
}

DistanceUnit TaxiRouteToTempParking::GetExitRouteDist( AirsideFlightInSim* pFlight )
{
	DistanceUnit dExitRouteDist = GetEndDist() - pFlight->GetLength()*0.5;//default exit route dist

	/*HoldInTaxiRoute* lastEntryHold =  GetlastEntryHold();
	if(lastEntryHold)
	{ 
		dExitRouteDist = lastEntryHold->m_dDistInRoute;
	}*/
	return dExitRouteDist;
}
