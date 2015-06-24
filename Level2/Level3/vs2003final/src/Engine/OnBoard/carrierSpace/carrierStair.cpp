#include "StdAfx.h"
#include "carrierStair.h"
#include "carrierGround.h"

carrierStair::carrierStair(LocatorSite* pLocatorSite,carrierGround& _carreirGound)
:carrierElement( pLocatorSite, _carreirGound )
{
}

carrierStair::~carrierStair(void)
{
}

void carrierStair::calculateSpace()
{

}