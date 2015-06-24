#pragma once

#include "../../InputAirside/Taxiway.h"
#include "../../InputAirside/TaxiwayConstraint.h"
#include "../../InputAirside/DirectionalityConstraints.h"
#include "../../InputAirside/WingspanAdjacencyConstraints.h"
#include "Clearance.h"
#include "../../Common/Path2008.h"
#include "FlightRouteSegInSim.h"
#include "HoldPositionInSim.h"


class ARCVector3;

class TaxiwayInSim;
class RunwayInSim;
class StandInSim;

class TaxiwaySegConstraintsProperties;
class TaxiwaySegInSim;
class FlightState;
class TaxiInterruptLineInSim;
class StartPositionInSim;

class ENGINE_TRANSFER TaxiwayDirectSegInSim : public FlightGroundRouteDirectSegInSim
{
	DynamicClassStatic(TaxiwayDirectSegInSim)	
	RegisterInherit(TaxiwayDirectSegInSim,FlightGroundRouteDirectSegInSim)
public:
	typedef ref_ptr<TaxiwayDirectSegInSim> RefPtr;

	TaxiwayDirectSegInSim(TaxiwaySegInSim *pSeg, FlightGroundRouteSegDirection dir );

	virtual ResourceType GetType()const { return AirsideResource::ResType_TaxiwayDirSeg; }
	virtual CString GetTypeName()const{ return _T("TaxiwayDirectSegment"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);
	
	TaxiwaySegInSim * GetTaxiwaySeg(){ return (TaxiwaySegInSim *)m_pSeg; } 
	const TaxiwaySegInSim* GetTaxiwaySeg()const{ return (const TaxiwaySegInSim *)m_pSeg; }
	//virtual void ReleaseLock(AirsideFlightInSim * pFlight, const ElapsedTime& tTime);

public:
	virtual int GetObjectID()const{ return GetTaxiwayID(); }
	//handle two flights have negative direction on taxiway adjacency conflict
	virtual bool adjacencyNegativeConflict(RouteDirPath* pPath,ARCMobileElement* pmob,ARCMobileElement* ptheMob);
	//get adjacency taxiway that map tow current taxiway lead mob
	virtual ARCMobileElement* getAdjacencyLeadMobile(ARCMobileElement* pmob,double& dDist);

	TaxiwayDirectSegInSim  * GetOppositeSegment();
	const TaxiwayDirectSegInSim * GetOppositeSegment()const;
	bool IsForbidDirection();
	//the dist to the exit node of radius of concern
	double GetDistToRadiusofConcern(double radius);
	
	//virtual bool IsDirectionActive()const;
	//virtual bool TryLockDir(AirsideFlightInSim* pFlight);
	//virtual bool GetDirLock(AirsideFlightInSim* pFlight);
	//virtual void ReleaseDirLock(AirsideFlightInSim* pFlight, const ElapsedTime& tTime);

	//virtual TaxiwaySegDirectionLock* GetDirectionLock();

	int GetTaxiwayID()const;
	//Negative direction
	void AddFirstAdjacencyNegativeConstraint(CWingspanAdjacencyConstraints* constraint);
	void AddSecondAdjacencyNegativeConstraint(CWingspanAdjacencyConstraints* constraint);

	//parallel direction
	void AddFirstAdjacencyParallelConstraint(CWingspanAdjacencyConstraints* constraint);
	void AddSecondAdjacencyParallelConstraint(CWingspanAdjacencyConstraints* constraint);

protected:
	//get map flight on head of current flight
	ARCMobileElement* getAdjacencyMappingHead(const FlightGroundRouteDirectSegList& segList,
		ARCMobileElement* pmob,
		const CWingspanAdjacencyConstraints* pConstraint,
		bool bCheckFirstConstraint,FlightGroundRouteSegDirection emType,double& dDist);
public:

	bool operator < (const TaxiwayDirectSegInSim& otherSeg)const;

protected:
	//negative direction constraint
	std::vector<CWingspanAdjacencyConstraints*> m_vFirstNegativeConflictConstraint;
	std::vector<CWingspanAdjacencyConstraints*> m_vSecondNegativeConflictConstraint;

	//parallel direction constraint
	std::vector<CWingspanAdjacencyConstraints*> m_vFirstParallelConflictConstraint;
	std::vector<CWingspanAdjacencyConstraints*> m_vSecondParallelConflictConstraint;
	
};
typedef std::vector<TaxiwayDirectSegInSim*> TaxiwayDirectSegList;

class TaxiwayConstraintsManager;
class TaxiwayInSim;
class VehicleLaneInSim;
class AirsideMeetingPointInSim;
class ENGINE_TRANSFER TaxiwaySegInSim: public FlightGroundRouteSegInSim
{
public:	
	friend class TaxiwayDirectSegInSim;
public:
	TaxiwaySegInSim(IntersectionNodeInSim * pNodeFrom , IntersectionNodeInSim * pNodeTo, TaxiwayInSim * pTaxiway);
	~TaxiwaySegInSim();



	//get distance between two hold positions  , less than the nodes distance
	DistanceUnit GetSegmentLength()const;	

	CPoint2008 GetDistancePoint(double dist)const;



