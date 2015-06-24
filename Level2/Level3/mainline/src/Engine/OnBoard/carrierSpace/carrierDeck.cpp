#include "StdAfx.h"
#include "carrierDeck.h"
#include "carrierDoor.h"
#include "../MobileAgent.h"
#include "../LocatorSite.h"

carrierDeck::carrierDeck(void)
{
}

carrierDeck::~carrierDeck(void)
{
}

carrierGround& carrierDeck::getCarrierGround()
{
	return mGround;
}

bool carrierDeck::isValid()
{
	// has seat
	// agents has movement space. etc

	return true;
}

bool carrierDeck::getMovementPath(MobileAgent* pMobAgent, MobAgentMovementPath& _movePath)
{
	mGround.getAssociablePath( pMobAgent->getLocatorSite(), pMobAgent->getFinalLocatorSite()
		, _movePath);

	return _movePath.getCount() > 1;
}



