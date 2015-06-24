#pragma once
#include "AirsideResource.h"
#include "IntersectionNodeInSim.h"
#include "AirsideEngineCommon.h"

#include "./RouteResource.h"
class FlightGroundRouteSegInSim;
class VehicleLaneInSim;
//class TaxiwaySegDirectionLock;
class ENGINE_TRANSFER FlightGroundRouteDirectSegInSim :  public RouteDirPath
{
	DynamicClassStatic(FlightGroundRouteDirectSegInSim)	
	RegisterInherit(FlightGroundRouteDirectSegInSim,RouteDirPath)
public:
	FlightGroundRouteDirectSegInSim(FlightGroundRouteSegInSim *pSeg, FlightGroundRouteSegDirection dir );
	virtual ~FlightGroundRouteDirectSegInSim();

	virtual RouteResource* getRouteResource()const;

	//get resource at altobject id
	virtual int  GetObjectID()const=0;

public:
	typedef ref_ptr<FlightGroundRouteDirectSegInSim> RefPtr;


	virtual CPoint2008 GetDistancePoint(double dist)const;

	double GetEndDist()const;

	double GetExitHoldDist()const;//hold enter exit node at the segment
	double GetEntryHoldDist()const; // hold exit entry node 
	int GetEntryHoldID()const;
	int GetExitHoldID()const;

	IntersectionNodeInSim * GetEntryNode()const;
	IntersectionNodeInSim * GetExitNode()const;

	DistanceUnit GetEnterPosInTaxiway()const;
	DistanceUnit GetExitPosInTaxiway()const;

	bool IsPositiveDir()const{ return m_dir == PositiveDirection; }	
	FlightGroundRouteSegDirection GetDir()const{ return m_dir; }

	FlightGroundRouteSegInSim * GetRouteSegInSim();

	virtual FlightGroundRouteDirectSegInSim  * GetOppositeSegment();
	virtual const FlightGroundRouteDirectSegInSim * GetOppositeSegment()const;

	bool IsDirectSegementOnRight(FlightGroundRouteDirectSegInSim* pDirectSegment);
	//bool IsFlightLockedResource(AirsideFlightInSim* pFlight);


	//virtual bool IsDirectionActive()const;
	//virtual bool TryLockDir(AirsideFlightInSim* pFlight);
	//virtual bool GetDirLock(AirsideFlightInSim* pFlight);
	//virtual void ReleaseDirLock(AirsideFlightInSim* pFlight, const ElapsedTime& tTime);

	//virtual TaxiwaySegDirectionLock* GetDirectionLock();

	//first point pos in taxiway

	DistanceUnit GetPosInTaxiway(DistanceUnit distInSeg)const;
	CPath2008 GetPath()const;
	DistanceUnit GetLength()const;

	// get all the flights now in path
	// vecFlights - the return vector holding all in path flights' pointer
	// return values, the flights' number
	// NOTE: it will not clear the vecFlights before adding new items
	int GetInPathAirsideFlightList(std::vector<AirsideFlightInSim* >& vecFlights) const;

protected:	
	FlightGroundRouteSegDirection m_dir;
	FlightGroundRouteSegInSim * m_pSeg;
	CPath2008 m_dirPath;
	DistanceUnit m_distance;

//	std::vector<AirsideFlightInSim*> m_vLockFlights;

private:
};
typedef std::vector<FlightGroundRouteDirectSegInSim *> FlightGroundRouteDirectSegList;

class FlightGroundRouteInSim;
class ENGINE_TRANSFER FlightGroundRouteSegInSim : public RouteResource
{
	DynamicClassStatic(FlightGroundRouteSegInSim)
	RegisterInherit(FlightGroundRouteSegInSim,RouteResource)
public:
	FlightGroundRouteSegInSim(IntersectionNodeInSim * pNodeFrom , IntersectionNodeInSim * pNodeTo, FlightGroundRouteInSim * pFlightRoute);
	virtual ~FlightGroundRouteSegInSim(void);

	virtual bool IsActiveFor(ARCMobileElement* pmob,const RouteDirPath* pPath)const;
	virtual bool bMayHaveConflict(const RouteDirPath *pPath)const;
	bool bLocked(ARCMobileElement* pmob,const RouteDirPath* pPath)const;

protected:
	bool _IsActiveForFlight(AirsideFlightInSim* pFlight, const FlightGroundRouteDirectSegInSim* pPath)const;

	

public:
	//get distance between to nodes in this taxiway
	DistanceUnit GetNodesDistance()const;

