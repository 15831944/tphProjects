#include "StdAfx.h"
#include "RectMap.h"
#include <math.h>
#include <algorithm>

using namespace OBGeo;

RectMap::RectMap(void)
:mCols(0),mRows(0),mTileCount(0)
,mTileWidth(100),mTileHeight(100)
,m_nTravelsalDirectlyCost(5)
,m_nTravelsalDiagonalCost(7)
{

}

RectMap::~RectMap(void)
{
	for(size_t i = 0 ; i < mRectArray.size(); i++)
		delete mRectArray.at( i );
	mRectArray.clear();
}

void RectMap::ResizeMap( int theCols, int theRows, int nTileWidth, int nTileHeight)
{
	mCols = theCols; mRows = theRows; mTileWidth = nTileWidth; mTileHeight = nTileHeight;
	mTileCount = mCols * mRows;
	mRectArray.resize( mTileCount );
	mBarrierArray.resize( mTileCount );

}

void RectMap::AddTileRect(OBGeo::FRect* pTileRect, int theTile)
{
	mRectArray[theTile] =  pTileRect;
	mBarrierArray[theTile] = 1; //1: can be cross 0:  barrier.
}

void RectMap::GetAdjacentTiles( int theCenterTile, std::vector<int>& theReturnList ) const
{
	// Note: If the Alpha is 0( barrier object), then we won't consider the tile
	if (GetAdjacentN(theCenterTile) >= 0 && mBarrierArray[GetAdjacentN(theCenterTile)])
		theReturnList.push_back(GetAdjacentN(theCenterTile));
	if (GetAdjacentE(theCenterTile) >= 0 && mBarrierArray[GetAdjacentE(theCenterTile)])
		theReturnList.push_back(GetAdjacentE(theCenterTile));
	if (GetAdjacentS(theCenterTile) >= 0 && mBarrierArray[GetAdjacentS(theCenterTile)])
		theReturnList.push_back(GetAdjacentS(theCenterTile));
	if (GetAdjacentW(theCenterTile) >= 0 && mBarrierArray[GetAdjacentW(theCenterTile)])
		theReturnList.push_back(GetAdjacentW(theCenterTile));

	if (GetAdjacentNE(theCenterTile) >= 0 && mBarrierArray[GetAdjacentNE(theCenterTile)])
		theReturnList.push_back(GetAdjacentNE(theCenterTile));
	if (GetAdjacentNW(theCenterTile) >= 0 && mBarrierArray[GetAdjacentNW(theCenterTile)])
		theReturnList.push_back(GetAdjacentNW(theCenterTile));
	if (GetAdjacentSE(theCenterTile) >= 0 && mBarrierArray[GetAdjacentSE(theCenterTile)])
		theReturnList.push_back(GetAdjacentSE(theCenterTile));
	if (GetAdjacentSW(theCenterTile) >= 0 && mBarrierArray[GetAdjacentSW(theCenterTile)])
		theReturnList.push_back(GetAdjacentSW(theCenterTile));

}

int RectMap::GetTraversalCost( int theStartTile, int theEndTile )
{
	int aCol1 = theStartTile%mCols;
	int aRow1 = theStartTile/mCols;
	int aCol2 = theEndTile%mCols;
	int aRow2 = theEndTile/mCols;

	// if directly above, below, left or right
	if (aCol1 == aCol2 || aRow1 == aRow2)
		return m_nTravelsalDirectlyCost;

	// otherwise, it's diagonal
	return m_nTravelsalDiagonalCost;
}

int RectMap::GetHeuristicDistance( int theStartTile, int theEndTile )
{
	if (theStartTile >= 0 && theStartTile < (int)mRectArray.size() && theEndTile >= 0 && theEndTile < (int)mRectArray.size())
	{ 
		double startx = mRectArray[theStartTile]->mX + mRectArray[theStartTile]->mWidth/2.0;
		double starty = mRectArray[theStartTile]->mY + mRectArray[theStartTile]->mHeight/2.0;
		double endx = mRectArray[theEndTile]->mX + mRectArray[theEndTile]->mWidth/2.0;
		double endy = mRectArray[theEndTile]->mY + mRectArray[theEndTile]->mHeight/2.0;

		startx /= mRectArray[theStartTile]->mWidth;
		starty /= mRectArray[theStartTile]->mHeight;
		endx /= mRectArray[theEndTile]->mWidth;
		endy /= mRectArray[theEndTile]->mHeight;

		double xDistance = abs(startx-endx);
		double yDistance = abs(starty-endy);

		// Another modification, taking into consideration that 
		// diagonals cost more than straight paths.
		if (xDistance > yDistance)
			return (int)( m_nTravelsalDiagonalCost*yDistance + m_nTravelsalDirectlyCost*(xDistance-yDistance) );
		else
			return (int) ( m_nTravelsalDiagonalCost*xDistance + m_nTravelsalDirectlyCost*(yDistance-xDistance) );
	}

	return 0x7fffffff;
}

OBGeo::FRect* RectMap::GetTileAt(int theTile) const
{
	if (theTile >= 0 && theTile < (int)mRectArray.size())
	{
		return mRectArray[ theTile ];
	}
	return NULL;

}

