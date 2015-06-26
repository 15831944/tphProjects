// AirsideReportListCtrl.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideReport.h"
#include "sortableListCtrl.h"


// CSortableListCtrl

IMPLEMENT_DYNAMIC(CSortableListCtrl, CXListCtrl)
CSortableListCtrl::CSortableListCtrl()
{
}

CSortableListCtrl::~CSortableListCtrl()
{
}


BEGIN_MESSAGE_MAP(CSortableListCtrl, CXListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnLvnColumnclick)
	//ON_MESSAGE(WM_XLISTCTRL_CHECKBOX_CLICKED,OnXListCtrlCheckBoxClicked)
	//ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	//ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
END_MESSAGE_MAP()



// CSortableListCtrl message handlers




//void CSortableListCtrl::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//
//	int nTestIndex = pNMLV->iSubItem;
//	if( nTestIndex >= 0 )
//	{
//		CWaitCursor	wCursor;
//		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
//			m_wndSortableHeaderCtrl.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
//		else
//			m_wndSortableHeaderCtrl.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
//		m_wndSortableHeaderCtrl.SaveSortList();
//	}	
//
//	*pResult = 0;
//
//}
//LRESULT CSortableListCtrl::OnXListCtrlCheckBoxClicked(WPARAM wParam,LPARAM lParam)
//{
//
//
//	return 0;
//}
int CSortableListCtrl::InsertColumn(int nCol, LPCTSTR lpszColumnHeading,int nFormat, int nWidth, int nSubItem,EDataType dataType)
{
	int nReturn = CXListCtrl::InsertColumn(nCol,lpszColumnHeading,nFormat,nWidth, nSubItem);
	m_wndSortableHeaderCtrl.SetDataType(nCol,dataType);
	return nReturn;
}
void CSortableListCtrl::SubClassHeadCtrl()
{
	m_wndSortableHeaderCtrl.SubclassWindow( GetHeaderCtrl()->m_hWnd );
}
void CSortableListCtrl::OnLvnColumnclick(NMHDR* pNMHDR, LRESULT* pResult)
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








//void CSortableListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}
//
//void CSortableListCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}
