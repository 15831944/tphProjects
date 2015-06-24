#pragma once
#include "AirspaceResource.h"

class Clearance;
class ClearanceItem;
class AirRoutePointInSim;
class AirspaceHoldInSim;


class AirEntrySystemHoldInInSim;



class ENGINE_TRANSFER AirRouteIntersectionInSim
{
public:
	AirRouteIntersectionInSim(AirRoutePointInSim* pIntersection);
	~AirRouteIntersectionInSim(void);

	//bool CalculateClearanceAtIntersection(AirsideFlightInSim* pFlight,AirsideFlightInSim* pFlightLead, ClearanceItem& lastItem,ClearanceItem& newItem);
	bool ConverseCalculateClearanceAtIntersection(AirsideFlightInSim* pFlight,AirsideResource* pLastSegment,AirsideResource* pNextSegment,
												  FlightRouteInSim* pCurrentRoute, ClearanceItem& nextItem,ClearanceItem& newItem, ElapsedTime& eMinFltPlanTime);
	bool CalculateClearanceAtIntersection(AirsideFlightInSim* pFlight,AirsideResource* pLastSegment,AirsideResource* pNextSegment,FlightRouteInSim* pCurrentRoute, ClearanceItem& lastItem,ClearanceItem& newItem);
	void GetAvailableTimeAtIntersection(AirsideFlightInSim* pFlight,AirsideResource* pNextSeg,FlightRouteInSim* pCurrentRoute,ElapsedTime tEnter,ElapsedTime& tAvailStartTime,ElapsedTime& tAvailEndTime);
	
	AirRoutePointInSim* getInputPoint(){ return m_pConcernIntersection; }

	void AddConvergentRouteSeg(AirRouteSegInSim* pSegment){ m_vConvergentRouteSegs.push_back(pSegment);}
	void AddDivergentRouteSeg(AirRouteSegInSim* pSegment){m_vDivergentRouteSegs.push_back(pSegment);}

	int getConvergentRouteSegCount(){ return m_vConvergentRouteSegs.size();}
	int getDivergentRouteSegCount(){ return m_vDivergentRouteSegs.size(); }

	AirRouteSegInSim* GetConvergentRouteSeg(int idx){ return m_vConvergentRouteSegs.at(idx); }
	AirRouteSegInSim* GetDivergentRouteSeg(int idx){ return m_vDivergentRouteSegs.at(idx); }

	AirEntrySystemHoldInInSim* GetEntrySysHold(){ return m_pEntrySystemHold; }
	void SetEntrySystemHold(AirEntrySystemHoldInInSim* pHold ){ m_pEntrySystemHold = pHold; }

	AirspaceHoldInSim* GetConnectHold();

	bool CalculateClearanceAtIntersectionCircute(AirsideFlightInSim* pFlight,AirsideResource* pLastSegment,AirsideResource* pNextSegment,FlightRouteInSim* pCurrentRoute, ClearanceItem& lastItem,ClearanceItem& newItem);

protected:
	AirEntrySystemHoldInInSim* m_pEntrySystemHold;

	bool ConverseCalculateClearanceWithConvergentSeg(AirsideFlightInSim* pFlight,ElapsedTime tNextItem,ElapsedTime tMaxSegTime, ElapsedTime tMinSegTime, ClearanceItem& newItem);
	bool ConverseCalculateClearanceWithDivergentSeg(AirsideFlightInSim* pFlight,ElapsedTime tNextItem,ElapsedTime tMaxSegTime,ElapsedTime tMinSegTime, ClearanceItem& newItem);

	bool CalculateClearanceWithConvergentSeg(AirsideFlightInSim* pFlight,ElapsedTime tLastItem,ElapsedTime tMaxSegTime, ElapsedTime tMinSegTime, ClearanceItem& newItem);
	bool CalculateClearanceWithDivergentSeg(AirsideFlightInSim* pFlight,ElapsedTime tLastItem,ElapsedTime tMaxSegTime,ElapsedTime tMinSegTime, ClearanceItem& newItem);

	AirRoutePointInSim* m_pConcernIntersection;

	std::vector<AirRouteSegInSim*> m_vConvergentRouteSegs;
	std::vector<AirRouteSegInSim*> m_vDivergentRouteSegs;

};

class ENGINE_TRANSFER ConflictConcernIntersectionInAirspaceList
{
public:
	ConflictConcernIntersectionInAirspaceList(){ m_vIntersectionList.clear(); }
	~ConflictConcernIntersectionInAirspaceList(){ m_vIntersectionList.clear(); }
	void Add(AirRouteIntersectionInSim* pIntersection){ m_vIntersectionList.push_back(pIntersection); }

	int GetCount()const{ return m_vIntersectionList.size(); }
	AirRouteIntersectionInSim* GetIntersection(int idx)
	{
		return m_vIntersectionList.at(idx);
	}
	AirRouteIntersectionInSim* GetIntersection(AirRoutePointInSim* pIntersection);

protected:
	std::vector<AirRouteIntersectionInSim*> m_vIntersectionList;	
};