#pragma once
#include <set>
#include <map>
#include "VehicleRouteInSim.h"
#include "VehicleRoutePathFinder.h"


class AirsideResource;
class VehicleRouteInSim;
class StandResourceManager;
class VehiclePoolResourceManager;
class VehicleRouteResourceManager;
class CVehicleRouteList;
class VehicleLaneInSim;
class CVehicleRoute;
class VehicleRoadIntersectionInSim;
class CPaxBusParkingInSim;
class VehiclePoolInSim;
class StandInSim;


//class dij_VechileRoute
//{
//public:
//	dij_VechileRoute();
//	DistanceUnit m_dist;	
//	std::vector<VehicleRouteNodePairDist> m_route;
//	bool m_bIsDoneSearch;
//};

//subset of the whole vehicle route graph user defined
class ENGINE_TRANSFER VehicleRouteUserDefineInSim
{
public:
	

	bool Init(VehicleRouteResourceManager* pManager , CVehicleRoute* pRoute);	
	
	inline std::set<VehicleLaneEntry*> GetEntries(){ return  m_vRouteEntries; }
	inline std::set<VehicleLaneExit*> GetExits(){ return m_vRouteExits; }
	
	//bool IsLinked(VehicleRouteNode* pEntry, VehicleRouteNode* pExit);
	bool GetShortestPath(VehicleRouteNode* pEntry, VehicleRouteNode* pExit, std::vector<VehicleRouteNodePairDist>& the_route);

	bool IsStretchExist(VehicleStretchSegmentInSim* pStretchSegEntry, VehicleStretchSegmentInSim* pStretchSegExit);

	bool GetEntryExitNode(AirsideResource * pResFrom, AirsideResource* pResTo,std::vector<VehicleRouteNode* >& vSameNodesF,std::vector<VehicleRouteNode*>& vSameNodesT);

protected:
	std::set<VehicleLaneEntry*> m_vRouteEntries;
	std::set<VehicleLaneExit*> m_vRouteExits;
	//void findShortestPath( VehicleRouteNode* pEntry, VehicleRouteNode* pExit );
	
	VehicleRouteNodeSet m_vRouteNodeSet;
	//std::map< VehicleRouteNodePair, dij_VechileRoute> m_ShortestRoutes;	
	
	std::vector<VehicleStretchSegmentInSim* > m_vRouteSketchSegments;

	bool SearchSibingNode(VehicleRouteNode* pEntry,VehicleRouteNode* pExit, std::vector<VehicleRouteNode*> vRouteNodes);
	bool SearchChildNode(VehicleRouteNode* pEntry,VehicleRouteNode* pExit, std::vector<VehicleRouteNode*> vRouteNodes);

	CVehicleRoutePathFinder m_pathFinder;


};


class ENGINE_TRANSFER VehicleStrethRouteUserDefineInSimList
{
public:
	//class ResStretchInfo
	//{
	//public:
	//	VehicleStretchSegmentInSim* pStretchSeg;
	//	double dDistanceInLane;
	//protected:
	//	ResStretchInfo()
	//	{
	//		pStretchSeg = NULL;
	//		dDistanceInLane = 0.0;
	//	}
	//	~ResStretchInfo()
	//	{

	//	}
	//private:
	//};
	
public:
	VehicleStrethRouteUserDefineInSimList();
	~VehicleStrethRouteUserDefineInSimList();

	void Init(int nPrjID,VehicleRouteResourceManager* pManager);
	int GetCount()const{ return m_vStretchRouteList.size(); }
	VehicleRouteUserDefineInSim * GetItem(int idx);

	bool GetVehicleDefineRoute(AirsideResource * pResFrom, AirsideResource* pResTo, VehicleRouteInSim& resltRoute);
	VehicleRouteNode* GetVehicleRouteNode();


protected:

	//bEntry, true, entry pRes
	//		  false, exit pRes
	VehicleStretchSegmentInSim *GetClosestLane( AirsideResource * pRes, VehicleRouteNodeSet vNodes, bool bEntry );

	VehicleStretchSegmentInSim*	GetClosestLaneToPaxBusParking(CPaxBusParkingInSim * pPaxBusRes);
	VehicleStretchSegmentInSim*	GetClosestLaneToVehiclePool(VehiclePoolInSim * pPoolRes);
	VehicleStretchSegmentInSim*	GetClosestLaneToStand(StandInSim * standres);

protected:
	std::vector<VehicleRouteUserDefineInSim*> m_vStretchRouteList;
	VehicleRouteResourceManager* m_pVehicleRouteResourceManager;
	
	std::map<AirsideResource *, VehicleStretchSegmentInSim*> m_mapResource_StretchSeg;


	




};