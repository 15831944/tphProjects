#pragma once
//#include "../MFCExControl/ListCtrlEx.h"
#include "../Engine/terminal.h"
#include "..\InputOnboard\OnboardPaxCabinAssign.h"
#include "PaxCabinListCtrl.h"
#include "..\InputOnboard\CInputOnboard.h"
#include "../MFCExControl/XTResizeDialog.h"
// DlgOnboardPaxCabinAssignment dialog

class DlgOnboardPaxCabinAssignment : public CXTResizeDialog
{
	DECLARE_DYNCREATE(DlgOnboardPaxCabinAssignment)

public:
	DlgOnboardPaxCabinAssignment();
	DlgOnboardPaxCabinAssignment(InputOnboard* pInputOnboard,int nProjID,Terminal* _pTerm,ProjectCommon *pPrjCommon,CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgOnboardPaxCabinAssignment();
// Overrides
	void OnButtonOK();
	void OnButtonCancel();
	void OnNewFlight();
	void OnDelFlight();
	void OnNewPaxCabin();
	void OnDelPaxCabin();
	bool FlightExist(COnboardFlight *flight);
	

	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
// Dialog Data
	enum { IDD = IDD_ONBOARD_PAXCABIN_ASSIGN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void InitToolBar();
	void InitFlightList();
	void InitPaxCabinList();
	CFlightPaxCabinAssign *getCurFlightPaxCabinAssign();
	CPaxSeatGroupAssign *getCurPaxSeatGroupAssign();
	afx_msg void OnSelChangeFlightList();
	afx_msg void OnSelChangePaxCabinList();
	afx_msg void OnUpdatePaxCabinBtn(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
    


	DECLARE_MESSAGE_MAP()
private:
	CToolBar m_toolBarFlight;
	CToolBar m_toolBarPaxCabin;
	CListBox m_listFlight;
	CPaxCabinListCtrl m_listPaxCabin;	
	int m_nPrjID;
	Terminal *m_pTerminal;
	COnboardPaxCabinAssign m_onboardPaxCabinAssign;
	InputOnboard* m_pInputOnboard;
	ProjectCommon *m_pPrjCommon;
 public:
 	afx_msg void OnLvnItemchangedListPaxcabin(NMHDR *pNMHDR, LRESULT *pResult);
};
