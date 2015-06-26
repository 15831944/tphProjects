#pragma once

#include "RunwayAssignmentStrategies.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API LandingRunwayAssignmentStrategies : public RunwayAssignmentStrategies
{
public:
	LandingRunwayAssignmentStrategies(int nProjectID);
	~LandingRunwayAssignmentStrategies(void);

};
