#pragma once
#include "TaxiRouteInSim.h"

class ENGINE_TRANSFER TaxiOutboundRouteInSim : public TaxiRouteInSim
{
public:
	TaxiOutboundRouteInSim(AirsideMobileElementMode mode,AirsideResource* pOrign,AirsideResource* pDest);
	~TaxiOutboundRouteInSim(void);

public:
	//get clearance 
	bool FindClearanceInConcern(AirsideFlightInSim * pFlight,ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance);

	bool IsLockDirectionOfWholeRoute();

};
