#pragma once
#include "OnBoardDefs.h"
#include "PathFinding/OBRect.h"

class MobileAgent;
class LocatorSite : public OBGeo::FRect
{
public:
	LocatorSite(long theTileIndex, OBGeo::FRect& aTile, double deckHeight);
	virtual ~LocatorSite();

public:
	
	CPoint2008 getLocation() { return mLocation; }
	long getTileIndex(){ return mTileIndex; }

	bool requestService( MobileAgent* pMobAgent);
	void makeAvailable( ElapsedTime p_time);

	bool isBarrier(){ return mIsBarrier; }
	void setAsBarrier(){ mIsBarrier = true; }

private:
	long			mTileIndex;
	CPoint2008		mLocation;
	bool			mIsBarrier;


private:
	bool isIdel();
	void lock();
	void unlock();


	int				mLockCount;

};


class MobAgentMovementPath : public UnsortedContainer < LocatorSite >
{
public:
	MobAgentMovementPath(void);
	~MobAgentMovementPath(void);

	long addLocatorSite(LocatorSite* pMoveSite);

	LocatorSite* getNextLocatorSite();
	void addWaitingLocatorSite(LocatorSite* pMoveSite);

};



