// DensityGLView.h: interface for the CDensityGLView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DENSITYGLVIEW_H__D6364903_8E35_4300_9703_33DEADBEC57C__INCLUDED_)
#define AFX_DENSITYGLVIEW_H__D6364903_8E35_4300_9703_33DEADBEC57C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <GL\gl.h>
#include <GL\glu.h>

class C3DCamera;
class CTermPlanDoc;
class ARCColor4;

class CDensityGLView : public CView  
{
public:
	CDensityGLView();
	DECLARE_DYNCREATE(CDensityGLView)

// Attributes
public:
	CTermPlanDoc* GetDocument();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3DView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	virtual ~CDensityGLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	BOOL SetupViewport(int cx, int cy);
	BOOL SetupPixelFormat(HDC hDC, DWORD dwFlags);
	BOOL InitOGL();


	HGLRC m_hRC;	//opengl RC
	CDC* m_pDC;		//windows DC
	BOOL m_bBusy;	//this flag is true while view is rendering
	double m_dAspect;	//aspect ratio of view
	int m_iPixelFormat;	 //pixel format used
	C3DCamera* m_pCamera;//the view's camera
	CPoint m_ptFrom;
	ARCColor4* m_pCellColors;


	// Generated message map functions
protected:
	//{{AFX_MSG(C3DView)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnUpdateAnimTime(CCmdUI *pCmdUI);
	afx_msg void OnViewPrint();
	afx_msg void OnViewExport();
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DensityGLView.cpp
inline CTermPlanDoc* CDensityGLView::GetDocument()
	{ return (CTermPlanDoc*)m_pDocument; }
#endif

#endif // !defined(AFX_DENSITYGLVIEW_H__D6364903_8E35_4300_9703_33DEADBEC57C__INCLUDED_)
