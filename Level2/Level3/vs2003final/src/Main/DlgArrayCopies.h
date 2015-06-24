#if !defined(AFX_DLGARRAYCOPIES_H__8D1415AF_BC88_4EA9_903C_D3F5767D67B6__INCLUDED_)
#define AFX_DLGARRAYCOPIES_H__8D1415AF_BC88_4EA9_903C_D3F5767D67B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgArrayCopies.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgArrayCopies dialog

class CDlgArrayCopies : public CDialog
{
// Construction
public:
	CDlgArrayCopies(int nCopies = 1, CWnd* pParent = NULL);   // standard constructor

	int GetNumCopies();

// Dialog Data
	//{{AFX_DATA(CDlgArrayCopies)
	enum { IDD = IDD_DIALOG_GETARRAYCOPIES };
	CSpinButtonCtrl	m_spinCopies;
	int		m_nCopies;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgArrayCopies)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgArrayCopies)
	afx_msg void OnDeltaposSpinNumCopies(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditNumCopies();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGARRAYCOPIES_H__8D1415AF_BC88_4EA9_903C_D3F5767D67B6__INCLUDED_)
