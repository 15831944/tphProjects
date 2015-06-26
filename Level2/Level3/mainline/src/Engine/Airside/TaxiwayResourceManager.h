#pragma once

#include "./../../Common/point.h"
#include "TaxiwayResource.h"
#include <set>
#include "ResourceManager.h"
#include "FindFlightShortestPath.h"
#include "HoldShortLineInSim.h"

class CAirportDatabase;
class ENGINE_TRANSFER TaxiwayResourceManager : public ResourceManager
{
public:
	TaxiwayResourceManager();
	~TaxiwayResourceManager();

	bool Init(int nPrjID, int nAirportID);	
	bool InitRelations(IntersectionNodeInSimList& NodeList);
	bool InitConstraints(int nProjID, int nAirportID, CAirportDatabase* pAirportDB);

	//Get path from source to dest, 
	//PRI : priority , 0 the shortest , 1 the second shortest ...,  pathNodeList:(pSrc, node1, node 2, pDest)
	//DistanceUnit GetRoute(IntersectionNodeInSim * pSrc, IntersectionNodeInSim * pDest, AirsideFlightInSim *pFlight,int nPRI, FlightGroundRouteDirectSegList& pathNodes );	

	TaxiwaySegInSim * GetTaxiwaySegment(IntersectionNodeInSim  *pNode1, IntersectionNodeInSim * pNode2);
	void GetTaxiwaySegments(int nTaxiwayID, int nIntersectNodeIDFrom, int nIntersectNodeIDTo, FlightGroundRouteDirectSegList& taxiwayDirectSegLst);
	bool IsNodeOnTaxiway(int nTaxiwayID, int nIntersectNodeID);
	TaxiwayDirectSegInSim* GetTaxiwayDirectSegment(int nIntersectNodeIDFrom, int nIntersectNodeIDTo, int nTaxiwayID);

	// get Direct segment list linked with this node 
	FlightGroundRouteDirectSegList GetLinkedDirectSegmentsTaxiway(IntersectionNodeInSim * pNode);
	FlightGroundRouteDirectSegList GetAllLinkedDirectSegmentsTaxiway(IntersectionNodeInSim * pNode);

	int GetTaxiwayCount()const{ return m_vTaxiways.size(); }
	TaxiwayInSim * GetTaxiwayByIdx(int idx);

	TaxiwayInSim * GetTaxiwayByID(int id);
	FlightGroundRouteInSim* GetGroundRouteByID(int id);

	int GetLinkedTaxiwayID(IntersectionNodeInSim * pNode1, IntersectionNodeInSim* pNode2);
	//nTaxiwayID, the taxiway which hold position belong to
	//nIntersectionID, the intersection which has hold position
	//return hold positions' list
	std::vector<HoldPositionInSim *> GetHoldPositionInSim(int nTaxiwayID,int nIntersectionID);
	void clear();
	std::vector< TaxiwayInSim >& getTaxiwayResource();

	//init taxiway adjacency data method
	void InitTaxiwayAdjacencyConstraints(int nProjID, CAirportDatabase* pAirportDB);
protected:
	//data struct for Floyd shortest path algorithm

	std::vector< TaxiwayInSim > m_vTaxiways;   //group of the taxiways;

	std::vector<HoldPositionInSim> m_vHoldShortLineProps; 

	CWingspanAdjacencyConstraintsList* m_pAdjcancyConstraintWingSpan;
};

