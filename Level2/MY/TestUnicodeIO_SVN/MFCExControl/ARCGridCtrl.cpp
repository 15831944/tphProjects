#include "StdAfx.h"
#include "ARCGridCtrl.h"


ARCGridCtrl::ARCGridCtrl()
{

}

ARCGridCtrl::~ARCGridCtrl()
{

}
BEGIN_MESSAGE_MAP(ARCGridCtrl,CGridCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void ARCGridCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CGridCtrl::OnLButtonDown(nFlags,point);
	BOOL bColumnClick = (m_MouseMode == MOUSE_SELECT_ROW)? TRUE:FALSE;
	GetParent()->SendMessage(WM_GRID_CLICK,(WPARAM)bColumnClick,(LPARAM)m_idCurrentCell.row);
}

void ARCGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CGridCtrl::OnLButtonDblClk(nFlags,point);
	GetParent()->SendMessage(WM_GRID_DBCLICK,NULL,(LPARAM)m_idCurrentCell.row);
}

// Sets the text associated with a particular item.
BOOL ARCGridCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
{
	return CGridCtrl::SetItemText(nItem,nSubItem,lpszText);
}

// Sets the data (lParam) associated with a particular item.
BOOL ARCGridCtrl::SetItemData(int nItem, DWORD_PTR dwData)
{
	for (int i = 0; i < GetColumnCount(); i++)
	{
		CGridCtrl::SetItemData(nItem,i,dwData);
	}
	return TRUE;
}

// Retrieves the data (lParam) associated with a particular item.
DWORD_PTR ARCGridCtrl::GetItemData(int nItem) const
{
	if(GetColumnCount() > 0)
		return CGridCtrl::GetItemData(nItem,0);
	return NULL;
}

// Retreives the number of selected items in the control.
UINT ARCGridCtrl::GetSelectedCount() const
{
	return 1;
}

// Adds an item to the control.
int ARCGridCtrl::InsertItem(int nItem, LPCTSTR lpszItem)
{
 	if ( GetRowCount() > GetVirtualRowCount() + 1)
 	{
 		return SetItemText(nItem+1,1,lpszItem);
 	}
	else
	{
		CString strRowTitle = _T("");
		strRowTitle.Format(_T("%d"),GetRowCount());
		InsertRow(strRowTitle);
		SetItemText(nItem+1,1,lpszItem);
	}
	return 1;
}

// Removes a single item from the control.
BOOL ARCGridCtrl::DeleteItem(int nItem)
{
	return DeleteRow(nItem);
}

// Removes all items from the control.
BOOL ARCGridCtrl::DeleteAllItems()
{
	return CGridCtrl::DeleteAllItems();
}

// Retrieves the number of items in the control.
int ARCGridCtrl::GetItemCount()
{
	return GetVirtualRowCount();
}

// Inserts a column into a report-mode control.
int ARCGridCtrl::InsertColumn(int nCol, LPCTSTR lpszColumnHeading,
				 int nFormat/* = LVCFMT_LEFT*/, int nWidth/* = -1*/, int nSubItem/* = -1*/)
{
	return CGridCtrl::InsertColumn(lpszColumnHeading);
}
