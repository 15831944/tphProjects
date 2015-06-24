#pragma once
#include "CarrierElement.h"

class carrierStair : public carrierElement
{
public:
	carrierStair(LocatorSite* pLocatorSite,carrierGround& _carreirGound);
	~carrierStair(void);

protected:
	virtual void calculateSpace();
};
