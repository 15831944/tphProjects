#include "StdAfx.h"
#include "carrierDoor.h"
#include "carrierGround.h"

carrierDoor::carrierDoor(LocatorSite* pLocatorSite,carrierGround& _carreirGound, carrierDoorType emType)
:carrierElement( pLocatorSite,_carreirGound)
,m_emType(emType)
{
}

carrierDoor::~carrierDoor(void)
{
}


carrierEntryDoor::carrierEntryDoor(LocatorSite* pLocatorSite,carrierGround& _carreirGound)
:carrierDoor( pLocatorSite,_carreirGound, EntryCarrierDoor)
{
}

carrierExitDoor::carrierExitDoor(LocatorSite* pLocatorSite,carrierGround& _carreirGound)
:carrierDoor( pLocatorSite,_carreirGound, ExitCarrierDoor)
{
}

carrierEmergencyDoor::carrierEmergencyDoor(LocatorSite* pLocatorSite,carrierGround& _carreirGound)
:carrierDoor( pLocatorSite,_carreirGound, EmergencyCarrierDoor)
{
}

