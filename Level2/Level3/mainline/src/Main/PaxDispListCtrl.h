#pragma once

#include "BmpBtnDropList.h"
#include "PaxDispPropItem.h"
#include "InPlaceComboBoxEx.h"
/////////////////////////////////////////////////////////////////////////////
// CPaxDispListCtrl window
#define IDC_BUTTON_DOWNLIST 0

#define PAX_DISP_PROP_MODIFICATION1    (WM_USER + 15061)
#define PAX_DISP_PROP_MODIFICATION2    (WM_USER + 15062)

class CPaxDispListCtrl : public CListCtrl
{
// Construction
public:
	CPaxDispListCtrl();

// Attributes
public:

protected:
	int m_nShapeSelItem;
	int m_nLineSelItem;
	CImageList m_shapesImageList;
	CImageList m_linesImageList;
	CInPlaceComboBoxEx m_linesInPlaceComboBox;
	HBITMAP m_hBitmapCheck;
	CSize m_sizeCheck;
	CBmpBtnDropList m_bbDropList;

// Operations
public:

protected:
	void GetColorBoxRect(CRect& r);
	void GetVisibleRect(CRect& r);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxDispListCtrl)
	public:
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	void InsertPaxDispItem( int _nIdx, CPaxDispPropItem* _pItem );
	virtual ~CPaxDispListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPaxDispListCtrl)
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
