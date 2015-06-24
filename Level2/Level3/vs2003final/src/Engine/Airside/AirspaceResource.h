#pragma once

//#include "AirsideResource.h"

#include "../../InputAirside/AirRoute.h"
#include "RunwayInSim.h"
//#include "FlightInAirsideSimulation.h"
#include <vector>

#include "FlightPlanInSim.h"
#include "AirRoutePointInSim.h"
#include "../../InputAirside/FlightPlan.h"


class ARCUnit;
class AirsideResourceManager;
class AirRouteNetworkInSim;
class AirRouteIntersectionInSim;
class CGoAroundCriteriaDataProcess;
class HeadingAirRouteSegInSim;

typedef std::vector<AirsideResource*> AirsideResourceList;

class ENGINE_TRANSFER AirRouteSegInSim : public AirsideResource
{
public:
	typedef ref_ptr<AirRouteSegInSim> RefPtr;

	AirRouteSegInSim()
	{ 
		m_vRoutePoints.clear();
		m_pFirstIntersection = NULL;
		m_pSecondIntersection = NULL;
	}

	//insert the airRoute to the segment, 
	void InsertRouteAfterPoint(AirRoutePointInSim *pAfter, AirRoutePointInSim* pInsert);
	void InsertRouteBeforePoint(AirRoutePointInSim *pBefore, AirRoutePointInSim* pInsert);
	void AddRoutePoint(AirRoutePointInSim* pPoint){ m_vRoutePoints.push_back(pPoint); }
	virtual ResourceType GetType()const{ return ResType_AirRouteSegment; }

	virtual CString GetTypeName()const{ return _T("AirRouteSegment"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);
	
	//horizontal distance
	DistanceUnit GetSegmentDistance()const;	

	virtual CPoint2008 GetDistancePoint(double dist) const;

	void CalculateClearanceItems(AirsideFlightInSim* pFlight,FlightRouteInSim* pRoute,ClearanceItem& lastItem, ElapsedTime tExit, Clearance& newClearance);
	
	AirRouteIntersectionInSim* GetFirstConcernIntersection();
	AirRouteIntersectionInSim* GetSecondConcernIntersection();

	void SetFirstIntersection(AirRouteIntersectionInSim* pIntersection){ m_pFirstIntersection = pIntersection; }
	void SetSecondIntersection(AirRouteIntersectionInSim* pIntersection);

	int GetRoutePointCount(){ return m_vRoutePoints.size();}
	AirRoutePointInSim* GetRoutePointByIdx(int nIdx);
	AirsideFlightInSim* GetForemostFlightInSeg(ElapsedTime tTime);
	AirsideFlightInSim* GetBackmostFlightInSeg(ElapsedTime tTime);

	void DeleteRoutePoint(int nIdx);

	bool operator == (const AirRouteSegInSim& pOtherSeg) const;

	AirspaceHoldInSim* GetConnectedHold();

	double GetDistLastWaypointToHold();

	AirsideFlightInSim* GetLeadFlight(AirsideFlightInSim* pFlight); 

	CPoint2008 GetATCManagedPoint(AirsideFlightInSim* pFlight);

protected:
	AirRouteIntersectionInSim* m_pFirstIntersection;
	AirRouteIntersectionInSim* m_pSecondIntersection;	
	std::vector<AirRoutePointInSim*> m_vRoutePoints;
};


class ENGINE_TRANSFER AirRouteSegInSimList
{
public:
	AirRouteSegInSim* GetSegmentInList(AirRouteSegInSim* pSegment);

	int GetCount()const{ return m_vRouteSegs.size(); }
	AirRouteSegInSim * GetSegment(int idx){
		return m_vRouteSegs.at(idx).get();
	}

protected:
	std::vector<AirRouteSegInSim::RefPtr> m_vRouteSegs;	
};


class Clearance;
class ClearanceItem;
class AirEntrySystemHoldInInSim;
// Air Route 
class ENGINE_TRANSFER FlightRouteInSim
{
public:
	FlightRouteInSim();
	~FlightRouteInSim();

	int GetItemCount()const{ return  m_vItems.size(); }
	bool IsEmpty(){ return m_vItems.empty(); }
	AirsideResource* GetItem(int idx);

	void Clear(){ m_vItems.clear(); }
	void AddItem(AirsideResource* pItem);

	double RecalculateFlightSpeed(double dSpeed,double dDecel,ElapsedTime tTime);
	double CalculateFlightDecel(double dForeSpd,double dAftSpd,double dDist, double dSep);
	bool IsFlightInAirspace(AirsideFlightInSim* pFlight);

	int GetSegmentIndex(AirsideResource * pResource)const;
	FlightPlanPropertyInSim GetRoutePointProperty(AirRoutePointInSim* pWaypt);


