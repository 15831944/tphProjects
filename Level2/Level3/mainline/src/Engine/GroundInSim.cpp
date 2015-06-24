#include "stdafx.h"
#include "GroundInSim.h"
#include "../Common/ARCPath.h"
#include "OnboardCellInSim.h"
#include "PaxOnboardBaseBehavior.h"
#include "PaxCellLocation.h"

using namespace OBGeo;

GroundInSim::GroundInSim()
{

}

GroundInSim::~GroundInSim()
{

}

void GroundInSim::generateGround(OBGeo::FRect& _outBoundRect,OnboardDeckGroundInSim *pDeckGround)
{
	//maximal tolerance: 10 centimeter. <TODO::> avoid tolerance.
	int nGridCols = (int)_outBoundRect.mWidth  / GRID_WIDTH;
	int nGridRows = (int)_outBoundRect.mHeight / GRID_HEIGHT;

	mRectMap.ResizeMap( nGridCols, nGridRows, GRID_WIDTH, GRID_HEIGHT);

	DistanceUnit _startX = _outBoundRect.mX, _startY = _outBoundRect.mY;
	long theTileIndex = 0;
	for (int i = 0 ; i < nGridRows; i++)
	{
		for (int j = 0 ; j < nGridCols; j++)
		{
			OBGeo::FRect aTileRect(_startX, _startY, GRID_WIDTH, GRID_HEIGHT);

			OnboardCellInSim* pNewCellInSim = new OnboardCellInSim(pDeckGround,theTileIndex, aTileRect, mHeight);
			mRectMap.AddTileRect( pNewCellInSim, theTileIndex);
			theTileIndex++;

			_startX += GRID_WIDTH;
		}
		_startX = _outBoundRect.mX;
		_startY += GRID_HEIGHT;
	}

}

void GroundInSim::setInVisibleAsBarriers(const Pollygon& pollygon)
{
	for (int i = 0; i < mRectMap.GetTileCount(); i++)
	{
		OnboardCellInSim* pGridSite = (OnboardCellInSim*)mRectMap.GetTileAt( i );

		Path path = pGridSite->GetPath();
		Pollygon cellPollygon;
		cellPollygon.init(path.getCount(),path.getPointList());
		
		for (int iPoint = 0; iPoint < cellPollygon.getCount(); iPoint++)
		{
			if (pollygon.contains(cellPollygon.getPoint(iPoint)))
			{
				// logic level.
				pGridSite->SetState(OnboardCellInSim::FullOccupied);
				// algorithm level.
				mRectMap.setTileAsBarrier( i, true );
				break;
			}
		}
	}
}

void GroundInSim::setOutsideLocatorSitesAsBarriers(POLLYGONVECTOR& _deckOutLine)
{
	for (int i = 0; i < mRectMap.GetTileCount(); i++)
	{
		OnboardCellInSim* pGridSite = (OnboardCellInSim*)mRectMap.GetTileAt( i );

		
		Path path = pGridSite->GetPath();
		Pollygon cellPollygon;
		cellPollygon.init(path.getCount(),path.getPointList());

		if(contain(_deckOutLine,cellPollygon))
			continue;

		// logic level.
		pGridSite->SetState(OnboardCellInSim::FullOccupied);
		// algorithm level.
		mRectMap.setTileAsBarrier( i, true );
	}
}

void GroundInSim::initCellHoldMobile()
{
	setOccupyCellAsBarriers();
	calculateMobileCanWalkThrough();
}

void GroundInSim::setOccupyCellAsBarriers()
{
	for (int i = 0; i < mRectMap.GetTileCount(); i++)
	{
		OnboardCellInSim* pGridSite = (OnboardCellInSim*)mRectMap.GetTileAt( i );
		// logic level.
		if (pGridSite->GetState() == OnboardCellInSim::FullOccupied || pGridSite->GetState() == OnboardCellInSim::HalfOccupied)
		{
			// algorithm level.
			mRectMap.setTileAsBarrier( i, true );
		}
	}
}

