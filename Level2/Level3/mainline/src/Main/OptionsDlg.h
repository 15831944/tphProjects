#include "afxcmn.h"
#if !defined(AFX_OPTIONSDLG_H__E0C34488_BA77_41B3_BEB8_EAD1A783E665__INCLUDED_)
#define AFX_OPTIONSDLG_H__E0C34488_BA77_41B3_BEB8_EAD1A783E665__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDlg.h : header file
//
class CTermPlanDoc;
/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog

class COptionsDlg : public CDialog
{
// Construction
public:
	COptionsDlg( CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsDlg)
	enum { IDD = IDD_DIALOG_OPTIONS };
	CSpinButtonCtrl	m_spinVal;
	int		m_nVal;
	int m_Timer ;
	CEdit m_EditTimer ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinVal(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnDeltaposSpinTimer(NMHDR* pNMHDR, LRESULT* pResult);

	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CSpinButtonCtrl m_SpinTimer;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__E0C34488_BA77_41B3_BEB8_EAD1A783E665__INCLUDED_)
