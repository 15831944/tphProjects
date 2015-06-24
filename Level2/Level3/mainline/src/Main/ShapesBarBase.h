// ShapesBarBase.h: interface for the CShapesBarBase class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "scbarg.h"
#include "XPStyle\GfxOutBarCtrl.h"

class CShapesBarBase : public CSizingControlBarG  
{
// Construction
public:
	CShapesBarBase();
	virtual ~CShapesBarBase();

// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectSelectBar)
	//}}AFX_VIRTUAL

// Implementation

	void CreateOutlookBar();
	void ClearAllContent();

	// Generated message map functions
protected:
	//{{AFX_MSG(CShapesBarBase)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	void AddLargeImage(HBITMAP hBitmap, COLORREF crMask);
	void AddSmallImage(HBITMAP hBitmap, COLORREF crMask);

	CGfxOutBarCtrl m_wndOutBarCtrl;

	DECLARE_MESSAGE_MAP()


private:
	CImageList m_smallIL;
	CImageList m_largeIL;

	CBrush m_brush;
	CFont m_font;
};
