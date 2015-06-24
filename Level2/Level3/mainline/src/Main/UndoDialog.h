#if !defined(AFX_UNDODIALOG_H__2CD405A3_F828_4A05_9FB4_99B076128DE2__INCLUDED_)
#define AFX_UNDODIALOG_H__2CD405A3_F828_4A05_9FB4_99B076128DE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UndoDialog.h : header file
//
#include "TermPlanDoc.h"
#include "common\undomanager.h"

/////////////////////////////////////////////////////////////////////////////
// CUndoDialog dialog

class CUndoDialog : public CDialog
{
// Construction
public:
	CUndoDialog(CTermPlanDoc* _pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUndoDialog)
	enum { IDD = IDD_DIALOG_UNDO };
	CListBox	m_listboxUndoProj;
	CButton	m_btnUndo;
	CString	m_csProjName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUndoDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CTermPlanDoc* m_pDoc;
	CUndoManager* m_pUndoMan;

	// Generated message map functions
	//{{AFX_MSG(CUndoDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListUndoproj();
	afx_msg void OnButtonUndo();
	afx_msg void OnProjectOptions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int FillUndoList(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNDODIALOG_H__2CD405A3_F828_4A05_9FB4_99B076128DE2__INCLUDED_)
