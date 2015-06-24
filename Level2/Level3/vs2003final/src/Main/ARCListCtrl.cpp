#include "StdAfx.h"
#include ".\arclistctrl.h"
#include "../MFCExControl/InPlaceEdit.h"
#include <common/CPPUtil.h>
#include "..\common\WinMsg.h"
#include "Resource.h"
#include "..\common\ProbabilityDistribution.h"
#include "..\common\probdistmanager.h"
#include "..\Inputs\probab.h"
#include "../MFCExControl\InPlaceList.h"
#include "DlgProbDIst.h"
#include "inputs/IN_TERM.H"
#include "DlgTimeRange.h"


#define ID_DATETIMEPICKER_TIME  101
#define ID_IPEDIT 102


BEGIN_MESSAGE_MAP(CARCListCtrl, CListCtrl)
	ON_WM_LBUTTONDBLCLK()	
	ON_NOTIFY_REFLECT_EX(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT_EX(LVN_COLUMNCLICK, OnClickColumn)
END_MESSAGE_MAP()




ListCtrlItemImpl* CARCListCtrl::GetItemImpl( int nItem, int nSubItem ) const
{
	ItemImplMap* pMap = GetImplMap(nItem);
	if(!pMap)
		return NULL;
	
	ItemImplMap::const_iterator itr = pMap->find(nSubItem);
	if(itr!=pMap->end())
	{
		return itr->second;
	}
	return NULL;
}

void CARCListCtrl::SetItemImpl( int nItem, int nSubItem, ListCtrlItemImpl* impl,bool bManageDel)
{
	ItemImplMap* pMap = GetImplMap(nItem);
	ASSERT(pMap);

	(*pMap)[nSubItem] = impl;
	if(impl)
	{
		impl->OnSetTo(*this,nItem,nSubItem);
		if(bManageDel)
			AddManageImp(impl);
	}
}

int CARCListCtrl::HitTestEx( CPoint& point, int* col )
{
	int colnum = 0;
	int row = HitTest(point, NULL);

	if(col) 
		*col = 0;

	// Make sure that the ListView is in LVS_REPORT
	if((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
		return row;

	// Get the number of columns
	CHeaderCtrl* pHeader = GetHeaderCtrl();
	int nColumnCount = pHeader->GetItemCount();

	// Get the top and bottom row visible
	row = GetTopIndex();

	int bottom = row + GetCountPerPage();
	if(bottom > GetItemCount())
		bottom = GetItemCount();

	// Loop through the visible rows
	for( ;row <=bottom; row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		if( GetItemRect(row, &rect, LVIR_BOUNDS) )
		{
			if( rect.PtInRect(point) )
			{
				// Now find the column
				for( colnum = 0; colnum < nColumnCount; colnum++ )
				{
					int colwidth = GetColumnWidth(colnum);
					if(point.x >= rect.left && point.x <= (rect.left + colwidth))
					{
						if(col) 
							*col = colnum;
						return row;
					}
					rect.left += colwidth;
				}
			}
		}		
	}
	return -1;
}

CEdit* ListCtrlEditImpl::ShowInPlaceEdit(CListCtrl& ctrl, int nItem, int nCol )
{
	// The returned pointer should not be saved

	// Make sure that the item is visible
	if(!ctrl.EnsureVisible(nItem, TRUE)) 
		return NULL;

	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = ctrl.GetHeaderCtrl();
	int nColumnCount = pHeader->GetItemCount();
	if(nCol >= nColumnCount || ctrl.GetColumnWidth(nCol) < 5)
		return NULL;

	// Get the column offset
	int offset = 0;
	for(int i = 0; i < nCol; i++)
		offset += ctrl.GetColumnWidth(i);

	CRect rect;
	ctrl.GetItemRect(nItem, &rect, LVIR_BOUNDS);

	// Now scroll if we need to expose the column
	CRect rcClient;
	ctrl.GetClientRect(&rcClient);
	if(offset + rect.left < 0 || offset + rect.left > rcClient.right)
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		ctrl.Scroll(size);
		rect.left -= size.cx;
	}
	// Get Column alignment
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	ctrl.GetColumn(nCol, &lvcol);
	DWORD dwStyle ;
	if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else dwStyle = ES_CENTER;

	rect.left += offset+4;
	rect.right = rect.left + ctrl.GetColumnWidth(nCol) - 3 ;
	if(rect.right > rcClient.right) 
		rect.right = rcClient.right;

	dwStyle |= WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;

	int nUseType = IPEDIT_TYPE_NORMAL;
	//if (((LVCOLDROPLIST*)ddStyleList[nCol])->Style == NUMBER_ONLY)
	//	nUseType = IPEDIT_TYPE_NUMBER;
	//if( ((LVCOLDROPLIST*)ddStyleList[nCol])->Style == TEXT_EDIT )
	//	dwStyle |=ES_UPPERCASE;

	CEdit *pEdit = new CInPlaceEdit(nItem, nCol, ctrl.GetItemText(nItem, nCol), nUseType);
	pEdit->Create( dwStyle, rect, &ctrl, ID_IPEDIT );
	return pEdit;
}






void ListCtrlEditImpl::DblClickItem( CListCtrl& ctrl, int index, int column )
{
	m_pEdit=ShowInPlaceEdit(ctrl,index,column);
}

void ListCtrlEditImpl::OnDoneEdit( CListCtrl& ctrl, int index, int column )
{
	if(m_pEdit)
	{	
		m_pEdit->GetWindowText(strRet);
		ctrl.SetItemText(index,column,strRet);
	}
}

int CARCListCtrl::GetSelectedItems( IntList& itemlist ) const
{	
	int nCount=0;
	POSITION   pos   =   GetFirstSelectedItemPosition();   
	if   (pos   ==   NULL)   
		TRACE0("No   items   were   selected!\n");   
	else   
	{   
		while   (pos)   
		{   
			int   nItem   =   GetNextSelectedItem(pos);   
			TRACE1("Item   %d   was   selected!\n",   nItem);   
			//   you   could   do   your   own   processing   on   nItem   here 
			itemlist.push_back(nItem);
			nCount++;
		}   
	}   
	return nCount;	
}
//
//int CARCListCtrl::GetCurSel() const
//{
//	return 0;
//}

BOOL CARCListCtrl::OnEndlabeledit( NMHDR* pNMHDR, LRESULT* pResult )
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* plvItem = &pDispInfo->item;
	if(plvItem)
	{
		ListCtrlItemImpl* pImpl = GetItemImpl(plvItem->iItem,plvItem->iSubItem);
		if(pImpl)
			pImpl->OnDoneEdit(*this, plvItem->iItem,plvItem->iSubItem);
	}

	return TRUE;
}

CARCListCtrl::ItemImplMap* CARCListCtrl::GetImplMap( int nItem )
{
	ItemImplMap* itemdata = (ItemImplMap*)GetItemData(nItem);
	if(std::find(m_vItemMapList.begin(),m_vItemMapList.end(),itemdata)==m_vItemMapList.end())
	{
		itemdata = new ItemImplMap;
		SetItemData(nItem,(DWORD_PTR)itemdata);
		m_vItemMapList.push_back(itemdata);
	}
	return itemdata;
}

CARCListCtrl::ItemImplMap* CARCListCtrl::GetImplMap( int nItem ) const
{
	ItemImplMap* itemdata = (ItemImplMap*)GetItemData(nItem);
	if(std::find(m_vItemMapList.begin(),m_vItemMapList.end(),itemdata)==m_vItemMapList.end())
	{
		itemdata = NULL;
	}
	return itemdata;
}

void CARCListCtrl::Clear()
{
	for (size_t i=0;i< m_vItemMapList.size();++i)
	{
		delete m_vItemMapList[i];
	}
	m_vItemMapList.clear();
}

void CARCListCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	// get the click column
	int index;
	int column;
	if((index = HitTestEx(point, &column)) != -1)
	{
		ListCtrlItemImpl* pImpl = GetItemImpl(index,column);
		if(pImpl)
			pImpl->DblClickItem(*this, index, column);
	}
	__super::OnLButtonDblClk(nFlags, point);
}