void GroundInSim::calculateMobileCanWalkThrough()
{
	for (int i = 0; i < mRectMap.GetTileCount(); i++)
	{
		OnboardCellInSim* pGridSite = (OnboardCellInSim*)mRectMap.GetTileAt( i );
		// logic level.
		if (pGridSite->GetState() == OnboardCellInSim::FullOccupied || pGridSite->GetState() == OnboardCellInSim::HalfOccupied)
		{
			continue;
		}

		calculateCellHoldMobile(pGridSite);
	}
}
//N---S, W--E
void GroundInSim::calculateCellHoldMobile(OnboardCellInSim* pCellInSim)
{
	int iStartCell = pCellInSim->getTileIndex();
	//check N & S
	double dDistanceN = GetDistanceFromNDirection(iStartCell);
	double dDistanceS = GetDistanceFromSDirection(iStartCell);

	if (dDistanceN + dDistanceS < PAXRADIUS)
	{
		pCellInSim->SetState(OnboardCellInSim::SmallSpace);
		mRectMap.setTileAsBarrier( iStartCell, true );
	}

	if(dDistanceN < PAXRADIUS/2 || dDistanceS < PAXRADIUS/2)//the cell is available, but not valid for path
	{
		pCellInSim->SetState(OnboardCellInSim::NoPath);
		mRectMap.setTileAsBarrier( iStartCell, true );
	}

	//check W & E
	double dDistanceW = GetDistanceFromWDirection(iStartCell);
	double dDistanceE =GetDistanceFromEDirection(iStartCell);

	if (dDistanceW + dDistanceE < PAXRADIUS)
	{
		pCellInSim->SetState(OnboardCellInSim::SmallSpace);
		mRectMap.setTileAsBarrier( iStartCell, true );
	}

	if(dDistanceW < PAXRADIUS/2 || dDistanceE < PAXRADIUS/2)//the cell is available, but not valid for path
	{
		pCellInSim->SetState(OnboardCellInSim::NoPath);
		mRectMap.setTileAsBarrier( iStartCell, true );
	}
}

double GroundInSim::GetDistanceFromNDirection(int iStartCell)
{
	double dDist = GRID_HEIGHT/2;
	while(iStartCell != -1 && !IsCellBarrier(iStartCell))
	{
		dDist += GRID_HEIGHT;
		iStartCell = mRectMap.GetAdjacentN(iStartCell);
	}

	return dDist;
}
bool GroundInSim::IsCellBarrier( int nIndex ) const
{
	if(nIndex < 0)
		return true;

	OnboardCellInSim* pGridCell = (OnboardCellInSim*)mRectMap.GetTileAt( nIndex );
	if(pGridCell == NULL)
		return true;

	if(pGridCell->GetState() == OnboardCellInSim::FullOccupied ||
		pGridCell->GetState() == OnboardCellInSim::HalfOccupied)
		return true;

	return false;

}
double GroundInSim::GetDistanceFromSDirection(int iStartCell)
{
	double dDist = GRID_HEIGHT/2;
	while(iStartCell != -1 && !IsCellBarrier(iStartCell))
	{
		dDist += GRID_HEIGHT;
		iStartCell = mRectMap.GetAdjacentS(iStartCell);
	}

	return dDist;
}

double GroundInSim::GetDistanceFromWDirection(int iStartCell)
{
	double dDist = GRID_WIDTH/2;
	while(iStartCell != -1 && !IsCellBarrier(iStartCell))
	{
		dDist += GRID_WIDTH;
		iStartCell = mRectMap.GetAdjacentW(iStartCell);
	}

	return dDist;
}

double GroundInSim::GetDistanceFromEDirection(int iStartCell)
{
	double dDist = GRID_WIDTH/2;
	while(iStartCell != -1 && !IsCellBarrier(iStartCell))
	{
		dDist += GRID_WIDTH;
		iStartCell = mRectMap.GetAdjacentE(iStartCell);
	}

	return dDist;
}

