#pragma once
#include ".\ResourceManager.h"
#include "AirspaceResource.h"
#include "ConflictConcernIntersectionInAirspace.h"
#include "HeadingAirRouteSegInSim.h"
#include <vector>
#include "../../common/TimeRange.h"
#include "RunwaysController.h"
#include "AirsideCircuitFlightInSim.h"

class Clearance;
class ClearanceItem;
class AirRouteIntersectionInSim;
class AirRouteSegInSim;
class FlightRouteInSim;
class AirsideResourceManager;
class ElapsedTime;

class ENGINE_TRANSFER AirspaceIntrailSeparationSpeed
{
public:
	AirspaceIntrailSeparationSpeed(AirsideResource* pCurrentRes,FlightRouteInSim* pAirRouteInSim);
	~AirspaceIntrailSeparationSpeed();

	double GetSparationSpeed(AirsideFlightInSim* pFlight,double dDist)const;


private:
	double GetLandingSparationSpeed(AirsideFlightInSim* pFlight,double dDist)const;
	double GetTakeoffSparationSpeed(AirsideFlightInSim* pFlight,double dDist)const;
	double GetSegmentSparationTime(AirsideFlightInSim* pFlight,AirRouteSegInSim* pSeg,double dDist)const;
private:
	AirsideResource* m_pCurrentRes;
	FlightRouteInSim* m_pAirRouteInSim;
};
//Airspace Resources : 
class ENGINE_TRANSFER AirRouteNetworkInSim  : public ResourceManager
{
	friend class AirspaceTrafficController;
public:
	class SegmentTimeInfo
	{
	public:
		ElapsedTime minTime;
		ElapsedTime maxTime;
		ElapsedTime curTime;
	};
public:
	AirRouteNetworkInSim(void);
	~AirRouteNetworkInSim(void);

	bool Init(int nPrjID,AirsideResourceManager * pAirsideRes );
	bool InitRelations();

	AirRouteList& GetAirRouteList(){ return m_vRouteList;}
	bool HasSID();
	bool HasSTAR();
	void CalculateWaypointConnectionSegmentList(AirRouteIntersectionInSim* pIntersectedWaypoint);
	
	AirRoutePointInSimList& GetAirRoutePointList(){ return m_vRoutePoints; }

	AirRouteInSim* GetAirLaneInSimByID( int nID);

	//return mis approach route of Runway
	FlightRouteInSim* GetMisApproachRoute(AirsideFlightInSim* pFlight);
	void GetMissApproachClearance(AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim, ClearanceItem lastItem,Clearance& newClearance);

	void AirRouteSegRegular(AirRouteSegInSim* pSeg);

	bool FindClearanceInConcern(AirsideFlightInSim * pFlight, FlightRouteInSim* pFlightRoute,ClearanceItem& lastItem, Clearance& newClearance );
	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//Summary:
	//			flight from circuit air route to runway
	//--------------------------------------------------------------------------------------------------------------------------------------------------
	bool FindCircuitFlightLandingClearance(AirsideCircuitFlightInSim * pFlight, FlightRouteInSim* pFlightRoute,ClearanceItem& lastItem, Clearance& newClearance,AirsideCircuitFlightInSim::LandingOperation emType);

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//Summary:
	//			flight from runway to air route
	//-------------------------------------------------------------------------------------------------------------------------------------------------
	bool FindCircuitFlightTakeoffClearance(AirsideCircuitFlightInSim * pFlight, FlightRouteInSim* pFlightRoute,ClearanceItem& lastItem, Clearance& newClearance,AirsideCircuitFlightInSim::LandingOperation emType);

	FlightRouteInSim* GetFlightEnroute(AirsideFlightInSim* pFlight,int nEntryWaypointID, int nExitWaypointID);
	void ReleaseFlightHoldLock(AirsideFlightInSim* pFlight);
	bool IsFlightNeedHolding(AirsideFlightInSim* pFlight, AirspaceHoldInSim* pHold,ClearanceItem& lastItem,Clearance& newClearance);
	bool IsFlightNeedDelayLandingOnRwy(AirsideFlightInSim* pFlight,const ClearanceItem& lastItem, ElapsedTime& tDelayTime);

	ElapsedTime GetLandingFlightTimeFromPointToRwy(AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim, const CPoint2008& point);
	ElapsedTime GetTakeoffFlightTimeFromRwyToPoint(AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim, const CPoint2008& point);

	ElapsedTime GetDelayTimeByEncroachTrailSeperationOnSameSID(AirsideFlightInSim* pFlight,const ElapsedTime& tCurTime);
	ElapsedTime GetFirstDelayTimeAtWaypoint(AirsideFlightInSim* pFlight,const ElapsedTime& tCurTime);