BOOL CARCListCtrl::OnClickColumn( NMHDR* pNMHDR, LRESULT* pResult )
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* plvItem = &pDispInfo->item;
	if(plvItem)
	{
		//m_lastSelItem = plvItem->iItem;
		//m_lastSelSubItem = plvItem->iSubItem;
	}
	return TRUE;
}

void CARCListCtrl::SelectItem( int nItem )
{
	if(nItem<GetItemCount() && nItem>=0)
	{
		SetFocus(); 
		SetItemState(nItem,   LVIS_SELECTED,   LVIS_SELECTED   |   LVIS_FOCUSED); 
		EnsureVisible(nItem,   FALSE); 
		::SendMessage(m_hWnd,   LVM_SETEXTENDEDLISTVIEWSTYLE, 
			LVS_EX_FULLROWSELECT,   LVS_EX_FULLROWSELECT); 
	}
}

CARCListCtrl::CARCListCtrl()
{
	//m_lastSelSubItem = m_lastSelSubItem = -1;
}

CARCListCtrl::~CARCListCtrl()
{
	Clear();
	ClearMangerList();
}

void CARCListCtrl::ClearMangerList()
{
	for(size_t i=0;i<m_vManageImpList.size();++i)
		delete m_vManageImpList[i];
	m_vManageImpList.clear();
}

void CARCListCtrl::AddManageImp( ListCtrlItemImpl* pImpl )
{

}

