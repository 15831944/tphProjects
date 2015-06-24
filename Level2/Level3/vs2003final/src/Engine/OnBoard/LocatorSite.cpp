#include "StdAfx.h"
#include "LocatorSite.h"
#include "MobileAgent.h"
#include "../SimEngineConfig.h"

using namespace OBGeo;

LocatorSite::LocatorSite(long theTileIndex, OBGeo::FRect& aTile, double deckHeight)
:FRect(aTile)
,mTileIndex(theTileIndex)
,mIsBarrier(false)
,mLockCount(0)
{

	mLocation.init(FRect::mX + 0.5*FRect::mWidth,  FRect::mY + 0.5*FRect::mHeight, deckHeight);

}

LocatorSite::~LocatorSite()
{

}

bool LocatorSite::requestService(MobileAgent* pMobAgent)
{
	// avoid lock-in tile.
	if(simEngineConfig()->getUseLockInTile() != 1)
		return true;

	if(!isIdel())
		return false;
	lock();

	return true;
}

void LocatorSite::makeAvailable(ElapsedTime p_time)
{
	// avoid lock-in tile.
	if(simEngineConfig()->getUseLockInTile() != 1)
		return;

	unlock();
}

bool LocatorSite::isIdel()
{
	return mLockCount == 0; 
}

void LocatorSite::lock()
{
	mLockCount = 1;
	//mLockCount++; ASSERT( mLockCount > 0 ); 
}

void LocatorSite::unlock()		
{
	mLockCount = 0;

	//mLockCount--; ASSERT( mLockCount >= 0 );
}

//-------------------------------------------------------------------------------
//

MobAgentMovementPath::MobAgentMovementPath(void)
{

}

MobAgentMovementPath::~MobAgentMovementPath(void)
{

}

long MobAgentMovementPath::addLocatorSite(LocatorSite* pMoveSite)
{
	addItem( pMoveSite );
	return getCount();
}

LocatorSite* MobAgentMovementPath::getNextLocatorSite()
{
	if(getCount() == 0)
		return NULL;

	LocatorSite* pNextSite = getItem( 0 );
	removeItem( 0 );

	return pNextSite;
}

void MobAgentMovementPath::addWaitingLocatorSite(LocatorSite* pMoveSite)
{
	ASSERT( pMoveSite != NULL );
	insert( begin(), 1, pMoveSite);
}