	void GenerateFlightClearance(AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList,ClearanceItem& preItem,Clearance& newClearance);

	bool FlightSchemeOnAirRouteTakeOff(AirsideFlightInSim* pFlight,
		FlightRouteInSim *pAirRouteInSim,
		AirsideResourceList& ResourceList,
		const ClearanceItem& currentItem, 
		const ElapsedTime& tTimeToRwy, 
		ElapsedTime& tMoveTime,
		std::vector<CRunwaySystem::RunwayOccupyInfo>& vRunwayArrangeInfo);

	bool FlightSchemeOnAirRouteLanding(AirsideFlightInSim* pFlight,
		FlightRouteInSim *pAirRouteInSim,
		AirsideResourceList& ResourceList,
		ClearanceItem& lastItem, 
		const ClearanceItem& flightState, 
		TimeRange& eLandingTime,
		ElapsedTime tNewEnterTime, 
		std::vector<CRunwaySystem::RunwayOccupyInfo>& vRunwayArrangeInfo);

	ElapsedTime CalculateOptimisticTimeThrouhRoute(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList);
protected:
	bool FlightPrearrangeOnAirRoute(AirsideFlightInSim* pFlight,
		FlightRouteInSim *pAirRouteInSim,
		AirsideResourceList& ResourceList,
		const ClearanceItem& currentItem,
		Clearance& newClearance);

	bool CircuitFlightTakeoffOnAirRoute(AirsideCircuitFlightInSim* pFlight,
		FlightRouteInSim *pAirRouteInSim,
		AirsideResourceList& ResourceList,
		const ClearanceItem& currentItem,
		Clearance& newClearance,
		AirsideCircuitFlightInSim::LandingOperation emType
		);

	bool FlightPrearrangeOnAirRouteExcepteTakeoffPart(AirsideFlightInSim* pFlight,
														FlightRouteInSim *pAirRouteInSim,
														AirsideResourceList& ResourceList,
														ClearanceItem& lastItem,
														Clearance& newClearance);

	static bool FlightSchemeOnAirRoute(AirsideFlightInSim* pFlight,
								FlightRouteInSim *pAirRouteInSim,
								AirsideResourceList& ResourceList,
								ClearanceItem& lastItem,
								const ClearanceItem& flightState);




	
	ElapsedTime CalculateOptimisticTimeThrouhRouteTakeOff(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList,int nStartSeg);
	ElapsedTime CalculateMinTimeThrouhRoute(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList);

	//ElapsedTime CalculateTimeFromLastSegToRunway(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim,AirRouteSegInSim *pLastSegment);
	ElapsedTime CalculateTimeFromOneSegToNextSeg(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim,AirRouteSegInSim *pCurrentSeg,AirRouteSegInSim *pNextSeg);

	//ElapsedTime CalculateTakeoffTimeFromRwyToFirstSeg(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim/*,AirRouteSegInSim *pLastSegment*/);
	ElapsedTime CalculateTimeFromOneSegToNextSegTakeOff(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim,AirRouteSegInSim *pCurrentSeg,AirRouteSegInSim *pNextSeg);

	bool AdjustMinTime(std::vector<SegmentTimeInfo>& vSegTimeInfo, std::vector<ClearanceItem >& vClearancItem,int nCurSeg);
	bool AdjustMaxTime(std::vector<SegmentTimeInfo>& vSegTimeInfo, std::vector<ClearanceItem >& vClearancItem,int nCurSeg);

	void InitAirspaceHold(int nAirspaceID, double dAirportVar);
	bool CheckIsFlightNeedWaitingInHold(AirsideFlightInSim* pFlight, AirsideResourceList nextResList, ClearanceItem lastItem);

	bool CheckIsFlightCanTakeoffOperation(AirsideCircuitFlightInSim * pFlight, FlightRouteInSim* pFlightRoute,ClearanceItem& lastItem, Clearance& newClearance);
protected:
	HeadingAirRouteSegInSim* GetHeadingSegmentInList(AirRouteIntersectionInSim* pWayPt1, AirRouteIntersectionInSim* pWayPt2);

	void GenerateLandingOperation(AirsideFlightInSim* pFlight);
protected:	
	AirRoutePointInSimList m_vRoutePoints;
	AirRouteSegInSimList m_vRoutSegs;
	//CFinalApproachSegInSimList m_vFinalApproachSegs;
	ConflictConcernIntersectionInAirspaceList m_vConcernIntersectionList;
	HeadingAirRouteSegInSimList m_vHeadingRouteSegs;
	std::vector<AirRouteInSim*> m_vAirLaneInSimList;
	std::vector<AirspaceHoldInSim*> m_vHoldInSimList;

	AirRouteList m_vRouteList;

	

};
