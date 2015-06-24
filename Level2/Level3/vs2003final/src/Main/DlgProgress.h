#if !defined(AFX_DLGPROGRESS_H__04CCA315_4049_42F5_88E4_ED0021B21AD1__INCLUDED_)
#define AFX_DLGPROGRESS_H__04CCA315_4049_42F5_88E4_ED0021B21AD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgProgress.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgProgress dialog

class CDlgProgress : public CDialog
{
// Construction
public:
	CDlgProgress(CWnd* pParent = NULL);   // standard constructor
	BOOL Create();
	void SetPercentage(short percent);
	void SetTitle(CString sTitle);
	void SetTitle(UINT nID);
	BOOL CheckCancelButton();

// Dialog Data
	//{{AFX_DATA(CDlgProgress)
	enum { IDD = IDD_PROGRESSDLG };
	CProgressCtrl	m_pbProgress;
	CString	m_sPercent;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProgress)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void PumpMessages();
	CWnd* m_pParent;
	int m_nID;
	BOOL m_bCancel;

	// Generated message map functions
	//{{AFX_MSG(CDlgProgress)
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROGRESS_H__04CCA315_4049_42F5_88E4_ED0021B21AD1__INCLUDED_)
