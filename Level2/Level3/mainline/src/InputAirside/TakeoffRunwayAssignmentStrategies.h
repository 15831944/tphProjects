#pragma once

#include "InputAirsideAPI.h"

#include "RunwayAssignmentStrategies.h"
class INPUTAIRSIDE_API CTakeoffRunwayAssignmentStandItem
{
public:
		CTakeoffRunwayAssignmentStandItem() ;
		~CTakeoffRunwayAssignmentStandItem() ;
protected:
		
};
class INPUTAIRSIDE_API TakeoffRunwayAssignmentStrategies : public RunwayAssignmentStrategies
{
public:
	TakeoffRunwayAssignmentStrategies(int nProjectID);
	~TakeoffRunwayAssignmentStrategies(void);
	void ReadData();
};
