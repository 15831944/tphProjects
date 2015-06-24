#pragma once

#include "ShapesListBox.h"
#include "scbarg.h"
#include "XPStyle\GfxOutBarCtrl.h"
// CAircarftModelBar
class InputOnboard;
class CAircarftModelBar : public CSizingControlBarG
{
	// Construction
public:
	CAircarftModelBar();
	// Attributes
protected:
	CBrush m_brush;
	BOOL m_bCreated;
	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectSelectBar)
	//}}AFX_VIRTUAL

	// Implementation
public:
	void CreateModellookBar(InputOnboard * pInputOnboard);
	void SetInputOnboard(InputOnboard* pInputOnboard);
	// Generated message map functions
protected:
	CFont m_font;
	CGfxOutBarCtrl m_wndModelBarCtrl;
	CImageList m_smallIL;
	CImageList m_largeIL;
	//{{AFX_MSG(CShapesBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnSLBSelChanged(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


