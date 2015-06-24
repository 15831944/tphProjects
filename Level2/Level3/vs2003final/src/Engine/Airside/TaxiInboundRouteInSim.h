#pragma once
#include "TaxiRouteInSim.h"

class ENGINE_TRANSFER TaxiInboundRouteInSim : public TaxiRouteInSim
{
public:
	TaxiInboundRouteInSim(AirsideMobileElementMode mode,AirsideResource* pOrign,AirsideResource* pDest);
	~TaxiInboundRouteInSim(void);

public:
	//get clearance 
	bool FindClearanceInConcern(AirsideFlightInSim * pFlight,ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance);

};
