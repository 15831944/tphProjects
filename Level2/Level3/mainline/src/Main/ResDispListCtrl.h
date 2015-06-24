// ResDispListCtrl.h: interface for the CResDispListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESDISPLISTCTRL_H__31C867B0_5973_4EDC_88A7_4C8BDA9F3C4F__INCLUDED_)
#define AFX_RESDISPLISTCTRL_H__31C867B0_5973_4EDC_88A7_4C8BDA9F3C4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ResourceDispPropItem.h"
#include "BmpBtnDropList.h"

class CTermPlanDoc;

class CResDispListCtrl : public CListCtrl
{
// Construction
public:
	CResDispListCtrl(CTermPlanDoc* pDoc);

// Attributes
public:

protected:
	int m_nShapeSelItem;
	CImageList m_shapesImageList;
	HBITMAP m_hBitmapCheck;
	CSize m_sizeCheck;
	CBmpBtnDropList m_bbDropList;
	CTermPlanDoc* m_pDoc;

// Operations
public:

protected:
	void GetColorBoxRect(CRect& r);
	void GetVisibleRect(CRect& r);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResDispListCtrl)
	public:
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	void InsertResDispItem( int _nIdx, CResourceDispPropItem* _pItem );
	virtual ~CResDispListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CResDispListCtrl)
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_RESDISPLISTCTRL_H__31C867B0_5973_4EDC_88A7_4C8BDA9F3C4F__INCLUDED_)
