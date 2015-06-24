// ListWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ListWnd.h"
//#include "termplan.h"

#include "ShapesManager.h"

#include "common\ProjectManager.h"
#include "common\WinMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CListWnd
#define IDC_DROPLIST 333
#define IDC_DROPLISTTYPE	334
CListWnd::CListWnd()
{
	//m_pParent=NULL;
	m_bHideParent = TRUE;
	m_nShapeItem = -1;
}

CListWnd::~CListWnd()
{
}


BEGIN_MESSAGE_MAP(CListWnd, CWnd)
	//{{AFX_MSG_MAP(CListWnd)
	ON_WM_CREATE()
	ON_NOTIFY(NM_CLICK, IDC_DROPLIST, OnClickListItems)
	ON_NOTIFY(NM_CLICK, IDC_DROPLISTTYPE, OnClickListSets)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_LISTCTRL_KILLFOCUS,OnListCtrlKillFocus)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CListWnd message handlers

int CListWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!CreateListItems())
		return -1;
	if(!CreateListSets())
		return -1;
	if(!CreatePreviewCtrl())
		return -1;

	return 0;
}

BOOL CListWnd::CreateListItems()
{
	CRect listRect(0,0,0,0);
	listRect.left+=LISTTYPEWND_WIDTH;
	listRect.right =LISTWND_WIDTH;
	listRect.bottom=LISTWND_HEIGHT;
	//can't have a control Id with WS_POPUP style
	if (! m_listCtrlItems.CreateEx ( NULL,
		WS_VISIBLE|WS_CHILD | WS_BORDER|LVS_ICON|
		LVS_ALIGNTOP|LVS_AUTOARRANGE|LVS_SINGLESEL |LVS_SHOWSELALWAYS  ,
		listRect, this, IDC_DROPLIST))
	{
		return FALSE;
	}

	m_listCtrlItems.m_pParent = this;
	m_listCtrlItems.CreateThumnailImageList();

	// draw thumbnail images in list control
	//m_listCtrlItems.DrawThumbnails(-1);

	// set focus and select the first thumbnail in the list control
	m_listCtrlItems.SetFocus();
	m_listCtrlItems.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	

	return TRUE;
}

BOOL CListWnd::CreateListSets()
{
	CRect listRect(0,0,0,0);
	listRect.right =LISTTYPEWND_WIDTH;
	listRect.bottom=LISTWND_HEIGHT;
	//can't have a control Id with WS_POPUP style
	if (! m_listCtrlSets.CreateEx ( NULL,
		WS_VISIBLE|WS_CHILD | WS_BORDER|
		LVS_ICON|LVS_ALIGNTOP|LVS_AUTOARRANGE 
		|LVS_SINGLESEL |LVS_SHOWSELALWAYS,
		listRect, this, IDC_DROPLISTTYPE))
	{
		return FALSE;
	}
	m_listCtrlSets.m_pParent = this;
	m_listCtrlSets.CreateThumnailImageList();

	// draw thumbnail images in list control
	m_listCtrlSets.DrawThumbnails(m_nShapeItem,0);

	// set focus and select the first thumbnail in the list control
	m_listCtrlSets.SetFocus();
	m_listCtrlSets.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	
	UpdateList( m_listCtrlSets.GetSelectedItem() );

	return TRUE;

}

void CListWnd::SetShapeItem(int nShapeItem)
{
	m_nShapeItem = nShapeItem;
}

BOOL CListWnd::CreatePreviewCtrl()
{
	CRect rect(0,0,64,64);
	BOOL bResult=m_staticPreviewCtrl.CreateEx(0,"static",NULL,
		WS_POPUP|WS_BORDER|SS_BITMAP|SS_LEFT,rect,this,0) ;
	return bResult;

}

void CListWnd::OnClickListItems(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CWnd* pWnd = GetParentWnd();
	LPNMITEMACTIVATE pItemAct = (LPNMITEMACTIVATE)pNMHDR;

	// determine which item receives the click
	LVHITTESTINFO  hitTest;
	memset(&hitTest, '\0', sizeof(LVHITTESTINFO));
	hitTest.pt = pItemAct->ptAction;
	m_listCtrlItems.SendMessage(LVM_SUBITEMHITTEST, 0, (LPARAM)&hitTest);

	// draw the selected image
	if(hitTest.iItem != m_listCtrlItems.GetSelectedItem() && hitTest.iItem >= 0)
	{
		m_listCtrlItems.SetSelectedItem( hitTest.iItem );
	}

	*pResult = 0;
}


void CListWnd::OnClickListSets(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CWnd* pWnd = GetParentWnd();
	LPNMITEMACTIVATE pItemAct = (LPNMITEMACTIVATE)pNMHDR;

	// determine which item receives the click
	LVHITTESTINFO  hitTest;
	memset(&hitTest, '\0', sizeof(LVHITTESTINFO));
	hitTest.pt = pItemAct->ptAction;
	m_listCtrlSets.SendMessage(LVM_SUBITEMHITTEST, 0, (LPARAM)&hitTest);

	// draw the selected image
	if(hitTest.iItem != m_listCtrlSets.GetSelectedItem() && hitTest.iItem >= 0)
	{
		m_listCtrlSets.SetSelectedItem( hitTest.iItem );
		UpdateList( m_listCtrlSets.GetSelectedItem() );
	}

	*pResult = 0;
}