bool GroundInSim::contain(POLLYGONVECTOR& _deckOutLine,const Pollygon& pollygon)const
{
	for (int iPollygon = 0; iPollygon < (int)_deckOutLine.size(); iPollygon++)
	{
		Pollygon& polly = _deckOutLine[iPollygon];
		if (polly.contains(pollygon))
		{
			return true;
		}
	}

	return false;
}
OnboardCellInSim* GroundInSim::getCell(int nTileIndex) 
{
	if(nTileIndex > mRectMap.GetTileCount() - 1)
		return NULL;

	return (OnboardCellInSim*)mRectMap.GetTileAt( nTileIndex );
}
OnboardCellInSim* GroundInSim::GetPointCell( CPoint2008& location )
{
	OnboardCellInSim* pCellInSim = NULL;
	pCellInSim = (OnboardCellInSim*)mRectMap.GetTileAt( location.getX(), location.getY());
	return pCellInSim;
}	
OnboardCellInSim* GroundInSim::getCell(CPoint2008& location) const
{
	OnboardCellInSim* pCellInSim = (OnboardCellInSim*)mRectMap.GetTileAt( location.getX(), location.getY());
	if (pCellInSim)
	{
		return pCellInSim;
	}

	return getBoundCell(location);
}

OnboardCellInSim* GroundInSim::getBoundCell(CPoint2008& location) const
{
	return (OnboardCellInSim*)mRectMap.GetBoundTitleAt( location.getX(), location.getY());
}

void GroundInSim::OnPathFound(const std::vector<int>& thePath)
{

}

void GroundInSim::GetPath( OnboardCellInSim* pStartCell, OnboardCellInSim* pEndCell, OnboardCellPath& _movePath )
{
	TRACE("\r\n FIND PATH, FROM %d, To %d",pStartCell->getTileIndex(), pEndCell->getTileIndex());


	mRectMap.mPath = mPathFinder.GetPath(&mRectMap, pStartCell->getTileIndex(),
		pEndCell->getTileIndex());

	for( std::vector<int>::reverse_iterator iter = mRectMap.mPath.rbegin();
		iter != mRectMap.mPath.rend(); iter++)
	{
		long nTileIndex = *iter;
		OnboardCellInSim* pMoveCell = (OnboardCellInSim*)mRectMap.GetTileAt( nTileIndex );
		_movePath.addCell( pMoveCell );
	}

}

OnboardCellInSim* GroundInSim::GetClosestAvailableCell( CPoint2008& location )
{
	OnboardCellInSim *pClosestCell = NULL;
	double  dDistance = (std::numeric_limits<double>::max)();
	int nCellCount = mRectMap.GetTileCount();
	for (int nCell = 0; nCell < nCellCount; ++ nCell)
	{
		OnboardCellInSim* pCell = (OnboardCellInSim*)mRectMap.GetTileAt( nCell );
		if(pCell)
		{
			double nCurDist = pCell->getLocation().distance(location);
			if(nCurDist < dDistance)
			{
				dDistance = nCurDist;
				pClosestCell = pCell;
			}
		}

	}
	return pClosestCell;
}

std::vector<OnboardCellInSim *> GroundInSim::GetNeighbourCells( OnboardCellInSim *pCenterCell ) const
{
	std::vector<OnboardCellInSim *> neighbourCells;
	if(pCenterCell != NULL)
	{
		int theCenterTile = pCenterCell->getTileIndex();
		std::vector<int> theReturnList;
		//mRectMap.GetAdjacentTiles(pCenterCell->getTileIndex() , theReturnList );

		int nIndex= -1;
		nIndex = mRectMap.GetAdjacentN(theCenterTile);
		if ( nIndex >= 0 && !IsCellBarrier(nIndex))
			theReturnList.push_back(nIndex);

		nIndex = mRectMap.GetAdjacentE(theCenterTile);
		if (nIndex >= 0 && !IsCellBarrier(nIndex))
			theReturnList.push_back(nIndex);
	
		nIndex = mRectMap.GetAdjacentS(theCenterTile);
		if (mRectMap.GetAdjacentS(theCenterTile) >= 0 && !IsCellBarrier(nIndex))
			theReturnList.push_back(nIndex);

		nIndex = mRectMap.GetAdjacentW(theCenterTile);
		if (mRectMap.GetAdjacentW(theCenterTile) >= 0 && !IsCellBarrier(nIndex))
			theReturnList.push_back(nIndex);

		nIndex = mRectMap.GetAdjacentNE(theCenterTile);
		if (mRectMap.GetAdjacentNE(theCenterTile) >= 0 && !IsCellBarrier(nIndex))
			theReturnList.push_back(nIndex);

		nIndex = mRectMap.GetAdjacentNW(theCenterTile);
		if (mRectMap.GetAdjacentNW(theCenterTile) >= 0 && !IsCellBarrier(nIndex))
			theReturnList.push_back(nIndex);

		nIndex = mRectMap.GetAdjacentSE(theCenterTile);
		if (mRectMap.GetAdjacentSE(theCenterTile) >= 0 && !IsCellBarrier(nIndex))
			theReturnList.push_back(nIndex);

		nIndex = mRectMap.GetAdjacentSW(theCenterTile);
		if (mRectMap.GetAdjacentSW(theCenterTile) >= 0 && !IsCellBarrier(nIndex))
			theReturnList.push_back(nIndex);

		std::vector<int>::iterator iter = theReturnList.begin();
		for(; iter != theReturnList.end(); ++iter)
		{
			neighbourCells.push_back((OnboardCellInSim*)mRectMap.GetTileAt( *iter ));
		}
	}
	return neighbourCells;	
}

