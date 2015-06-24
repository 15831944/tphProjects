#include "StdAfx.h"
#include "SimulationDiagnoseDelivery.h"
#include "Common/WinMsg.h"

SimulationDiagnoseDelivery::SimulationDiagnoseDelivery(void)
:m_hSimEngineDlg(NULL)
{
}

SimulationDiagnoseDelivery::~SimulationDiagnoseDelivery(void)
{
}

void SimulationDiagnoseDelivery::setHandle(HWND hWnd)
{
	m_hSimEngineDlg = hWnd;
}

void SimulationDiagnoseDelivery::DeliveryTerminalDiagnose(TerminalDiagnose* pDiagnose)
{
	DeliveryDiagnose(pDiagnose, ARCDiagnose::em_TerminalDiagnose);
}

void SimulationDiagnoseDelivery::DeliveryAirsideDiagnose(AirsideDiagnose* pDiagnose)
{
	DeliveryDiagnose(pDiagnose, ARCDiagnose::em_AirsideDiagnose);
}

void SimulationDiagnoseDelivery::DeliveryLandsideDiagnose(LandsideDiagnose* pDiagnose)
{
	DeliveryDiagnose(pDiagnose, ARCDiagnose::em_LandsideDiagnose);
}

void SimulationDiagnoseDelivery::DeliveryOnBoardDiagnose(OnBoardDiagnose* pDiagnose)
{
	DeliveryDiagnose(pDiagnose, ARCDiagnose::em_OnBoardDiagnose);
}


void SimulationDiagnoseDelivery::DeliveryDiagnose(ARCDiagnose* pDiagnose, ARCDiagnose::DiagnoseType emType)
{
	if(m_hSimEngineDlg != NULL)
		SendMessage( m_hSimEngineDlg, WM_SIM_STATUS_APPEND_DIAGNOSE, (int)emType, (long)pDiagnose );
}
