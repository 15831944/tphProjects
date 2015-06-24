#pragma once
#include "Common/Pollygon2008.h"
#include "OnboardCellInSim.h"

class GroundInSim;
class OnboardAircraftElementInSim;

class OnboardElementSpace
{
public:
	OnboardElementSpace(GroundInSim* pGroundInSim);
	~OnboardElementSpace();

public:
	OnboardCellInSim* getLocationCell()const {return m_pCellInSim;}
	bool getOccupySpace(OnboardSpaceCellInSim& spaceCellInSim)const;
	void setPollygon(const CPollygon2008& pollygon){m_pollygon = pollygon;}
	const CPollygon2008& GetPolygon()const {return m_pollygon;}
	void CalculateSpace();
	void CreatePollygon(OnboardAircraftElementInSim* pOnboardElement);
	bool GetCenterPoint(CPoint2008& location);

protected:
	CPoint2008 getLeftTop();
	CPoint2008 getRightTop();
	CPoint2008 getLeftBottom();
	CPoint2008 getRightBottom();

	GroundInSim* GetGround();
	void SearchSpace(int iStartCell,int iRightTopCell,int iLeftBottomCell,int iEndCell);
	bool contain(OnboardCellInSim* pCellInSim);
	bool intersect(OnboardCellInSim* pCellInSim);
	void SetCellState(OnboardCellInSim* pCellInSim);
protected:
	OnboardCellInSim* m_pCellInSim;
	OnboardSpaceCellInSim mSpaceCellInSim;
	CPollygon2008 m_pollygon;
	GroundInSim* m_pGroundInSim;
};