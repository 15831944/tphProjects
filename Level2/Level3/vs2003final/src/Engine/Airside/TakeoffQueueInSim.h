#pragma once


class RunwayExitInSim;
class AirportResourceManager; 
class TakeoffQueuePosi;
class TaxiwayDirectSegInSim;
class LogicRunwayInSim;
class IntersectionNodeInSim;
class ClearanceItem;
class Clearance;

#include "TaxiwayResource.h"
#include "TaxiRouteInSim.h"

class ENGINE_TRANSFER TakeoffQueueInSim
{
public:
	TakeoffQueueInSim(void);
	~TakeoffQueueInSim(void);

	void Init(TakeoffQueuePosi* pQueueInput, AirportResourceManager * pAirportRes );
	
	void AddDirectSeg(FlightGroundRouteDirectSegInSim * pDirSeg);
	
	
	LogicRunwayInSim * GetRunway(){ return m_pRunway; }
	IntersectionNodeInSim * GetNodeOnRunway(){ return m_pNodeOnRunway; }
	FlightGroundRouteDirectSegList GetTaxiSegments()const { return m_vTaxiSegs; }

	int GetInQueueFlights(std::vector<AirsideFlightInSim*> vFlights )const;
	ElapsedTime GetInQueueTime(AirsideFlightInSim * pFlight,const ElapsedTime& curTime)const;   // get in queue flights average time 

	bool FindClearanceInConcern(AirsideFlightInSim * pFlight,ClearanceItem& lastItem, Clearance& newClearance);

	//get a taxi route from entry node to the take off position //choose the shortest and the lest flights
	TaxiRouteInSim GetTaxiRoute(IntersectionNodeInSim * entryNode);

	bool IsHaveSegment(FlightGroundRouteDirectSegInSim * pSeg)const;

	int GetEntryNodeCount()const{ return m_vEntryNodes.size();}
	IntersectionNodeInSim * GetEntryNode(int idx)const{ return m_vEntryNodes[idx]; }
protected: 
	
	std::vector<IntersectionNodeInSim* > m_vEntryNodes;
	FlightGroundRouteDirectSegList m_vTaxiSegs;
	LogicRunwayInSim * m_pRunway;
	IntersectionNodeInSim * m_pNodeOnRunway;
};


class ENGINE_TRANSFER TakeoffQueuesManager
{
public:
	TakeoffQueuesManager(){}
	~TakeoffQueuesManager();
	int GetQueueCount()const{ return m_vTakeoffQueues.size(); }
	TakeoffQueueInSim * GetQueue(int idx)const{ return m_vTakeoffQueues[idx]; }

	TakeoffQueueInSim * GetRunwayExitQueue(RunwayExitInSim * pRunwayExit);


	void Init(int nPrjId, AirportResourceManager * pAirportRes);

protected:
	std::vector<TakeoffQueueInSim* > m_vTakeoffQueues;
};


class ENGINE_TRANSFER TakeoffQueueRouteFinder
{
public:
	TakeoffQueueRouteFinder(TakeoffQueueInSim* pQueue){ m_pQueue = pQueue; }
	void InitPathInfo();
	bool FindRoute(IntersectionNodeInSim * pNodeFrom, IntersectionNodeInSim* pNodeTo, TaxiRouteInSim& route);

protected:
	class dijRouteInQueue
	{
	public:
		dijRouteInQueue();
		FlightGroundRouteDirectSegList m_vRoutes;
		bool m_bIsDoneSearch;    //dij shortest path flag
		double GetWeight()const;		
	};

	typedef std::pair<IntersectionNodeInSim * , IntersectionNodeInSim * > NodePair;	
	typedef std::set<IntersectionNodeInSim*> IntersectionNodesSet;	

	IntersectionNodesSet m_vNodeList;
	std::map<NodePair,dijRouteInQueue> m_dijRouteMap;

private:
	TakeoffQueueInSim * m_pQueue;
};