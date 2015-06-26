#pragma once
#include "../EngineDll.h"
#include "../../common/TimeRange.h"
#include "../../InputAirside/ALT_BIN.h"
#include "AirsideResource.h"
#include "RunwayInSim.h"
#include "AirsideCircuitFlightInSim.h"

class AirsideFlightInSim;
class RunwayResourceManager;
class AircraftSeparationManager;
class CApproachRunwayNode;
class LogicRunwayInSim;
class CApproachSeparationInSim;
class CTakeoffSeparationInSim;
class AirTrafficController;
class ClearanceItem;
class Clearance;
class IntersectionNodeInSim;
class CRunwayExitAssignmentStrategiesInSim;
class CRunwayTakeoffPositionAssignInSim;
class OutputAirside;
class AirRouteNetworkInSim;
class CRunwaySeparationRelation;
class RunwayRelatedIntersectionNodeInSim;
class FlightRouteInSim;


class ENGINE_TRANSFER CRunwaySystem
{
public:
	class RunwayOccupyInfo
	{
	public:
		RunwayOccupyInfo(LogicRunwayInSim *port,TimeRange range,IntersectionNodeInSim *pNode,bool b = false,ElapsedTime time = ElapsedTime(0L) )
		{
			pLogicRunway = port;
			pIntNode = pNode;
			timeRange = range;
			bLASHO = b;
			eLASHOTime = time;
		}

		LogicRunwayInSim *pLogicRunway;
		IntersectionNodeInSim *pIntNode;
		bool bLASHO;
		TimeRange timeRange;
		ElapsedTime eLASHOTime;
	protected:
	private:
	};

	typedef std::vector<RunwayOccupyInfo> RunwayOccupyInfoList;

public:
	CRunwaySystem(void);
	~CRunwaySystem(void);

public:
	void Initlization(int nProjID,
						AirTrafficController *pATC, 
						CRunwayExitAssignmentStrategiesInSim* pExitStrategy,
						CRunwayTakeoffPositionAssignInSim* pTakeoffStrategy,
						OutputAirside *pOutputAirside);

	bool ApplyForLandingTime(AirsideFlightInSim* pFlight,ElapsedTime timeToLanding, TimeRange& landingTimeRange,std::vector<RunwayOccupyInfo>& vRunwayArrangeInfo);
	bool ApplyForTakeoffTime(AirsideFlightInSim* pFlight,ElapsedTime tTimeToRwy,ElapsedTime timeToTakeOff, TimeRange& takeoffTimeRange,std::vector<RunwayOccupyInfo>& vRunwayArrangeInfo);

	ClearanceItem GetRunwayTouchdownClearanceItem(AirsideFlightInSim *pFlight,  ElapsedTime eLandingTime,ClearanceItem& lastItem);
	void WriteRunwayLogs(AirsideFlightInSim *pFlight,bool bBackup, ClearanceItem& lastItem,Clearance& newClearance);
	void WriteRunwayTakeOffLogs(AirsideFlightInSim *pFlight, std::vector<RunwayOccupyInfo>& vRunwayArrangeInfo, ElapsedTime eTakeOffTime,ElapsedTime tTimeToRwy, ClearanceItem& lastItem,Clearance& newClearance);
	void WriteCircuitFlightRunwayTakeoffLogs(AirsideCircuitFlightInSim *pFlight, std::vector<RunwayOccupyInfo>& vRunwayArrangeInfo, Clearance& newClearance);
	void GetFlightTakeOffClearance(AirsideFlightInSim *pFlight,/*ElapsedTime eTakeOffTime,*/ElapsedTime tTimeToRwy, ClearanceItem& lastItem,Clearance& vClearance);
	bool GetRunwayLandingClearance(AirsideFlightInSim * pFlight, ClearanceItem& lastItem, Clearance& newClearance);
	//from runway exit to taxiway fillet time
	bool GetRunwayExitFilletTime(AirsideFlightInSim* pFlight,const LogicRunwayInSim *pLogicRunway,const ElapsedTime& timeTakeoff,ElapsedTime& costTime);

