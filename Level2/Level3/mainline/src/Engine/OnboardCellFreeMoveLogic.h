#pragma once
#include "OnboardCellInSim.h"
#include "PaxCellLocation.h"
#include "Common\elaptime.h"
#include "PaxCircleSensor.h"

class PaxOnboardBaseBehavior;
class OnboardFlightInSim;
class GroundInSim;
class OnboardCellFreeMoveLogic
{
	friend class OnboardPaxFreeMovingConflict;
public:
	enum enumState 
	{
		LogicMove = 0,//out of free moving, in logic processing
		Moving,
		Waiting,
	};


public:
	OnboardCellFreeMoveLogic(PaxOnboardBaseBehavior *pBehavior);
	~OnboardCellFreeMoveLogic(void);

public:
	//check ahead only
	int Step(ElapsedTime& eTime);
	//check all direction 
	int Step2(const ElapsedTime& eTime); 
	PaxCircleSensor mSensor;
	//check head left right 
	int Step3(const ElapsedTime& eTime);
	void StepToCell(OnboardCellInSim* pNextCell, const ElapsedTime& eNextTime);

	//the dist and way
	struct DistBlock
	{
		DistBlock();
		double dist;
		PaxOnboardBaseBehavior* pPax;
		bool bStaticBlock;
		std::vector<PaxOnboardBaseBehavior*> vPaxList;
		bool HavePax(PaxOnboardBaseBehavior* pPax)const;
	};
	struct DistBlockAround
	{
		DistBlock dpHead;
		DistBlock dpLeft;
		DistBlock dpRight;
	};
	DistBlockAround mlastAround;
	bool mbSideWalk;
	//Person* mpSideWalkPax;
	CPoint2008 mCurDir;
	PaxOnboardBaseBehavior*	findDirConflict(DistBlock& dbahead);
	void WaitForNextRound(const ElapsedTime& eNextTime);


	void StartMove(const ElapsedTime& eTime);

	//if the passenger's location changed, set the new location
	//conflict resolution
	bool ResetLocation(PaxCellLocation& newLocation);
public:

	//OnboardCellInSim * getDest() const { return m_pDest; }
	//void setDest(OnboardCellInSim * pDestination) { m_pDest = pDestination; }

	void setCellPath(OnboardCellPath& cellPath);
	void setCurLocation(const PaxCellLocation& paxLocation);


	bool GetPaxLocation(CPoint2008 ptLocation, CPoint2008 ptDirection,PaxCellLocation &, GroundInSim *pGroundInSim = NULL) const;

protected:
	OnboardFlightInSim *getFlight();

	bool GetPaxLocation(OnboardCellInSim *pCell, OnboardCellInSim *pNextCell,PaxCellLocation &) const;


	void WriteLog(OnboardCellInSim *pCell, const ElapsedTime& eTime);
	double getSpeed() const;

public:

	//when the free moving end, the state would be used. 
	//indicate the next status
	void SetEndPaxState(int nState){m_nEndPaxState = nState;}
	void setState(enumState state) {m_curState = state;}
protected:
	int m_nEndPaxState;


protected:
	//clean the contents, pepare for next time
	void Clean();
	
	void BackToFlow(ElapsedTime eTime);

	PaxCellLocation SearchSpace(GroundInSim *pGround, int iStartCell,int iRightTopCell,int iLeftBottomCell,int iEndCell ) const;

	//check the cell is valid or not
	//not arrive edge
	//not occupied by furnish
	//return the cell which occupied by another passenger, the nearest one
	//pax location, this passenger's next location
	//pCurCenterCell, this passnenger's current center cell 
	Person* CheckLocationConflict(GroundInSim *pGround,
		PaxCellLocation& paxLocation,
		OnboardCellInSim *pCurCenterCell,
		OnboardPaxFreeMovingConflict& conflict) const;

	bool CheckLocationAvailable(PaxCellLocation& paxLocation);

	GroundInSim * getGroundInSim(OnboardCellInSim *pCell) const;
	void WakeupWaitingPax( PaxOnboardBaseBehavior *pBehavior ,ElapsedTime& etime );


	//base on the pax's center location and direction, get the passenger's space
	//path, 4 points
	//			2	1
	//	
	//			3   4
	bool getCurrentPaxRect(CPath2008& path) const;
	bool getCurrentPaxDirection(CPoint2008& ptAngle) const;

	bool IsStopped(const ElapsedTime& eCurrentTime) const;
	bool StepToLocation(const PaxCellLocation& newLocation );

	void GenerateStepEvent(const ElapsedTime& eTime);
protected:
	//the start cell of this free moving
	OnboardCellInSim *m_pOrigin;

	//the end cell of this free moving
	OnboardCellInSim *m_pDest;

	//on board cell path
	OnboardCellPath  m_cellPath;
	
	//on board Location current
	PaxCellLocation m_curLocation;
	
	//passenger's next location
	PaxCellLocation m_nextLocation;

	//on board flight
	OnboardFlightInSim *m_pFlight;

	PaxOnboardBaseBehavior *m_pBehavior;

	//double m_dCurDirection;

	enumState m_curState;

	//execute time
	ElapsedTime m_eCurrentTime;
	
	bool m_bIsOutOfDoor;

	//the last move time of the pax
	ElapsedTime m_eLastMoveTime;

	//if true, write log, else does not need to write log
	bool m_bNeedToWriteLog;

protected:
	void SensorEnviroment();
	void SensorEnv( const CPoint2008& pos, const CPoint2008& dir, bool bSideSense, DistBlockAround& reslt );
};
