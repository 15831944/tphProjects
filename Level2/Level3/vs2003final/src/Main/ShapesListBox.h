#if !defined(AFX_SHAPESLISTBOX_H__B8C31411_F919_44B9_BDC8_CD8EA9FBB602__INCLUDED_)
#define AFX_SHAPESLISTBOX_H__B8C31411_F919_44B9_BDC8_CD8EA9FBB602__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShapesListBox.h : header file
//

#include <CommonData/Shape.h>

/////////////////////////////////////////////////////////////////////////////
// CShapesListBox window

class CShapesListBox : public CListBox
{
// Construction
public:
	CShapesListBox();

// Attributes
public:
	std::vector<CSize> m_vImgSizes;
protected:
	CBrush m_brush;

// Operations
public:
	int AddItem(CShape* pShape);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShapesListBox)
	public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpmis);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpdis);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CShapesListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CShapesListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHAPESLISTBOX_H__B8C31411_F919_44B9_BDC8_CD8EA9FBB602__INCLUDED_)
