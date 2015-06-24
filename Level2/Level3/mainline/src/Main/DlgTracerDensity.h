#if !defined(AFX_DLGTRACERDENSITY_H__D8F6FEAD_13FD_4CE8_80C8_14F2AEA4BB72__INCLUDED_)
#define AFX_DLGTRACERDENSITY_H__D8F6FEAD_13FD_4CE8_80C8_14F2AEA4BB72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTracerDensity.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgTracerDensity dialog

class CDlgTracerDensity : public CDialog
{
// Construction
public:
	CDlgTracerDensity(double _density, CWnd* pParent = NULL);   // standard constructor


	double GetDensity();

// Dialog Data
	//{{AFX_DATA(CDlgTracerDensity)
	enum { IDD = IDD_DIALOG_TRACERDENSITY };
	CSpinButtonCtrl	m_spinDensity;
	double	m_dDensity;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTracerDensity)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgTracerDensity)
	afx_msg void OnDeltaPosSpinDensity(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusEditDensity();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTRACERDENSITY_H__D8F6FEAD_13FD_4CE8_80C8_14F2AEA4BB72__INCLUDED_)
