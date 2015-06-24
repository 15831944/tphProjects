#pragma once

#include "OnBoard/PathFinding/OBRect.h"
#include "OnBoard/PathFinding/RectMap.h"
#include "OnBoard/PathFinding/PathFinderListener.h"
#include "OnBoard/PathFinding/Pathfinder.h"
#include "../Common/Point2008.h"
#include "../Common/pollygon.h"

class OnboardCellInSim;
class OnboardCellPath;
class Point;
class OnboardDeckGroundInSim;
class PaxCellLocation;
class PaxOnboardBaseBehavior;

class GroundInSim : public PathFinderListener
{
public:
	GroundInSim();
	virtual ~GroundInSim(void);

	void setHeight(double dHeight){ mHeight = dHeight; }
	double getHeight(){ return mHeight; }

	void generateGround(OBGeo::FRect& _outBoundRect,OnboardDeckGroundInSim *pDeckGround);
	void setOutsideLocatorSitesAsBarriers(POLLYGONVECTOR& _deckOutLine);
	void setInVisibleAsBarriers(const Pollygon& pollygon);
	void initCellHoldMobile();

	virtual void OnPathFound(const std::vector<int>& thePath);

	OnboardCellInSim* getCell( CPoint2008& location) const;
	OnboardCellInSim* getCell( int nTileIndex);
	OnboardCellInSim* getBoundCell(CPoint2008& location) const;

	//get the cell at the point, if the point is out of deck, it will return NULL
	OnboardCellInSim* GetPointCell(CPoint2008& location);


	RectMap& getLogicMap(){ return mRectMap; }
	int getLocatorSiteCount();

	//small space, full occupied, half occupied
	bool IsCellBarrier(int nIndex) const;


	//calculate location
	//ptLocation, the center
	//ptDirection, the direction
	//dWidth, dLength
	//return the location
	bool CalculateLocation( CPoint2008 ptLocation, CPoint2008 ptDirection, 
		double dLength, double dWidth, 
		PaxCellLocation & paxLocation) const;

	void getCellPos(const CPoint2008& ptLocation, int& x, int& y)const;
	//get cell count to 
	void getFreeSpace(PaxOnboardBaseBehavior *pBehavior,int x0, int y0, int x1, int y1, int& endx, int& endy)const;
	double GetDirValidDist(PaxOnboardBaseBehavior *pBehavior,const CPoint2008&ptLocation  ,const CPoint2008& dir, double dMaxLen )const;
public:
	//give origin cell, Destination Cell, 
	//return a path
	void GetPath(OnboardCellInSim* pStartCell, OnboardCellInSim* pEndCell, 
		OnboardCellPath& _movePath);

	OnboardCellInSim* GetClosestAvailableCell(CPoint2008& location);

	//eight direction cells
	std::vector<OnboardCellInSim *> GetNeighbourCells(OnboardCellInSim *pCenterCell) const;

protected:
	bool contain(POLLYGONVECTOR& _deckOutLine,const Pollygon& pollygon)const;
	void calculateCellHoldMobile(OnboardCellInSim* pCellInSim);
	void calculateMobileCanWalkThrough();
	void setOccupyCellAsBarriers();

	double GetDistanceFromNDirection(int iStartCell);
	double GetDistanceFromSDirection(int iStartCell);

	double GetDistanceFromWDirection(int iStartCell);
	double GetDistanceFromEDirection(int iStartCell);
protected:
	RectMap				mRectMap;
	PathFinder			mPathFinder;
	double				mHeight;
};