#pragma once
#include "OnboardAircraftElementInSim.h"
#include "../Common/Pollygon2008.h"

class COnBoardWall;
class GroundInSim;
class OnboardFlightInSim;
class OnboardElementSpace;

class OnboardWallInSim : public OnboardAircraftElementInSim 
{
public:
	OnboardWallInSim(COnBoardWall *pWall,OnboardFlightInSim* pFlightInSim);
	~OnboardWallInSim(void);

	COnBoardWall* GetWallInput()const;
	virtual CString GetType()const;
	bool GetLocation(CPoint2008& location)const;

	virtual int GetResourceID() const;
protected:
	virtual void CalculateSpace();
	void ClearSpace();
	GroundInSim* GetGround();
	void CreatePollygon();

protected:
	COnBoardWall* m_pOnboardWall;
	OnboardElementSpace* m_pElementSpace;
};