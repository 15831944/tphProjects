#include "StdAfx.h"
#include "carrierStorage.h"
#include "carrierGround.h"

carrierStorage::carrierStorage(LocatorSite* pLocatorSite,carrierGround& _carreirGound)
:carrierElement( pLocatorSite, _carreirGound)
{
}

carrierStorage::~carrierStorage(void)
{
}


void carrierStorage::calculateSpace()
{

}