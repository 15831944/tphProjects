#pragma once


class PathNode
{
public:
	PathNode(int theHeuristicCost = 0, int theTraversalCost = 0,  
		int theParent = -1, int theParentCost = 0)
		:mTotalCost(0)
		,mParent(theParent)
		,mParentCost(theParentCost)
		,mHeuristicCost(theHeuristicCost)
		,mTraversalCost(theTraversalCost)
	{
		CalculateTotalCost();
	}
	~PathNode(){};

	virtual void CalculateTotalCost()
	{
		mTotalCost = mHeuristicCost + mTraversalCost + mParentCost;
	};

	int	mTotalCost;
	int	mHeuristicCost;
	int mTraversalCost;
	int	mParent;
	int mParentCost;
};

