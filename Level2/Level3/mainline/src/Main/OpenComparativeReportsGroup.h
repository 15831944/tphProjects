#if !defined(AFX_OPENCOMPARATIVEREPORTSGROUP_H__73FBB898_A40D_429A_A70C_42D0BC7D7F88__INCLUDED_)
#define AFX_OPENCOMPARATIVEREPORTSGROUP_H__73FBB898_A40D_429A_A70C_42D0BC7D7F88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenComparativeReportsGroup.h : header file
//
#include "..\MFCExControl\ListCtrlEx.h"
#include "../MFCExControl/sortableheaderctrl.h"
#include "printablelistctrl.h"
/////////////////////////////////////////////////////////////////////////////
// COpenComparativeReportsGroup dialog

class COpenComparativeReportsGroup : public CDialog
{
// Construction
public:
	int GetSelIndex();
	void InitListCtrl();
	COpenComparativeReportsGroup(CWnd* pParent = NULL);   // standard constructor

	CString m_strName;
	CString m_strDesc;

// Dialog Data
	//{{AFX_DATA(COpenComparativeReportsGroup)
	enum { IDD = IDD_OPENCOMPARATIVEREPGROUPS };
	CPrintableListCtrl	m_ListReportsGroup;
	CSortableHeaderCtrl	m_headerCtl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenComparativeReportsGroup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillProjectList();
	

	// Generated message map functions
	//{{AFX_MSG(COpenComparativeReportsGroup)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblclkProjlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListreportgroups(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENCOMPARATIVEREPORTSGROUP_H__73FBB898_A40D_429A_A70C_42D0BC7D7F88__INCLUDED_)
