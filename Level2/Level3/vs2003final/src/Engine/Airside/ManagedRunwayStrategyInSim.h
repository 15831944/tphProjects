#pragma once
#include "runwayassignmentstrategyinsim.h"
#include <vector>

class RunwayAssignmentPercentItem;
class LogicRunwayInSim;
class RunwayResourceManager;

class ENGINE_TRANSFER CManagedRunwayStrategyInSim :	public CRunwayAssignmentStrategyInSim
{
public:
	CManagedRunwayStrategyInSim(void);
	~CManagedRunwayStrategyInSim(void);

	LogicRunwayInSim* GetRunway(RunwayResourceManager* pRunwayRes);
	void SetRunwayAssignmentPercentItems( std::vector<RunwayAssignmentPercentItem*> & vPercentItems ){ m_vRunwayAssignmentPercentItems = vPercentItems; }
	bool IsAssignedRunway(int nRwyID, int nMark);

protected:
	std::vector<RunwayAssignmentPercentItem*> m_vRunwayAssignmentPercentItems;
};
