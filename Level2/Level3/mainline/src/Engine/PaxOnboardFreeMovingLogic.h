#pragma once
#include "ISteppable.h"
#include "common/elaptime.h"
#include "PaxOnboardRoute.h"
#include <algorithm>
class PaxOnboardBaseBehavior;
class CARCportEngine;
class CNumRange;



class PaxOnboardFreeMovingLogic : public ISteppable
{
public:
	enum MoveState //moving logic state
	{
		_holdIn,      //hold in route
		_movingInSeg, //moving along the path
		_movingToSeg, //moving directly to a dest point
		_endofRoute,		//
		_null, //not active
	};
	
	PaxOnboardFreeMovingLogic(void);
	//~PaxOnboardFreeMovingLogic(void);
	void SetBaseBehavior(PaxOnboardBaseBehavior* pBehave){ m_Parent = pBehave; }

	//assigned new route
	void ResetRoute(PaxOnboardRoute& route);	
	void StartMove(const ElapsedTime& t, PaxOnboardRoute& route,CARCportEngine& state);
	void EndMove(const ElapsedTime& t);
	void step(CARCportEngine& state);
	MoveState getCurState()const{ return m_curState;}

	void DebugLog();

	//check can have person move on the 
	bool CanMoveOnCorridor(OnboardCorridorInSim* pCor, DistanceUnit distInCor, const ARCVector3& pos);
	ElapsedTime GetInterval()const;
protected:
	void OnMovingInPath(CARCportEngine& state);
	void OnHoldInPath(CARCportEngine& state);
	void OnEndofPath(CARCportEngine& state);
	void OnMovingToPointInSeg(CARCportEngine& state);
	//void MovToCell(int idx);
	
	//void UpdateSearchCell(CARCportEngine& state);
	void StepInCorridor();

	//
	void UpdateNextPos(const CPoint2008& nextPos); //update move the the next pos, the segment index etc.
	void HoldForStep(CARCportEngine& engine );
	void StepTo(const CPoint2008& pos, const ElapsedTime&t);
	void StepTo(int item, DistanceUnit distInitem,DistanceUnit spanInitem);
	
	void GetPaxInConcern(const CPoint2008& dir, std::vector<Person*>& vPaxList, std::vector<Person*>& vblockpax);

	void Repeat(CARCportEngine& engine, const ElapsedTime& nextTime);//add to step list
	void SetState(MoveState s,bool bForce = false);

	bool CheckConflictWithPax(const CPoint2008& dir,Person* person,bool bLeft, CPoint2008& dirNext, DistanceUnit& distToPax)const;
	bool AvoidBound(const CPoint2008& orginDir, CPoint2008& destDir, DistanceUnit& distLen)const;
	bool FixDir(const CPoint2008& orginDir, CPoint2008& destDir, DistanceUnit& distLen)const;

	bool CheckConflictInDir(const CPoint2008& dir, const std::vector<Person*>& vPaxList, double& dfardist)const;
	bool CheckConflictInDir(const CPoint2008& dir, Person*person, double& dfardist)const;

	double getSpeed()const;
	CPoint2008 getCurPos()const;
	DistanceUnit getStepDist()const;

	OnboardAircraftElementInSim* getCurElement()const;
	DistanceUnit getCurDistInElement()const;
	bool IsDirPositiveInElement()const;


	PaxOnboardBaseBehavior*	m_Parent;
	PaxOnboardRoute m_Route;	
	double m_dRadius; //radius of concern

	//SearchBox m_searchBox;
	DistanceUnit m_dMinRad;

	
	//dynamic vars
	ElapsedTime m_lastStateTime;
	
	ElapsedTime m_curTime;
	CPoint2008 m_curDir;
	MoveState m_curState;
	
	DistanceUnit m_dCurDistInSeg; //remember last dist in item
	DistanceUnit m_dCurSpanInSeg;
	int m_iCurrItemIdx;


	bool ResolveConflictInCorridor(double dLen, CPoint2008& nextPoint, DistanceUnit& distInSeg ,DistanceUnit& spanInSeg);
	
	//to be observed by other pax
	//double m_dDistInElement; //the dist in the element	
	//OnboardAircraftElementInSim* m_pCurElemet;
	//bool m_PositiveInElementPath;


	CPoint2008 m_nextPointInSeg;
	
};
