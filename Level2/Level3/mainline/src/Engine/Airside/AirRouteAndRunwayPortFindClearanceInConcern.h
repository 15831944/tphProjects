#pragma once
#include "../../Common/point.h"

class AirsideFlightInSim;
class ClearanceItem;
class Clearance;

class AirRouteAndRunwayPortFindClearanceInConcern
{
public:
	AirRouteAndRunwayPortFindClearanceInConcern(void);
	~AirRouteAndRunwayPortFindClearanceInConcern(void);

public:
	static bool FindClearanceInConcern(AirsideFlightInSim * pFlight,  ClearanceItem& lastItem ,DistanceUnit radius, Clearance& newClearance);
};
