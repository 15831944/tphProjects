#if !defined(AFX_FRAMECTRLDLG_H__C7D1D017_C067_4689_BE1F_B3962581A6CF__INCLUDED_)
#define AFX_FRAMECTRLDLG_H__C7D1D017_C067_4689_BE1F_B3962581A6CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameCtrlDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrameCtrlDlg dialog
#include "FrameCtrl.h"
class CFrameCtrlDlg : public CDialog
{
// Construction
public:
	CFrameCtrlDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFrameCtrlDlg)
	enum { IDD = IDD_FRAMECTRL };
	CFrameCtrl	m_frameCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameCtrlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFrameCtrlDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMECTRLDLG_H__C7D1D017_C067_4689_BE1F_B3962581A6CF__INCLUDED_)
