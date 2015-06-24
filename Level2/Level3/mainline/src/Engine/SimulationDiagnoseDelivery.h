#pragma once
#include "ServiceObject.h"

#include "Common/EngineDiagnoseEx.h"
#include "Common/ARCExceptionEx.h"
#include "Common/OnBoardException.h"

class SimulationDiagnoseDelivery : public ServiceObject < SimulationDiagnoseDelivery >
{
public:
	SimulationDiagnoseDelivery(void);
	virtual ~SimulationDiagnoseDelivery(void);

public:
	void setHandle(HWND hWnd);

	void DeliveryTerminalDiagnose(TerminalDiagnose* pDiagnose);
	void DeliveryAirsideDiagnose(AirsideDiagnose* pDiagnose);
	void DeliveryLandsideDiagnose(LandsideDiagnose* pDiagnose);
	void DeliveryOnBoardDiagnose(OnBoardDiagnose* pDiagnose);

protected:
	void DeliveryDiagnose(ARCDiagnose* pDiagnose, ARCDiagnose::DiagnoseType emType);

protected:
	HWND   m_hSimEngineDlg;

};

SimulationDiagnoseDelivery* DiagnoseDelivery();
inline SimulationDiagnoseDelivery* DiagnoseDelivery()
{
	return SimulationDiagnoseDelivery::GetInstance();

}





