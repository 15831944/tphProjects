#include "StdAfx.h"
#include "PathFinder.h"
#include "IMap.h"
#include "PathFinderListener.h"
#include <algorithm>


PathFinder::PathFinder(PathFinderListener* theListener /* = NULL */)
{
	SetListener(theListener);
	mOpenListSize = 0;
	mCloseListSize = 0;
	mOpenList.resize(64);
	mClosedList.resize(64);
	
	mVariableTerrain = true;
}

PathFinder::~PathFinder(void)
{
}

void PathFinder::AddNodeToOpenList( int theNodeId )
{
	if(mNodeList.find(theNodeId) == mNodeList.end()) 
		return;

	// grow the open list
	if (mOpenListSize == mOpenList.size())
		mOpenList.resize(mOpenList.size()*2);

	// Insert the node Id into the Open list
	mOpenList[mOpenListSize] = theNodeId;
	// Increase the Open list size for next insertion
	mOpenListSize++;

	// Insert the OpenListId into the heap
	mOpenListHeap.Insert(mNodeList[theNodeId].mTotalCost, theNodeId);
}

bool PathFinder::IsNodeOnClosedList( int theNodeId )
{
	return (mCloseListSize > 0 && std::find(mClosedList.begin(), mClosedList.begin() + mCloseListSize, theNodeId) != (mClosedList.begin() + mCloseListSize));
}

void PathFinder::Reset()
{
	mOpenListSize = 0;
	mCloseListSize = 0;
	mNodeList.clear();
	mSelectedNode = -1;
	mOpenListHeap.Clear();

	mAsyncPath.clear();
}

void PathFinder::SetMap( IMap* val )
{
	mMap = val;
	Reset();
}

std::vector<int> PathFinder::GetPath( IMap* theMap, int theStart, int theEnd )
{
	// LOL!  Internally, I'll simply use the Asynchronous path finding 
	// in a blocking manner!
	SetupPathFinder(theMap, theStart, theEnd);

	// Turn off the callback because we are immediately returning!
	PathFinderListener* tempListener = mAStarListener;
	mAStarListener = NULL;

	// Update only returns true when it is done searching.
	while(!Update()){};

	// Reset the callback listener
	mAStarListener = tempListener;

	// Reset the Error string.  During the ASync Search, ErrorString is set as "Still Working"
	//if(mAsyncPath.size() == 0)
	//	;

	return mAsyncPath;
}

int PathFinder::SwitchNodeFromOpenToClosedList()
{
	int theLowestCoseNodeonOpenList = mOpenListHeap.Remove();

	// remove the node from the OpenList
	for (int i = 0; i < mOpenListSize ; i++)
	{
		if (mOpenList[i] == theLowestCoseNodeonOpenList)
		{
			mOpenList[i] = mOpenList[mOpenListSize - 1];
			mOpenListSize--;
			break;
		}
	}

	// Grow Closed list
	if (mCloseListSize >= (int)mClosedList.size())
		mClosedList.resize(mClosedList.size()*2);

	// Add to Closed List
	mClosedList[mCloseListSize] = theLowestCoseNodeonOpenList;
	mCloseListSize++;

	return theLowestCoseNodeonOpenList;
}

bool PathFinder::IsNodeOnOpenList( int theNodeId )
{
	return (mOpenListSize > 0 && std::find(mOpenList.begin(), mOpenList.begin() + mOpenListSize, theNodeId) != (mOpenList.begin() + mOpenListSize));
}

void PathFinder::ResortOpenHeap()
{
	mOpenListHeap.Clear();

	for (int i = 0; i < mOpenListSize; i++)
		mOpenListHeap.Insert(mNodeList[mOpenList[i]].mTotalCost, mOpenList[i]);
}

void PathFinder::SetupPathFinder( IMap* val, int theStart, int theEnd )
{
	Reset();
	mStartNode = theStart;
	mEndNode = theEnd;
	mMap = val;

	if(theStart < 0 || theEnd < 0) return;

	// Insert into node List
	mNodeList.insert(std::pair<int, PathNode>(mStartNode, PathNode(mMap->GetHeuristicDistance(mStartNode, mEndNode))));
	AddNodeToOpenList(mStartNode);
}

bool PathFinder::Update()
{
	if(mMap == NULL) return true;

	if (mOpenListSize == 0)
		return true;

	/************************************************************************/
	/* This is the actual looping require to find the path                  */
	/************************************************************************/

	bool aPathFound = false;
	std::vector<int> theAdjacentNodes; // Keep this on the stack for speed

	int num_iterations = 5; // short circuit to keep from hogging the CPU

	while(!aPathFound && mOpenListSize > 0 && num_iterations--)
	{
		// Select the node with the smallest total cost
		mSelectedNode = SwitchNodeFromOpenToClosedList();

		if (mSelectedNode == mEndNode)
		{
			aPathFound = true;
			break;
		}

		// clear and pass by reference, a little speed boost
		theAdjacentNodes.clear();
		mMap->GetAdjacentTiles(mSelectedNode, theAdjacentNodes);

		for (unsigned int i = 0; i < theAdjacentNodes.size(); i++)
		{
			// Do not Add the currently selected node (which is on the closed list)
			// Do not re-add nodes we have already looked at (on the closed list)
			if (theAdjacentNodes[i] != mSelectedNode && !IsNodeOnClosedList(theAdjacentNodes[i]))
			{
				// We've already looked at this Node before!
				if (IsNodeOnOpenList(theAdjacentNodes[i]))
				{
					// Ok, this only runs if mVariableTerrain is set to true, see 
					// the Ctor for a detailed description.
					if (mVariableTerrain && mMap->GetTraversalCost(mSelectedNode, theAdjacentNodes[i]) < mNodeList[theAdjacentNodes[i]].mTraversalCost)
					{
						// If the traversal cost from the parent of theAdjacent 
						// Node is less than the traversal cost from theSelected node
						// then change the parents
						mNodeList[theAdjacentNodes[i]].mTraversalCost = mMap->GetTraversalCost(mSelectedNode, theAdjacentNodes[i]);
						mNodeList[theAdjacentNodes[i]].mParent = mSelectedNode;
						mNodeList[theAdjacentNodes[i]].CalculateTotalCost();

						// and resort the now corrupted heap (the slow bit)
						ResortOpenHeap();
					}
				}
				else
				{
					// Create node and add to Open list
					mNodeList.insert(
						std::pair<int, PathNode>(theAdjacentNodes[i], 
						PathNode(mMap->GetHeuristicDistance(theAdjacentNodes[i], mEndNode), 
						mMap->GetTraversalCost(theAdjacentNodes[i], mSelectedNode), mSelectedNode, 
						mNodeList[mSelectedNode].mTotalCost))
						);

					AddNodeToOpenList(theAdjacentNodes[i]);
				}
			}
		}
	} 

	if (aPathFound)
	{
		// report the path
		while(mSelectedNode != mNodeList[mSelectedNode].mParent )
		{
			mAsyncPath.push_back(mSelectedNode);
			mSelectedNode = mNodeList[mSelectedNode].mParent;
		}

		if(mAStarListener)
			mAStarListener->OnPathFound(mAsyncPath);

		mOpenListSize = 0;
		return true;
	}
	else
		;

	// No Path Found!
	if (mOpenListSize == 0)
	{
		if(mAStarListener)
			mAStarListener->OnPathFound(mAsyncPath);
		return true;
	}

	return aPathFound;
}

std::vector<int> PathFinder::RetrieveFoundPath()
{
	return mAsyncPath;
}


