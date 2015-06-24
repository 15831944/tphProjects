#pragma once

#include "../MFCExControl/ListCtrlEx.h"
#include "BmpBtnDropList.h"
#include "resource.h"

class LoadedStateAndShapData;

class LoadedStateAndShapeListCtrl: public CListCtrlEx
{
public:
	LoadedStateAndShapeListCtrl(void);
	~LoadedStateAndShapeListCtrl(void);

	void InsertDataItem(int nIdx, LoadedStateAndShapData* pItem);
	//int InsertColumn(int nCol, const LV_COLUMNEX* pColumn);

protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
	void ShowDropList(int nItem, int nCol);

	//{{AFX_MSG(CPaxDispListCtrl)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult); 
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CBmpBtnDropList m_bbDropList;//add by sky for bmp display
	CImageList m_shapesImageList;
	int m_nShapeSelItem;
	int m_nShapeSelSubItem;

	HBITMAP m_hBitmapCheck;
	CSize m_sizeCheck;
};
