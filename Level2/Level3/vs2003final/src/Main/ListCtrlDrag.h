#if !defined(AFX_LISTCTRLDRAG_H__5792F3B2_5896_41C0_96C1_2B958F5521BE__INCLUDED_)
#define AFX_LISTCTRLDRAG_H__5792F3B2_5896_41C0_96C1_2B958F5521BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlDrag.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListCtrlDrag window
#include "HeaderCtrlDrag.h"
#define UM_BEGIN_DRAG WM_USER+100
class CImportFlightFromFileDlg;
class CListCtrlDrag : public CListCtrl
{
// Construction
public:
	CListCtrlDrag();

// Attributes
	int m_nNumCtrl;                // number of class copy - number of list control
	CImportFlightFromFileDlg* m_pViewPos;
	int m_nItemHeight;                // height of an item
	CHeaderCtrlDrag m_headerCtrl;
public:

// Operations
public:
	void RedirectHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void RedirectVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void RedirectKeyScroll(UINT nChar);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlDrag)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	int GetColIndexUnderPoint(CPoint point,int nColNum);
	int m_nColIndexDragged;
	void EndDrag();
	void SubClassHeaderCtrl();
	void BeginDrag(CPoint point,int nColNum);
	void RedirectMouseWheel(UINT nFlags,short zDelta,CPoint pt);
	void RedirectSelectSynchro(UINT nFlags, CPoint point);
	int GetItemHeight();
	virtual ~CListCtrlDrag();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlDrag)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLDRAG_H__5792F3B2_5896_41C0_96C1_2B958F5521BE__INCLUDED_)
