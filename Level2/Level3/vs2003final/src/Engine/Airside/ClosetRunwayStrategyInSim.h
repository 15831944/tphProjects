#pragma once
#include "runwayassignmentstrategyinsim.h"
#include "../../Common/point2008.h"

class RunwayResourceManager;
class LogicRunwayInSim;
class AirsideFlightInSim;


class ENGINE_TRANSFER CClosetRunwayStrategyInSim:
	public CRunwayAssignmentStrategyInSim
{
public:
	CClosetRunwayStrategyInSim();
	~CClosetRunwayStrategyInSim();

	LogicRunwayInSim* GetRunway(RunwayResourceManager* pRunwayRes);
	void SetFlight(AirsideFlightInSim* pFlight){ m_pFlight = pFlight; }

protected:
	AirsideFlightInSim* m_pFlight;

};
