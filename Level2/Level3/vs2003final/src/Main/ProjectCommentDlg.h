#if !defined(AFX_PROJECTCOMMENTDLG_H__83496D35_4874_4A6A_A068_65A1ECD27724__INCLUDED_)
#define AFX_PROJECTCOMMENTDLG_H__83496D35_4874_4A6A_A068_65A1ECD27724__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProjectCommentDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProjectCommentDlg dialog
#include "TermPlanDoc.h"
class CProjectCommentDlg : public CDialog
{
// Construction
public: 
	void SaveToFile();
	CProjectCommentDlg(CTermPlanDoc* _pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProjectCommentDlg)
	enum { IDD = IDD_DIALOG_PROJECT_COMMENT };
	CString	m_commets;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectCommentDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProjectCommentDlg)
	afx_msg void OnSave();
	virtual void OnOK();
	afx_msg void OnChangeEditComment();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ReadFromFile();
	CTermPlanDoc* m_pDoc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTCOMMENTDLG_H__83496D35_4874_4A6A_A068_65A1ECD27724__INCLUDED_)
