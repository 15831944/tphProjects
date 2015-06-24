// TermPlanView.h : interface of the CTermPlanView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERMPLANVIEW_H__5D30106D_30BE_11D4_92F6_0080C8F982B1__INCLUDED_)
#define AFX_TERMPLANVIEW_H__5D30106D_30BE_11D4_92F6_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTermPlanView : public CView
{
protected: // create from serialization only
	CTermPlanView();
	DECLARE_DYNCREATE(CTermPlanView)

// Attributes
public:
	CTermPlanDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTermPlanView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTermPlanView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTermPlanView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TermPlanView.cpp
inline CTermPlanDoc* CTermPlanView::GetDocument()
   { return (CTermPlanDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERMPLANVIEW_H__5D30106D_30BE_11D4_92F6_0080C8F982B1__INCLUDED_)
