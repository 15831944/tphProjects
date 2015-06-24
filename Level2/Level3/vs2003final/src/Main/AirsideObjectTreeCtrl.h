#pragma once
#include "TreeCtrlEx.h"
#include "../MFCExControl/CoolTree.h"
#include <vector>
#include <algorithm>

// CAirsideObjectTreeCtrl
//////////////////////////////////////////////////////////////////////////
//
//
//	Please do not use this class anymore
//
//	this control has been move to MFCExControl, name CARCTreeCtrlExWithColor
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////

typedef struct _ItemStringSectionColor//issc, this for item text (section too,certainly) show with some color 
{
	_ItemStringSectionColor(void)
	{
		strSection = _T("");
		colorSection = RGB(0,0,0);
	} 
	CString strSection;
	COLORREF colorSection;
}ItemStringSectionColor;

typedef struct _AirsideObjectTreeCtrlItemDataEx //aoid
{
	long lSize;//size of struct ,for version control	
	int nSubType;
	DWORD_PTR dwptrItemData;//item data	 
	std::vector<ItemStringSectionColor> vrItemStringSectionColorShow;//section string must has order in item text.
}AirsideObjectTreeCtrlItemDataEx;

#define WM_OBJECTTREE_DOUBLECLICK WM_USER + 0x100
//#define WM_OBJECTTREE_LBUTTONDOWN WM_USER + 0x101

class CAirsideObjectTreeCtrl : public CTreeCtrlEx
{
	DECLARE_DYNAMIC(CAirsideObjectTreeCtrl)

public:
	CAirsideObjectTreeCtrl();
	virtual ~CAirsideObjectTreeCtrl();
public:
	BOOL SetItemData( HTREEITEM hItem, DWORD_PTR dwData );//warining:this function not same as CTreeCtrl::SetItemData( HTREEITEM hItem, DWORD_PTR dwData )
	DWORD_PTR GetItemData(HTREEITEM hItem);
	void SetItemDataEx(HTREEITEM hItem,const AirsideObjectTreeCtrlItemDataEx& aoidDataEx);
	AirsideObjectTreeCtrlItemDataEx* GetItemDataEx(HTREEITEM hItem);
	BOOL DeleteItem( HTREEITEM hItem );
	BOOL DeleteAllItems(void);
protected:
	void ClearItemDatas(void);
protected:
	bool IsItemInList(AirsideObjectTreeCtrlItemDataEx* pEx)const;
	std::vector<AirsideObjectTreeCtrlItemDataEx*> m_vrItemDataEx;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
};