	IntersectionNodeInSim * GetNode1()const{ return (IntersectionNodeInSim*)m_pNode1.get(); }
	IntersectionNodeInSim * GetNode2()const{ return (IntersectionNodeInSim*)m_pNode2.get(); }


	void SetNode1(IntersectionNodeInSim* pNode){ m_pNode1 = pNode; }
	void SetNode2(IntersectionNodeInSim * pNode){ m_pNode2 = pNode; }


	DistanceUnit GetHold1Offset()const{ return m_dHoldOffsetNode1;}
	DistanceUnit GetHold2Offset()const{ return m_dHoldOffsetNode2 ; }


	void SetHold1Offset(DistanceUnit doffset){ m_dHoldOffsetNode1 = doffset; }
	void SetHold2Offset(DistanceUnit doffset){ m_dHoldOffsetNode2 = doffset; }

	//hold offset for node1
	void SetUserHold1Offset(DistanceUnit doffset,int nHoldId){ m_dHoldOffsetNode1Ext = doffset; m_nHoldId1 = nHoldId; }
	//hold offset for node2
	void SetUserHold2Offset(DistanceUnit doffset,int nHoldId){ m_dHoldOffsetNode2Ext = doffset; m_nHoldId2 = nHoldId; }


	DistanceUnit GetUserHold1Offset()const{ return m_dHoldOffsetNode1Ext; }
	DistanceUnit GetUserHold2Offset()const{ return m_dHoldOffsetNode2Ext; }
	int GetHold1Id()const{ return m_nHoldId1; }
	int GetHold2Id()const{ return m_nHoldId2; }

	virtual double GetNode1Pos()const;
	virtual double GetNode2Pos()const;

	virtual FlightGroundRouteDirectSegInSim * GetPositiveDirSeg()const{ return (FlightGroundRouteDirectSegInSim *)m_positiveDirSeg.get(); }
	virtual FlightGroundRouteDirectSegInSim * GetNegativeDirSeg()const{ return (FlightGroundRouteDirectSegInSim *)m_negativeDirSeg.get(); }

	virtual bool CanServeFlight(AirsideFlightInSim *pFlight);
	void InitLanes();
	FlightGroundRouteInSim*	GetGroundRoute(){ return m_pFlightRouteInSim; }

	VehicleLaneInSim* GetLane(int idx);
	VehicleLaneInSim* GetExitLane(IntersectionNodeInSim* pNode);
	bool GetExitLane(IntersectionNodeInSim* pNode,std::vector<VehicleLaneInSim*>& vLinkedLane);
	int GetLaneCount();

	void addIntersectionLane(VehicleLaneInSim* pLane);
	int getIntersectLaneCount()const{ return (int)m_vIntersectLanes.size(); }
	VehicleLaneInSim* getIntersectLane(int idx){ return m_vIntersectLanes[idx]; }

	bool IsPointInSeg(const CPoint2008& pos);
	DistanceUnit getDistInSegAccordToExitNode(const CPoint2008& pos, const IntersectionNodeInSim* pExitNode);

protected:

	FlightGroundRouteDirectSegInSim::RefPtr m_positiveDirSeg;
	FlightGroundRouteDirectSegInSim::RefPtr m_negativeDirSeg;

	std::vector<VehicleLaneInSim*> m_vLanes;

	IntersectionNodeInSim::RefPtr m_pNode1;
	IntersectionNodeInSim::RefPtr m_pNode2;

	DistanceUnit m_dHoldOffsetNode1;
	DistanceUnit m_dHoldOffsetNode2;
	int m_nHoldId1; // node1  hold in seg
	int m_nHoldId2; // node2 hold in seg
	DistanceUnit m_dHoldOffsetNode1Ext; //for user defined hold short line as for runway and taxiway
	DistanceUnit m_dHoldOffsetNode2Ext; //for user defined hold short line

	FlightGroundRouteInSim * m_pFlightRouteInSim;
	std::vector<VehicleLaneInSim*> m_vIntersectLanes;
};

class ENGINE_TRANSFER FlightGroundRouteInSim
{
public:
	FlightGroundRouteInSim(double dWidth, int nDBId, const CPath2008& path);
	virtual ~FlightGroundRouteInSim();

	double getRouteWidth() const;
	int GetId() const;
	const CPath2008& GetPath();

protected:
	void GetMinMaxFilletDistToNode(IntersectionNodeInSim* pNodeInSim,const int& nObjID, DistanceUnit& dMin, DistanceUnit& dMax );



protected:
	const static DefaultHoldOffset = 3200;

	double m_dWidth;
	int m_nDBId;
	CPath2008 m_path;
};
