#if !defined(AFX_DLGGETSTRING_H__5604ECD5_3898_4F75_B74A_3F24A1C6902E__INCLUDED_)
#define AFX_DLGGETSTRING_H__5604ECD5_3898_4F75_B74A_3F24A1C6902E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgGetString.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgGetString dialog

class CDlgGetString : public CDialog
{
// Construction
public:
	CDlgGetString(UINT nDlgTitle, CString sValue = _T(""), CWnd* pParent = NULL);   // standard constructor
	CDlgGetString(CString strTitle, CString sValue = _T(""), CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgGetString)
	enum { IDD = IDD_GETSTRINGDLG };
	CString	m_sValue;
	//}}AFX_DATA

	void SetTitle(CString strTitle){ m_sTitle = strTitle; }
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgGetString)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_sTitle;

	// Generated message map functions
	//{{AFX_MSG(CDlgGetString)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGGETSTRING_H__5604ECD5_3898_4F75_B74A_3F24A1C6902E__INCLUDED_)
