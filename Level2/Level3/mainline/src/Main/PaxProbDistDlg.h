#if !defined(AFX_PAXPROBDISTDLG_H__9367474F_69C3_4009_93A1_D48F4870DC90__INCLUDED_)
#define AFX_PAXPROBDISTDLG_H__9367474F_69C3_4009_93A1_D48F4870DC90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxProbDistDlg.h : header file
//

#include "inputs\con_db.h"

/////////////////////////////////////////////////////////////////////////////
// CPaxProbDistDlg dialog

class CPaxProbDistDlg : public CDialog
{
// Construction
public:
	ConstraintEntry* m_pEntry;
	CPaxProbDistDlg(ConstraintEntry* _pEntry, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPaxProbDistDlg)
	enum { IDD = IDD_DIALOG_PAXPROBDIST };
	CEdit	m_editTotal;
	CStatic	m_staticTotal;
	CListCtrl	m_listCtrlValue;
	CEdit	m_editForthVal;
	CStatic	m_staticForthVal;
	CEdit	m_editThirdVal;
	CStatic	m_staticThirdVal;
	CEdit	m_editSecondVal;
	CStatic	m_staticSecondVal;
	CEdit	m_editFirstVal;
	CStatic	m_staticFirstVal;
	CEdit	m_editUnts;
	CStatic	m_staticUnits;
	CTreeCtrl	m_treeDist;
	double	m_dFirstVal;
	double	m_dSecondVal;
	double	m_dThirdVal;
	double	m_dForthVal;
	UINT	m_nTotal;
	CString	m_csPaxType;
	CString	m_csDistName;
	CString	m_csUnits;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxProbDistDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString GetTreeDistName();
	int GetProbTypeFromTreeItem();
	int GetTreeDistType();
	CImageList m_ilNodes;
	void RefreshDistControl();
	void LoadTree();

	// Generated message map functions
	//{{AFX_MSG(CPaxProbDistDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTreeDistribution(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAXPROBDISTDLG_H__9367474F_69C3_4009_93A1_D48F4870DC90__INCLUDED_)
