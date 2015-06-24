#if !defined(AFX_DLGCOMMENTS_H__AFDC87C3_A513_43E1_AF21_0FE0B460C84E__INCLUDED_)
#define AFX_DLGCOMMENTS_H__AFDC87C3_A513_43E1_AF21_0FE0B460C84E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgComments.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgComments dialog

class CDlgComments : public CDialog
{
// Construction
public:
	CDlgComments(TCHAR* pBuffer, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgComments)
	enum { IDD = IDD_DIALOG_COMMENTS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgComments)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	TCHAR* m_pBuffer;

	// Generated message map functions
	//{{AFX_MSG(CDlgComments)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOMMENTS_H__AFDC87C3_A513_43E1_AF21_0FE0B460C84E__INCLUDED_)
