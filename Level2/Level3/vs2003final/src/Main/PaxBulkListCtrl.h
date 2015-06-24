#if !defined(AFX_PAXBULKLISTCTRL1_H__ABA06C4B_7617_440E_ADE7_97666E771334__INCLUDED_)
#define AFX_PAXBULKLISTCTRL1_H__ABA06C4B_7617_440E_ADE7_97666E771334__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxBulkListCtrl1.h : header file
//
#include "..\MFCExControl\ListCtrlEx.h"

/////////////////////////////////////////////////////////////////////////////
// CPaxBulkListCtrl window

class CPaxBulkListCtrl : public CListCtrl//Ex
{
// Construction
public:
	CPaxBulkListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxBulkListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPaxBulkListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPaxBulkListCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAXBULKLISTCTRL1_H__ABA06C4B_7617_440E_ADE7_97666E771334__INCLUDED_)
