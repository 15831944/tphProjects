#include "StdAfx.h"
#include "carrierElement.h"
#include "../../SimEngineConfig.h"
#include "carrierGround.h"

carrierElement::carrierElement(LocatorSite* pLocatorSite,carrierGround& _carreirGound)
:m_pLocatorSite(pLocatorSite)
,mGround( _carreirGound )
{
	mTileWidth = simEngineConfig()->getOnBoardModeTileWidth();
	mTileHeight = simEngineConfig()->getOnBoardModeTileHeight();
}

carrierElement::~carrierElement(void)
{
}

LocatorSite* carrierElement::getLocatorSite()
{
	ASSERT( m_pLocatorSite != NULL );
	return m_pLocatorSite;
}

carrierElemSpace& carrierElement::getSpace( void )
{
	return mSpace;
}

CPoint2008 carrierElement::getLocation()
{
	ASSERT( m_pLocatorSite != NULL );
	return m_pLocatorSite->getLocation();
}




