#if !defined(AFX_DENSITYANIMVIEW_H__417DE48F_13EC_4E57_BD67_D4A12D4F72D7__INCLUDED_)
#define AFX_DENSITYANIMVIEW_H__417DE48F_13EC_4E57_BD67_D4A12D4F72D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DensityAnimView.h : header file
//
class CTermPlanDoc;

/////////////////////////////////////////////////////////////////////////////
// CDensityAnimView view

class CDensityAnimView : public CView
{
protected:
	CDensityAnimView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDensityAnimView)

// Attributes
public:
	CTermPlanDoc* GetDocument();

// Operations
public:
	BOOL GenerateFloorOverlayTexture();
	void OnPrint(CDC* pDC, CPrintInfo* pInfo);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDensityAnimView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDensityAnimView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	HBITMAP m_hMapBM;
	CSize m_sizeBM;
	float* m_pDensities;
	
	CDC m_bitmapDC;

	////////////////////
	HBITMAP m_hScreenBM;

	// Generated message map functions
protected:
	//{{AFX_MSG(CDensityAnimView)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnUpdateAnimTime(CCmdUI *pCmdUI);
	afx_msg void OnViewPrint();
	afx_msg void OnViewExport();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#ifndef _DEBUG  // debug version in TermPlanView.cpp
inline CTermPlanDoc* CDensityAnimView::GetDocument()
   { return (CTermPlanDoc*)m_pDocument; }
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DENSITYANIMVIEW_H__417DE48F_13EC_4E57_BD67_D4A12D4F72D7__INCLUDED_)