OBGeo::FRect* RectMap::GetTileAt(double dX, double dY)const
{
	if (mRectArray.empty())
		return NULL;

 	FRect* _first = mRectArray[0];
	
	int rowOffset =(int)((dY - _first->mY)/mTileHeight);
	int colOffset =(int)((dX - _first->mX)/mTileWidth);

	int theTitle = rowOffset*mCols + colOffset;
	return GetTileAt(theTitle);
}

OBGeo::FRect* RectMap::GetTileAt( int ix, int iy ) const
{
	int theTitle = iy*mCols + ix;
	return GetTileAt(theTitle);
}
OBGeo::FRect* RectMap::GetBoundTitleAt(double dX, double dY)const
{
	if (mRectArray.empty())
		return NULL;
	
	FRect* _first = mRectArray[0];

	int rowOffset =(int)((dY - _first->mY)/mTileHeight);
	int colOffset =(int)((dX - _first->mX)/mTileWidth);

	if (rowOffset < 0)
	{
		rowOffset = 0;
	}
	else if (rowOffset >= mRows)
	{
		rowOffset = mRows - 1;
	}

	if (colOffset < 0)
	{
		colOffset = 0;
	}
	else if (colOffset >= mCols)
	{
		colOffset = mCols - 1;
	}

	int theTitle = rowOffset*mCols + colOffset;
	return GetTileAt(theTitle);
}

bool RectMap::setTileAsBarrier( int theTile, bool isBarrier)
{
	if (theTile >= 0 && theTile < (int)mRectArray.size())
	{
		isBarrier ? (mBarrierArray[theTile]  = 0) : (mBarrierArray[theTile] = 1);
		return true;
	}

	return false;
}

bool RectMap::isBarrierTile( int theTile )const
{
	if (theTile >= 0 && theTile < (int)mRectArray.size())
	{
		return mBarrierArray[theTile] == 0;
	}

	ASSERT(FALSE);
	return false;
}

int RectMap::GetAdjacentNW( int theStartTile )const
{
	int theCol = theStartTile%mCols;
	int theRow = theStartTile/mCols;

	if ( theCol == 0 || theRow == 0)
	{
		return -1;
	}
	if (	mBarrierArray[(theRow - 1)*mCols + (theCol)] == 0
		||	mBarrierArray[(theRow)*mCols + (theCol - 1)] == 0)
	{
		return -1;
	}
	return (theRow - 1)*mCols + (theCol - 1);

}

int RectMap::GetAdjacentN( int theStartTile )const
{
	int theCol = theStartTile%mCols;
	int theRow = theStartTile/mCols;

	if ( theRow == 0)
	{
		return -1;
	}
	return (theRow - 1)*mCols + (theCol);

}

int RectMap::GetAdjacentNE( int theStartTile ) const
{
	int theCol = theStartTile%mCols;
	int theRow = theStartTile/mCols;

	if ( theCol == (mCols - 1) || theRow == 0)
	{
		return -1;
	}
	if (	mBarrierArray[(theRow - 1)*mCols + (theCol)] == 0
		||	mBarrierArray[(theRow)*mCols + (theCol + 1)] == 0)
	{
		return -1;
	}
	return (theRow - 1)*mCols + (theCol + 1);
}

int RectMap::GetAdjacentE( int theStartTile )const
{
	int theCol = theStartTile%mCols;
	int theRow = theStartTile/mCols;

	if ( theCol == mCols - 1 )
	{
		return -1;
	}
	return (theRow)*mCols + (theCol + 1);

}

int RectMap::GetAdjacentSE( int theStartTile )const
{
	int theCol = theStartTile%mCols;
	int theRow = theStartTile/mCols;

	if ( theCol == (mCols - 1) || theRow == (mRows - 1))
	{
		return -1;
	}
	if (	mBarrierArray[(theRow + 1)*mCols + (theCol)] == 0 
		||	mBarrierArray[(theRow)*mCols + (theCol + 1)] == 0)
	{
		return -1;
	}
	return (theRow + 1)*mCols + (theCol + 1);

}

int RectMap::GetAdjacentS( int theStartTile )const
{
	int theCol = theStartTile%mCols;
	int theRow = theStartTile/mCols;

	if ( theRow == mRows - 1)
	{
		return -1;
	}
	return (theRow + 1)*mCols + (theCol);

}

int RectMap::GetAdjacentSW( int theStartTile )const
{
	int theCol = theStartTile%mCols;
	int theRow = theStartTile/mCols;

	if ( theCol == 0 || theRow == (mRows - 1))
	{
		return -1;
	}
	if (	mBarrierArray[(theRow + 1)*mCols + (theCol)] == 0 
		||	mBarrierArray[(theRow)*mCols + (theCol - 1)] == 0)
	{
		return -1;
	}
	return (theRow + 1)*mCols + (theCol - 1);

}

int RectMap::GetAdjacentW( int theStartTile )const
{
	int theCol = theStartTile%mCols;
	int theRow = theStartTile/mCols;

	if ( theCol == 0 )
	{
		return -1;
	}
	return (theRow)*mCols + (theCol - 1);

}

void RectMap::GetTileIndex( double dX, double dY, int& ix, int& iy ) const
{
	FRect* _first = mRectArray[0];

	iy =(int)((dY - _first->mY)/mTileHeight);
	ix =(int)((dX- _first->mX)/mTileWidth);
}







