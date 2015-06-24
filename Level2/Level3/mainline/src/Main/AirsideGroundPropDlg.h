#pragma once
#include "floorpropertiesdlg.h"
#include "AirsideGround.h"
class CTermPlanDoc;

class CAirsideGroundPropDlg :
	public CFloorPropertiesDlg
{
public:
	CAirsideGroundPropDlg(int nAirportID,CAirsideGround* pFloor,CTermPlanDoc *pDoc, CWnd* pParent = NULL);
	~CAirsideGroundPropDlg(void);

public:
	void OnGridOptions();

private:
	int m_nAirportID;

protected:
	virtual void OnOK();
	virtual void OnCancel();
};
