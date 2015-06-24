#pragma once

#include <set>
#include "TaxiwayResource.h"
#include "RunwayInSim.h"
class AirsideFlightInSim;
class IntersectionNodeInSim;
class TaxiwayDirectSegInSim;
class FlightGroundRouteDirectSegInSim;

//////////////////////////////////////////////////////////////////////////

class CFindFlightShortestPath
{
public:	
	typedef std::set<IntersectionNodeInSim*> IntersectionNodesSet;	
	typedef std::pair<IntersectionNodeInSim * , IntersectionNodeInSim * > NodePair;	

	struct TaxiGraphInSim
	{
		IntersectionNodesSet m_vNodeList;
		FlightGroundRouteDirectSegList m_vEdegeList;
		FlightGroundRouteDirectSegInSim* GetValidEdge(const NodePair& nodePair,AirsideFlightInSim* pFlight);
		void clear();
		void AddEdge(FlightGroundRouteDirectSegInSim* pSeg); 
	};

	
	
	void Init(std::vector<TaxiwayInSim>& vTaxiways, std::vector<FlightGroundRouteSegInSim*> excludeSegs );
	void Init(std::vector<TaxiwayInSim>& vTaxiways,std::vector<RunwayInSim::RefPtr>& vRunways ,std::vector<FlightGroundRouteSegInSim*> excludeSegs );

	DistanceUnit GetRoute(AirsideFlightInSim *pFlight,IntersectionNodeInSim * pSrc,IntersectionNodeInSim * pDest,FlightGroundRouteDirectSegList& pathNodes);	
	
public:	
	class RouteInGraph
	{
	public:
		RouteInGraph();
		RouteInGraph(FlightGroundRouteDirectSegInSim* pDirSeg);

		DistanceUnit m_dist;
		FlightGroundRouteDirectSegList m_vSegs;
		bool m_bIsDoneSearch;    //dij shortest path flag

		bool operator < (const RouteInGraph& otherRoute)const;	
		bool CanServeFlight(AirsideFlightInSim*pFlight)const;
	};
	typedef std::map<NodePair, RouteInGraph> ShortestRouteMapType;

protected:
	//the map for all flights
	ShortestRouteMapType m_ShortestRouteMap;
	TaxiGraphInSim m_taxiwayGraph;
};
