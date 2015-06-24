#pragma once
/**
	OnBoard Agents PathFinding functionality based on AStar Algorithm.
*/

#include "BinHeap.h"
#include <map>
#include "PathNode.h"

class IMap;
class PathFinderListener;

class PathFinder
{
protected:
	IMap*							mMap;
	MinHeap							mOpenListHeap;
	PathFinderListener*				mAStarListener;
	std::vector<int>				mClosedList;
	int								mCloseListSize;

	std::vector<int>				mOpenList;
	int								mOpenListSize;

	std::map<int, PathNode>			mNodeList;
	int								mSelectedNode;

	bool							mVariableTerrain;

	int								mStartNode;
	int								mEndNode;

	std::vector<int>				mAsyncPath;
public:
	PathFinder(PathFinderListener* theListener = NULL);
	virtual ~PathFinder(void);

	// Setup Methods
public:
	virtual void					SetMap(IMap* val);
	virtual void					SetListener(PathFinderListener* val) { mAStarListener = val; }

	// Set to true if your terrain is like hills
	virtual bool					GetVariableTerrain() const { return mVariableTerrain; }
	virtual void					SetVariableTerrain(bool val) { mVariableTerrain = val; }


	// Blocking Methods
public:
	std::vector<int>				GetPath(IMap* theMap, int theStart, int theEnd);

	// Asynchronous Methods
	virtual void					SetupPathFinder(IMap* val, int theStart, int theEnd);
	virtual bool					Update();
	virtual std::vector<int>		RetrieveFoundPath();

	// Helper Methods
protected:
	virtual void					AddNodeToOpenList(int theNodeId);
	virtual bool					IsNodeOnClosedList(int theNodeId);
	virtual bool					IsNodeOnOpenList(int theNodeId);
	virtual void					Reset();
	virtual int						SwitchNodeFromOpenToClosedList();
	virtual void					ResortOpenHeap();
};


