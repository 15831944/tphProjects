#pragma once
#include "IMap.h"

/**
	<TODO::> Mobile Agent movement behavior simulation based on HexMap 
	is better than based on RectMap.
*/
class HexMap : public IMap
{

public:
	HexMap(void);
	virtual ~HexMap(void);

public:
	std::vector<int> mPath;

public:
	virtual void GetAdjacentTiles(int theCenterTile, std::vector<int>& theReturnList){};
	virtual int GetTraversalCost(int theStartTile, int theEndTile){ return 0;};
	virtual int GetHeuristicDistance(int theStartTile, int theEndTile){ return 0;};
};
