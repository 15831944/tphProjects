#pragma once
#include "Engine/EngineDll.h"

class LogicRunwayInSim;
class RunwayResourceManager;

class ENGINE_TRANSFER CRunwayAssignmentStrategyInSim
{
public:
	CRunwayAssignmentStrategyInSim(void);
	~CRunwayAssignmentStrategyInSim(void);

	virtual LogicRunwayInSim* GetRunway( RunwayResourceManager* pRunwayRes ) = 0;
};
