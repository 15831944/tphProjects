#pragma once
/**
	Class to provide barrier functionality on deck.
	Misc device element does not need interact with Mobile Agents.
	eg: electronic device on aircraft.
*/
#include "CarrierElement.h"

class carrierMiscDevice : public carrierElement
{
public:
	carrierMiscDevice(LocatorSite* pLocatorSite,carrierGround& _carreirGound);
	~carrierMiscDevice(void);

protected:
	virtual void calculateSpace();
};
