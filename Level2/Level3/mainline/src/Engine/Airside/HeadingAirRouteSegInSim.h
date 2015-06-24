#pragma once
#include "AirsideResource.h"
#include "AirRoutePointInSim.h"
#include "AirspaceResource.h"

class Clearance;
class ClearanceItem;
class AirRouteIntersectionInSim;
class FlightRouteInSim;
class ENGINE_TRANSFER HeadingAirRouteSegInSim : public AirsideResource
{
public:
	HeadingAirRouteSegInSim(void);
	~HeadingAirRouteSegInSim(void);

	enum HeadingType
	{
		Aligned = 0, 
		Direct
	};

	virtual ResourceType GetType()const{ return ResType_HeadingAirRouteSegment; }

	virtual CString GetTypeName()const{ return _T("HeadingAirRouteSegment"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);

	CPoint2008 GetDistancePoint(double dist)const;

	HeadingAirRouteSegInSim(AirRouteIntersectionInSim* pWayPt1, AirRouteIntersectionInSim* pWayPt2)
	{ 
		m_pFirstIntersection = pWayPt1; 
		m_pSecondIntersection = pWayPt2; 
	}

	AirRouteIntersectionInSim* GetFirstIntersection()const{  return m_pFirstIntersection; }
	AirRouteIntersectionInSim* GetSecondIntersection()const{  return m_pSecondIntersection; }

	CPoint2008 GetWPExtentPoint(){ return m_WPExtentPoint; }
	void SetWPExtentPoint(CPoint2008 extentPoint){ m_WPExtentPoint = extentPoint; }

	HeadingType GetHeadingType(){ return m_HeadingType; }
	void SetHeadingType(HeadingType type){ m_HeadingType = type; }

	bool operator == (const HeadingAirRouteSegInSim& routeSeg)const;

	bool CalculateFlightClearance(AirsideFlightInSim* pFlight,AirsideResource* pLastSegment,FlightRouteInSim* pCurrentRoute,ClearanceItem& newItem,ElapsedTime tEnterTime);
	bool ConverseCalculateFlightClearance(AirsideFlightInSim* pFlight,AirsideResource* pLastSegment,FlightRouteInSim* pCurrentRoute, 
										  ClearanceItem& nextItem,ClearanceItem& newItem, AirsideResourceList& ResourceList,ElapsedTime tEnterTime);
	
	void CalculateClearanceItems(AirsideFlightInSim* pFlight, FlightRouteInSim* pRoute,ClearanceItem& lastItem, AirsideResource* pNextRes, Clearance& newClearance);


	//will change the last item work for circute flight
	void GenerateClearanceItems(AirsideFlightInSim* pFlight, FlightRouteInSim* pRoute,AirsideResource* pNextRes , ClearanceItem& lastItem, Clearance& newClearane);

	AirspaceHoldInSim* GetConnectedHold();
	ElapsedTime GetMaxTimeInSegment(AirsideFlightInSim* pFlight, double dSped1,  double dSped2, AirsideResource* pNextRes);
	ElapsedTime GetMinTimeInSegment(AirsideFlightInSim* pFlight, double dSped1,  double dSped2, AirsideResource* pNextRes);

	//--------------------------------------------------------------------------------------------------------------------------------
	//Summary:
	//		Separation distance calculate
	//--------------------------------------------------------------------------------------------------------------------------------
	double GetExpectDistance();
	ElapsedTime GetDistExpectTimeInSegment(FlightRouteInSim* pFlightRoute,AirsideFlightInSim* pFlight,double dDist);
	ElapsedTime GetExpectTimeInSegment(FlightRouteInSim* pFlightRoute,AirsideFlightInSim* pFlight);
private:
	CPoint2008 GetVectorAvailablePosition(ElapsedTime tEnter,ElapsedTime tExit, double dSped1, double dSped2, AirsideResource* pNextRes,AirsideFlightInSim* pFlight);

	CPoint2008 GetProjectionPosition(CPoint2008 CurrentPos, AirsideResource* pNextRes,AirsideFlightInSim* pFlight);
	CLine2008 GetAlignProjectLine(AirRouteSegInSim* pNextSeg, AirsideFlightInSim* pFlight);

protected:
	AirRouteIntersectionInSim* m_pFirstIntersection;
	AirRouteIntersectionInSim* m_pSecondIntersection;

	CPoint2008 m_WPExtentPoint;
	HeadingType m_HeadingType;
};

class ENGINE_TRANSFER HeadingAirRouteSegInSimList
{
public:
	HeadingAirRouteSegInSimList(){ m_vRouteSegs.clear(); }
	~HeadingAirRouteSegInSimList(){ m_vRouteSegs.clear(); }
	void Add(HeadingAirRouteSegInSim* pSegment){ m_vRouteSegs.push_back(pSegment); }

	int GetCount()const{ return m_vRouteSegs.size(); }
	HeadingAirRouteSegInSim* GetSegment(int idx)
	{
		return m_vRouteSegs.at(idx);
	}

protected:
	std::vector<HeadingAirRouteSegInSim*> m_vRouteSegs;	
};
