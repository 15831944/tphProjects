#pragma once
#include "..\inputs\in_term.h"
#include "..\inputs\con_db.h"
#include "..\inputs\fltdata.h"
#include "..\MFCExControl\CoolTree.h"

class ConstraintWithProcIDEntry;
class FlightConWithProcIDDatabase;

class CBoardingCallDlg : public CDialog
{
public:
	HTREEITEM m_hRoot;
	void ReloadRoot();
	void ReloadStage(BoardingCallFlightTypeDatabase* pFlightTypeDB, HTREEITEM hTreeItemStage);
	void ReloadFlightType( BoardingCallFlightTypeEntry* pFlightEntry, HTREEITEM hTreeItemFlight );
	void ReloadStand( BoardingCallStandEntry* pStandEntry, HTREEITEM hTreeItemStand );
	void ReloadPaxType( BoardingCallPaxTypeEntry* pPaxEntry, HTREEITEM hTreeItemPax );
	void ReloadTriggers(std::vector<BoardingCallTrigger*>& vTriggers, HTREEITEM hTriggerAll);

	CString GetProjPath();
	InputTerminal* GetInputTerminal();
	CToolBar m_toolbar;
	CBoardingCallDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(CBoardingCallDlg)
	enum { IDD = IDD_DIALOG_BOARDINGCALL };
	CButton	m_btnSave;
	CButton	m_btnOk;
	CButton	m_btnCancel;
	CCoolTree	m_tree;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CBoardingCallDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CBoardingCallDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnButtonSave();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangedBoardingCallTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnToolbarButtonAddStage();
	afx_msg void OnToolbarButtonAddFlightType();
	afx_msg void OnToolbarButtonAddStand();
	afx_msg void OnToolbarButtonAddPaxType();
	afx_msg void OnToolbarButtonDel();
	afx_msg void OnToolbarButtonEdit();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChooseMenu(UINT nID);
protected:
	void DisableAllToolBarButtons();
	void RemoveTreeItem(HTREEITEM hItem);
	void RemoveSubItem(HTREEITEM hItem);

private:
	ProbabilityDistribution* CopyProbDistribution(ProbabilityDistribution* _pPD);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

