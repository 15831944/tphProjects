#if !defined(AFX_DLGFIREIMPACT_H__32DAE632_1CA9_4AD5_839E_AFE461B4DC8F__INCLUDED_)
#define AFX_DLGFIREIMPACT_H__32DAE632_1CA9_4AD5_839E_AFE461B4DC8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFireImpact.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgFireImpact dialog
#include "SimEngineDlg.h"

class CDlgFireImpact : public CDialog
{
// Construction
public:
	void SetSimEngineDlg(CSimEngineDlg* pDlg);
	CSimEngineDlg* m_pSimEngineDlg;
	CDlgFireImpact(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFireImpact();

// Dialog Data
	//{{AFX_DATA(CDlgFireImpact)
	enum { IDD = IDD_DIALOG_FIREIMPACT };
	CProgressCtrl	m_progressFireImpact;
	BOOL	m_bViewStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFireImpact)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	UINT m_nTimerID;

	// Generated message map functions
	//{{AFX_MSG(CDlgFireImpact)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg LRESULT OnSetPos(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFIREIMPACT_H__32DAE632_1CA9_4AD5_839E_AFE461B4DC8F__INCLUDED_)