// pass in the CThumbnailsList2 pointer to identify which the control is
void CListWnd::DisplayPreviewCtrl(CThumbnailsList2* pThumbnailList/* add by Benny */)
{
// 	int _nSet = m_listCtrlSets.GetSelectedItem();
// 	int _nItem = m_listCtrlItems.GetSelectedItem();
	// we get the indices in new method
	int _nSet = -1;
	int _nItem = -1;
	if (&m_listCtrlSets == pThumbnailList)
	{
		_nSet = m_listCtrlSets.GetMouseMoveItem();
		_nItem = 0;
	}
	else if (&m_listCtrlItems == pThumbnailList)
	{
		_nSet = m_listCtrlSets.GetSelectedItem();
		_nItem = m_listCtrlItems.GetMouseMoveItem();
	}


	if(m_nShapeItem != -1)
		_nSet = m_nShapeItem;

	if( -1 == _nSet || -1 == _nItem )
		return;

	HBITMAP hBmp = SHAPESMANAGER->GetShapeImgBitmap(_nSet, _nItem);

	m_staticPreviewCtrl.SetBitmap(hBmp) ;

	CRect rect;
	m_listCtrlItems.GetWindowRect(rect) ;
	rect.left =rect.right ;

	m_staticPreviewCtrl.SetWindowPos (&wndNoTopMost, rect.left, rect.top,
		0,0, SWP_NOSIZE|SWP_SHOWWINDOW |SWP_NOACTIVATE);
}

void CListWnd::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	if( m_listCtrlSets.GetSafeHwnd() )
		m_listCtrlSets.SetFocus();
}


void CListWnd::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	if(::GetFocus()!=m_listCtrlItems.GetSafeHwnd()&&::GetFocus()!=m_listCtrlSets.GetSafeHwnd()&&::GetFocus()!=m_hWnd)  
	{
		ShowWindow(SW_HIDE);
		if(m_staticPreviewCtrl.GetSafeHwnd() )
			m_staticPreviewCtrl.ShowWindow(SW_HIDE);
		
		if(GetParentWnd() && GetParentWnd()->GetSafeHwnd())
		{
			if(m_bHideParent)
				GetParentWnd()->ShowWindow(SW_HIDE);
		}

		// we should clear the preselected index to avoid some wrong GUI drawing
		m_listCtrlSets.ClearPrevSeledIndex();
		m_listCtrlItems.ClearPrevSeledIndex();
	}	
}

LRESULT CListWnd::OnListCtrlKillFocus(WPARAM wParam,LPARAM lParam)
{
	if(::GetFocus()!=m_listCtrlItems.GetSafeHwnd()&&::GetFocus()!=m_listCtrlSets.GetSafeHwnd()&&::GetFocus()!=m_hWnd)  
	{
		ShowWindow(SW_HIDE);
		if(m_staticPreviewCtrl.GetSafeHwnd() )
			m_staticPreviewCtrl.ShowWindow(SW_HIDE) ;
		
		if(GetParentWnd() && GetParentWnd()->GetSafeHwnd())
		{
			if(m_bHideParent)
				GetParentWnd()->ShowWindow(SW_HIDE);
		}

		// we should clear the preselected index to avoid some wrong GUI drawing
		m_listCtrlSets.ClearPrevSeledIndex();
		m_listCtrlItems.ClearPrevSeledIndex();
	}
	return TRUE;

}

void CListWnd::UpdateList( int nSeledIndex )
{
	ASSERT( -1 != nSeledIndex );
	m_listCtrlItems.DrawThumbnails(m_nShapeItem == -1?nSeledIndex:m_nShapeItem);
}

void CListWnd::CompleteSelect(CThumbnailsList2* pThumbnailList/* add by Benny */)
{
	ShowWindow(SW_HIDE);
	int nSet = -1;
	int nItem = -1;
	if (&m_listCtrlSets == pThumbnailList)
	{
		nSet = m_listCtrlSets.GetSelectedItem();
		nItem = 0;
	}
	else if (&m_listCtrlItems == pThumbnailList)
	{
		nSet = m_listCtrlSets.GetSelectedItem();
		nItem = m_listCtrlItems.GetSelectedItem();
	}

	if(m_nShapeItem != -1)
		nSet = m_nShapeItem;

	if (-1 == nSet || -1 == nItem)
	{
		return;
	}

	
	if( !GetParentWnd() )
		return;

	if( m_bHideParent && GetParentWnd()->GetParent() )
		GetParentWnd()->GetParent()->SendMessage(WM_INPLACE_COMBO, (WPARAM)nSet, (LPARAM)nItem);
	else
		GetParentWnd()->SendMessage(WM_INPLACE_COMBO, (WPARAM)nSet, (LPARAM)nItem);

	if( GetParentWnd()->GetParent() )
		GetParentWnd()->GetParent()->RedrawWindow();

}

void CListWnd::ShouldHideParent(BOOL bHide /* = TRUE*/)
{
	m_bHideParent = bHide;
}

CWnd* CListWnd::GetParentWnd()
{
	return m_pParent;
}

CWnd* CListWnd::SetParentWnd(CWnd* _pNewParent)
{
	ASSERT( _pNewParent->GetSafeHwnd() );
	CWnd* pOldParent = m_pParent;
	m_pParent = _pNewParent;
	return pOldParent;
}