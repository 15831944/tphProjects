#pragma once
#include "OnboardAircraftElementInSim.h"

class OnBoardVerticalDevice;
class ARCVector3;
class CPath2008;
class CDeck;
class OnboardCellInSim;

class OnboardDeviceInSim : public OnboardAircraftElementInSim
{
public:
	OnboardDeviceInSim(OnBoardVerticalDevice *pDevice,OnboardFlightInSim* pFlightInSim);
	~OnboardDeviceInSim(void);
public:
	virtual CString GetType()const;

	virtual int GetResourceID() const;

	bool onDeck(const CDeck* pDeck)const;
	bool GetRealPath(CPath2008& path);
	OnboardCellInSim* GetOnboardCell(OnboardDeckGroundInSim* pDeckGround)const;
	OnboardCellInSim* GetEntryCell(OnboardDeckGroundInSim* pDeckGround)const;
public:
	OnBoardVerticalDevice* m_pDevice;
	std::vector<OnboardCellInSim*> m_entryPointList;
};