void ListCtrlProbDistImpl::DblClickItem( CListCtrl& ctrl, int index, int column )
{
	//CStringList strList;
	strList.RemoveAll();
	CString s;
	s.LoadString( IDS_STRING_NEWDIST );
	strList.InsertAfter( strList.GetTailPosition(), s );
	int nCount = _g_GetActiveProbMan( m_pInTerm )->getCount();
	for( int m=0; m<nCount; m++ )
	{
		CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInTerm )->getItem( m );			
		strList.InsertAfter( strList.GetTailPosition(), pPBEntry->m_csName );
	}

	UINT flag = LVIS_FOCUSED;
	if(ctrl.GetItemState(index, flag) & flag)
	{
		// Add check for LVS_EDITLABELS
		// if(GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS)
		__super::DblClickItem(ctrl,index,column);
	}
}

void ListCtrlProbDistImpl::OnDoneEdit( CListCtrl& ctrl, int index, int column )
{
	// change data.
	//pProbDist = NULL;
	if(m_pCombCtrl->GetCurSel()==0)
	{
		CDlgProbDist dlg(m_pInTerm->m_pAirportDB,  true );
		if(dlg.DoModal()==IDOK) {
			int idxPD = dlg.GetSelectedPDIdx();
			ASSERT(idxPD!=-1);
			CProbDistEntry* pde = _g_GetActiveProbMan( m_pInTerm )->getItem(idxPD);			
			if(pde)
				strDist = pde->m_csName;
			//pProbDist = pde->m_pProbDist;
		}	

	}	
	else
	{
		
		int nCount = _g_GetActiveProbMan( m_pInTerm )->getCount();
		int nSel = m_pCombCtrl->GetCurSel()-1;
		if(nSel<nCount && nSel>=0)
		{
			CProbDistEntry* pde = _g_GetActiveProbMan( m_pInTerm )->getItem( nSel);
			if(pde)
				strDist = pde->m_csName;
			//ctrl.SetItemText( index,column,pde->m_csName );
			//pProbDist = pde->m_pProbDist;
		}				
	}
	//assert( pProbDist );	
}

ListCtrlProbDistImpl::ListCtrlProbDistImpl( InputTerminal* pInputTerm ) :m_pInTerm(pInputTerm)
{
//	pProbDist = NULL;
}
CComboBox* ListCtrlCombomBoxImpl::ShowInPlaceList( CListCtrl& ctrl,int nItem, int nCol )
{
	// The returned pointer should not be saved

	// Make sure that the item is visible
	if(!ctrl.EnsureVisible(nItem, TRUE)) 
		return NULL;

	// Make sure that nCol is valid 
	CHeaderCtrl* pHeader = ctrl.GetHeaderCtrl();
	int nColumnCount = pHeader->GetItemCount();
	if(nCol >= nColumnCount || ctrl.GetColumnWidth(nCol) < 10)
		return NULL;

	// Get the column offset
	int offset = 0;
	for(int i = 0; i < nCol; i++)
		offset += ctrl.GetColumnWidth(i);

	CRect rect;
	ctrl.GetItemRect(nItem, &rect, LVIR_BOUNDS);

	// Now scroll if we need to expose the column
	CRect rcClient;
	ctrl.GetClientRect(&rcClient);
	if(offset + rect.left < 0 || offset + rect.left > rcClient.right)
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		ctrl.Scroll(size);
		rect.left -= size.cx;
	}
	rect.left += (offset + 4);
	rect.right = rect.left + ctrl.GetColumnWidth(nCol) - 3;
	int height = rect.bottom - rect.top;
	rect.bottom += (10 * height);
	if(rect.right > rcClient.right) 
		rect.right = rcClient.right;

	DWORD dwStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST|CBS_DISABLENOSCROLL;


	//CStringList& strList = ((LVCOLDROPLIST*)ddStyleList[nCol])->List;
	CString csSelStr = ctrl.GetItemText( nItem, nCol );
	POSITION pos;
	i = 0;
	int nSelIndex = 0;
	for( pos = strList.GetHeadPosition(); pos; i++ )
	{

		CString csStr = strList.GetAt( pos );
		if( csStr == csSelStr )
		{
			nSelIndex = i;
			break;
		}
		strList.GetNext( pos );
	}	

	CComboBox *pList = new CInPlaceList(nItem, nCol, &strList, nSelIndex );
	pList->Create(dwStyle, rect, &ctrl, ID_IPEDIT);
	pList->SetItemHeight(-1, height);
	pList->SetHorizontalExtent(ctrl.GetColumnWidth(nCol));
	return pList;
}

void ListCtrlCombomBoxImpl::DblClickItem( CListCtrl& ctrl, int index, int column )
{
	m_pCombCtrl = ShowInPlaceList(ctrl,index,column) ;
}


void ListCtrlTimeRangeImpl::DblClickItem( CListCtrl& ctrl, int index, int column )
{
	CDlgTimeRange dlgTimeRange(startT,endT,FALSE, ctrl.GetParent());
	if(dlgTimeRange.DoModal()==IDOK)
	{
		startT = dlgTimeRange.GetStartTime();
		endT  = dlgTimeRange.GetEndTime();
	}
	OnDoneEdit(ctrl,index,column);
}

