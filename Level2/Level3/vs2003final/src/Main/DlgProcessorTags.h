#if !defined(AFX_DLGPROCESSORTAGS_H__9678B227_398B_4648_A3F3_BB48047D8864__INCLUDED_)
#define AFX_DLGPROCESSORTAGS_H__9678B227_398B_4648_A3F3_BB48047D8864__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgProcessorTags.h : header file
//
#include "..\MFCExControl\CoolTree.h"

class CTermPlanDoc;
/////////////////////////////////////////////////////////////////////////////
// CDlgProcessorTags dialog

class CDlgProcessorTags : public CDialog
{
// Construction
public:
	CDlgProcessorTags(CTermPlanDoc* pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgProcessorTags)
	enum { IDD = IDD_DIALOG_PROCESSORTAGS };
	CCoolTree	m_treeProcTags;
	CButton	m_btnFrame1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProcessorTags)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    void InitTree();
	void LoadData();

	CTermPlanDoc* m_pDoc;

	HTREEITEM* m_htreeitems;
	

	// Generated message map functions
	//{{AFX_MSG(CDlgProcessorTags)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROCESSORTAGS_H__9678B227_398B_4648_A3F3_BB48047D8864__INCLUDED_)
