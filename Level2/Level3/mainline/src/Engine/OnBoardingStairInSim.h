#pragma once
#include "OnBoardDeviceInSim.h"
#include <common/Path2008.h>
#include "Common/Pollygon2008.h"
class OnboardFlightInSim;
class COnBoardStair;
class ARCVector3;
class GroundInSim;
class OnboardElementSpace;

class OnboardStairInSim : public OnboardDeviceInSim
{
public:
	OnboardStairInSim(COnBoardStair *pStair,OnboardFlightInSim* pFlightInSim);
	~OnboardStairInSim(void);

public:
	bool GetLocation(CPoint2008& location)const;
protected:
	virtual void CalculateSpace();
	void ClearSpace();
	GroundInSim* GetGround(CDeck* pDeck);
	void CreatePollygon();
	void CreateEntryPoint();
	void CreateBackElementSpace();
public:
	CPath2008 m_realpath;
	OnboardElementSpace* m_pElementSpace;
	OnboardElementSpace* m_pBackElementSpace;
};