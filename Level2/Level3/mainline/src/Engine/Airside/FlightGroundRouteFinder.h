#pragma once


#include "TaxiwayResource.h"
#include "RunwayInSim.h"
#include "FlightGroundRouteFinderStrategy.h"

class AirsideFlightInSim;
class IntersectionNodeInSim;
class TaxiwayDirectSegInSim;
class FlightGroundRouteDirectSegInSim;

class CBoostPathFinder;


class CFlightGroundRouteFinder
{
public:
	CFlightGroundRouteFinder();
	~CFlightGroundRouteFinder();
	void Init(std::vector<TaxiwayInSim>& vTaxiways,std::vector<FlightGroundRouteSegInSim*> excludeSegs );
	void Init(std::vector<TaxiwayInSim>& vTaxiways,std::vector<RunwayInSim::RefPtr>& vRunways ,std::vector<FlightGroundRouteSegInSim*> excludeSegs );

	bool GetRoute(AirsideFlightInSim *pFlight,IntersectionNodeInSim * pSrc,IntersectionNodeInSim * pDest,
		FlightGroundRouteDirectSegList& pathNodes, double& dPathWeight, FlightGroundRouteFinderStrategy* pStrategy = NULL, bool bIgnoreDoubtfulSeg = false);
	DistanceUnit GetRouteDist(IntersectionNodeInSim * pSrc,IntersectionNodeInSim * pDest)const;

	int GetNodeIndex(IntersectionNodeInSim* pNode)const;
	IntersectionNodeInSim* GetNode(int idx)const;

	int AddNode(IntersectionNodeInSim* pNode);	
	FlightGroundRouteDirectSegInSim* GetEdge(IntersectionNodeInSim* pNode1, IntersectionNodeInSim* pNode2)const;	


	void InitBoostGraph(AirsideFlightInSim *pFlight = NULL); // build the graph that serves pFlight, if NULL, ignore it
	std::vector<IntersectionNodeInSim*> m_vNodeList;
	

	typedef std::vector<FlightGroundRouteDirectSegInSim*> EdgeSet;
	EdgeSet m_vEdges;

	CBoostPathFinder* mpBoostPathFinder;



protected:
	//------------------------------------------------------------
	// strategy apply
	// apply you customized strategy
	void SetStrategy(const FlightGroundRouteFinderStrategy* pNewStrategy);
	//  restore the strategy to the basic one
	void RestoreBasicStrategy();
	//------------------------------------------------------------
	void MakeCanServeFlightSubGraphFinder(CFlightGroundRouteFinder* pSubGraphFinder, AirsideFlightInSim *pFlight) const;

private:
	const FlightGroundRouteFinderStrategy* m_pAppliedStrategy;

	static const FlightGroundRouteFinderStrategy m_sBasicStrategy;
};
