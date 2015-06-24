#include "StdAfx.h"
#include ".\simualtioncontrollicense.h"

CSimualtionControlLicense::CSimualtionControlLicense(void)
{
	m_bAirsideSim = false;
	m_bLandsideSim = false;
	m_bTerminalSim = false;
	m_bOnBoardSim = false;
}

CSimualtionControlLicense::~CSimualtionControlLicense(void)
{
}

void CSimualtionControlLicense::GetSimualtionControl()
{
	unsigned char SimPermission = 0x00;

    SimPermission = License.GetProductLicenseSimulationControl("AT");

	unsigned char cSim = SimPermission;
	cSim &= 1;
	if (cSim == 1)
	{
		m_bLandsideSim = true;
	}

	cSim = SimPermission;
	cSim &= 2;
	if (cSim == 2)
	{
		m_bTerminalSim = true;
	}

	cSim = SimPermission;
	cSim &= 8;
	if (cSim == 8)
	{
		m_bOnBoardSim = true;
	}

	cSim = SimPermission;
	cSim &= 4;
	if (cSim == 4)
	{
		m_bAirsideSim = true;
	}


}

int CSimualtionControlLicense::GetProductLicenseSecondsRemaining()
{
	return License.GetProductLicenseSecondsRemaining("AT");
}