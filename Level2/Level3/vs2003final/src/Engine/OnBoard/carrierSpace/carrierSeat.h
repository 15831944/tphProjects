#pragma once
#include "CarrierElement.h"

class CSeat;
class MobileAgent;
class carrierSeat : public carrierElement
{
public:
	carrierSeat(LocatorSite* pLocatorSite,carrierGround& _carreirGound, CSeat* pLayout);
	~carrierSeat(void);

public:
	void setOccupant( MobileAgent* pMobAgent);
	MobileAgent* getOccupant();

	void setOccupied(){ mIsOccupied = true; }
	bool isOccupied(){ return mIsOccupied;}

	void setEntryLocatorSite(LocatorSite* pEntry);
	LocatorSite* getEntryLocatorSite();

	Point getLocation();
	Point getEntryLocation();

protected:
	virtual void calculateSpace();
	


private:
	CSeat* m_pLayout;
	LocatorSite* m_pEntryLocatorSite;
	MobileAgent* mOccupant;
	bool mIsOccupied;
};
