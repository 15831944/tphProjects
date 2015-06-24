#pragma once
#include "airsideresource.h"
#include "GroundIntersectionNodeInSim.h"
#include "../../InputAirside/Intersections.h"
#include "../../InputAirside/IntersectionNode.h"
#include "../../InputAirside/IntersectedStretch.h"
#include "VehicleStretchInSim.h"

class VehicleLaneInSim;
class IntersectedStretchInAirport;

class VehicleRoadIntersectionInSim :
	public GroundIntersectionNodeInSim
{
public:
typedef ref_ptr<VehicleRoadIntersectionInSim> RefPtr;

public:
	explicit VehicleRoadIntersectionInSim(const IntersectionNode& nodeInput);
	explicit VehicleRoadIntersectionInSim(Intersections* pIntersect);
	virtual ResourceType GetType()const { return AirsideResource::ResType_VehicleRoadIntersection; }
	virtual CString GetTypeName()const{ return _T("VehicleRoadIntersection"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);
	virtual CPoint2008 GetDistancePoint(double dist)const;

	CPoint2008 m_vCenter;

public:
	//exits 
	//int GetExitCount()const{ return (int)m_vExits.size(); }
	//VehicleLaneExit* GetExit(int idx)const;
	void AddExit(VehicleLaneExit* pExit){ m_vExits.insert(pExit); }

	//entries
	//int GetEntryCount()const{ return (int)m_vEntrys.size(); }
	//VehicleLaneEntry * GetEntry(int idx);
	void AddEntry(VehicleLaneEntry* pEntry){ m_vEntrys.insert(pEntry); }
	
	const VehicleRouteExitSet& getExitSet();
	const VehicleRouteEntrySet& getEntrySet();
	
	void InitIntersectStretch(const IntersectedStretchInAirport& intersectStretchs);
	IntersectionNode& GetNodeInput(){ return m_nodeInput ; }

	int GetEdgeCount()const{ return (int)m_vExitEntryPairs.size(); }
	VehicleRouteNodePairDist GetEdge(int idx)const{ return m_vExitEntryPairs.at(idx); }
	
	void InitGraph();	

	void GetMinMaxHoldDistAtStretch(int nStretchID , DistanceUnit& minDist, DistanceUnit& maxDist);

	bool IsHeadNodeOfStretch(VehicleStretchInSim* pStretch);
	bool IsTrailNodeOfStretch(VehicleStretchInSim * pStretch);

	bool IsIntersectionOf(int nStrtechId1, int nStretchId2);

	bool IsRelateWithLanes( VehicleLaneInSim* pExitLane,VehicleLaneInSim *pEntryLane);
protected:	
	IntersectionNode m_nodeInput;
	std::vector<IntersectedStretch> m_vIntersectStretchs;
	Intersections::RefPtr m_pUserDefineInteresct;
	
	VehicleRouteExitSet m_vExits;
	VehicleRouteEntrySet m_vEntrys;
	
	std::vector<VehicleRouteNodePairDist> m_vExitEntryPairs;
};
