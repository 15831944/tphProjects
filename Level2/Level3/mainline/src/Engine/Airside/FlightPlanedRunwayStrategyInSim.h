#pragma once
#include "runwayassignmentstrategyinsim.h"

class AirsideFlightInSim;
class LogicRunwayInSim;
class RunwayResourceManager;

class ENGINE_TRANSFER CFlightPlanedRunwayStrategyInSim :
	public CRunwayAssignmentStrategyInSim
{
public:
	CFlightPlanedRunwayStrategyInSim(void);
	~CFlightPlanedRunwayStrategyInSim(void);

	LogicRunwayInSim* GetRunway(RunwayResourceManager* pRes);
	void SetFlight(AirsideFlightInSim* pFlight){ m_pFlight = pFlight; }

private:
	AirsideFlightInSim* m_pFlight;
};
