#pragma once

#include "AirsideResource.h"
#include "GroundIntersectionNodeInSim.h"
#include "../../InputAirside/IntersectionNode.h"
#include "../../InputAirside/IntersectionNodesInAirport.h"

#include "Clearance.h"
#include "common/ARCMathCommon.h"
#include "../../InputAirside/FiletTaxiway.h"


class AirsideFlightInSim;
class TaxiwayDirectSegInSim;
class LogicRunwayInSim;
class FlightGroundRouteDirectSegInSim;
class RunwayDirectSegInSim;
#include ".\RouteResource.h"

// Taxiway Intersections With Other Resource
class IntersectionNodeInSim : public GroundIntersectionNodeInSim, public RouteResource
{

public:
	typedef ref_ptr<IntersectionNodeInSim> RefPtr;

	explicit IntersectionNodeInSim(const IntersectionNode& node);

public:
	int GetID()const{ return m_nodeinput.GetID(); }
	ResourceType GetType()const { return AirsideResource::ResType_IntersectionNode; }
	virtual CString GetTypeName()const { return _T("IntersectionNode"); } 
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);

	IntersectionNode& GetNodeInput(){ return m_nodeinput; }
	const IntersectionNode& GetNodeInput()const { return m_nodeinput; }
	bool FindClearanceInConcern(AirsideFlightInSim * pFlight,  ClearanceItem& lastItem ,DistanceUnit radius, Clearance& newClearance);

	virtual CPoint2008 GetDistancePoint(double dist)const;

	//virtual void ReleaseLock(AirsideFlightInSim * pFlight, const ElapsedTime& tTime);
	//virtual bool GetLock(AirsideFlightInSim * pFlight);
	//virtual AirsideFlightInSim * GetLockedFlight();
	//virtual bool TryLock(AirsideFlightInSim * pFlight);

	FilletTaxiway* GetFilletFromTo(TaxiwayDirectSegInSim * pSeg1, TaxiwayDirectSegInSim* pSeg2,DistanceUnit distInSeg1, DistanceUnit dsitInSeg2);
	FilletTaxiway* GetFilletFromTo(TaxiwayDirectSegInSim * pSeg1, TaxiwayDirectSegInSim* pSeg2);
	FilletTaxiway* GetFilletFromTo(TaxiwayDirectSegInSim * pSeg1, RunwayDirectSegInSim* pSeg2);
	FilletTaxiway* GetFilletFromTo(RunwayDirectSegInSim * pSeg1, TaxiwayDirectSegInSim* pSeg2);
	FilletTaxiway* GetFilletFromTo(TaxiwayDirectSegInSim * pSeg1, LogicRunwayInSim  *pRunway);
	FilletTaxiway* GetFilletFromTo(FlightGroundRouteDirectSegInSim * pSeg1, LogicRunwayInSim  *pRunway);

	FilletTaxiway* GetFilletFromTo(LogicRunwayInSim* pRunway, TaxiwayDirectSegInSim* pSeg);
	FilletTaxiway* GetFilletFromTo(LogicRunwayInSim* pRunway, FlightGroundRouteDirectSegInSim* pSeg);

	FilletTaxiway* GetFilletFromTo(FlightGroundRouteDirectSegInSim* pSeg1, FlightGroundRouteDirectSegInSim* pSeg2);

	CPath2008 GetFiletRoutePath(const FilletTaxiway& fillet, int nLeadObjID);

	std::vector<FilletTaxiway>& GetFilletList(){ return m_vFilletTaxiways; }

	bool IsNoParking()const;
	void SetBlock(bool bBlock);

	ElapsedTime GetAvailableEntryTime(AirsideFlightInSim* pFlight, const ElapsedTime& entryTime, const ElapsedTime& exitTime );
	ElapsedTime GetLastOcyTime()const;

	virtual bool IsActiveFor(ARCMobileElement* pmob,const RouteDirPath* pPath)const;
	virtual bool bMayHaveConflict(const RouteDirPath *pPath)const{ return true; }
	virtual bool bLocked(ARCMobileElement* pmob, const RouteDirPath* pPath)const;
	AirsideFlightInSim* GetLockedFlight()const{ return m_pLockFlight; }
	
	void OnFlightEnter(AirsideFlightInSim* pflt, const ElapsedTime&t);
	void OnFlightExit(AirsideFlightInSim* pflt, const ElapsedTime& t);	
	
	bool bMobileInResource(ARCMobileElement* pmob)const;
protected:
	IntersectionNode m_nodeinput;	
	std::vector<FilletTaxiway> m_vFilletTaxiways;
	AirsideFlightInSim * m_pLockFlight;
	bool m_bBlock;
};

class IntersectionNodeInSimList 
{
public:	
	//add a point to the list , exam the position
	int GetNodeCount()const{ return (int)m_vNodes.size(); }
	IntersectionNodeInSim * GetNodeByIndex(int idx){ return m_vNodes.at(idx).get(); }
	IntersectionNodeInSim * GetNodeByID(int nId);
	IntersectionNodeInSim * GetNodeByName(const CString& sName);

	IntersectionNodesInAirport& GetInputNodes(){ return m_vNodeInputs; }

	IntersectionNodeInSim* GetIntersectionNode(int obj1Id, int obj2Id , int idx);

	bool Init(int nPrjId, int nAirportId);
	void Add(IntersectionNodeInSim* pIntersectionNodeInSim);
	void Add(size_t nPos,IntersectionNodeInSim* pIntersectionNodeInSim);	
protected:
	std::vector<IntersectionNodeInSim::RefPtr> m_vNodes;
	IntersectionNodesInAirport m_vNodeInputs;
	
};