#pragma once
#include <vector>
#include "../enginedll.h"
#include <map>

class LandingRunwayAssignmentStrategies;
class TakeoffRunwayAssignmentStrategies;
class FlightTypeRunwayAssignmentStrategyItem;
class ElapsedTime;
class AirsideFlightInSim;
class CAirportDatabase;
class LogicRunwayInSim;
class RunwayResourceManager;
class CTakeoffStandRunwayAssign;
class CTakeoffFlightTypeRunwayAssignment;
class CTakeOffTimeRangeRunwayAssignItem;
class RunwayExitInSim;
class CManageAssignPriorityData;
enum TakeOffBackTrack;

class ENGINE_TRANSFER CFlightTypeRunwayAssignmentStrategiesInSim
{
public:
	CFlightTypeRunwayAssignmentStrategiesInSim(void);
	~CFlightTypeRunwayAssignmentStrategiesInSim(void);


	void Init(int nPrj,CAirportDatabase *pAirportDatabase);

	//LogicRunwayInSim* GetDepartureRunway(AirsideFlightInSim* pFlight, RunwayResourceManager* pRunwayRes);
	LogicRunwayInSim* GetArrivalRunway(AirsideFlightInSim* pFlight, RunwayResourceManager* pRunwayRes);
	RunwayExitInSim* GetTakeoffPosition(AirsideFlightInSim* pFlight, RunwayResourceManager* pRunwayRes);

	TakeOffBackTrack GetTakeoffBacktrack(AirsideFlightInSim* pFlight,RunwayResourceManager * pRunwayRes);

	bool IsDepartureRunway(AirsideFlightInSim* pFlight,int nRwyID, int nMark);
	bool IsArrivalRunway(AirsideFlightInSim* pFlight, int nRwyID, int nMark);


	bool IsAbleToChangeTakeoffPos(AirsideFlightInSim* pFlight)const;
private:
	FlightTypeRunwayAssignmentStrategyItem* GetFlightTypeLandingRunwayAssignmentStrategyItem( AirsideFlightInSim *pFlight ); 
	CTakeOffTimeRangeRunwayAssignItem* GetFlightTypeTakeoffRunwayAssignmentStrategyItem( AirsideFlightInSim *pFlight)const; 

	TakeOffBackTrack GetFlightTakeoffBacktrack(AirsideFlightInSim* pFlight,RunwayResourceManager * pRunwayRes,CTakeOffTimeRangeRunwayAssignItem* pStrategyItem);
	LogicRunwayInSim* GetRunway(AirsideFlightInSim* pFlight, RunwayResourceManager * pRunwayRes,FlightTypeRunwayAssignmentStrategyItem* pStrategyItem);
	//LogicRunwayInSim* AssignDepartureRunway(AirsideFlightInSim* pFlight, RunwayResourceManager * pRunwayRes,CTakeoffFlightTypeRunwayAssignment* pStrategyItem);
	LogicRunwayInSim* GetManagedDepartureRunway(AirsideFlightInSim* pFlight, RunwayResourceManager * pRunwayRes,CTakeOffTimeRangeRunwayAssignItem* pTimeItem);

	RunwayExitInSim* GetTakeoffPosition(AirsideFlightInSim* pFlight, RunwayResourceManager * pRunwayRes,CTakeOffTimeRangeRunwayAssignItem* pStrategyItem);
	RunwayExitInSim* GetManagedTakeoffPos(AirsideFlightInSim* pFlight, RunwayResourceManager * pRunwayRes,CTakeOffTimeRangeRunwayAssignItem* pTimeIte);

private:
	LandingRunwayAssignmentStrategies* m_pFlightTypeLandingRunwayAssignmentStrategies;
	CTakeoffStandRunwayAssign* m_pTakeoffRunwayAssignmentStrategies;

	typedef std::map<AirsideFlightInSim*, CManageAssignPriorityData*> PriorityMap;
	PriorityMap m_assignPriority;
	CManageAssignPriorityData* getFlightLastPriority(AirsideFlightInSim* pFlight)const;


};
