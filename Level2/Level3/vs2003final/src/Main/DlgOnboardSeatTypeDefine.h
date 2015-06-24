#pragma once
// #include "../MFCExControl/ListCtrlEx.h"
#include "../Engine/terminal.h"
#include "..\InputOnboard\SeatTypeSpecification.h"

// #include "..\InputOnboard\CInputOnboard.h"
#include ".\SeatTypeDefListCtrl.h"
// DlgOnboardSeatTypeDefine dialog

class DlgOnboardSeatTypeDefine : public CXTResizeDialog
{
	DECLARE_DYNCREATE(DlgOnboardSeatTypeDefine)

public:
	DlgOnboardSeatTypeDefine();
 	DlgOnboardSeatTypeDefine(InputOnboard* pInputOnboard,/*int nProjID,*/Terminal* _pTerm,CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgOnboardSeatTypeDefine();
// Overrides
	void OnButtonOK();
	void OnButtonCancel();
	void OnNewFlight();
	void OnDelFlight();
	void OnNewSeatTypeDef();
	void OnDelSeatTypeDef();
	bool FlightExist(COnboardFlight *flight);
	

	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
// Dialog Data
	enum { IDD = IDD_ONBOARD_SEATTYPEDEFINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void InitToolBar();
	void InitFlightList();
	void CreatSeatTypeDefList();
	CFlightSeatTypeDefine *getCurFlightSeatTypeDef();
	CSeatTypeDefine *getCurSeatTypeDef();
	afx_msg void OnSelChangeFlightList();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
private:
	CToolBar m_toolBarFlight;
	CToolBar m_toolBarSeatType;
 	CListBox m_listFlight;
 	CSeatTypeDefListCtrl m_listSeatType;	
 	Terminal *m_pTerminal;
	COnboardSeatTypeDefine *m_pSeatTypeDefine;
 	InputOnboard* m_pInputOnboard;
public:
	afx_msg void OnBnClickedButtonCurbsave();
	afx_msg void OnLvnItemchangedListSeattype(NMHDR *pNMHDR, LRESULT *pResult);
};
