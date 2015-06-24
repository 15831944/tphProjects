#pragma  once

#include "AirsideResource.h"
#include "GroundIntersectionNodeInSim.h"
#include "../../InputAirside/Stretch.h"
//#include "./LaneStandIntersectionInSim.h"
#include <set>
#include ".\RouteResource.h"



class VehicleStretchSegmentInSim;

class VehicleLaneInSim;
class IntersectionNodeInSim;
class FlightGroundRouteSegInSim;
class ARCVector2;

class ResourceDesc;

class ENGINE_TRANSFER LaneFltGroundRouteIntersectionInSim : public GroundIntersectionNodeInSim, public RouteResource 
{
friend class VehicleLaneInSim;
public:
	typedef ref_ptr<LaneFltGroundRouteIntersectionInSim> RefPtr;

	LaneFltGroundRouteIntersectionInSim();
	LaneFltGroundRouteIntersectionInSim(VehicleLaneInSim * plane , FlightGroundRouteSegInSim *pTaxiSeg , int idx);
	void SetHold1(DistanceUnit dist){ m_holdDist1 = dist; }
	void SetHold2(DistanceUnit dist){ m_holdDist2 = dist; }
	DistanceUnit GetHold1()const{ return m_holdDist1; }
	DistanceUnit GetHold2()const{ return m_holdDist2; }

	bool MakeValid();//for data not valid, which only set one hold

	DistanceUnit GetIntersectDistAtLane()const{ return m_intersectDistAtLane; }
	DistanceUnit GetIntersectDistAtTaxiwaySeg()const{ return m_intersectDistAtTaxiway; }

	void SetIntersectDistAtLane(const DistanceUnit& dist);
	void SetIntersectDistAtTaxiwaySeg(const DistanceUnit& dist){ m_intersectDistAtTaxiway = dist; }

	FlightGroundRouteSegInSim * GetFltRouteSegment()const{ return m_pFltRouteSeg; }

	AirsideFlightInSim * GetLatestFlightInSight(const DistanceUnit& radius,const ElapsedTime&tTime)const;

	bool bMobileInResource(ARCMobileElement* pmob)const;

	//get flight dist away form this Intersection return false if not in the route
	bool GetFlightDistToHere(AirsideFlightInSim* pFlight,const ElapsedTime& t, double& dist, ARCVector2& dir)const;
	//get flight dist 

	CPoint2008 GetPosition()const;

	//be lock for the mob
	bool IsLockedfor(ARCMobileElement* pMob)const;
	void OnFlightEnter(AirsideFlightInSim* pFlt,const ElapsedTime& t);
	void OnFlightExit(AirsideFlightInSim* pFlt,const ElapsedTime& t);
	virtual bool IsActiveFor(ARCMobileElement* pmob,const RouteDirPath* pPath)const{ return !IsLockedfor(pmob); }
	virtual bool bMayHaveConflict(const RouteDirPath *pPath)const{ return true; }
	virtual bool bLocked(ARCMobileElement* pmob,const RouteDirPath* pPath)const{ return IsLockedfor(pmob); };

	virtual AirsideResource::ResourceType GetType()const;
	virtual CString GetTypeName()const;
	virtual CString PrintResource()const;

	virtual CPoint2008 GetDistancePoint(double dist)const; 
	virtual void getDesc(ResourceDesc& resDesc);

protected:
	DistanceUnit m_holdDist1;
	DistanceUnit m_holdDist2;
	DistanceUnit m_intersectDistAtLane;
	DistanceUnit m_intersectDistAtTaxiway;
	std::set<ARCMobileElement*> mvLockMobs;

	VehicleLaneInSim  * m_pLane;
	FlightGroundRouteSegInSim * m_pFltRouteSeg;
	int m_idx;
};

//node of the vehicle route graph
class ENGINE_TRANSFER VehicleRouteNode
{
public:
	enum NODETYPE{ LANE_ENTRY, LANE_EXIT };
	virtual NODETYPE GetType()const = 0;
	virtual AirsideResource* GetOrignResource()const =0;
	virtual AirsideResource* GetDestResource()const =0;
	virtual CPoint2008 GetPosition()const =0 ;
	virtual VehicleLaneInSim* GetLane() const = 0;
};
typedef std::pair<VehicleRouteNode*, VehicleRouteNode*> VehicleRouteNodePair;
typedef std::set<VehicleRouteNode*> VehicleRouteNodeSet;
//edge of the two node
class ENGINE_TRANSFER VehicleRouteNodePairDist
{
public:	
	VehicleRouteNodePairDist(VehicleRouteNode* pNodeF, VehicleRouteNode* pNodeT, DistanceUnit dDist)
	{
		m_pNodeFrom = pNodeF;
		m_pNodeTo = pNodeT;
		m_dist = dDist; 		
	}
	DistanceUnit GetDistance()const{ return m_dist; }
	VehicleRouteNode* GetNodeFrom()const{ return m_pNodeFrom; }
	VehicleRouteNode* GetNodeTo()const{ return m_pNodeTo; }
protected:
	VehicleRouteNode * m_pNodeFrom;
	VehicleRouteNode * m_pNodeTo;
	DistanceUnit m_dist;   //graph weight	
};


