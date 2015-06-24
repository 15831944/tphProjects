#pragma once
#include "Common/Point2008.h"
#include "Common/elaptime.h"
#include "PathFinding/OBRect.h"
#include "Common/containr.h"

class MobileAgent;
class MobAgentSite : public OBGeo::FRect
{
public:
	MobAgentSite(long theTileIndex, OBGeo::FRect& aTile, double deckHeight);
	~MobAgentSite();

public:
	
	CPoint2008 getLocation() { return mLocation; }
	long getTileIndex(){ return mTileIndex; }

	bool requestService( MobileAgent* pMobAgent);
	void makeAvailable( ElapsedTime p_time);

	bool isIdel()		{ return mLockCount == 0; }

private:
	long			mTileIndex;
	CPoint2008		mLocation;


private:
	void lock()			{ mLockCount++; ASSERT( mLockCount > 0 ); }
	void unlock()		{ mLockCount--; ASSERT( mLockCount >= 0 );}


	int				mLockCount;

};

class LocationSite : public MobAgentSite
{
public:
};

//-------------------------------------------------------------------------------
//
class MobAgentSiteList : public UnsortedContainer < MobAgentSite >
{
public:
	MobAgentSiteList(void);
	~MobAgentSiteList(void);

	long addMobAgentSite(MobAgentSite* pAgent);
};


class MobAgentMovementPath : public UnsortedContainer < MobAgentSite >
{
public:
	MobAgentMovementPath(void);
	~MobAgentMovementPath(void);

	long addCrossingSite(MobAgentSite* pMoveSite);

	MobAgentSite* getNextCrossingSite();

};



