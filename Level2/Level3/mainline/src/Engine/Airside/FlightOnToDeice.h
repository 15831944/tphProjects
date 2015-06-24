#pragma once

class AirsideFlightInSim;
class ClearanceItem;
class Clearance;

class FlightGroundRouteResourceManager;

class FlightOnToDeice
{
public:
	static bool FindClearanceInConcern( AirsideFlightInSim * pFlight, FlightGroundRouteResourceManager* pResource, ClearanceItem& lastItem ,Clearance& newClearance );
};