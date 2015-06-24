#pragma once
#include "CarrierElement.h"

class carrierCabin : public carrierElement
{
public:
	carrierCabin(LocatorSite* pLocatorSite,carrierGround& _carreirGound);
	~carrierCabin(void);

protected:
	virtual void calculateSpace();
};
