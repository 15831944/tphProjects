#pragma once
// ACDistListCtrl.h : header file
//
#include "InPlaceComboBoxEx.h"
#include "BmpBtnDropList.h"

class CAircraftDispPropItem;

/////////////////////////////////////////////////////////////////////////////
// CPaxDispListCtrl window

#define AC_DISP_PROP_CHANGE1     (WM_USER + 12019)
#define AC_DISP_PROP_CHANGE2     (WM_USER + 12020)

class CACDispListCtrl : public CListCtrl
{
	// Construction
public:
	CACDispListCtrl();

protected:
	int m_nShapeSelItem;
	int m_nLineSelItem;
	CImageList m_shapesImageList;
	CImageList m_linesImageList;
	CInPlaceComboBoxEx m_linesInPlaceComboBox;
	HBITMAP m_hBitmapCheck;
	CSize m_sizeCheck;
	CBmpBtnDropList m_bbDropList;

	void GetColorBoxRect(CRect& r);
	void GetVisibleRect(CRect& r);

public:
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);

protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void InsertACDispItem( int _nIdx, CAircraftDispPropItem* _pItem );
	virtual ~CACDispListCtrl();

protected:
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};