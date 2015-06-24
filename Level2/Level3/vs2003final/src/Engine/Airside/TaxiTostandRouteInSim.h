#pragma once
#include "TaxiRouteInSim.h"

class ENGINE_TRANSFER TaxiTostandRouteInSim : public TaxiRouteInSim
{
public:
	TaxiTostandRouteInSim(AirsideMobileElementMode mode,AirsideResource* pRes, AirsideResource* pStand);
	~TaxiTostandRouteInSim(void);

public:
	//get clearance 
	bool FindClearanceInConcern(AirsideFlightInSim * pFlight,ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance);

	bool IsLockDirectionOfWholeRoute();
};