	bool bMobileInResource(ARCMobileElement* pmob)const;
	TaxiwayDirectSegInSim * GetPositiveDirSeg()const{ return (TaxiwayDirectSegInSim *)m_positiveDirSeg.get(); }
	TaxiwayDirectSegInSim * GetNegativeDirSeg()const{ return (TaxiwayDirectSegInSim *)m_negativeDirSeg.get(); }

	TaxiwaySegConstraintsProperties* GetConstraints(){ return m_pConstraintsProperties; }



	//get distance hold position in the taxiway
	//double GetHold1Pos()const;
	//double GetHold2Pos()const;
	//get distance node position in the taxiway
	double GetNode1Pos()const;
	double GetNode2Pos()const;
	TaxiwayInSim* GetTaxiway(){ return m_pTaxiway; }
	

	//constraints interface
	void AddWeightWingspanConstraint(TaixwayConstraintType type, int nValue);
	void AddFlightTypeRestrictions(FlightConstraint& fltCons);
	void SetDirectionConstraintType(DirectionalityConstraintType dir);
	void SetBearingDegree(double degree);
	void AddClosureTimeConstraint(ElapsedTime epStartTime,ElapsedTime epEndTime);
	void AddTaxiSpeedConstraint(const FlightConstraint& fltCons, double dSpeed);

	TaxiwayInSim * m_pTaxiway;

	virtual bool CanServeFlight(AirsideFlightInSim *pFlight);
	int GetTaxiwayID();

	//TaxiwaySegDirectionLock* GetDirectionLock(){ return m_pDirLock; }
	void SetSegmentDirection(FlightGroundRouteSegDirection dir){ m_direction = dir; }
	FlightGroundRouteSegDirection GetSegmentDirection(){ return m_direction; }

	void SetHoldShortLine(TaxiInterruptLineInSim* pHoldShortLine);
	const std::vector<TaxiInterruptLineInSim*>& GetHoldShortLine();
	bool IsHoldShortLineOnSegment(int nInterruptlineID);

	void AddMeetingPoints(AirsideMeetingPointInSim* pMeetingPoint);
	const std::vector<AirsideMeetingPointInSim*>& GetMeetingPoints();
	AirsideMeetingPointInSim* GetMeetingPointInSeg(const std::vector<ALTObjectID>& vObjNames);

	void AddStartPosition(StartPositionInSim* pStartPosition);
	const std::vector<StartPositionInSim*>& GetStartPostions();
	StartPositionInSim* GetStartPostionInSeg(const std::vector<ALTObjectID>& vObjNames);	

	bool GetTaxiwayConstrainedSpeed(const AirsideFlightInSim* pFlight, double& dSpeed) const;

public:
	//Constraints  interface
	//TaxiwayDirectSegInSim::RefPtr m_positiveDirSeg;
	//TaxiwayDirectSegInSim::RefPtr m_negativeDirSeg;


protected:

	
//	TaxiwaySegDirectionLock* m_pDirLock; //lock of the taxiway direction
	
	TaxiwaySegConstraintsProperties *m_pConstraintsProperties;
	FlightGroundRouteSegDirection m_direction;
	std::vector<TaxiInterruptLineInSim*> m_vHoldShortLines;
	std::vector<AirsideMeetingPointInSim*> m_vMeetingPoints;
	std::vector<StartPositionInSim*> m_vStartPositions;

};

typedef std::vector<HoldPositionInSim> HoldShortLinePropList;

//a Route of taxiway  composed of many taxiway segments
class ENGINE_TRANSFER TaxiwayInSim : public FlightGroundRouteInSim
{
public:
	explicit TaxiwayInSim( Taxiway* pTaxiwayInput );
	~TaxiwayInSim();

	TaxiwaySegInSim * GetSegment(int idx);
	int GetSegmentCount ()const{ return static_cast<int>(m_vSegments.size()); }	

public:

	Taxiway * GetTaxiwayInput(){ return m_pTaxiwayInput.get(); }
	
	//Get Taxiway , form node1 to node2
	TaxiwayDirectSegInSim * GetTaxiwayDirectSegment(IntersectionNodeInSim* pNodeFrom, IntersectionNodeInSim * pNodeTo);
	void GetTaxiwayDirectSegmentList(int nIntersectNodeIDFrom, int nIntersectNodeIDTo, FlightGroundRouteDirectSegList& taxiwayDirectSegList);
	bool InitSegments(IntersectionNodeInSimList& NodeList,const HoldShortLinePropList&  );

	void InitConstraints(TaxiwayConstraintsManager& consManager);
	void InitTaxiwayAdjacencyConstraints(const CWingspanAdjacencyConstraintsList* pAdjacencyConstraint);

	//Get the idx hold position
	CPoint2008 GetHoldPositionPos(int idx)const;
	TaxiwaySegInSim* GetPointInSegment(const CPoint2008& point);

	int GetTaxiwayID();

	IntersectionNodeInSimList GetIntersectionNodeList();
	static bool includeSeg(const FlightGroundRouteDirectSegList& segList,TaxiwaySegInSim* pSegment,FlightGroundRouteSegDirection& emType);
protected:
	void AdjacencyConstraint(CWingspanAdjacencyConstraints* constraint,const FlightGroundRouteDirectSegList& segList,bool firstConstraint);
protected:
	Taxiway::RefPtr m_pTaxiwayInput;
	std::vector<TaxiwaySegInSim*>  m_vSegments;  //sequence of segments of the taxiway;	
};
