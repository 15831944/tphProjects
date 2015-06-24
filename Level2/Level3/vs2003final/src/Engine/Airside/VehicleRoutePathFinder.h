#pragma once


#include "VehicleStretchInSim.h"


//////////////////////////////////////////////////////////////////////////
//Vehicle Route Finder
//
//
//////////////////////////////////////////////////////////////////////////
class CBoostPathFinder;

class VehicleRoadIntersectionInSim;
class VehicleStretchInSim;
class VehicleRouteNode;
class AirsideResource;

class CVehicleRoutePathFinder
{
public:
	CVehicleRoutePathFinder();
	~CVehicleRoutePathFinder();
	void Init(const std::vector<VehicleStretchInSim*>& m_vStretches,const std::vector<VehicleRoadIntersectionInSim*>& m_vIntersections);
	void Init(const std::set<VehicleStretchSegmentInSim*>& vStretches,const std::vector<VehicleRoadIntersectionInSim*>& vIntersections);

	bool GetRoute(  VehicleRouteNode* pNodeF,VehicleRouteNode* pNodeT, std::vector<VehicleRouteNodePairDist>& routeRet );	
	bool GetRoute(   AirsideResource * pResFrom, AirsideResource* pResTo, std::vector<VehicleRouteNodePairDist>& routeRet );

	VehicleRouteNode* GetNode(int idx)const;
	int GetNodeIndex(VehicleRouteNode* pNode)const;
	int AddNode(VehicleRouteNode* pNode);
	bool GetEntryExitNode(AirsideResource * pResFrom, AirsideResource* pResTo,std::vector<VehicleRouteNode* >& vSameNodesF,std::vector<VehicleRouteNode*>& vSameNodesT);

protected:
	CBoostPathFinder * mpPathFinder;

	std::vector<VehicleStretchInSim*> m_vStretches;
	std::vector<VehicleRoadIntersectionInSim*> m_vIntersections;
	std::vector<VehicleRouteNode*> mNodes;
};