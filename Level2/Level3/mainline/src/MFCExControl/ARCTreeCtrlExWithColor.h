#pragma once
#include "arctreectrlex.h"

//#include "../MFCExControl/CoolTree.h"
#include <vector>
#include <algorithm>

using namespace std;
#define WM_OBJECTTREE_DOUBLECLICK WM_USER + 0x100
//#define WM_OBJECTTREE_LBUTTONDOWN WM_USER + 0x101



class MFCEXCONTROL_API CARCTreeCtrlExWithColor :
	public CARCTreeCtrlEx
{
public:

	class CColorStringSection//issc, this for item text (section too,certainly) show with some color 
	{
	public:
		CColorStringSection(void)
		{
			strSection = _T("");
			colorSection = RGB(0,0,0);
		} 
		CString strSection;
		COLORREF colorSection;
	};

	class CItemData //aoid
	{
	public:
		long lSize;//size of struct ,for version control	
		DWORD_PTR dwptrItemData;//item data	 
		std::vector<CColorStringSection> vrItemStringSectionColorShow;//section string must has order in item text.
	};


	DECLARE_DYNAMIC(CARCTreeCtrlExWithColor)
public:
	CARCTreeCtrlExWithColor(void);
	~CARCTreeCtrlExWithColor(void);

public:
	BOOL SetItemData( HTREEITEM hItem, DWORD_PTR dwData );//warining:this function not same as CTreeCtrl::SetItemData( HTREEITEM hItem, DWORD_PTR dwData )
	DWORD_PTR GetItemData(HTREEITEM hItem);
	void SetItemDataEx(HTREEITEM hItem,CItemData& aoidDataEx);
	CItemData* GetItemDataEx(HTREEITEM hItem);
	BOOL DeleteItem( HTREEITEM hItem );
	BOOL DeleteAllItems(void);
protected:
	void Clear(void);
protected:
	std::vector<CItemData*> m_vrItemDataEx;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
