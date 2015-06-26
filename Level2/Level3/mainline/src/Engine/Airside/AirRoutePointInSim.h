#pragma once
#include "AirsideResource.h"
#include "../../InputAirside/AirWayPoint.h"
#include "AirspaceHoldInSim.h"

class AirRouteSegInSim;
class ClearanceItem;
class AirsideFlightInSim;
class AirRouteInSim;
class CAirRoute;
class AirspaceIntrailSeparationSpeed;

class ENGINE_TRANSFER  AirRoutePointInSim : public AirsideResource
{
public:
	typedef ref_ptr<AirRoutePointInSim> RefPtr;

	AirRoutePointInSim();
	~AirRoutePointInSim();

	virtual ResourceType GetType()const = 0;
	virtual CString GetTypeName()const = 0;
	virtual CString PrintResource()const = 0;
	virtual int getID()const = 0;

	void SetAltHigh(double dAlt){ m_dAltHigh = dAlt;}
	void SetAltLow(double dAlt) { m_dAltLow = dAlt;}

	void SetPosition(CPoint2008 pt){m_pos = pt;}
	CPoint2008 GetPosition()const { return m_pos; }
	double GetAltHigh()const { return m_dAltHigh; }
	double GetAltLow()const { return m_dAltLow; }
	CPoint2008 GetDistancePoint(double dist)const{ return GetPosition(); }

	void AddIntersectionRoute(CAirRoute* pRoute){ m_vIntersectionRoutes.push_back(pRoute); }
	bool IsIntersectionRoute(AirRouteInSim* pRoute);
	bool IsIntersection();

	OccupancyInstance GetFrontOccupancyInstance(AirsideFlightInSim* pFlight, ElapsedTime tEnter);

	void CalculateClearanceAtPoint(AirsideFlightInSim* pFlight, AirsideFlightInSim* pFlightLead,ClearanceItem& lastItem, ClearanceItem& newItem);

	ElapsedTime RecalculateEnterTime(AirsideFlightInSim* pFlight,AirsideFlightInSim* pFlightLead,ElapsedTime tLeadEnter, ElapsedTime tEnter,const AirspaceIntrailSeparationSpeed& leadSpd);

	double getDistToRunway() const;
	void setDistToRunway(double dDist);

	//virtual void SetEnterTime(CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode);
	virtual void SetEnterTime(CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode,double dSpd);

protected:	
	CPoint2008 m_pos;
	double m_dAltHigh;
	double m_dAltLow;

	std::vector<CAirRoute*> m_vIntersectionRoutes;
	
	//distance between runway port and current point, through air route
	//this distance cannot be used to calculate the time to runway
	//
	double m_dDistToRunway;

};

class AirWayPointInSim;
class AirspaceIntersectionInSim;
class ARWaypoint;
class CAirRouteIntersection;

class ENGINE_TRANSFER AirRoutePointInSimList
{
public:
	AirRoutePointInSim * GetRoutePointByIdx(int nIdx);

	AirWayPointInSim* GetAirWaypointInSim(ARWaypoint* pWaypoint);
	AirWayPointInSim* GetAirWaypointInSimByID(int nID);
	AirspaceIntersectionInSim* GetRouteIntersectionInSim(CAirRouteIntersection* pIntersection);

	int GetAirWaypointIdxInListByID(int nID);
	int GetRouteIntersectionIdxInListByID(int nID);

	bool Add(AirRoutePointInSim* pRoutePoint);

	void InsertPoint(int nIdx, AirRoutePointInSim* pRoutePoint);

	int GetRoutePointCount(){ return int(m_vRoutePoints.size()); }

protected:
	std::vector<AirRoutePointInSim::RefPtr> m_vRoutePoints;

};


class ENGINE_TRANSFER AirWayPointInSim : public AirRoutePointInSim
{
public:
	typedef ref_ptr<AirWayPointInSim> RefPtr;

	explicit AirWayPointInSim( int nWayPtId,const ALTAirport& refARP );

	ResourceType GetType()const{ return ResType_WayPoint; }
	virtual CString GetTypeName()const{ return _T("WayPoint"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);

	int getID()const{ return m_wayPtInput->getID(); }
	AirWayPoint* getAirWaypointInput(){ return m_wayPtInput.get();}

	AirspaceHoldInSim* GetHold(){ return m_pHold;}
	void SetHold(AirspaceHoldInSim* pHold){ m_pHold = pHold; }

protected:	
	AirWayPoint::RefPtr m_wayPtInput;
	AirspaceHoldInSim* m_pHold;
};

class ENGINE_TRANSFER FlightPlanPhasePointInSim : public AirRoutePointInSim
{
public:
	typedef ref_ptr<FlightPlanPhasePointInSim> RefPtr;

	explicit FlightPlanPhasePointInSim();

	ResourceType GetType()const{ return ResType_FlightPlanPhase; }
	virtual CString GetTypeName()const{ return _T("FlightPlanPhase"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);
	virtual int getID()const;
protected:	

};


class ENGINE_TRANSFER AirspaceIntersectionInSim : public AirRoutePointInSim
{
public:
	typedef ref_ptr<AirspaceIntersectionInSim> RefPtr;

	explicit AirspaceIntersectionInSim(  CAirRouteIntersection* pIntersection );

	ResourceType GetType()const{ return ResType_AirRouteIntersection; }
	virtual CString GetTypeName()const{ return _T("AirRouteIntersection"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);

	int getID()const;
	CAirRouteIntersection* getIntersectionInput(){ return m_pRouteIntersection;}

protected:	
	CAirRouteIntersection* m_pRouteIntersection;
};
