#if !defined(AFX_PROJECTCONTROLDLG_H__34900980_432E_4169_B65F_9A83B66018A9__INCLUDED_)
#define AFX_PROJECTCONTROLDLG_H__34900980_432E_4169_B65F_9A83B66018A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProjectControlDlg.h : header file
//
#include "..\common\ProjectManager.h"
/////////////////////////////////////////////////////////////////////////////
// CProjectControlDlg dialog

class CProjectControlDlg : public CDialog
{

	void CProjectControlDlg::FreeItemMemory();

	static	int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	
public:


	

	CProjectControlDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProjectControlDlg)
	enum { IDD = IDD_DIALOG_PROJECT_CONTROL };
	CListCtrl	m_cDeletedList;
	CListCtrl	m_cProjList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectControlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProjectControlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonToDelete();
	afx_msg void OnButtonToRestore();
	afx_msg void OnButtonPurge();
	afx_msg void OnDestroy();
	afx_msg void OnGetdispinfoListProject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoListDeletedProject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListProject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListDeletedProject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListProject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemclickListProject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListProject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListDeletedProject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListDeletedProject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void CloseThisWin(PROJECTINFO* pi);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTCONTROLDLG_H__34900980_432E_4169_B65F_9A83B66018A9__INCLUDED_)
