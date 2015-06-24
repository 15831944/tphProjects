#include "StdAfx.h"
#include ".\taxitostandrouteinsim.h"
#include "./StandInSim.h"
TaxiTostandRouteInSim::TaxiTostandRouteInSim(AirsideMobileElementMode mode,AirsideResource* pRes, AirsideResource* pStand)
:TaxiRouteInSim(mode,pRes,pStand)
{
}

TaxiTostandRouteInSim::~TaxiTostandRouteInSim(void)
{
}

bool TaxiTostandRouteInSim::FindClearanceInConcern(AirsideFlightInSim * pFlight,ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance)
{
	return TaxiRouteInSim::FindClearanceInConcern(pFlight,lastItem,radius,newClearance);
}

bool TaxiTostandRouteInSim::IsLockDirectionOfWholeRoute()
{
	return true;
}