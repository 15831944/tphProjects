#pragma once

#include <vector>

class IMap
{
public:
	IMap(void);
	virtual ~IMap(void);

	// Abstract interface Methods:
	// These methods are required by the Pathfinder to search the map
public:
	virtual void						GetAdjacentTiles(int theCenterTile, std::vector<int>& theReturnList) const = 0;
	virtual int							GetTraversalCost(int theStartTile, int theEndTile) = 0;
	virtual int							GetHeuristicDistance(int theStartTile, int theEndTile) = 0;
};
