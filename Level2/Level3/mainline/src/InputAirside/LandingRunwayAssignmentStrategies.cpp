#include "StdAfx.h"
#include "LandingRunwayAssignmentStrategies.h"

LandingRunwayAssignmentStrategies::LandingRunwayAssignmentStrategies(int nProjectID)
:RunwayAssignmentStrategies(nProjectID)
{
	m_enumRunwayAssignType = RunwayAssignmentType_Landing;
}

LandingRunwayAssignmentStrategies::~LandingRunwayAssignmentStrategies(void)
{
}


