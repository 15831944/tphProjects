#if !defined(AFX_UNSELECTABLETREECTRL_H__3D31155C_C70A_4FE5_BAF5_CDE4A0E7F0CA__INCLUDED_)
#define AFX_UNSELECTABLETREECTRL_H__3D31155C_C70A_4FE5_BAF5_CDE4A0E7F0CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UnselectableTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUnselectableTreeCtrl window

class CUnselectableTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CUnselectableTreeCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnselectableTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUnselectableTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUnselectableTreeCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNSELECTABLETREECTRL_H__3D31155C_C70A_4FE5_BAF5_CDE4A0E7F0CA__INCLUDED_)
