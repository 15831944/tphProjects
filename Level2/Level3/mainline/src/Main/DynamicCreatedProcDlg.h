#if !defined(AFX_DYNAMICCREATEDPROCDLG_H__4FAF68D5_778F_4B34_A1F8_E3AA9CAA215C__INCLUDED_)
#define AFX_DYNAMICCREATEDPROCDLG_H__4FAF68D5_778F_4B34_A1F8_E3AA9CAA215C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DynamicCreatedProcDlg.h : header file
//
#include "ProcessorTreeCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CDynamicCreatedProcDlg dialog

class CDynamicCreatedProcDlg : public CDialog
{
// Construction
public:
	CDynamicCreatedProcDlg(InputTerminal* _pTerm, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDynamicCreatedProcDlg)
	enum { IDD = IDD_DIALOG_DYNAMIC_CREATED_PROCESSORS };
	CListBox	m_listProcs;
	//}}AFX_DATA

	InputTerminal* m_pTerm;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDynamicCreatedProcDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDynamicCreatedProcDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DYNAMICCREATEDPROCDLG_H__4FAF68D5_778F_4B34_A1F8_E3AA9CAA215C__INCLUDED_)
