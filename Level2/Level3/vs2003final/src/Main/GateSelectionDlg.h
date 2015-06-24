#if !defined(AFX_GATESELECTIONDLG_H__28BC1FE3_F754_47AC_9473_3B8A2BABFC80__INCLUDED_)
#define AFX_GATESELECTIONDLG_H__28BC1FE3_F754_47AC_9473_3B8A2BABFC80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GateSelectionDlg.h : header file
//

#include "ProcessorTreeCtrl.h"
#include "ConDBListCtrl.h"
#include "Inputs\ProcDB.h"
#include "TermPlanDoc.h"
#include "GatePriorityDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CGateSelectionDlg dialog

class CGateSelectionDlg : public CDialog
{
protected:
	void ReloadDatabase();
	InputTerminal* GetInputTerminal();
	int m_nInx;
	BOOL m_bParentIsOtherDlg;
public:
	ProcessorID m_ProcID;
	BOOL m_bStandOnly;
// Construction
public:
	CGateSelectionDlg(CWnd* pParent, int nIdx = -1, BOOL _bFlag = FALSE);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGateSelectionDlg)
	enum { IDD = IDD_DIALOG_GATE_SELECTION };
	CProcessorTreeCtrl m_treeProc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGateSelectionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGateSelectionDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GATESELECTIONDLG_H__28BC1FE3_F754_47AC_9473_3B8A2BABFC80__INCLUDED_)
