#pragma once
#include "OnBoardDeviceInSim.h"
#include "Common/Pollygon2008.h"

class OnboardFlightInSim;
class COnBoardEscalator;
class ARCVector3;
class Person;
class GroundInSim;
class OnboardElementSpace;

class OnboardEscalatorInSim : public OnboardDeviceInSim
{
public:
	OnboardEscalatorInSim(COnBoardEscalator *pEcalator,OnboardFlightInSim* pFlightInSim);
	~OnboardEscalatorInSim(void);

public:
	bool GetLocation(CPoint2008& location)const;
protected:
	virtual void CalculateSpace();
	GroundInSim* GetGround(CDeck* pDeck);
	void ClearSpace();
	void CreatePollygon();
	void CreateEntryPoint();
	void CreateBackElementSpace();
protected:
	CPath2008 m_realpath;
	OnboardElementSpace* m_pElementSpace;
	OnboardElementSpace* m_pBackElementSpace;
};