//exit to other resource 
class ENGINE_TRANSFER VehicleLaneExit : public VehicleRouteNode
{
public:
	VehicleLaneExit(VehicleLaneInSim* pLane, AirsideResource * pDestRes , DistanceUnit atDist)
	{ 
		m_pDestResource = pDestRes; 
		m_atDist = atDist;  
		m_pLane = pLane;
	}

	virtual NODETYPE GetType()const{ return LANE_EXIT ; }
	AirsideResource* GetOrignResource()const{ return (AirsideResource*)m_pLane; }
	AirsideResource* GetDestResource()const{ return m_pDestResource; }
	DistanceUnit GetDistInLane()const{ return m_atDist; }
	virtual VehicleLaneInSim* GetLane()const{ return m_pLane; }

	CPoint2008 GetPosition()const; 

protected:
	DistanceUnit m_atDist;
	VehicleLaneInSim * m_pLane;
	AirsideResource * m_pDestResource;

};
//entry from other resource to the lane
class ENGINE_TRANSFER VehicleLaneEntry : public VehicleRouteNode
{
public:

	VehicleLaneEntry(VehicleLaneInSim* pLane ,AirsideResource* pOrignRes, DistanceUnit atDist){
		m_atDist = atDist;
		m_pLane = pLane;
		m_pOrignResource = pOrignRes;
	}

	virtual NODETYPE GetType()const{ return LANE_ENTRY ; }

	AirsideResource* GetOrignResource()const{ return m_pOrignResource; } 
	AirsideResource* GetDestResource()const{ return (AirsideResource*)m_pLane; }
	DistanceUnit GetDistInLane()const{ return m_atDist; }
	virtual VehicleLaneInSim* GetLane()const{ return m_pLane; }

	CPoint2008 GetPosition()const; 

protected:
	
	DistanceUnit m_atDist;
	VehicleLaneInSim * m_pLane;
	AirsideResource* m_pOrignResource;
};

//entry exit pair
typedef std::set<VehicleLaneEntry*> VehicleRouteEntrySet;
typedef std::set<VehicleLaneExit*> VehicleRouteExitSet;
//
class ENGINE_TRANSFER VehicleLaneInSim : public RouteDirPath
{
public: // Resource 
	typedef ref_ptr<VehicleLaneInSim> RefPtr;

	virtual CPoint2008 GetDistancePoint(double dist)const;

public: 
	VehicleLaneInSim(RouteResource* pParRoute, int laneNo);
	virtual~VehicleLaneInSim();

	void SetPath(const CPath2008& path){ m_path = path; m_dLength = m_path.GetTotalLength(); }
	const CPath2008& GetPath()const{ return m_path; }
	DistanceUnit GetLength()const{ return m_dLength; }


	//exits 
	int GetExitCount()const{ return (int)m_vExits.size(); }
	VehicleLaneExit* GetExit(int idx)const;
	void AddExit(VehicleLaneExit* pExit){ m_vExits.push_back(pExit); }
	void SetEndExit(VehicleLaneExit* pExit){ m_pEndExit = pExit; }
	VehicleLaneExit* GetEndExit()const{ return m_pEndExit; }
	VehicleLaneExit* GetFirstExit();
	
	//entries
	int GetEntryCount()const{ return (int)m_vEntrys.size(); }
	VehicleLaneEntry * GetEntry(int idx);
	void AddEntry(VehicleLaneEntry* pEntry){ m_vEntrys.push_back(pEntry); }
	void SetBeginEntry(VehicleLaneEntry* pEntry){ m_pBeginEntry = pEntry; }
	VehicleLaneEntry * GetBeginEntry()const{ return m_pBeginEntry; }

	//Intersections with taxiway
	int GetTaxiwayIntersectionCount()const{ return (int)m_vTaxiwayIntersections.size(); }
	LaneFltGroundRouteIntersectionInSim* FltGroundRouteIntersectionAt(int idx);
	const LaneFltGroundRouteIntersectionInSim* FltGroundRouteIntersectionAt(int idx)const;
	LaneFltGroundRouteIntersectionInSim* GetFltGroundRouteIntersection(FlightGroundRouteSegInSim * pSeg, int idx);


