#pragma once
/**
* Class to provide carrier elements space info.
*/
#include "../OnBoardDefs.h"
#include "../LocatorSite.h"

typedef UnsortedContainer< LocatorSite > carrierElemSpace;
class carrierGround;
class carrierElement
{
public:
	carrierElement(LocatorSite* pLocatorSite,carrierGround& _carreirGound);
	~carrierElement(void);

	LocatorSite* getLocatorSite();
	CPoint2008 getLocation();
	
	carrierElemSpace& getSpace( void );
	
protected:
	virtual void calculateSpace() = 0;
	LocatorSite * m_pLocatorSite;

	double mTileWidth;
	double mTileHeight;

	// space parameters
	//
	carrierGround& mGround;
	carrierElemSpace mSpace;
};
