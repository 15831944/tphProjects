#pragma once
#include "OnboardAircraftElementInSim.h"
#include "../Common/Pollygon2008.h"

class CSeatGroup;
class OnboardFlightInSim;
class CSeatRow;
class GroundInSim;
class OnboardSeatInSim;
class OnboardSeatRowInSim;
class OnboardElementSpace;

class OnboardSeatGroupInSim : public OnboardAircraftElementInSim
{
public:
	OnboardSeatGroupInSim(CSeatGroup* pSeatGroup,OnboardFlightInSim* pFlight);
	virtual ~OnboardSeatGroupInSim();

public:
	CSeatGroup* GetSeatGroupInput()const {return m_pSeatGroup;}
	CString GetType()const {return _T("Seat Group");}
	
	virtual int GetResourceID() const;

	void AddSeatInSim(OnboardSeatInSim* pSeatInSim) {ASSERT(pSeatInSim); m_vSeatInSim.push_back(pSeatInSim);}
	void InitSeatRow();
	virtual void CalculateSpace();
	bool GetLocation(CPoint2008& location)const;

	bool frontSeatRow(OnboardSeatRowInSim* pSeatRowInSim)const;
	const CPollygon2008& GetPolygon()const;
protected:
	void CalculateOneRowPollygon();
	void CalculateMoreThanOneRowPollygon();
	void CreatePollygon();

 	GroundInSim* GetGround();	
	//find which row is front
	bool CompareTwoPosition(OnboardSeatRowInSim* pOneRowInSim,OnboardSeatRowInSim* pOtherRowInSim)const;

protected:
	CSeatGroup* m_pSeatGroup;
	std::vector<OnboardSeatRowInSim*> m_vSeatRow;
	OnboardElementSpace* m_pElementSpace;
	std::vector<OnboardSeatInSim*>m_vSeatInSim;
};