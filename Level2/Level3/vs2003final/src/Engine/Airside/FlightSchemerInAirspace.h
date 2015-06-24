#pragma once
#include "AirsideResource.h"
#include "vector"
#include "../../InputAirside/TimeRange.h"
#include "RunwaysController.h"

class Clearance;
class ClearanceItem;
class AirRouteIntersectionInSim;
class AirRouteSegInSim;
class FlightRouteInSim;

typedef std::vector<AirsideResource*> AirsideResourceList;

class ENGINE_TRANSFER FlightSchemerInAirspace
{

public:
	class SegmentTimeInfo
	{
	public:
		ElapsedTime minTime;
		ElapsedTime maxTime;
		ElapsedTime curTime;
	};

public:
	FlightSchemerInAirspace(void);
	~FlightSchemerInAirspace(void);

public:
	bool FlightSchemeOnAirRoute(AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList,ClearanceItem& lastItem,Clearance& newClearance);

	bool FlightSchemeOnAirRoute(AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList,ClearanceItem& lastItem, TimeRange& eLandingTime,
								ElapsedTime tNewBirthTime, std::vector<CRunwaysController::RunwayOccupyInfo>& vRunwayArrangeInfo);
	
	bool FlightSchemeOnAirRouteTakeOff(AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList,ClearanceItem& lastItem, TimeRange& eTakeOffTime,std::vector<CRunwaysController::RunwayOccupyInfo>& vRunwayArrangeInfo);

	ElapsedTime CalculateOptimisticTimeThrouhRoute(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList,int nStartSeg);
	ElapsedTime CalculateOptimisticTimeThrouhRouteTakeOff(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList,int nStartSeg);



protected:
	AirRouteIntersectionInSim *m_pAirspaceConflictConcern;
//	bool FlightSchemeOnAirRoute(AirsideFlightInSim* pFlight,AirsideResourceList& ResourceList,ClearanceItem& lastItem,Clearance& newClearance);


	ElapsedTime CalculateTimeFromLastSegToRunwayPort(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim,AirRouteSegInSim *pLastSegment);
	ElapsedTime CalculateTimeFromOneSegToNextSeg(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim,AirRouteSegInSim *pCurrentSeg,AirRouteSegInSim *pNextSeg);
	ElapsedTime CalculateTimeFromFirstSegToRunwayPortTakeOff(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim,AirRouteSegInSim *pLastSegment);
	ElapsedTime CalculateTimeFromOneSegToNextSegTakeOff(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim,AirRouteSegInSim *pCurrentSeg,AirRouteSegInSim *pNextSeg);

	bool AdjustMinTime(std::vector<SegmentTimeInfo>& vSegTimeInfo, std::vector<ClearanceItem >& vClearancItem,int nCurSeg);
	bool AdjustMaxTime(std::vector<SegmentTimeInfo>& vSegTimeInfo, std::vector<ClearanceItem >& vClearancItem,int nCurSeg);

};
