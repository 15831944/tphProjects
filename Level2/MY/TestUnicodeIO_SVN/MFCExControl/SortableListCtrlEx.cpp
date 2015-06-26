#include "StdAfx.h"
#include ".\sortablelistctrlex.h"

IMPLEMENT_DYNAMIC(CSortableListCtrlEx, CListCtrlEx)

CSortableListCtrlEx::CSortableListCtrlEx(void)
{
}

CSortableListCtrlEx::~CSortableListCtrlEx(void)
{
}

BEGIN_MESSAGE_MAP(CSortableListCtrlEx, CListCtrlEx)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnLvnColumnclick)
END_MESSAGE_MAP()

int CSortableListCtrlEx::InsertColumn(int nCol, const LV_COLUMNEX* pColumn, EDataType dataType)
{
	int nReturn = CListCtrlEx::InsertColumn(nCol, pColumn);
	m_wndSortableHeaderCtrl.SetDataType(nCol,dataType);
	return nReturn;
}

void CSortableListCtrlEx::SubClassHeadCtrl()
{
	m_wndSortableHeaderCtrl.SubclassWindow( GetHeaderCtrl()->m_hWnd );
}

void CSortableListCtrlEx::OnLvnColumnclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nTestIndex = pNMListView->iSubItem;
	if( nTestIndex >= 0 )
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
			m_wndSortableHeaderCtrl.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
		else
			m_wndSortableHeaderCtrl.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
		m_wndSortableHeaderCtrl.SaveSortList();
	}	

	*pResult = 0;
}








//void CSortableListCtrlEx::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}
//
//void CSortableListCtrlEx::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}
