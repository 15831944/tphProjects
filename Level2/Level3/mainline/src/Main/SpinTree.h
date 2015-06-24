#if !defined(AFX_SPINTREE_H__AAAC10EB_EB78_4F2C_80D4_499E170886E7__INCLUDED_)
#define AFX_SPINTREE_H__AAAC10EB_EB78_4F2C_80D4_499E170886E7__INCLUDED_

#include "MySpin.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpinTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpinTree window

class CSpinTree : public CTreeCtrl
{
// Construction
public:
	CSpinTree();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpinTree)
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	CEdit* EditLabel( HTREEITEM hItem );
	virtual ~CSpinTree();

	// Generated message map functions
protected:
	HTREEITEM m_hEditedItem;
//	CMySpin m_MySpin;
	
	CMySpin m_Edit;

	//{{AFX_MSG(CSpinTree)	
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPINTREE_H__AAAC10EB_EB78_4F2C_80D4_499E170886E7__INCLUDED_)