	ElapsedTime GetSeperationTime(AirsideFlightInSim* pTrailFlight, AirsideFlightInSim* pLeadFlight, AIRCRAFTOPERATIONTYPE eOpType);
	ElapsedTime GetFlightDelayTimeAtRunwayRelatedIntersections(AirsideFlightInSim* pFlight, ElapsedTime tRwyTime,double dDistToRwyPort );
	ElapsedTime getFlightTakeoffTime(AirsideFlightInSim* pFlight);

	//void GetCircuitFlightTakeoffClearance(AirsideCircuitFlightInSim *pFlight,ElapsedTime tTimeToRwy, ClearanceItem& lastItem,Clearance& newClearance,AirsideCircuitFlightInSim::LandingOperation emType);

	//bool GetCircuitFlightRunwayLandingClearance(AirsideCircuitFlightInSim * pFlight, ClearanceItem& lastItem, Clearance& newClearance,AirsideCircuitFlightInSim::LandingOperation emType);
	//bool GenerateRunwayToCircuitRouteClearance(AirsideCircuitFlightInSim * pFlight, FlightRouteInSim* pCircuitRoute,ClearanceItem& lastItem, Clearance& newClearance);
	
	
	bool GenerateLowAndOverClearance(AirsideCircuitFlightInSim * pFlight, LogicRunwayInSim* pRunway, FlightRouteInSim* pCircuitRoute,ClearanceItem& lastItem, Clearance& newClearance);
	bool GenerateTouchAndGoClearance(AirsideCircuitFlightInSim * pFlight,LogicRunwayInSim* pRunway,FlightRouteInSim* pCircuitRoute,ClearanceItem& lastItem, Clearance& newClearance, bool OccupyRunway = false );
	bool GenerateStopAndGoClearance(AirsideCircuitFlightInSim * pFlight,LogicRunwayInSim* pRunway, FlightRouteInSim* pCircuitRoute,ClearanceItem& lastItem, Clearance& newClearance, bool OccupyRunway = false);
	bool GenerateLandingCleaerance(AirsideFlightInSim * pFlight,LogicRunwayInSim* pRunway,RunwayExitInSim*pexit, bool bBackTrack, ClearanceItem& lastItem, Clearance& newClearance,bool OccupyRunway = false);
	//bool GenerateForceAndGoClearance(AirsideCircuitFlightInSim * pFlight, ClearanceItem& lastItem, Clearance& newClearance);
protected:
	void InitlizationRelatedUnintersectedRunways();
	bool _GenTakeoffFirstWaypointItem(AirsideCircuitFlightInSim * pFlight,FlightRouteInSim* pCircuitRoute,ClearanceItem& lastItem);

	void _WritePreNodesTime(AirsideFlightInSim* pFlight , LogicRunwayInSim* pRunway , const ClearanceItem& beginItem );
	void _WriteRunwayNodeTime(AirsideFlightInSim* pFlight , LogicRunwayInSim* pRunway , const ClearanceItem& itemFrom, const ClearanceItem& itemTo);
	void _WriteAfterNodesTime(AirsideFlightInSim* pFlight, LogicRunwayInSim* pRunway, const ClearanceItem& endItem );
protected:
	std::vector<LogicRunwayInSim *> m_vRunwayInSim;
	std::vector<RunwayRelatedIntersectionNodeInSim*> m_vRunwayRelatedIntersectionNodes;
	int m_nProjID;
	//ElapsedTime m_eTimeCrossRunwayBuffer;
	AirRouteNetworkInSim* m_pAirRouteNetwork;
	//ElapsedTime GetEstimateStayTimeInRunway(AirsideFlightInSim *pFlight);

	LogicRunwayInSim *GetLogicRunwayResource(int nRunwayID,RUNWAY_MARK runwayMark);
	RunwayInSim *GetRunwayResource(int nRunwayID);

	//CApproachRunwayNode *GetApproachRules(std::vector<CApproachRunwayNode *>& runwayRules, int nRunwayID, RUNWAY_MARK runwaymark);

	//separations
	CApproachSeparationInSim *m_pApproachSeparationInSim;
	CTakeoffSeparationInSim	 *m_pTakeOffSeparationInSim;

	CRunwayExitAssignmentStrategiesInSim* m_pRunwayExitStrategy;
	CRunwayTakeoffPositionAssignInSim* m_pRunwayTakeoffPosStrategy;


