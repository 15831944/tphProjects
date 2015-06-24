#if !defined(AFX_HEADERCTRLDRAG_H__553D9D98_C1D2_43E2_84B4_2272B747EF8E__INCLUDED_)
#define AFX_HEADERCTRLDRAG_H__553D9D98_C1D2_43E2_84B4_2272B747EF8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HeaderCtrlDrag.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlDrag window
#define UM_HEADERCTRL_CLICK_COLUMN	WM_USER+200
class CHeaderCtrlDrag : public CHeaderCtrl
{
// Construction
public:
	CHeaderCtrlDrag();
	friend class CListCtrlDrag;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHeaderCtrlDrag)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL IsResizingItemWidth(CPoint point);
	BOOL m_bDragging;
	virtual ~CHeaderCtrlDrag();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHeaderCtrlDrag)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEADERCTRLDRAG_H__553D9D98_C1D2_43E2_84B4_2272B747EF8E__INCLUDED_)
