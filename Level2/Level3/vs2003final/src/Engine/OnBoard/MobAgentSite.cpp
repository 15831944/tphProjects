#include "StdAfx.h"
#include "MobAgentSite.h"
#include "MobileAgent.h"

using namespace OBGeo;

MobAgentSite::MobAgentSite(long theTileIndex, OBGeo::FRect& aTile, double deckHeight)
:FRect(aTile)
,mTileIndex(theTileIndex)
,mLockCount(0)
{

	mLocation.init(FRect::mX + 0.5*FRect::mWidth,  FRect::mY + 0.5*FRect::mHeight, deckHeight);

}

MobAgentSite::~MobAgentSite()
{

}

bool MobAgentSite::requestService(MobileAgent* pMobAgent)
{
	if(!isIdel())
		return false;
	lock();

	return true;
}

void MobAgentSite::makeAvailable(ElapsedTime p_time)
{
	unlock();
}


//-------------------------------------------------------------------------------
//

MobAgentMovementPath::MobAgentMovementPath(void)
{

}

MobAgentMovementPath::~MobAgentMovementPath(void)
{

}

long MobAgentMovementPath::addCrossingSite(MobAgentSite* pMoveSite)
{
	addItem( pMoveSite );
	return getCount();
}

MobAgentSite* MobAgentMovementPath::getNextCrossingSite()
{
	if(getCount() == 0)
		return NULL;

	MobAgentSite* pNextSite = getItem( 0 );
	removeItem( 0 );

	return pNextSite;
}