	//get logic runway free time
	bool GetRunwayOccupancyInstance(LogicRunwayInSim* pRunwayInSim,ElapsedTime elpTimeBegin,OccupancyTable orderOccupancyTable);
	AirsideFlightInSim*  GetFirstFlightBeforeTime(const OccupancyTable& occupancyTable,ElapsedTime elapTime,OccupancyInstance& occuInstance);
	AirsideFlightInSim*  GetFirstFlightAfterTime(const OccupancyTable& occupancyTable,ElapsedTime elapTime,OccupancyInstance& occuInstance);
	AirsideFlightInSim*  GetFirstLandingFlightBeforeTime(const OccupancyTable& occupancyTable,ElapsedTime elapTime,OccupancyInstance& occuInstance);
	AirsideFlightInSim*  GetFirstLandingFlightAfterTime(const OccupancyTable& occupancyTable,ElapsedTime elapTime,OccupancyInstance& occuInstance);
	AirsideFlightInSim*  GetFirstTakeOffFlightBeforeTime(const OccupancyTable& occupancyTable,ElapsedTime elapTime,OccupancyInstance& occuInstance);
	AirsideFlightInSim*  GetFirstTakeOffFlightAfterTime(const OccupancyTable& occupancyTable,ElapsedTime elapTime,OccupancyInstance& occuInstance);
	
	void PerformanceApproachSeparation( AirsideFlightInSim *pFlight,
										const LogicRunwayInSim *pLogicRunway,
										const IntersectionNodeInSim *pNode,
										ElapsedTime needTimeStayInRunway,
										ElapsedTime& timelanding,
										TimeRange& timeRange);

	ElapsedTime GetFlightMinTimeInRunway(AirsideFlightInSim *pFlight);

	//std::pair-> runway ptr, double , the distance from the beginning of runway port
	void GetRunwayIntersectionNodes(LogicRunwayInSim *pCurLandingRunway ,std::vector<std::pair<RunwayInSim*, IntersectionNodeInSim *> >& vRunways);

	bool CheckTimeRangeAvaiable(TimeRange& timeRange, TimeRange compareTimeRange);

	//bSameRunway,
	//ElapseTime& eEnterRunwayTime, means, if the preflight is take off, the next flight's time to runway
	void PerformanceTakeOffSeparation( AirsideFlightInSim *pFlight,
										const LogicRunwayInSim *pLogicRunway,
										const IntersectionNodeInSim* pNode,
										ElapsedTime tTimeToRwy,
										ElapsedTime needTimeStayInRunway,
										ElapsedTime& timeTakeoff,
										TimeRange& timeRange,
										bool bSameRunway,
										ElapsedTime& eEnterRunwayTime);


protected:
	//called while initialize runway system
	void SetOutputAirside(OutputAirside *pOutput);
	int GetFrontFlightOccupancyInstanceIndex(const OccupancyTable& occupancyTable,ElapsedTime elapTime);

	void GetLogicRunwayIntersectionNodeList(LogicRunwayInSim* pLogicRunway, std::vector<IntersectionNodeInSim*>& vNodes);

	bool IsLandingFlightCrossingRunwayIntersectionNodes(AirsideFlightInSim* pFlight,LogicRunwayInSim* pLandingRunway, ElapsedTime& timeToLanding, const std::vector<IntersectionNodeInSim*>& vIntNodes);
	bool IsTakeoffFlightCrossingRunwayIntersectionNodes(AirsideFlightInSim* pFlight, LogicRunwayInSim* pTakeOffRunway, ElapsedTime tTimeToRwy, ElapsedTime& timeToTakeOff, 
														ElapsedTime& eTempEnterRunwayTime, const std::vector<IntersectionNodeInSim*>& vIntNodes);

	void AddNewRelatedRunwayIntersectionNode(LogicRunwayInSim* pRwy1,LogicRunwayInSim* pRwy2,CRunwaySeparationRelation* pRwyRelation, AIRCRAFTOPERATIONTYPE eType);

	//write runway resource log
	//the flights' operation information log
	//void WriteRunwayResourceLog(AirsideFlightInSim *pFlight);
	//out output airside, for write runway log
	OutputAirside *m_pOutput;
};
