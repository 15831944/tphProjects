#pragma once
#include "CarrierElement.h"

class carrierDoor : public carrierElement
{	
public:
	enum carrierDoorType 
	{
		EntryCarrierDoor = 0,
		ExitCarrierDoor,
		EmergencyCarrierDoor
	};

public:
	carrierDoor(LocatorSite* pLocatorSite,carrierGround& _carreirGound, carrierDoorType emType);
	~carrierDoor(void);

	virtual void calculateSpace(){};
private:
	carrierDoorType m_emType;
};

class carrierEntryDoor : public carrierDoor
{
public:
	carrierEntryDoor(LocatorSite* pLocatorSite,carrierGround& _carreirGound);

};

class carrierExitDoor : public carrierDoor
{
public:
	carrierExitDoor(LocatorSite* pLocatorSite,carrierGround& _carreirGound);

};

class carrierEmergencyDoor : public carrierDoor
{
public:
	carrierEmergencyDoor(LocatorSite* pLocatorSite,carrierGround& _carreirGound);

};


