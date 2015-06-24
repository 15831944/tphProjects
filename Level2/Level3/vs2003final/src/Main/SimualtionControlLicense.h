#pragma once
#include "CLicense.h"



class CSimualtionControlLicense
{
public:
	CSimualtionControlLicense(void);
	~CSimualtionControlLicense(void);

public:
	void GetSimualtionControl();
	int  GetProductLicenseSecondsRemaining();


public:
	bool m_bAirsideSim;
	bool m_bTerminalSim;
	bool m_bLandsideSim;
	bool m_bOnBoardSim;
private:
	    CLicense       License;
};
