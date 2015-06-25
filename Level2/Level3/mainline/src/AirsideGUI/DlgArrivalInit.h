#pragma once
#include "..\MFCExControl\ToolTipDialog.h"
#include "ArrivalInitPara.h"

class CDlgArrivalInit : public CToolTipDialog
{
public:
	CDlgArrivalInit( /*ns_AirsideInput::CFlightPlans* pFlightPlans,*/ std::vector<FlightArrivalInit>& vArrivalInitPara,  CWnd* pParent = NULL);
	~CDlgArrivalInit();

private:
	enum { IDD = IDD_ARRIVALINIT };
	DECLARE_MESSAGE_MAP()

	CListCtrl m_wndListCtrl;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	void SetListColumns();
	void SetListContents();

	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	//TermPlanDoc *m_pTermPlandoc;
	std::vector<FlightArrivalInit> m_vArrivalInitPara;

};
