// mainView.h : interface of the CmainView class
//


#pragma once
#include "../RenderEngine/RenderEngine.h"
#include "../RenderEngine/3DBaseView.h"


class CmainView : public C3DBaseView
{
protected: // create from serialization only
	CmainView();
	DECLARE_DYNCREATE(CmainView)

// Attributes
public:
	CmainDoc* GetDocument() const;

// Operations

// Overrides
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	CPoint m_lbutDownPt;
	CPoint m_lastMousePt;

// Implementation
public:
	virtual ~CmainView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in mainView.cpp
inline CmainDoc* CmainView::GetDocument() const
   { return reinterpret_cast<CmainDoc*>(m_pDocument); }
#endif

