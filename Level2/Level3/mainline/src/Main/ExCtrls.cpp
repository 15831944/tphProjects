#include "stdafx.h"
#include "ExCtrls.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CTreeCtrlEx, CTreeCtrl)
	//{{AFX_MSG_MAP(CTreeCtrlEx)
	ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeyDown)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTreeCtrlEx::CTreeCtrlEx() :	m_iComboMinWidth( 150 ),
								m_iComboMaxWidth( UINT_MAX ),
								m_iComboDroppedHeight( 200 )
{
}

CTreeCtrlEx::~CTreeCtrlEx()
{
	RemoveAllComboBoxes();
}

inline void CTreeCtrlEx::SetComboDroppedHeight( int iNewHeight )
{
	m_iComboDroppedHeight = iNewHeight;
}

inline void CTreeCtrlEx::SetComboMinMaxWidths( UINT iNewMinWidth /*= 150*/, UINT iNewMaxWidth /*= UINT_MAX*/ )
{
	m_iComboMinWidth = iNewMinWidth;
	if( iNewMaxWidth < iNewMinWidth )
		m_iComboMaxWidth = iNewMinWidth;
	else
		m_iComboMaxWidth = iNewMaxWidth;
}

HTREEITEM CTreeCtrlEx::InsertItemWithComboBox(LPCTSTR lpszItem, BOOL bEditableCombo, HTREEITEM hParent /*= TVI_ROOT*/, HTREEITEM hInsertAfter /*= TVI_LAST*/)
{
	HTREEITEM hti = InsertItem( lpszItem, hParent, hInsertAfter );

	if( NULL != hti )
	{
		CTreeComboBox *pCB = new CTreeComboBox(1);
		CTreeComboBox *pCB2=new CTreeComboBox(2);
		DWORD dwComboBoxStyle = WS_CHILD | WS_VSCROLL;
		if( bEditableCombo )
			dwComboBoxStyle |= CBS_DROPDOWN;
		else
			dwComboBoxStyle |= CBS_DROPDOWNLIST;

		if( pCB->Create( dwComboBoxStyle, CRect(), this, 0 ) && pCB2->Create( dwComboBoxStyle, CRect(), this, 0 ))
		{
			// Add TreeCtrl items text into ComboBox to be sure that item in TreeCtrl is from its combo list.
			pCB->InsertString( 0, lpszItem );
			pCB2->InsertString( 0, lpszItem );
			m_mapExControls[ hti ] = pCB;
			m_mapExControls2[ hti ] = pCB2;
		} // if( pCB->Create( WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST, CRect(), this, 0 ) )'s end
	} // if( NULL != hti )'s end

	return hti;
}

void CTreeCtrlEx::RemoveComboBox( HTREEITEM htiRemoveFrom )
{
	CTreeComboBox *pCB;
	if( !m_mapExControls.Lookup( htiRemoveFrom, (CWnd*&)pCB ) )
		return;
	if( !m_mapExControls2.Lookup( htiRemoveFrom, (CWnd*&)pCB ) )
		return;

	m_mapExControls.RemoveKey( htiRemoveFrom );
	m_mapExControls2.RemoveKey( htiRemoveFrom );
	if( ::IsWindow( pCB->m_hWnd ) )
		pCB->DestroyWindow();
	delete pCB;
}

void CTreeCtrlEx::RemoveAllComboBoxes()
{
	HTREEITEM htiCurrent;
	CTreeComboBox *pCB;

	POSITION pos = m_mapExControls.GetStartPosition();
	while( pos )
	{
		m_mapExControls.GetNextAssoc( pos, htiCurrent, (CWnd*&)pCB );
		RemoveComboBox( htiCurrent );
	}
	pos = m_mapExControls2.GetStartPosition();
	while( pos )
	{
		m_mapExControls2.GetNextAssoc( pos, htiCurrent, (CWnd*&)pCB );
		RemoveComboBox( htiCurrent );
	}
}

int CTreeCtrlEx::InsertComboBoxString( HTREEITEM htiInsertTo, LPCTSTR lpszItem, int iIndex /*= -1*/ )
{
	CTreeComboBox *pCB;
	CTreeComboBox *pCB2;
	if( !m_mapExControls.Lookup( htiInsertTo, (CWnd*&)pCB ) )
		return -3; // ComboBox not found for given tree item
	if( !m_mapExControls2.Lookup( htiInsertTo, (CWnd*&)pCB2 ) )
		return -3; // ComboBox not found for given tree item
	pCB2->InsertString( iIndex, lpszItem );
	return pCB->InsertString( iIndex, lpszItem );
}

