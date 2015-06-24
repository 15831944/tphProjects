#pragma once
#include "IMap.h"
#include "OBPoint.h"
#include "OBRect.h"
#include "OBColor.h"

namespace OBGeo
{
	class OBPoint;
	class OBRect;
};

class RectMap :	public IMap
{
public:
	RectMap(void);
	virtual ~RectMap(void);

public:
	std::vector<OBGeo::FRect*> mRectArray;
	std::vector< int > mBarrierArray;
	int mRows;
	int mCols;
	int mTileCount;
	int m_nTravelsalDirectlyCost;
	int m_nTravelsalDiagonalCost;

	int mTileWidth;
	int mTileHeight;


public:
	std::vector<int> mPath;

public:

	virtual void ResizeMap(int theCols, int theRows, int nTileWidth, int nTileHeight);
	void setTravelsalDirectlyCost(int nCost){ m_nTravelsalDirectlyCost = nCost; }
	void setTravelsalDiagonalCost(int nCost){ m_nTravelsalDiagonalCost = nCost; }

	virtual void AddTileRect(OBGeo::FRect* pTileRect, int theTile);
	OBGeo::FRect* GetTileAt(int theTile)const;
	OBGeo::FRect* GetTileAt(double dX, double dY) const;
	OBGeo::FRect* GetTileAt(int ix, int iy)const;
	OBGeo::FRect* GetBoundTitleAt(double dX, double dY)const;
	void GetTileIndex(double dX, double dY, int& ix, int& iy)const;
	int GetTileWidth(){ return mTileWidth; }
	int GetTileHeight(){ return mTileHeight; }
	int GetTileCount(){ return mTileCount; }
	virtual bool setTileAsBarrier(int theTile, bool isBarrier);
	bool isBarrierTile( int theTile )const;

public://IMap Interfaces.
	virtual void GetAdjacentTiles(int theCenterTile, std::vector<int>& theReturnList) const;
	virtual int GetTraversalCost(int theStartTile, int theEndTile);
	virtual int GetHeuristicDistance(int theStartTile, int theEndTile);

public:

	/**
	Eight direction traversing.

			N
		 NW | NE	
	   W----|----E
		 SW | SE
			S
	**/
	
	// north west
	virtual int GetAdjacentNW(int theStartTile)const;
	// north
	virtual int GetAdjacentN(int theStartTile)const;
	// north east
	virtual int GetAdjacentNE(int theStartTile)const;
	// east
	virtual int GetAdjacentE(int theStartTile)const;
	// south east
	virtual int GetAdjacentSE(int theStartTile)const;
	// south
	virtual int GetAdjacentS(int theStartTile)const;
	// south west
	virtual int GetAdjacentSW(int theStartTile)const;
	// west
	virtual int GetAdjacentW(int theStartTile)const;
};


