#pragma once
#include "OnBoardDeviceInSim.h"
#include "Common/Pollygon2008.h"

class OnboardFlightInSim;
class COnBoardElevator;
class OnboardDeviceInSim;
class GroundInSim;
class CDeck;
class OnboardElementSpace;

typedef std::pair<GroundInSim*,POLLYGON2008VECTOR> GroundPollygon;

class OnboardElevatorInSim : public OnboardDeviceInSim
{
public:
	OnboardElevatorInSim(COnBoardElevator *pElevator,OnboardFlightInSim* pFlightInSim);
	~OnboardElevatorInSim(void);
public:
	bool GetLocation(CPoint2008& location)const;
protected:
	virtual void CalculateSpace();
	void ClearSpace();
	GroundInSim* GetGround(CDeck* pDeck);
	void CreatePollygon();
	void CreateEntryPoint();

protected:
	CPath2008 m_realpath;
	std::vector<OnboardElementSpace*>m_pElementSpaceList;
};
