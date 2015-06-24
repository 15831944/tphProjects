#pragma once
#include "../enginedll.h"
#include "./InputAirside/FlightTypeRunwayAssignmentStrategyItem.h"

class FlightTypeRunwayAssignmentStrategyItem;
class ElapsedTime;
class FlightInAirsideSimulation;
class RunwayResourceManager;
class RunwayPortInSim;

class ENGINE_TRANSFER CFlightTypeRunwayAssignmentStrategyItemInSim
{
public:
	CFlightTypeRunwayAssignmentStrategyItemInSim(void);
	~CFlightTypeRunwayAssignmentStrategyItemInSim(void);

	RunwayPortInSim* GetRunway(FlightInAirsideSimulation* pFlight, RunwayResourceManager * pRunwayRes);

	void SetFlightTypeRunwayAssignmentStrategyItem( FlightTypeRunwayAssignmentStrategyItem* pStrategyItem){ m_pFlightTypeRunwayAssignmentStrategyItem = pStrategyItem;}
	FlightTypeRunwayAssignmentStrategyItem* GetFlightTypeRunwayAssignmentStrategyItem() {return m_pFlightTypeRunwayAssignmentStrategyItem;}


private:
	FlightTypeRunwayAssignmentStrategyItem* m_pFlightTypeRunwayAssignmentStrategyItem;
};