	AirWayPointInSim* GetLastHoldConnectedWaypoint();

	ElapsedTime GetLatestRunwayAvalableTime(AirsideFlightInSim* pFlight,ElapsedTime tSepartionTime, double dSeparationDistance);

	LogicRunwayInSim* GetRelateRunway();
	void SetRelateRunway(LogicRunwayInSim* pLogicRunway);

	FlightPlanPropertiesInSim& GetPlanProperties(){ return m_FlightPlanProperties; }

	void GetConcernRouteSegs(AirsideResource* pCurrentResource, AirsideResourceList& ResourceList);
	void GetResouceRouteSegs(AirsideResource* pCurrentResource, AirsideResourceList& ResourceList);
	void GetConcernCircuitRouteSegs(AirsideResource* pCurrentResource, AirsideResourceList& ResourceList);
	AirEntrySystemHoldInInSim* GetEntrySystemHold();

	void SetAirRouteInSim(AirRouteInSim *pAirRouteInSim);
	AirRouteInSim *GetAirRouteInSim();

	CString GetName()const{ return m_strName; }
	void SetName(CString strName){ m_strName = strName; } 

	bool IsPointAtFlightRouteToRwyPhase(AirsideFlightInSim* pFlight, const CPoint2008& point);
	virtual void InitializeSegments(AirsideFlightInSim *pFlight){};

public:

	virtual ns_FlightPlan::FlightPlanBase::ProfileType getType() = 0;

	virtual ElapsedTime GetFlightTimeInSeg(AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg);
	virtual ElapsedTime GetFlightMaxTimeInSeg(AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg);
	virtual ElapsedTime GetFlightMinTimeInSeg(AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg);

	double CalculateDistanceFromLastSegToLogicRunway( AirsideFlightInSim *pFlight );
	//STAR
	virtual ElapsedTime CalculateTimeFromLastSegToLogicRunway(AirsideFlightInSim *pFlight);
	virtual ElapsedTime CalculateMinTimeFromLastSegToLogicRunway(AirsideFlightInSim *pFlight);

	virtual ElapsedTime GetHeadingMinTimeInSeg(AirsideFlightInSim *pFlight, HeadingAirRouteSegInSim*pSeg, HeadingAirRouteSegInSim*pNextSeg);
	virtual ElapsedTime GetHeadingMaxTimeInSeg(AirsideFlightInSim *pFlight, HeadingAirRouteSegInSim*pSeg, HeadingAirRouteSegInSim*pNextSeg);

	//SID
	virtual ElapsedTime CalculateTakeoffTimeFromRwyToFirstSeg(AirsideFlightInSim *pFlight);


protected:
	std::vector<AirsideResource*> m_vItems;
	CString m_strName;

	FlightPlanPropertiesInSim m_FlightPlanProperties;

	//the air route which the flight route comes from
	AirRouteInSim *m_pAirRouteInSim;

};



class ENGINE_TRANSFER AirRouteInSim
{
public:
	AirRouteInSim(CAirRoute* pRoute);
	~AirRouteInSim();

	void AddSegment( AirsideResource* pSeg);

	void AddRoutePoint(int Idx,AirRoutePointInSim* pPoint);

	AirRoutePointInSim* GetRoutePointByIdx( int nIdx);
	AirRoutePointInSim* GetRwyConnectedRoutePoint();
	int GetRoutePointIdx(ARWaypoint* pWaypoint);

	int GetRoutePointIdx(AirRoutePointInSim* pPoint);

	int GetWaypointIdxByID(int nID){ return m_RoutePointList.GetAirWaypointIdxInListByID(nID); }

	int GetID(){ return m_nID; }

	int GetRoutePointCount(){ return m_RoutePointList.GetRoutePointCount();}
	CAirRoute* GetAirRouteInput(){ return m_pRouteInput; }

	AirsideResourceList GetRouteSegmentList(AirRoutePointInSim* pFirstIntersection, AirRoutePointInSim* pSecondIntersection);
	//AirsideResource* GetRouteSegment(AirRouteIntersectionInSim* pFirstInt,AirRouteIntersectionInSim* pSecondInt);
	AirsideResourceList GetAirRouteFromIntersectionPoint(AirRoutePointInSim* pFirstIntersection);


	int GetRouteSegCount(){ return m_vRouteSegList.size();}
	 
	AirsideResource* GetSeg(int idx){ return m_vRouteSegList.at(idx); }

	CAirRoute::RouteType getRouteType() const;
private:
	CAirRoute* m_pRouteInput;
	int m_nID;
	AirRoutePointInSimList m_RoutePointList;
	std::vector<AirsideResource*> m_vRouteSegList;
};
