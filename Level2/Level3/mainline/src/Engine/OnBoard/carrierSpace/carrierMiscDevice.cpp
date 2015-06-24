#include "StdAfx.h"
#include "carrierMiscDevice.h"
#include "carrierGround.h"

carrierMiscDevice::carrierMiscDevice(LocatorSite* pLocatorSite,carrierGround& _carreirGound)
:carrierElement( pLocatorSite, _carreirGound )
{
}

carrierMiscDevice::~carrierMiscDevice(void)
{
}


void carrierMiscDevice::calculateSpace()
{

}