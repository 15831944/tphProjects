#pragma once
#include "FlightRouteSegInSim.h"

class AirportResourceManager;
class IntersectionNodeInSim;
class TaxiwayToNearestStretchRouteFinder
{
public:
	TaxiwayToNearestStretchRouteFinder(	AirportResourceManager* pAirportRes );
	~TaxiwayToNearestStretchRouteFinder(void);

	void GetShortestPathFromTaxiIntersectionToNearestStretch(const IntersectionNodeInSim* pInNode, FlightGroundRouteDirectSegList& path);

private:
	IntersectionNodeInSim* GetTaxiIntersectionBesideNearestStretch(const IntersectionNodeInSim* pInNode, TaxiwayDirectSegInSim** pLaneEntrySeg);

private:
	AirportResourceManager* m_pAirportRes;
	std::map<IntersectionNodeInSim*, FlightGroundRouteDirectSegList> m_mapFoundRoute;

};
