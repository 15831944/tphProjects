#pragma once
#include "GateAssignDlg.h"

// CDlgArrivalGateAssignment dialog

class CDlgArrivalGateAssignment : public CGateAssignDlg
{
	DECLARE_DYNAMIC(CDlgArrivalGateAssignment)

public:
	CDlgArrivalGateAssignment(int nProjID, CAirportDatabase* pAirportDatabase,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgArrivalGateAssignment();

	virtual void SetAssignedFlight();
	virtual GateDefineType GetGateDefineType(){return ArrGateType;}
	virtual void SetGate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
	afx_msg void OnRClickGtchartctrl(long nItemIndex, long nLineIndex);
	afx_msg void OnStandMapping(void);
	afx_msg void OnButtonUnassignall();

	void ButtonConstraints();

};
