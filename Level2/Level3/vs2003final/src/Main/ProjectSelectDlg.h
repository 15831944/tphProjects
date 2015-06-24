#if !defined(AFX_PROJECTSELECTDLG_H__E23B7218_5FEA_43DF_B765_103BDE0D9987__INCLUDED_)
#define AFX_PROJECTSELECTDLG_H__E23B7218_5FEA_43DF_B765_103BDE0D9987__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProjectSelectDlg.h : header file
//

#define PSD_BASE_ID	61459

/////////////////////////////////////////////////////////////////////////////
// CDlgProjectSelect dialog

class CDlgProjectSelect : public CDialog
{
// Construction
public:
	CDlgProjectSelect(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgProjectSelect)
	enum { IDD = IDD_PROJECTSELECT };
	CListCtrl	m_cProjList;
	//}}AFX_DATA

	CString m_sSelProjName;

	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProjectSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	void FreeItemMemory();
	//{{AFX_MSG(CDlgProjectSelect)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPLColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPLGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnClickProjList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkProjlist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTSELECTDLG_H__E23B7218_5FEA_43DF_B765_103BDE0D9987__INCLUDED_)