bool GroundInSim::CalculateLocation( CPoint2008 ptLocation, CPoint2008 ptDirection, double dLength, double dWidth, PaxCellLocation & paxLocation ) const
{
	//the passenger has more cell to move
	//
	paxLocation.Clear();
	const GroundInSim *pGround = this;

	//get passenger's location
	//
	//	double dPaxLength = 20.0;
	//	double dPaxWidth = 40.0;

	double zPos = ptLocation.getZ();
	std::vector<CPoint2008> vPaxSpace;

	CPoint2008 ptAngle =  ptDirection;
	ptAngle.Normalize();

	CPoint2008 ptFront = ptAngle;
	ptFront.length(dLength/2);

	CPoint2008 ptLeft  = ptAngle.perpendicular();
	ptLeft.length(dWidth/2);

	CPoint2008 ptBack  = ptAngle * (-1);
	ptBack.length(dLength/2);

	CPoint2008 ptRight = ptBack.perpendicular();
	ptRight.length(dWidth/2);

	//get the passenger' rect
	CPoint2008 ptLeftFront = ptFront + ptLeft;
	CPoint2008 ptLeftBack  = ptLeft + ptBack;
	CPoint2008 ptRightBack = ptBack + ptRight;
	CPoint2008 ptRightFront = ptRight + ptFront;

	vPaxSpace.push_back(ptLeftFront);
	vPaxSpace.push_back(ptLeftBack);
	vPaxSpace.push_back(ptRightBack);
	vPaxSpace.push_back(ptRightFront);


	CPath2008 paxSpace;
	paxSpace.init(4,&vPaxSpace[0]);

	//move to the cell
	paxSpace.DoOffset(ptLocation.getX(), ptLocation.getY(),0);

	//check the Rect is valid
	OnboardCellInSim *pStartCell = pGround->getCell(paxSpace[0]);
	OnboardCellInSim *pEndCell = pGround->getCell(paxSpace[3]);

	//center cell
	OnboardCellInSim *pCenterCell = pGround->getCell(ptLocation);

	//start
	int nStartCell = -1;
	if(pStartCell)
		nStartCell = pStartCell->getTileIndex();

	//right top cell
	int nRightTopCell = -1;
	OnboardCellInSim* pRightTopCell = pGround->getCell(paxSpace[1]);
	if(pRightTopCell)
		nRightTopCell= pRightTopCell->getTileIndex();

	//left bottom cell
	int nLeftBottomCell = -1;
	OnboardCellInSim* pLeftBottomCell = pGround->getCell(paxSpace[2]);
	if(pLeftBottomCell)
		nLeftBottomCell= pLeftBottomCell->getTileIndex();

	//end
	int nEndCell = -1;
	if(pEndCell)
		nEndCell = pEndCell->getTileIndex();

	if(nStartCell >= 0 &&
		nRightTopCell >=0 &&
		nLeftBottomCell >=0 &&
		nEndCell >= 0
		)
	{
		// return this passenger location
		std::vector<OnboardCellInSim *> vCellsOccupied;			
		std::vector<OnboardCellInSim *> vCellsNeedToCheck;
		std::vector<OnboardCellInSim *> vCellsChecked;

		CPollygon2008 paxPollygon;
		paxPollygon.init(paxSpace.getCount(),paxSpace.getPointList());
		double dDistanceMax = sqrt((0.5 * dLength) * (0.5 * dLength) + (0.5 * dWidth) *(0.5 *dWidth));
		{
			//center cell
			vCellsOccupied.push_back(pCenterCell);

			//get all neighborhood cells
			vCellsNeedToCheck = pGround->GetNeighbourCells(pCenterCell);

			while (vCellsNeedToCheck.size() > 0)
			{	
				std::vector<OnboardCellInSim *> vNeighbourCells;

				std::vector<OnboardCellInSim *>::iterator iterCheck = vCellsNeedToCheck.begin();
				for (; iterCheck != vCellsNeedToCheck.end(); ++iterCheck)
				{
					//check the cells
					OnboardCellInSim *pTmpCell = *iterCheck;
					if(pTmpCell)
					{
						if(std::find(vCellsChecked.begin(), vCellsChecked.end(), pTmpCell) != vCellsChecked.end())
							continue;

						vCellsChecked.push_back(pTmpCell);

						if(paxPollygon.contains2( pTmpCell->getLocation()))
						{
							paxLocation.addCell(pTmpCell);
							std::vector<OnboardCellInSim *> tempCells = pGround->GetNeighbourCells(pTmpCell);
							vNeighbourCells.insert(vNeighbourCells.end(), tempCells.begin(), tempCells.end());
						}
					}
				}
				vCellsNeedToCheck = vNeighbourCells;
			}
		}

		//center cell of passenger
		paxLocation.setCellCenter(pCenterCell);

		//set path cell in the route
		paxLocation.setPathCell(pCenterCell);

		//the passenger's rect
		paxLocation.setPaxRect(paxSpace);

		return true;

	}
	else
		return false;
}

