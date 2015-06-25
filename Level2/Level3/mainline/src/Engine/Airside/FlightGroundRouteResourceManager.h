#pragma once
#include "ResourceManager.h"
#include "TaxiwayResource.h"
#include "FindFlightShortestPath.h"
#include "RunwayInSim.h"
#include "FlightGroundRouteFinder.h"
class IntersectionNodeInSim;
class AirsideFlightInSim;
class TemporaryParkingInSim;
class TaxiwayResourceManager;
class RunwayResourceManager;



class FlightGroundRouteResourceManager :
	public ResourceManager
{
public:
	FlightGroundRouteResourceManager(void);
	~FlightGroundRouteResourceManager(void);
public:
	void Init(TemporaryParkingInSim* tempParkingsegs  );
	//Get path from source to dest,
	//PRI : priority , 0 the shortest , 1 the second shortest ...,  pathNodeList:(pSrc, node1, node 2, pDest)
	bool GetRoute( IntersectionNodeInSim * pSrc, IntersectionNodeInSim * pDest, AirsideFlightInSim *pFlight,int nPRI,
		FlightGroundRouteDirectSegList& path, double& dPathWeight, FlightGroundRouteFinderStrategy* pStrategy = NULL );
	bool GetRoute(FlightGroundRouteDirectSegInSim* pSeg, IntersectionNodeInSim* pDest, AirsideFlightInSim* pFlight,
		FlightGroundRouteDirectSegList& path, double& dPathWeight, FlightGroundRouteFinderStrategy* pStrategy = NULL );

	void SetResourceManager(TaxiwayResourceManager *pTaxiwayResourceManager, RunwayResourceManager * pRunwayResourceManager);


	FlightGroundRouteDirectSegList GetAllLinkedDirectSegments( IntersectionNodeInSim * pNode );
	FlightGroundRouteDirectSegList GetLinkedDirectSegments( IntersectionNodeInSim * pNode );

protected:
	void SetParkingSegments(TemporaryParkingInSim* tempParkingsegs);


protected:
	CFindFlightShortestPath m_pathFinder;
	//CFindFlightShortestPath m_taxiwayFinder;
	CFlightGroundRouteFinder m_routeFinder;
	std::vector< FlightGroundRouteSegInSim*> m_vParkingSegs;
	TaxiwayResourceManager *m_pTaxiwayResourceManager;
	RunwayResourceManager *m_pRunwayResourceManager;


	CFlightGroundRouteFinder* _getSegPath(FlightGroundRouteDirectSegInSim*pSeg);
	void _addSegPath(FlightGroundRouteDirectSegInSim* pSeg, CFlightGroundRouteFinder* pPath);
	typedef std::map<FlightGroundRouteDirectSegInSim*, CFlightGroundRouteFinder*> SegPathMap;
	SegPathMap m_segPathFinders;
};
