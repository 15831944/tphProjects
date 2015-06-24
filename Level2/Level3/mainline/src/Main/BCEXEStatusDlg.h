#if !defined(AFX_BCEXESTATUSDLG_H__B86634FD_EBA0_4C6B_AD4F_C27BBBFE7633__INCLUDED_)
#define AFX_BCEXESTATUSDLG_H__B86634FD_EBA0_4C6B_AD4F_C27BBBFE7633__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BCEXEStatusDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBCEXEStatusDlg dialog

class CBCEXEStatusDlg : public CDialog
{
// Construction
public:
	bool m_bOkOnExit;
	CString m_csErrorMsg;
	CBCEXEStatusDlg(const CString _csExeFileName, const CString _csTitle, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBCEXEStatusDlg)
	enum { IDD = IDD_DIALOG_BCEXESTATUS };
	CProgressCtrl	m_progressBCEXE;
	CString	m_csStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCEXEStatusDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bRunning;
	CString m_csTitle;
	CString m_csExeFileName;
	const static CString M_CSSTATUSFILENAME;
	const static int M_NHEARTBEATCOUNTMAX;
	int m_nHeartBeat;
	int m_nHeartBeatCount;


	bool ProcessEXEStatus();

	// Generated message map functions
	//{{AFX_MSG(CBCEXEStatusDlg)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCEXESTATUSDLG_H__B86634FD_EBA0_4C6B_AD4F_C27BBBFE7633__INCLUDED_)
