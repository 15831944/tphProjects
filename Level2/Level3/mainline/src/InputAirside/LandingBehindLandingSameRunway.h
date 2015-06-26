#pragma once
#include "ApproachSeparationType.h"
#include "ApproachSeparationItem.h"
#include <vector>
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API LandingBehindLandingSameRunway : public AircraftClassificationBasisApproachSeparationItem
{
public:
	LandingBehindLandingSameRunway(int nProjID);
	~LandingBehindLandingSameRunway(void);
};
