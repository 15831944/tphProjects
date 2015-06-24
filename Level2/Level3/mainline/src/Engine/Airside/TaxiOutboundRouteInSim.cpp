#include "StdAfx.h"
#include ".\taxioutboundrouteinsim.h"

TaxiOutboundRouteInSim::TaxiOutboundRouteInSim(AirsideMobileElementMode mode,AirsideResource* pOrign,AirsideResource* pDest)
:TaxiRouteInSim(mode,pOrign,pDest)
{
}

TaxiOutboundRouteInSim::~TaxiOutboundRouteInSim(void)
{
}

bool TaxiOutboundRouteInSim::FindClearanceInConcern(AirsideFlightInSim * pFlight,ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance)
{
	return TaxiRouteInSim::FindClearanceInConcern(pFlight,lastItem,radius,newClearance);
}

bool TaxiOutboundRouteInSim::IsLockDirectionOfWholeRoute()
{
	return true;
}