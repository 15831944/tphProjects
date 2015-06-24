#if !defined(AFX_ATTACHWALKDLG_H__CB7C6130_B3EC_44AB_9C70_9035ECD5D0A1__INCLUDED_)
#define AFX_ATTACHWALKDLG_H__CB7C6130_B3EC_44AB_9C70_9035ECD5D0A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AttachWalkDlg.h : header file
//

#include "..\Inputs\PipeDataSet.h"
#include "TermPlanDoc.h"
/////////////////////////////////////////////////////////////////////////////
// CAttachWalkDlg dialog

class CAttachWalkDlg : public CDialog
{
// Construction
public:
	CAttachWalkDlg(CPipe* _pPipe,CTermPlanDoc* _pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAttachWalkDlg)
	enum { IDD = IDD_DIALOG_ATTACH_WALK };
	CListBox	m_listWalk;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttachWalkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPipe* pPipe;
	CTermPlanDoc* m_pDoc;
	// Generated message map functions
	//{{AFX_MSG(CAttachWalkDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeListWalk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTACHWALKDLG_H__CB7C6130_B3EC_44AB_9C70_9035ECD5D0A1__INCLUDED_)
