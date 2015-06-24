#pragma once
/**
* Class to provide geometry space and pathfinding logic holder.
* DONOT use any mobile agent's sensce in this class.
*/
#include "../OnBoardDefs.h"
#include "../PathFinding/OBRect.h"
#include "../PathFinding/PathFinderListener.h"
#include "../PathFinding/Pathfinder.h"
#include "../PathFinding/RectMap.h"

#include "carrierSeat.h"

class carrierEntryDoor;
class carrierExitDoor;
class MobAgentMovementPath;
class LocatorSite;
carrierElement;
class carrierGround : public PathFinderListener
{
public:
	carrierGround();
	~carrierGround(void);

	void setHeight(double dHeight){ mHeight = dHeight; }
	double getHeight(){ return mHeight; }

	void addCarrierSeat( carrierSeat *pCarrierSeat);

	void generateGround(OBGeo::FRect& _outBoundRect);
	void setOutsideLocatorSitesAsBarriers(ARCPath& _deckOutLine);

	virtual void OnPathFound(const std::vector<int>& thePath);

	void getAssociablePath(LocatorSite* pStartSite, LocatorSite* pEndSite, 
		MobAgentMovementPath& _movePath);

	LocatorSite* getLocatorSite( CPoint2008& location);
	LocatorSite* getLocatorSite( int nTileIndex);

	RectMap& getLogicMap(){ return mRectMap; }
	int getLocatorSiteCount();

	void setCarrierElementBarriers(carrierElement* pCarrierElement);
	void swapCarrierSeatBarrierState(carrierSeat* pCarrierSeat, bool isBarrier);


	void setMobAgentIntelligence(MobileAgent* pMobAgent);

	void addCarrierExitDoors(carrierExitDoor* pDoor);
	void addCarrierEntryDoors(carrierEntryDoor* pDoor);
	carrierEntryDoor* getCarrierEntryDoor();
	carrierExitDoor* getCarrierExitDoor();
private:
	
	RectMap				mRectMap;
	PathFinder			mPathFinder;
	double				mHeight;

	UnsortedContainer< carrierSeat > mSeatList;
	carrierEntryDoor *m_pEntryDoor;
	carrierExitDoor* m_pExitDoor;
	
};
