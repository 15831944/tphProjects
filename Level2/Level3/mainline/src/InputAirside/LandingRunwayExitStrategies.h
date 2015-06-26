#pragma once

#include <vector>
#include "RunwayExitStrategies.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API LandingRunwayExitStrategies : public RunwayExitStrategies
{
public:
	LandingRunwayExitStrategies(int nProjectID);
	~LandingRunwayExitStrategies(void);
};
