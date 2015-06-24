#pragma once
#include "CarrierElement.h"

class carrierStorage : public carrierElement
{
public:
	carrierStorage(LocatorSite* pLocatorSite,carrierGround& _carreirGound);
	~carrierStorage(void);

protected:
	virtual void calculateSpace();
};