void GroundInSim::getFreeSpace(PaxOnboardBaseBehavior *pBehavior, int x0, int y0, int x1, int y1, int& endx, int& endy ) const
{
	int dx = x1-x0; int dy = y1- y0; int e=-dx;
	endx = x0;
	endy = y0;

	int x=x0;  int y=y0;
//InterBresenhamline
	for (int i=0; i<dx; i++)
	{
		//draw pixel (x, y, color);
		OnboardCellInSim* pCell = ( OnboardCellInSim* )mRectMap.GetTileAt(x, y) ;
		if( pCell && pCell->IsAvailable(pBehavior) )
		{
			endx = x; endy = y;
		}
		else 
			return;

		x++;
		e=e+2*dy;

		if (e>0) { y++; e=e-2*dx;}
	}
}


void GroundInSim::getCellPos( const CPoint2008& ptLocation, int& x, int& y )const
{
	mRectMap.GetTileIndex(ptLocation.x, ptLocation.y, x,y);
}

double GroundInSim::GetDirValidDist( PaxOnboardBaseBehavior *pBehavior,const CPoint2008&ptLocation  ,const CPoint2008& dir, double dMaxLen ) const
{
	int x0,y0;
	int x1, y1;
	int endx , endy;
	//CPoint2008 ptLocation = pFromCell->getLocation();
	getCellPos(ptLocation, x0,y0);
	getCellPos(ptLocation+dir*dMaxLen, x1,y1);

	getFreeSpace(pBehavior,x0,y0, x1,y1, endx,endy);
	
	OnboardCellInSim* pFromCell =( OnboardCellInSim* )mRectMap.GetTileAt(x0, y0);
	OnboardCellInSim* pToCell = ( OnboardCellInSim* )mRectMap.GetTileAt(endx, endy);
	ASSERT(pToCell);
	if(pFromCell && pToCell)
		return pFromCell->GetDistanceTo(pToCell);
	return 0;

}