#include "StdAfx.h"
#include "CarrierSeat.h"
#include "InputOnBoard/Seat.h"
#include "carrierGround.h"
#include "../PathFinding/RectMap.h"
#include "Engine/SimEngineConfig.h"

carrierSeat::carrierSeat(LocatorSite* pLocatorSite,carrierGround& _carreirGound, CSeat* pLayout)
:carrierElement( pLocatorSite, _carreirGound )
,m_pLayout( pLayout )
,mOccupant(NULL)
,m_pEntryLocatorSite(NULL)
,mIsOccupied(false)
{
	ASSERT(pLocatorSite != NULL );
	ASSERT( pLayout != NULL);

	calculateSpace(); 

}

carrierSeat::~carrierSeat(void)
{
}

void carrierSeat::setEntryLocatorSite(LocatorSite* pEntry)
{
	ASSERT( pEntry != NULL );
	m_pEntryLocatorSite = pEntry;
}

LocatorSite* carrierSeat::getEntryLocatorSite()
{
	return m_pEntryLocatorSite;
}

Point carrierSeat::getLocation()
{
	Point ptLocation;

	// assign MobAgent model with seat model.
	ARCVector3 Location;
	ARCVector2 vDir(1,0);
	vDir.Rotate( -m_pLayout->GetRotation());
	Location =  m_pLayout->GetPosition() + ARCVector3( vDir*m_pLayout->GetLength()*.2,  0);
	
	ptLocation.setPoint( Location[VX], Location[VY], mGround.getHeight());
	return ptLocation;
}


Point carrierSeat::getEntryLocation()
{
	Point ptLocation;
	ARCVector3 Location = m_pLayout->GetEntryPos();
	ptLocation.setPoint( Location[VX], Location[VY], mGround.getHeight());
	return ptLocation;
}


/**
	by now, does not consider seat's rotation

		|_|_|
		|_|_|
**/

static void ARCVector3ToPoint2008(ARCVector3 v3, CPoint2008& ptResult)
{
	ptResult.setPoint( v3[VX], v3[VY], v3[VZ]);
}

void carrierSeat::calculateSpace()
{
	//debug purpose.
	if(simEngineConfig()->getUseExtendBarrier() == 0)
		return;

	CPoint2008 ptLeftTop,ptRightTop,ptLeftBottom,ptRightBottom;
	ARCVector3ToPoint2008(m_pLayout->getLeftTop(),ptLeftTop);
	ARCVector3ToPoint2008(m_pLayout->getRightTop(),ptRightTop);
	ARCVector3ToPoint2008(m_pLayout->getLeftBottom(),ptLeftBottom);
	ARCVector3ToPoint2008(m_pLayout->getRightBottom(),ptRightBottom);

	RectMap& _Gndmap = mGround.getLogicMap();
	int TileWidth = _Gndmap.GetTileWidth();
	int TileHeight = _Gndmap.GetTileHeight();

	//int nEastIndex = _Gndmap.GetAdjacentE( m_pLocatorSite->getTileIndex() );
	//if(nEastIndex)
	//{
	//	LocatorSite* pLocator = mGround.getLocatorSite( nEastIndex );
	//	if(pLocator->Contains( ))
	//	mSpace.addItem( );
	//}
	//
	//int nSouthIndex = _Gndmap.GetAdjacentS( m_pLocatorSite->getTileIndex() );
	//if(nSouthIndex)
	//	mSpace.addItem( mGround.getLocatorSite( nSouthIndex ));
	//
	//int nWestIndex = _Gndmap.GetAdjacentW( m_pLocatorSite->getTileIndex() );
	//if(nWestIndex)
	//	mSpace.addItem( mGround.getLocatorSite( nWestIndex ));
	//
	//int nNorthIndex = _Gndmap.GetAdjacentN( m_pLocatorSite->getTileIndex() );
	//if(nNorthIndex)
	//	mSpace.addItem( mGround.getLocatorSite( nNorthIndex ));

}

void carrierSeat::setOccupant(MobileAgent* pMobAgent)
{
	//ASSERT( pMobAgent != NULL );
	mOccupant = pMobAgent;
}

MobileAgent* carrierSeat::getOccupant()
{
	return mOccupant;
}


