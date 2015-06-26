#include "StdAfx.h"
#include "TakeoffRunwayAssignmentStrategies.h"
#include "..\Database\ADODatabase.h"
#include "FlightTypeRunwayAssignmentStrategyItem.h"
TakeoffRunwayAssignmentStrategies::TakeoffRunwayAssignmentStrategies(int nProjectID)
:RunwayAssignmentStrategies(nProjectID)
{
	m_enumRunwayAssignType = RunwayAssignmentType_TakeOff;
}

TakeoffRunwayAssignmentStrategies::~TakeoffRunwayAssignmentStrategies(void)
{
}
void TakeoffRunwayAssignmentStrategies::ReadData()
{
	
}