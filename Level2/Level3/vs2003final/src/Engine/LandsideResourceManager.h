#pragma once
#include "LandsideStretchInSim.h"
#include "LandsideCurbsideInSim.h"
#include "TrafficObjectmanagerInSim.h"

class LandsideLeadToInfo
{
public:
	LandsideLeadToInfo(){  bDirectTo = false; distDirectTo = 0 ; bChangeLaneTo = false; distChageLane = 0; }
	bool bDirectTo;
	DistanceUnit distDirectTo;
	bool bChangeLaneTo;
	DistanceUnit distChageLane;
};



//route graph
class CBoostPathFinder;
class LandsideRouteGraph
{
public:
	LandsideRouteGraph();

	void BuildGraph(LandsideResourceManager* allResource);

	LandsideLaneNodeInSim* GetNode(int iIndex)const{ return mNodes[iIndex]; }
	
	void AddNode(LandsideLaneNodeInSim* pNode);
	void AddEdge(LandsideLaneNodeInSim* pNode1, LandsideLaneNodeInSim*pNode2, DistanceUnit dist );

	//find route
	bool FindRouteResToRes(  LandsideResourceInSim * pResFrom, LandsideResourceInSim* pResTo, LandsideLaneNodeList& routeRet );
	bool FindRouteNodeToRes( LandsideLaneNodeInSim* pFromNode, LandsideResourceInSim* pDestRes, LandsideLaneNodeList& routeRet);
	bool FindRouteResToNode ( LandsideResourceInSim* pFromRes , LandsideLaneNodeInSim* pDsetNode, LandsideLaneNodeList& routeRet);
	bool FindRouteStretchPosToRes(LandsideLaneInSim* plane, DistanceUnit distInLane, LandsideResourceInSim* pRes, LandsideLaneNodeList& routeRet);
	bool FindRouteResToObject( LandsideResourceInSim * pResFrom, LandsideLayoutObjectInSim* pResTo, LandsideLaneNodeList& routeRet );

	
	//node lead to info , change lane
	typedef std::pair<LandsideLaneNodeInSim* , LandsideLayoutObjectInSim*> KeyNodeObject;
	typedef std::map<KeyNodeObject,LandsideLeadToInfo> NodeLeadToMap;

	LandsideLeadToInfo& getLeadToInfo(LandsideLaneNodeInSim* pNode, LandsideLayoutObjectInSim* pDest);
	
	//get nodes exit from the resource
	void GetExitResourceNodes(LandsideResourceInSim* pResFrom, LandsideLaneNodeList& outNodes);
protected:
	void Clear();

	bool _FindRouteNodeToNode(  LandsideLaneNodeInSim* pNodeF,LandsideLaneNodeInSim* pNodeT, LandsideLaneNodeList& routeRet,DistanceUnit& dist );	
	bool _FindRouteNodesToNodes( const LandsideLaneNodeList& pNodeF,const LandsideLaneNodeList& pNodeT, LandsideLaneNodeList& routeRet, DistanceUnit& dist);
	
	
	void _GetEntryExitNode( LandsideResourceInSim * pResFrom, LandsideResourceInSim* pResTo,LandsideLaneNodeList& vFromNodes,LandsideLaneNodeList& vToNodes );
	//void _GetDestNode( LandsideLayoutObjectInSim* pDest, LandsideLaneNodeList& vDestNodes );
	void _GetEntryExitNodeByObject( LandsideResourceInSim * pResFrom, LandsideLayoutObjectInSim* pDest,LandsideLaneNodeList& vFromNodes,LandsideLaneNodeList& vToNodes );

	struct OutEdge
	{
		OutEdge(){ pNode = NULL; distToNode=0; }
		LandsideLaneNodeInSim* pNode;
		DistanceUnit distToNode;
	};
	typedef std::vector<OutEdge> OutEdgeList;
	OutEdgeList _getOutEdge(LandsideLaneNodeInSim* pNode)const;

protected:
	CBoostPathFinder * mpPathFinder;
	NodeLeadToMap m_leadtoMaps;   //lead to informations	
	LandsideLaneNodeList mNodes;
};


//////////////////////////////////////////////////////////////////////////
class InputLandside;
class LandsideFacilityLayoutObject;
class LandsideParkingLotInSim;
class LaneNode;
class LandsideLaneNodeInSim;
class LandsideRoundaboutInSim;
class LandsideBusStationInSim;
class StretchSpeedConstraintList;
class CFacilityBehaviorList;
class LandsideStretchSegmentInSim;
class LandsideDecisionPointInSim;

class LandsideResourceManager
{
public:
	LandsideResourceManager();
	~LandsideResourceManager();
	void Init(InputLandside* pInput,CFacilityBehaviorList* pBehavior);
	void Clear();


	LandsideRouteGraph mRouteGraph;

	LandsideStretchInSim* getStretchByID(int id);
	LandsideParkingLotInSim* getParkingLotByID(int id);
	LandsideBusStationInSim* getBusStationByID(int id);


	CTrafficObjectmanagerInSim m_vTrafficObjectList;;

	LandsideLaneNodeInSim *GetlaneNode(const LaneNode& node);

	LandsideLayoutObjectInSim* getLayoutObjectInSim(int nObjID);
	LandsideLayoutObjectInSim* getLayoutObjectInSim(const ALTObjectID& name);

	LandsideCurbSideInSim* getRandomCurb(const ALTObjectID& id);
	LandsideBusStationInSim* getRandomBusStation(const ALTObjectID& id);
	LandsideTaxiQueueInSim* getRandomTaxiQueue(const ALTObjectID& id);
	LandsideParkingLotInSim* getRandomParkinglot(const ALTObjectID& id);

	void FlushOnResourcePax(const ElapsedTime& endTime);

	CWalkwayInSim *GetWalkwayInSim(const ALTObjectID& walkWayID) const;
	//
	void GetWalkWayListAtLevel(std::vector<CWalkwayInSim *>& vWakwayInSim, double dLevel) const;

	CCrossWalkInSim* GetCrosswalk(LandsideCrosswalk* pCrossWalk);

public:
	void CreatInSimObject(LandsideFacilityLayoutObject* pObj,bool bLeftDrive);
	std::vector<LandsideStretchInSim*> m_vStretches;
	std::vector<LandsideIntersectionInSim*> m_Intersections;
	std::vector<LandsideCurbSideInSim*> m_Curbsides;
	std::vector<LandsideBusStationInSim*> m_BusStation;
	std::vector<LandsideParkingLotInSim*> m_ParkingLots;
	std::vector<LandsideExternalNodeInSim*> m_extNodes;
	std::vector<LandsideRoundaboutInSim*> m_vRoundAbouts;
	std::vector<LandsideStretchSegmentInSim*> m_vStretchSegs;
	std::vector<LandsideTaxiQueueInSim*> m_vTaxiQueues;
	std::vector<LandsideTaxiPoolInSim*> m_vTaxiPools;
	std::vector<LandsideDecisionPointInSim*> m_vDecidePoints;


	//all objects
	std::vector<LandsideLayoutObjectInSim*> m_vAllObjects; 

};
