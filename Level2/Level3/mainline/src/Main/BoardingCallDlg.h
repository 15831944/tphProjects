#if !defined(AFX_BOARDINGCALLDLG_H__5998FD68_9553_4672_99AB_57F91A4F9DFF__INCLUDED_)
#define AFX_BOARDINGCALLDLG_H__5998FD68_9553_4672_99AB_57F91A4F9DFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BoardingCallDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBoardingCallDlg dialog
#include "..\inputs\in_term.h"
#include "..\inputs\con_db.h"
#include "..\MFCExControl\CoolTree.h"

class ConstraintWithProcIDEntry;
class FlightConWithProcIDDatabase;


class CBoardingCallDlg : public CDialog
{
// Construction
public:
	void DisableAllToolBarButtons();
	HTREEITEM AddStage(FlightConWithProcIDDatabase* pConDB,int iIndex);
	void DisableMenuItem(CMenu* pMenu);
	void ChangeProbDist(HTREEITEM hItem,int nIndexSeled);
	int GetConstraintDBCount();
	BOOL DeleteStageChild();
	HTREEITEM AddStageChild2Tree(HTREEITEM hItemParent,ConstraintWithProcIDEntry* pEntry);
	BOOL NewStageChild(HTREEITEM hItem,Constraint* _pNewCon,DWORD pStage);
	HTREEITEM m_hBoardingCalls;
	void LoadTreeItems();
	CString GetProjPath();
	FlightConWithProcIDDatabase* GetConstraintDatabase(int nStageIndex);
	InputTerminal* GetInputTerminal();
	CToolBar m_wndToolBar;
	CBoardingCallDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBoardingCallDlg)
	enum { IDD = IDD_DIALOG_BOARDINGCALL };
	CButton	m_btnSave;
	CButton	m_btnOk;
	CButton	m_btnCancel;
	CCoolTree	m_tree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBoardingCallDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBoardingCallDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnToolbarButtonAdd();
	afx_msg void OnToolbarButtonDel();
	afx_msg void OnToolbarButtonEdit();
	afx_msg void OnButtonSave();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeProcessor();
	afx_msg void OnDefaultProcessor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOARDINGCALLDLG_H__5998FD68_9553_4672_99AB_57F91A4F9DFF__INCLUDED_)