	//get the Vehicle Ahead from the dist
	AirsideVehicleInSim * GetVehicleAhead(DistanceUnit dist);

	//get first intersection after dist;
	int GetFirstTaxiwayIntersection(DistanceUnit dist)const;

	//Get the conflict flight int the giving intersection , if giving intersection node has conflict intersection, then the conflict intersection will be in concern
	AirsideFlightInSim * GetConflictFlightArroundIntersection(const LaneFltGroundRouteIntersectionInSim& curIntersection,const DistanceUnit& radius,const ElapsedTime&tTime);
	
	//find the closest intersection ahead giving intersection
	int FindConflictIntersectionAhead(int nCurIndex);
	
	//get first intersection after dist;
	int GetFirstStandIntersection(DistanceUnit dist)const;
	
	//make TaxiwayIntersections in dist order 
	void SortFltGroundRouteIntersections();
	int GetLaneNO()const{ return m_nLaneNo; }
	virtual RouteResource* getRouteResource()const;

protected:
	CPath2008 m_path;	
	DistanceUnit m_dLength;
	int m_nLaneNo;	

	std::vector<VehicleLaneExit*> m_vExits;
	std::vector<VehicleLaneEntry*> m_vEntrys;
	VehicleLaneEntry * m_pBeginEntry;
	VehicleLaneExit * m_pEndExit;

	std::vector<LaneFltGroundRouteIntersectionInSim::RefPtr> m_vTaxiwayIntersections;
	RouteResource* m_pParRoute;
	
};


class VehicleStretchInSim;
class VehicleRoadIntersectionInSim;
class ENGINE_TRANSFER VehicleStretchSegmentInSim: public RouteResource
{
public:	

	VehicleStretchSegmentInSim(VehicleStretchInSim*pStretch, DistanceUnit distF, DistanceUnit distT );
	~VehicleStretchSegmentInSim();
	
	//init the graph  edges of the Stretch
	void InitLanes();
	void InitGraph();

	int GetLaneCount()const{ return (int)m_vLanes.size(); }
	VehicleLaneInSim * GetLane(int idx)const;	
	
	VehicleStretchInSim* GetStretch()const{ return m_pStretchInSim; }
	
	void SetNode( VehicleRoadIntersectionInSim* pNode1, VehicleRoadIntersectionInSim* pNode2 ){
		m_pNode1 = pNode1;
		m_pNode2 = pNode2;
	}
	VehicleRoadIntersectionInSim* GetNode1(){ return m_pNode1; }
	VehicleRoadIntersectionInSim* GetNode2(){ return m_pNode2; }

	int GetEdgeCount()const{ return (int)m_vEntryExitPairs.size(); }
	VehicleRouteNodePairDist GetEdge(int idx)const{ return m_vEntryExitPairs.at(idx); }

	virtual bool IsActiveFor(ARCMobileElement* pmob,const RouteDirPath* pPath)const;
	virtual bool bMayHaveConflict(const RouteDirPath *pPath)const;
	virtual bool bLocked(ARCMobileElement* pmob,const RouteDirPath* pPath)const;
	virtual bool bMobileInResource(ARCMobileElement* pmob)const;

protected:
	DistanceUnit m_dDistFromAtStretch;
	DistanceUnit m_dDistToAtStretch;
	CPath2008 m_path;
	VehicleRoadIntersectionInSim* m_pNode1;
	VehicleRoadIntersectionInSim* m_pNode2; 

	std::vector<VehicleLaneInSim*> m_vLanes;

	VehicleStretchInSim* m_pStretchInSim;
	//graph data
	std::vector<VehicleRouteNodePairDist> m_vEntryExitPairs;	
};

class IntersectionNodeInSimList;
class VehicleRoadIntersectionInSim;

class ENGINE_TRANSFER VehicleStretchInSim
{
public:
	VehicleStretchInSim(Stretch* pStretch);
	void InitSegments(std::vector<VehicleRoadIntersectionInSim*> & NodeList);
	~VehicleStretchInSim();
	Stretch* GetInput()const{ return (Stretch*)m_stretchInput.get(); } 
	int GetSegmentCount()const{ return m_vSegments.size(); }
	VehicleStretchSegmentInSim* GetSegment(int idx)const;

	std::vector<VehicleStretchSegmentInSim*> GetSegments(VehicleRoadIntersectionInSim*fromNode, VehicleRoadIntersectionInSim* toNode );

protected:
	std::vector<VehicleStretchSegmentInSim*> m_vSegments;
	Stretch::RefPtr m_stretchInput;

};