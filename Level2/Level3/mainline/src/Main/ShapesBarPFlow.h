#if !defined(AFX_SHAPESBARPFLOW_H__B0FE9EDA_60AF_4211_8B36_44F6A68F260D__INCLUDED_)
#define AFX_SHAPESBARPFLOW_H__B0FE9EDA_60AF_4211_8B36_44F6A68F260D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShapesBarPFlow.h : header file
//
#include "ShapesListBox.h"
#include "XPStyle/GfxOutBarCtrl.h"
#include "XPStyle/NewMenu.h"

/////////////////////////////////////////////////////////////////////////////
// CShapesBarPFlow window

class CShapesBarPFlow : public CSizingControlBarG
{
// Construction
public:
	CShapesBarPFlow();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShapesBarPFlow)
	//}}AFX_VIRTUAL

// Implementation
public:
	void DeleteAllProcesses();
	BOOL UpdateProcess(CString& strOld, CString& strNew);
	UINT DeleteProcess(const CString& strName);
	int AddNewProcess(const CString& strName);
	virtual ~CShapesBarPFlow();
	void CreateOutlookBar(CShape::CShapeList* pSL);
	// Generated message map functions
protected:
	//{{AFX_MSG(CShapesBarPFlow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnSLBSelChanged(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point); 
	afx_msg void OnMmprocAdd();
	afx_msg void OnMmprocComments();
	afx_msg void OnMmprocHelp();
	afx_msg void OnMmitemEdit();
	afx_msg void OnMmitemDelete();
	afx_msg void OnMmitemComments();
	afx_msg void OnMmitemHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CBrush m_brush;
	CFont m_font;
	CGfxOutBarCtrl m_wndOutBarCtrl;
	CImageList m_smallIL;
	CImageList m_largeIL;
	CShape::CShapeList* m_pSL;
	CNewMenu m_nMenu, *m_pSubMenu;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHAPESBARPFLOW_H__B0FE9EDA_60AF_4211_8B36_44F6A68F260D__INCLUDED_)
