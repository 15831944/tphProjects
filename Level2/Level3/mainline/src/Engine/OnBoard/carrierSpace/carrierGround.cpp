#include "StdAfx.h"
#include "../../SimEngineConfig.h"
#include "carrierGround.h"
#include "../LocatorSite.h"
#include "Common/pollygon.h"
#include "../MobileAgent.h"
#include "carrierDoor.h"

using namespace OBGeo;

carrierGround::carrierGround()
:mHeight(0.0)
,m_pEntryDoor(NULL)
,m_pExitDoor(NULL)
{
	mRectMap.setTravelsalDirectlyCost( simEngineConfig()->getTravelsalDirectlyCost() );
	mRectMap.setTravelsalDiagonalCost( simEngineConfig()->getTravelsalDiagonalCost() );

	mPathFinder.SetMap( &mRectMap );
	mPathFinder.SetListener( this );
}

carrierGround::~carrierGround(void)
{
	delete m_pEntryDoor;
	delete m_pExitDoor;

	mSeatList.Flush(TShouldDelete::Delete);
}


void carrierGround::setOutsideLocatorSitesAsBarriers(ARCPath& _deckOutLine)
{
	Point* ptList = new Point[_deckOutLine.size()];
	for (size_t i=0; i<_deckOutLine.size(); i++)
	{
		ARCPoint2 point = _deckOutLine.at(i);
		ptList[i].init( point[VX], point[VY], point[VZ]);
	}
	Pollygon _pollygonHelper;
	_pollygonHelper.init( _deckOutLine.size(), ptList );
	delete[] ptList;

	for (int i = 0; i < mRectMap.GetTileCount(); i++)
	{
		LocatorSite* pLocatorSite = (LocatorSite*)mRectMap.GetTileAt( i );

		Point _location;
		_location.init(pLocatorSite->getLocation().getX(),
			pLocatorSite->getLocation().getY(),pLocatorSite->getLocation().getZ());

		if( _pollygonHelper.contains( _location) )
			continue;

		// logic level.
		pLocatorSite->setAsBarrier();
		// algorithm level.
		mRectMap.setTileAsBarrier( i, true );
	}

}

void carrierGround::addCarrierSeat(carrierSeat *pCarrierSeat)
{
	ASSERT( pCarrierSeat != NULL );
	mSeatList.addItem( pCarrierSeat );

	setCarrierElementBarriers( pCarrierSeat);
}

void carrierGround::setCarrierElementBarriers(carrierElement* pCarrierElement)
{
	// set position placed site as barrier
	int nLocationTileIndex = 
		pCarrierElement->getLocatorSite()->getTileIndex();
	mRectMap.setTileAsBarrier( nLocationTileIndex, true);

	// set element space locator sites as barriers.
	carrierElemSpace& elemSpace = pCarrierElement->getSpace();
	for (int i = 0 ; i < elemSpace.getCount(); i++)
	{
		LocatorSite* pBarrierSite = elemSpace.getItem( i );
		pBarrierSite->setAsBarrier();
		mRectMap.setTileAsBarrier( nLocationTileIndex, true);
	}
}

/**
	While MobAgent do Path Finding process, need set owner seat state as Non barrier.
	when done. reset to barrier state.
	Note. donot need change seat's space locator sites barrier state.
	just change RectMap at algorithm level.
**/
void carrierGround::swapCarrierSeatBarrierState(carrierSeat* pCarrierSeat, bool isBarrier)
{
	// set position placed site as barrier
	/*int nLocationTileIndex = 
		pCarrierSeat->getLocatorSite()->getTileIndex();
	mRectMap.setTileAsBarrier( nLocationTileIndex, isBarrier);*/
	mRectMap.setTileAsBarrier( pCarrierSeat->getEntryLocatorSite()->getTileIndex(), isBarrier);

	// set element space locator sites as barriers.
	//carrierElemSpace& elemSpace = pCarrierSeat->getSpace();
	//for (int i = 0 ; i < elemSpace.getCount(); i++)
	//{
	//	LocatorSite* pBarrierSite = elemSpace.getItem( i );
	//	pBarrierSite->setAsBarrier();
	//	mRectMap.setTileAsBarrier( nLocationTileIndex, isBarrier);
	//}
}

void carrierGround::generateGround(OBGeo::FRect& _outBoundRect)
{
	// PathFinding parameter
	int nTileWidth = simEngineConfig()->getOnBoardModeTileWidth();
	int nTileHeight = simEngineConfig()->getOnBoardModeTileHeight();

	//maximal tolerance: 10 centimeter. <TODO::> avoid tolerance.
	int nGridCols = (int)_outBoundRect.mWidth  / nTileWidth;
	int nGridRows = (int)_outBoundRect.mHeight / nTileHeight;

	mRectMap.ResizeMap( nGridCols, nGridRows, nTileWidth, nTileHeight);

	DistanceUnit _startX = _outBoundRect.mX, _startY = _outBoundRect.mY;
	long theTileIndex = 0;
    for (int i = 0 ; i < nGridRows; i++)
	{
		for (int j = 0 ; j < nGridCols; j++)
		{
			OBGeo::FRect aTileRect(_startX, _startY, nTileWidth, nTileHeight);

			LocatorSite* pNewLocatorSite = new LocatorSite(theTileIndex, aTileRect, mHeight);
			mRectMap.AddTileRect( pNewLocatorSite, theTileIndex);
			theTileIndex++;
			
			_startX += nTileWidth;
		}
		_startX = _outBoundRect.mX;
		_startY += nTileHeight;
	}

}

void carrierGround::getAssociablePath(LocatorSite* pStartSite, LocatorSite* pEndSite, 
									  MobAgentMovementPath& _movePath)
{
	mRectMap.mPath = mPathFinder.GetPath(&mRectMap, pStartSite->getTileIndex(),
		pEndSite->getTileIndex()/*, <TODO::> Mobile Agent's space data*/);

	for( std::vector<int>::reverse_iterator iter = mRectMap.mPath.rbegin();
		iter != mRectMap.mPath.rend(); iter++)
	{
		long nTileIndex = *iter;
		LocatorSite* pMoveSite = (LocatorSite*)mRectMap.GetTileAt( nTileIndex );
		_movePath.addLocatorSite( pMoveSite );
	}

}

// Asynchronous PathFinding result
void carrierGround::OnPathFound(const std::vector<int>& thePath)
{

}

LocatorSite* carrierGround::getLocatorSite(CPoint2008& location)
{
	return (LocatorSite*)mRectMap.GetTileAt( location.getX(), location.getY());
}

LocatorSite* carrierGround::getLocatorSite(int nTileIndex)
{
	if(nTileIndex > mRectMap.GetTileCount() - 1)
		return NULL;

	return (LocatorSite*)mRectMap.GetTileAt( nTileIndex );
}

int carrierGround::getLocatorSiteCount()
{
	return mRectMap.GetTileCount();
}

void carrierGround::setMobAgentIntelligence(MobileAgent* pMobAgent)
{
	pMobAgent->assignIntelligence( &mPathFinder );
}

void carrierGround::addCarrierExitDoors(carrierExitDoor* pDoor)
{
	ASSERT( pDoor != NULL );
	m_pExitDoor = pDoor;
}

void carrierGround::addCarrierEntryDoors(carrierEntryDoor* pDoor)
{
	ASSERT( pDoor != NULL );
	m_pEntryDoor = pDoor;
}

carrierEntryDoor* carrierGround::getCarrierEntryDoor()
{
	return m_pEntryDoor;
}

carrierExitDoor* carrierGround::getCarrierExitDoor()
{
	return m_pExitDoor;
}
















