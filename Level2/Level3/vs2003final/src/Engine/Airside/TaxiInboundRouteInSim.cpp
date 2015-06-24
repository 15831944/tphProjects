#include "StdAfx.h"
#include ".\taxiinboundrouteinsim.h"

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