BOOL CTreeCtrlEx::DeleteItem( HTREEITEM hItem )
{
	RemoveComboBox( hItem );
	return CTreeCtrl::DeleteItem( hItem );
}

BOOL CTreeCtrlEx::DeleteAllItems()
{
	RemoveAllComboBoxes();
	return CTreeCtrl::DeleteAllItems();
}

bool CTreeCtrlEx::ShowComboBox( HTREEITEM hti )
{
	if( NULL == hti )
		return false;

	// Retrive attached combo box control for given Tree box item
	CTreeComboBox *pCB;
	CTreeComboBox *pCB2;
	if( !m_mapExControls.Lookup(hti, (CWnd*&)pCB) )
		return false;
	if( !m_mapExControls2.Lookup(hti, (CWnd*&)pCB2) )
		return false;

	// Set combo box dimentions and position 
	CRect itemRect;
	CRect clientRect;
	SCROLLINFO sInfo;
	int iVScrollWidth = 3; // Make the combo box fully visible

	GetItemRect( hti, &itemRect, TRUE );
	GetWindowRect( &clientRect );

	// Determine if scrollbar is visible
	memset( &sInfo, 0, sizeof(SCROLLINFO) );
	sInfo.cbSize = sizeof( SCROLLINFO );
	GetScrollInfo( SB_VERT, &sInfo );
	if( sInfo.nPage ) // Determine if is there vertical scroll bar in tree ctrl.
		iVScrollWidth += ::GetSystemMetrics( SM_CXVSCROLL );

	int iComboLeft = itemRect.left;
	UINT iComboWidth;
	int iClientWidth = clientRect.Width();

	if( m_iComboMaxWidth < m_iComboMinWidth )
		m_iComboMaxWidth = m_iComboMinWidth;

	if( (m_iComboMaxWidth == UINT_MAX) ||
		(m_iComboMaxWidth >= UINT(iClientWidth - itemRect.left - iVScrollWidth))
	  )
		iComboWidth = iClientWidth - itemRect.left - iVScrollWidth;
	else
		iComboWidth = m_iComboMaxWidth;

	if( (iComboWidth < m_iComboMinWidth) &&
		(int(iComboWidth) + iComboLeft >= iClientWidth - iVScrollWidth)
	  )
	{
		iComboLeft = iClientWidth - iVScrollWidth - m_iComboMinWidth;
		iComboWidth = m_iComboMinWidth;
	}

	if( m_iComboMinWidth > UINT(iClientWidth - iVScrollWidth) )
	{
		iComboLeft = 0;
		iComboWidth = iClientWidth - iVScrollWidth;
	}

	pCB->SetWindowPos( NULL, iComboLeft, itemRect.top, iComboWidth/2, m_iComboDroppedHeight/2, SWP_NOZORDER );
	pCB2->SetWindowPos( NULL, iComboLeft+iComboWidth/2, itemRect.top, iComboWidth/2, m_iComboDroppedHeight/2, SWP_NOZORDER );
	// Set the same font as CTreeCtrl has.
	pCB->SetFont( GetFont(), FALSE );
	pCB2->SetFont( GetFont(), FALSE );

	// Select current item in combo box if it's found.
	CString strItemText = GetItemText(hti);
	int iFIndex = pCB->FindString( -1, strItemText );
	if( iFIndex != CB_ERR )
		pCB->SetCurSel( iFIndex );
	else
		pCB->SetWindowText( strItemText );

	pCB->ShowWindow( SW_SHOWNORMAL );
	pCB->ShowDropDown( TRUE );
	//pCB->SetFocus();
	pCB2->ShowWindow( SW_SHOWNORMAL );
	pCB2->ShowDropDown( TRUE );
	//pCB2->SetFocus();

	return true;
}

void CTreeCtrlEx::OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;

	if( (pTVKeyDown->wVKey == VK_F4) &&
		!( GetKeyState( VK_MENU ) & 0x8000 ) &&
		ShowComboBox(GetSelectedItem())
	  )
	{
		*pResult = 1;
		return;
	}

	*pResult = 0;
}

void CTreeCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if( GetKeyState(VK_CONTROL) & 0x8000 )
	{
		CTreeCtrl::OnLButtonDblClk(nFlags, point);
		return;
	}

	// Determine where DblClick appears
	UINT flags;
	if( HitTest(point, &flags) &&
		(flags & TVHT_ONITEMLABEL) &&
		ShowComboBox(GetSelectedItem())
	  )
		return;

	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

// This class can not work with tree controls, which has TVS_EDITLABELS style,
// that is why we need to remove that style even if it was set from resource editor.
// If someone will set this style after subclassing, it will be on his own risk.
void CTreeCtrlEx::PreSubclassWindow() 
{
	SetWindowLong( m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) & ~TVS_EDITLABELS );
	CTreeCtrl::PreSubclassWindow();
}

// Implementation of CTreeCtrlEx::CTreeComboBox class
BEGIN_MESSAGE_MAP(CTreeCtrlEx::CTreeComboBox, CComboBox)
	//{{AFX_MSG_MAP(CTreeComboBox)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, CTreeComboBox::OnCloseUp)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, CTreeComboBox::OnKillFocus)
	ON_CONTROL_REFLECT(CBN_EDITUPDATE, CTreeComboBox::OnEditUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTreeCtrlEx::CTreeComboBox::OnCloseUp() 
{
	CString strText;
	CTreeCtrlEx* pParent = (CTreeCtrlEx*)GetParent();
	ASSERT( pParent );

	int iCurSel = GetCurSel();
	if( iCurSel == CB_ERR )
		strText = strCurrText;
	else
		GetLBText( iCurSel, strText );

	if( !strText.IsEmpty() )
	{
		if(nID==1)
		{
			pParent->SetItemText1( pParent->GetSelectedItem(), strText );
		}
		else
		{
			if(nID==2)
			{
				pParent->SetItemText2( pParent->GetSelectedItem(), strText );
			}
		}
	}
		
	
	pParent->SetFocus();
}

void CTreeCtrlEx::CTreeComboBox::OnKillFocus() 
{
	ShowWindow( SW_HIDE );
}

void CTreeCtrlEx::CTreeComboBox::OnEditUpdate() 
{
	// We need to hold last entered text to set it as an item description, 'cause
	// combo box sets matched text into it's edit part
	GetWindowText( strCurrText );
}



//BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
//	//{{AFX_MSG_MAP(CListCtrlEx)
//	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
//	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeyDown)
//	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()
//
//CListCtrlEx::CListCtrlEx() :	m_iComboMinWidth( 150 ),
//								m_iComboMaxWidth( UINT_MAX ),
//								m_iComboDroppedHeight( 200 )
//{
//}
//
//CListCtrlEx::~CListCtrlEx()
//{
//	RemoveAllComboBoxes();
//}
//
//inline void CListCtrlEx::SetComboDroppedHeight( int iNewHeight )
//{
//	m_iComboDroppedHeight = iNewHeight;
//}
//
//inline void CListCtrlEx::SetComboMinMaxWidths( UINT iNewMinWidth /*= 150*/, UINT iNewMaxWidth /*= UINT_MAX*/ )
//{
//	m_iComboMinWidth = iNewMinWidth;
//	if( iNewMaxWidth < iNewMinWidth )
//		m_iComboMaxWidth = iNewMinWidth;
//	else
//		m_iComboMaxWidth = iNewMaxWidth;
//}
//
//int CListCtrlEx::InsertItemWithComboBox( LPCTSTR lpszItem, BOOL bEditableCombo )
//{
//	int nItem = InsertItem( GetItemCount(), lpszItem );
//
//	if( nItem >= 0 )
//	{
//		CListComboBox *pCB = new CListComboBox;
//		DWORD dwComboBoxStyle = WS_CHILD | WS_VSCROLL;
//		if( bEditableCombo )
//			dwComboBoxStyle |= CBS_DROPDOWN;
//		else
//			dwComboBoxStyle |= CBS_DROPDOWNLIST;
//
//		if( pCB->Create( dwComboBoxStyle, CRect(), this, 0 ) )
//		{
//			// Add ListCtrl items text into ComboBox to be sure that item in ListCtrl is from its combo list.
//			pCB->InsertString( 0, lpszItem );
//			m_mapExControls[ nItem ] = pCB;
//		}
//	}
//
//	return nItem;
//}
//
//void CListCtrlEx::RemoveComboBox( int nItemRemoveFrom )
//{
//	CListComboBox *pCB;
//	if( !m_mapExControls.Lookup( nItemRemoveFrom, (CWnd*&)pCB ) )
//		return;
//
//	m_mapExControls.RemoveKey( nItemRemoveFrom );
//	if( ::IsWindow( pCB->m_hWnd ) )
//		pCB->DestroyWindow();
//	delete pCB;
//}
//
//void CListCtrlEx::RemoveAllComboBoxes()
//{
//	int nItem;
//	CListComboBox *pCB;
//
//	POSITION pos = m_mapExControls.GetStartPosition();
//	while( pos )
//	{
//		m_mapExControls.GetNextAssoc( pos, nItem, (CWnd*&)pCB );
//		RemoveComboBox( nItem );
//	}
//}
//
//int CListCtrlEx::InsertComboBoxString( int nItemInsertTo, LPCTSTR lpszItem, int iIndex /*= -1*/ )
//{
//	CListComboBox *pCB;
//	if( !m_mapExControls.Lookup( nItemInsertTo, (CWnd*&)pCB ) )
//		return -3; // ComboBox not found for given tree item
//
//	return pCB->InsertString( iIndex, lpszItem );
//}
//
//BOOL CListCtrlEx::DeleteItem( int nItem )
//{
//	RemoveComboBox( nItem );
//	return CListCtrl::DeleteItem( nItem );
//}
//
//BOOL CListCtrlEx::DeleteAllItems()
//{
//	RemoveAllComboBoxes();
//	return CListCtrl::DeleteAllItems();
//}
//
//bool CListCtrlEx::ShowComboBox( int nItem )
//{
//	//Never show combo box while not in report or list view
//	if( !(GetWindowLong(m_hWnd, GWL_STYLE) & LVS_REPORT) && !(GetWindowLong(m_hWnd, GWL_STYLE) & LVS_LIST) )
//		return false;
//
//	// Retrive attached combo box control for given list box item
//	CComboBox *pCB;
//	if( !m_mapExControls.Lookup( nItem, (CWnd*&)pCB ) )
//		return false;
//
//	// Set combo box dimentions and position 
//	CRect itemRect;
//	CRect clientRect;
//	int iMaxWidth = GetColumnWidth( 0 );
//	int iVScrollWidth = 3; // Make the combo box fully visible
//
//	GetItemRect( nItem, &itemRect, LVIR_LABEL );
//	GetWindowRect( &clientRect );
//
//	// Determine if scrollbar is visible and LVS_NOSCROLL not set
//	CSize sz = ApproximateViewRect();
//	if( (sz.cy > clientRect.Height()) &&
//		!(GetWindowLong(m_hWnd, GWL_STYLE) & LVS_NOSCROLL)
//	  )
//		iVScrollWidth += ::GetSystemMetrics( SM_CXVSCROLL );
//
//	// Determine the maximum available width for combo box
//	if( iMaxWidth > clientRect.Width() - iVScrollWidth )
//		iMaxWidth = clientRect.Width() - iVScrollWidth;
//
//	int iComboLeft = itemRect.left;
//	UINT iComboWidth;
//	if( (m_iComboMaxWidth == UINT_MAX) ||
//		(m_iComboMaxWidth > UINT(iMaxWidth - itemRect.left))
//	  )
//		iComboWidth = iMaxWidth - itemRect.left;
//	else
//		iComboWidth = m_iComboMaxWidth;
//
//	if( iComboWidth < m_iComboMinWidth )
//	{
//		iComboLeft = iMaxWidth - m_iComboMinWidth;
//		iComboWidth = m_iComboMinWidth;
//	}
//
//	if( m_iComboMinWidth > UINT(iMaxWidth) )
//	{
//		iComboLeft = 0;
//		iComboWidth = iMaxWidth;
//	}
//
//	pCB->SetWindowPos( NULL, iComboLeft, itemRect.top, iComboWidth, m_iComboDroppedHeight, SWP_NOZORDER );
//
//	pCB->SetFont( GetFont(), FALSE );
//
//	// Select current item in combo box if it's found.
//	CString strItemText = GetItemText( nItem, 0 );
//	int iFIndex = pCB->FindString( -1, strItemText );
//	if( iFIndex != CB_ERR )
//		pCB->SetCurSel( iFIndex );
//	else
//		pCB->SetWindowText( strItemText );
//
//	pCB->ShowDropDown( TRUE );
//	pCB->ShowWindow( SW_SHOWNORMAL );
//	pCB->SetFocus();
//
//	return true;
//}
//
//void CListCtrlEx::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
//{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
//	*pResult = 0;
//
//	if( GetKeyState(VK_CONTROL) & 0x8000 )
//		return;
//
//	// Determine where DblClick appears
//	UINT flags;
//	if( (HitTest(pNMListView->ptAction, &flags) > -1) &&
//		(flags & LVHT_ONITEMLABEL) &&
//		ShowComboBox( pNMListView->iItem )
//	  )
//		*pResult = 1;
//}
//
//void CListCtrlEx::OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult) 
//{
//	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
//	*pResult = 0;
//
//	if( pLVKeyDow->wVKey == VK_F4 )
//	{
//		POSITION pos = GetFirstSelectedItemPosition();
//		if( pos )
//		{
//			ShowComboBox( GetNextSelectedItem(pos) );
//			*pResult = 1;
//		}
//	}
//}
//
//// This class can not work with list control, which hasn't LVS_SINGLESEL style,
//// that is why we need to add that style even if it wasn't set from resource editor.
//// If someone will remove this style after subclassing, it will be on his own risk.
//void CListCtrlEx::PreSubclassWindow()
//{
//	SetWindowLong( m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) | LVS_SINGLESEL );
//	CListCtrl::PreSubclassWindow();
//}
//
//// Implementation of CListCtrlEx::CListComboBox class
//BEGIN_MESSAGE_MAP(CListCtrlEx::CListComboBox, CComboBox)
//	//{{AFX_MSG_MAP(CListComboBox)
//	ON_CONTROL_REFLECT(CBN_CLOSEUP, CListComboBox::OnCloseUp)
//	ON_CONTROL_REFLECT(CBN_KILLFOCUS, CListComboBox::OnKillFocus)
//	ON_CONTROL_REFLECT(CBN_EDITUPDATE, CListComboBox::OnEditUpdate)
//	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()
//
//void CListCtrlEx::CListComboBox::OnCloseUp() 
//{
//	CString strText;
//	CListCtrl* pParent = (CListCtrl*)GetParent();
//	ASSERT( pParent );
//
//	int iCurSel = GetCurSel();
//	if( iCurSel == CB_ERR )
//		strText = strCurrText;
//	else
//		GetLBText( iCurSel, strText );
//
//	POSITION pos = pParent->GetFirstSelectedItemPosition();
//	if( pos && !strText.IsEmpty() )
//		pParent->SetItemText( pParent->GetNextSelectedItem( pos ), 0, strText );
//
//	pParent->SetFocus();
//}
//
//void CListCtrlEx::CListComboBox::OnKillFocus() 
//{
//	ShowWindow( SW_HIDE );
//}
//
//void CListCtrlEx::CListComboBox::OnEditUpdate() 
//{
//	GetWindowText( strCurrText );
//}

void CTreeCtrlEx::SetItemText1(HTREEITEM hItem, LPCTSTR lpszItem)
{
	CString strItemText=GetItemText(hItem);
	int nCount=strItemText.Find("||");
	CString str1=strItemText.Left(nCount);
	CString str2=strItemText.Delete(0,nCount+2);

	str1=lpszItem;

	str1.TrimLeft();
	str2.TrimLeft();
	str1.TrimRight();
	str2.TrimRight();
	SetItemText(hItem,str1+"   ||   "+str2);
}

void CTreeCtrlEx::SetItemText2(HTREEITEM hItem, LPCTSTR lpszItem)
{
	CString strItemText=GetItemText(hItem);
	int nCount=strItemText.Find("||");
	CString str1=strItemText.Left(nCount);
	CString str2=strItemText.Delete(0,nCount+2);

	str2=lpszItem;
	str1.TrimLeft();
	str2.TrimLeft();
	str1.TrimRight();
	str2.TrimRight();
	SetItemText(hItem,str1+"   ||   "+str2);
}
