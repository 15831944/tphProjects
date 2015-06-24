#if !defined(AFX_DLGPROCDISPOVERRIDES_H__93DE21B5_A26E_4CE0_A593_04CCF4717EDE__INCLUDED_)
#define AFX_DLGPROCDISPOVERRIDES_H__93DE21B5_A26E_4CE0_A593_04CCF4717EDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgProcDispOverrides.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgProcDispOverrides dialog

class CDlgProcDispOverrides : public CDialog
{
// Construction
public:
	CDlgProcDispOverrides(BOOL* pVals, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgProcDispOverrides)
	enum { IDD = IDD_PROCDISPOVERRIDE };
	BOOL	m_bDisplay[6];
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProcDispOverrides)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgProcDispOverrides)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROCDISPOVERRIDES_H__93DE21B5_A26E_4CE0_A593_04CCF4717EDE__INCLUDED_)
