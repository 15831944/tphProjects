// ShapesBar.h: interface for the CShapesBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAPESBAR_H__B592C191_B7B4_4123_BD3B_9E5D858B5DB9__INCLUDED_)
#define AFX_SHAPESBAR_H__B592C191_B7B4_4123_BD3B_9E5D858B5DB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ShapesListBox.h"
#include "scbarg.h"
#include "XPStyle\GfxOutBarCtrl.h"


class CShapesBar : public CSizingControlBarG  
{
// Construction
public:
	//CShapesBar();
	//virtual ~CShapesBar();

// Attributes
public:
protected:
	CBrush m_brush;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectSelectBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	void CreateOutlookBar(CShape::CShapeList* pSL);

	// Generated message map functions
protected:
	CFont m_font;
	CGfxOutBarCtrl m_wndOutBarCtrl;
	CImageList m_smallIL;
	CImageList m_largeIL;
	//{{AFX_MSG(CShapesBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnSLBSelChanged(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_SHAPESBAR_H__B592C191_B7B4_4123_BD3B_9E5D858B5DB9__INCLUDED_)
