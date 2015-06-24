#pragma once
#include <vector>
#include "../enginedll.h"
//#include "./InputAirside/RunwayExitStrategies.h"

class ElapsedTime;
class AirsideFlightInSim;
class CAirportDatabase;
class RunwayExitStrategies;
class FlightTypeRunwayExitStrategyItem;
class TimeRangeRunwayExitStrategyItem;
class LogicRunwayInSim;
class RunwayExitInSim;
class StandResourceManager;
class CAirportDatabase;
class RunwayExitStrategyPercentItem;
class RunwayExitStrategyPriorityItem;

class ENGINE_TRANSFER CRunwayExitAssignmentStrategiesInSim
{
	friend class LogicRunwayInSim;
public:
	CRunwayExitAssignmentStrategiesInSim(void);
	~CRunwayExitAssignmentStrategiesInSim(void);

	//bCheckCanHoldFlight Check Can Hold Flight
	RunwayExitInSim* GetAvailableRunwayExit( AirsideFlightInSim *pFlight, CAirportDatabase *pAirportDatabase,StandResourceManager* pStandResManager,bool bCheckCanHold  ); 

	void Init(int nPrj,CAirportDatabase* pAirportDatabase);

	bool IsBackTrackOp(AirsideFlightInSim* pFlight);
	double GetFlightNormalDecelOnRunway(AirsideFlightInSim* pFlight);
	double GetFlightMaxDecelOnRunway(AirsideFlightInSim* pFlight);


	//get runway exit if using max breaking on either side
	//move from protected to public, for write runway log
	std::vector<RunwayExitInSim*> GetMaxBrakingFirstExitOnEitherSide(AirsideFlightInSim* pFlight,LogicRunwayInSim* pLogicRunway);
protected:
	std::vector<RunwayExitInSim*> GetMaxBrakingFirstExitOnSameSideAsArrivalGate(AirsideFlightInSim* pFlight,LogicRunwayInSim* pLogicRunway,StandResourceManager* pStandResManager);
	std::vector<RunwayExitInSim*> GetNormalBrakingExitOnEitherSide(AirsideFlightInSim* pFlight,LogicRunwayInSim* pLogicRunway);
	std::vector<RunwayExitInSim*> GetNormalBrakingExitOnSameSideAsArrivalGate(AirsideFlightInSim* pFlight,LogicRunwayInSim* pLogicRunway,StandResourceManager* pStandResManager);
	std::vector<RunwayExitInSim*> GetManagedExit(AirsideFlightInSim* pFlight,LogicRunwayInSim* pLogicRunway,TimeRangeRunwayExitStrategyItem* pItem,bool bCheckCanHold);
	bool IsExitAvailable(RunwayExitInSim *pRunwayExit);
	std::vector<RunwayExitInSim*> GetRunwayExitByFlightType( AirsideFlightInSim *pFlight, CAirportDatabase *pAirportDatabase,StandResourceManager* pStandResManager,bool bCheckCanHold ); 

	std::vector<RunwayExitInSim*> GetManagedPercentExit(AirsideFlightInSim* pFlight,LogicRunwayInSim* pLogicRunway,RunwayExitStrategyPercentItem* pItem,bool bCheckCanHold);
	std::vector<RunwayExitInSim*> GetManagedPriorityExit(AirsideFlightInSim* pFlight,LogicRunwayInSim* pLogicRunway,RunwayExitStrategyPriorityItem* pItem,bool bCheckCanHold);
private:
	RunwayExitStrategies* m_pRunwayExitStrategies;


};
