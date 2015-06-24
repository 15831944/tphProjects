#pragma once
#include "OnBoard/PathFinding/OBRect.h"
#include "OnBoard/PathFinding/PathFinderListener.h"
#include "OnBoard/PathFinding/Pathfinder.h"
#include "OnBoard/PathFinding/RectMap.h"
#include "../Common/Point2008.h"
#include "../Common/Path2008.h"
#include "../Common/containr.h"
#include "../Common/path.h"
class OnboardCellInSim;
class OnboardDeckGroundInSim;
class PaxOnboardBaseBehavior;
class OnboardAircraftElementInSim;

#define GRID_WIDTH 5
#define GRID_HEIGHT 5
typedef UnsortedContainer<OnboardCellInSim > OnboardSpaceCellInSim;


class OnboardCellInSim : public OBGeo::FRect
{
public:
	OnboardCellInSim(OnboardDeckGroundInSim *pDeckGround,long theTileIndex, OBGeo::FRect& aTile, double deckHeight);
	virtual ~OnboardCellInSim();

	enum CellState
	{
		Idle			= 1,
		NoPath			= 10,//not available for finding path
		SmallSpace		= 100,
		HalfOccupied	= 1000,
		FullOccupied	= 10000
	};

public:

	CPoint2008 getLocation() const { return mLocation; }
	long getTileIndex() const{ return mTileIndex; }

	void SetState(CellState state) {mState = state;}
	CellState GetState()const {return mState;}

	CPath2008 GetPath2008()const;
	Path GetPath()const;

	bool entryPoint()const {return mbEntryPoint;}
	void entryPoint(bool bEntryPoint){mbEntryPoint = bEntryPoint;}

	bool IsAtSameDeck(OnboardCellInSim *pCell) const;

	OnboardDeckGroundInSim * GetDeckGround() const{return m_pDeckGround;}

	double GetDistanceTo( OnboardCellInSim *pCell) const;

	PaxOnboardBaseBehavior * getBehavior() const { return m_pBehavior; }
	void setBehavior(PaxOnboardBaseBehavior * pBehavior) { m_pBehavior = pBehavior; }


	PaxOnboardBaseBehavior * getReservePax() const { return m_pReservePax; }
	void PaxReserve(PaxOnboardBaseBehavior * pPaxReserve) { m_pReservePax = pPaxReserve; }

public:
	void PaxOccupy(PaxOnboardBaseBehavior *pBehavior);

	void PaxLeave( PaxOnboardBaseBehavior *pBehavior, const ElapsedTime& eTime);

	//the cell is barrier
	bool IsValid() const;

	bool entryPointValid()const
	{
		if(GetState() == HalfOccupied ||
			GetState() == FullOccupied ||
			GetState() == SmallSpace)
			return false;
		return true;
	}
	//the cell is available for occupied
	//not barrier
	//not occupied by other passenger
	bool IsAvailable(PaxOnboardBaseBehavior *pBehavior) const;


	void AddNotifyObjectWhenPaxLeave(OnboardAircraftElementInSim *pObject);
private:
	long			mTileIndex;
	CPoint2008		mLocation;
	CellState		mState;
	bool			mbEntryPoint;
	OnboardDeckGroundInSim *m_pDeckGround;
	PaxOnboardBaseBehavior *m_pBehavior;


	PaxOnboardBaseBehavior *m_pReservePax;



	std::vector< OnboardAircraftElementInSim *> m_vObjectNeedNotify;
};

class OnboardCellPath
{
public:
	OnboardCellPath();
	~OnboardCellPath();
public:

	void addCell(OnboardCellInSim* pCell);
//	OnboardCellInSim* getNextLocatorSite();
//	void addWaitingLocatorSite(OnboardCellInSim* pCell);

	int getCellCount() const;
	OnboardCellInSim* getCell(int nIndex) const;
	
	
	bool Contains(OnboardCellInSim *pCell);

	//will always got a cell the cell is not in 
	OnboardCellInSim* GetNextCell(OnboardCellInSim *pCurCell, int nStep)const;

	OnboardCellInSim* GetNextCell(OnboardCellInSim *pCurCell) const;
	void MergeCellPath(OnboardCellPath& cellPath);
	std::vector<OnboardCellInSim *>& GetCellPath();

	virtual void Clear();
protected:
	std::vector<OnboardCellInSim *> m_vPath;

};



















