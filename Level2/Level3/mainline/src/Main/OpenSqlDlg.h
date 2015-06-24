#if !defined(AFX_OPENSQLDLG_H__8B3411AB_AD3D_4FBE_9547_F55EF62935E0__INCLUDED_)
#define AFX_OPENSQLDLG_H__8B3411AB_AD3D_4FBE_9547_F55EF62935E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenSqlDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COpenSqlDlg dialog

class COpenSqlDlg : public CDialog
{
// Construction
public:
	void SetCommonSql(BOOL bValue);
	COpenSqlDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COpenSqlDlg)
	enum { IDD = IDD_DIALOG_OPEN_SQLSERVER };
	CEdit	m_edtPassword;
	CEdit	m_edtLoginName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenSqlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitControl();
	BOOL m_bCommonSql;

	// Generated message map functions
	//{{AFX_MSG(COpenSqlDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSqlAuthor();
	afx_msg void OnWindowsAuthor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENSQLDLG_H__8B3411AB_AD3D_4FBE_9547_F55EF62935E0__INCLUDED_)
