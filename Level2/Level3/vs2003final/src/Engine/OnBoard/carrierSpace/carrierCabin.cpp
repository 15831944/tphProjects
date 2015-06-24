#include "StdAfx.h"
#include "carrierCabin.h"
#include "carrierGround.h"

carrierCabin::carrierCabin(LocatorSite* pLocatorSite,carrierGround& _carreirGound)
:carrierElement( pLocatorSite, _carreirGound )
{
}

carrierCabin::~carrierCabin(void)
{
}

void carrierCabin::calculateSpace()
{

}