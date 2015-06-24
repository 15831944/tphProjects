#pragma once
#include "../MFCExControl/ListCtrlEx.h"
#include "BmpBtnDropList.h"
#include "resource.h"

class CabinCrewPhysicalCharacteristics;

class CCabinCrewDispListCtrl :public CListCtrlEx
{
public:
	CCabinCrewDispListCtrl();
	~CCabinCrewDispListCtrl();

	void InsertCrewDispItem( int _nIdx, CabinCrewPhysicalCharacteristics* _pItem );
	int InsertColumn(int nCol, const LV_COLUMNEX* pColumn);

protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
	void ShowDropList(int nItem, int nCol);

protected:
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult); 
	DECLARE_MESSAGE_MAP()
protected:
	CBmpBtnDropList m_bbDropList;//add by sky for bmp display
	CImageList m_shapesImageList;
	int m_nShapeSelItem;
	int m_nShapeSelSubItem;

	HBITMAP m_hBitmapCheck;
	CSize m_sizeCheck;
};


