// ListWnd.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ListWndOld.h"
#include "Common\ProjectManager.h"
#include "common\WinMsg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListWndOld
#define IDC_DROPLIST 333
#define IDC_DROPLISTTYPE	334
CListWndOld::CListWndOld():m_listCtrl(TRUE)
{
	m_pParent=NULL;
	m_bHideParent = TRUE;
}

CListWndOld::~CListWndOld()
{
}


BEGIN_MESSAGE_MAP(CListWndOld, CWnd)
	//{{AFX_MSG_MAP(CListWndOld)
	ON_WM_CREATE()
	ON_NOTIFY(NM_CLICK, IDC_DROPLIST, OnClickListThumb)
	ON_NOTIFY(NM_DBLCLK, IDC_DROPLIST, OnDblClickListThumb)
	ON_NOTIFY(NM_CLICK, IDC_DROPLISTTYPE, OnClickListType)
	ON_NOTIFY(NM_DBLCLK, IDC_DROPLISTTYPE, OnDblClickListType)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_LISTCTRL_KILLFOCUS,OnListCtrlKillFocus)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CListWndOld message handlers

int CListWndOld::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if(!CreateList())
		return -1;
	if(!CreateListType())
		return -1;
	if(!CreatePreviewCtrl())
		return -1;
	return 0;
}

BOOL CListWndOld::CreateList()
{
	CRect listRect(0,0,0,0);
	listRect.left+=LISTTYPEWND_WIDTH_OLD;
	listRect.right =LISTWND_WIDTH_OLD;
	listRect.bottom=LISTWND_HEIGHT_OLD;
	//can't have a control Id with WS_POPUP style
//    if (! m_listCtrl.CreateEx ( 0,
//		                   WC_LISTVIEW, NULL,
//						   	WS_VISIBLE|WS_CHILD | WS_BORDER|LVS_ICON|
//							LVS_ALIGNTOP|LVS_AUTOARRANGE|LVS_SINGLESEL |LVS_SHOWSELALWAYS  ,
//							listRect, this, IDC_DROPLIST, NULL))
    if (! m_listCtrl.CreateEx ( NULL,
						   	WS_VISIBLE|WS_CHILD | WS_BORDER|LVS_ICON|
							LVS_ALIGNTOP|LVS_AUTOARRANGE|LVS_SINGLESEL |LVS_SHOWSELALWAYS  ,
							listRect, this, IDC_DROPLIST))
	{
		return FALSE;
	}
	m_listCtrl.m_pParent =this;
	m_listCtrl.m_ImageListThumb.Create(LW_THUMBNAIL_WIDTH, LW_THUMBNAIL_HEIGHT, ILC_COLOR24, 0, 1);
	m_listCtrl.SetImageList(&m_listCtrl.m_ImageListThumb, LVSIL_NORMAL);
	m_listCtrl.m_nSelectedItem = 0;

	

	// draw thumbnail images in list control
	//m_listCtrl.DrawThumbnails();


	// set focus and select the first thumbnail in the list control
	m_listCtrl.SetFocus();
	m_listCtrl.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	
	
	return TRUE;

}


void CListWndOld::OnClickListThumb(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CWnd* pWnd=GetParent();
	LPNMITEMACTIVATE pItemAct = (LPNMITEMACTIVATE)pNMHDR;

	// determine which item receives the click
	LVHITTESTINFO  hitTest;
	memset(&hitTest, '\0', sizeof(LVHITTESTINFO));
	hitTest.pt = pItemAct->ptAction;
	m_listCtrl.SendMessage(LVM_SUBITEMHITTEST, 0, (LPARAM)&hitTest);

	// draw the selected image
	if(hitTest.iItem != m_listCtrl.m_nSelectedItem && hitTest.iItem >= 0)
	{
		m_listCtrl.m_nSelectedItem = hitTest.iItem;
	}
	
	*pResult = 0;
}

void CListWndOld::OnDblClickListThumb(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CompleteSelect();
}

void CListWndOld::OnClickListType(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CWnd* pWnd=GetParent();
	LPNMITEMACTIVATE pItemAct = (LPNMITEMACTIVATE)pNMHDR;

	// determine which item receives the click
	LVHITTESTINFO  hitTest;
	memset(&hitTest, '\0', sizeof(LVHITTESTINFO));
	hitTest.pt = pItemAct->ptAction;
	m_listCtrlType.SendMessage(LVM_SUBITEMHITTEST, 0, (LPARAM)&hitTest);

	// draw the selected image
	if(hitTest.iItem != m_listCtrlType.m_nSelectedItem && hitTest.iItem >= 0)
	{
		m_listCtrlType.m_nSelectedItem = hitTest.iItem;
		UpdateList(m_listCtrlType.m_nSelectedItem );
	}
	
	*pResult = 0;
}

void CListWndOld::OnDblClickListType(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}



BOOL CListWndOld::CreatePreviewCtrl()
{
	CRect rect(0,0,64,64);
	BOOL bResult=m_staticPreviewCtrl.CreateEx(0,"static",NULL,
		WS_POPUP|WS_BORDER|SS_BITMAP|SS_LEFT,rect,this,0) ;
	return bResult;

}

void CListWndOld::DisplayPreviewCtrl(int nSeledIndex)
{
	CString strFileName=m_listCtrl.m_strImageDir+"\\" ;
	CString strItem=m_listCtrl.m_VectorItemDataLeft[nSeledIndex]+".bmp";
	strFileName+=strItem;
	HBITMAP hBmp=(HBITMAP)::LoadImage(AfxGetInstanceHandle(),strFileName,IMAGE_BITMAP,LR_DEFAULTSIZE,LR_DEFAULTSIZE,LR_LOADFROMFILE);
	
	m_staticPreviewCtrl.SetBitmap(hBmp) ;


	
	/*
	CImageList imgList;
	imgList.Create(32, 32, ILC_COLOR24|ILC_MASK, 0, 4);
	CBitmap bmp;
	bmp.Attach(hBmp);
	imgList.Add(&bmp, RGB(255, 0, 255));
	imgList.Draw(GetDC(), 0, CPoint(0, 0), ILD_NORMAL);*/
	
	CRect rect;
	m_listCtrl.GetWindowRect(rect) ;
	rect.left =rect.right ;

	m_staticPreviewCtrl.SetWindowPos (&wndNoTopMost, rect.left, rect.top,
		                0,0, SWP_NOSIZE|SWP_SHOWWINDOW |SWP_NOACTIVATE);

	/*
	CDC dcMem;
	dcMem.CreateCompatibleDC(GetDC());
	CBitmap bmp;
	bmp.Attach(hBmp);
	CBitmap* pOldBitmap = (CBitmap*)dcMem.SelectObject(&bmp);
	::TransparentBlt(m_staticPreviewCtrl.GetDC()->m_hDC, 0, 0, 100, 100, 
		dcMem.m_hDC, 0, 0, 32, 32, RGB(255,0, 255));
	*/
}

BOOL CListWndOld::CreateListType()
{
	CRect listRect(0,0,0,0);
	listRect.right =LISTTYPEWND_WIDTH_OLD;
	listRect.bottom=LISTWND_HEIGHT_OLD;
	//can't have a control Id with WS_POPUP style
//    if (! m_listCtrlType.CreateEx ( 0,
//		                   WC_LISTVIEW, NULL,
//						   	WS_VISIBLE|WS_CHILD | WS_BORDER|
//							LVS_ICON|LVS_ALIGNTOP|LVS_AUTOARRANGE 
//							|LVS_SINGLESEL |LVS_SHOWSELALWAYS,
//							listRect, this, IDC_DROPLISTTYPE, NULL))
    if (! m_listCtrlType.CreateEx ( NULL,
						   	WS_VISIBLE|WS_CHILD | WS_BORDER|
							LVS_ICON|LVS_ALIGNTOP|LVS_AUTOARRANGE 
							|LVS_SINGLESEL |LVS_SHOWSELALWAYS,
							listRect, this, IDC_DROPLISTTYPE))
	{
		return FALSE;
	}
	m_listCtrlType.m_pParent =this;
	m_listCtrlType.m_ImageListThumb.Create(LW_THUMBNAIL_WIDTH, LW_THUMBNAIL_HEIGHT, ILC_COLOR24, 0, 1);
	m_listCtrlType.SetImageList(&m_listCtrlType.m_ImageListThumb, LVSIL_NORMAL);
	m_listCtrlType.m_nSelectedItem = 0;

	
	if( !m_listCtrlType.GetImageFileNames("TYPE") )
		return FALSE;

	// draw thumbnail images in list control
	m_listCtrlType.DrawThumbnails();


	// set focus and select the first thumbnail in the list control
	m_listCtrlType.SetFocus();
	m_listCtrlType.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	
	UpdateList(m_listCtrlType.m_nSelectedItem );
	
	return TRUE;

}

void CListWndOld::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	if(::GetFocus()!=m_listCtrl.GetSafeHwnd()&&::GetFocus()!=m_listCtrlType.GetSafeHwnd()&&::GetFocus()!=m_hWnd)  
	{
		ShowWindow(SW_HIDE);
		if(m_staticPreviewCtrl.GetSafeHwnd() )
			m_staticPreviewCtrl.ShowWindow(SW_HIDE) ;
		if(m_pParent&&m_pParent->GetSafeHwnd())
		{
			if (m_bHideParent)
				m_pParent->ShowWindow(SW_HIDE) ;
		}

		m_listCtrl.ClearPrevSeledIndex();
		m_listCtrlType.ClearPrevSeledIndex();
	}
	// TODO: Add your message handler code here
	
}

LRESULT CListWndOld::OnListCtrlKillFocus(WPARAM wParam,LPARAM lParam)
{
	if(::GetFocus()!=m_listCtrl.GetSafeHwnd()&&::GetFocus()!=m_listCtrlType.GetSafeHwnd()&&::GetFocus()!=m_hWnd)  
	{
		ShowWindow(SW_HIDE);
		if(m_staticPreviewCtrl.GetSafeHwnd() )
			m_staticPreviewCtrl.ShowWindow(SW_HIDE) ;
		if(m_pParent&&m_pParent->GetSafeHwnd())
		{
			if (m_bHideParent)
				m_pParent->ShowWindow(SW_HIDE) ;
		}
		m_listCtrl.ClearPrevSeledIndex();
		m_listCtrlType.ClearPrevSeledIndex();
	}
	return TRUE;
		
}

void CListWndOld::UpdateList(int nSeledIndex)
{
	CString strSection=m_listCtrlType.GetItemText(m_listCtrlType.m_nSelectedItem,0) ;
	m_listCtrl.GetImageFileNames(strSection);
	m_listCtrl.DrawThumbnails(); 

}

void CListWndOld::CompleteSelect()
{
	ShowWindow(SW_HIDE);
	DWORD dData=m_listCtrl.GetItemData(m_listCtrl.m_nSelectedItem ) ;
	//m_pParent->GetParent()->SendMessage(WM_INPLACE_COMBO, (WPARAM)0, (LPARAM)dData);	
 	if (m_bHideParent)
		m_pParent->GetParent()->SendMessage(WM_INPLACE_COMBO, (WPARAM)0, (LPARAM)dData);	
	else
	{
		//	Parent deal with the message
		m_pParent->SendMessage(WM_INPLACE_COMBO, (WPARAM)0, (LPARAM)dData);	
	}
	m_pParent->GetParent()->RedrawWindow();

}

void CListWndOld::ShouldHideParent(BOOL bHide /* = TRUE*/)
{
	m_bHideParent = bHide;
}

void CListWndOld::SetImageDir(const CString& str)
{
	m_listCtrl.SetImageDir(str);
	m_listCtrlType.SetImageDir(str);
}

void CListWndOld::SetLabelString(const char* szLabel[])
{
	m_listCtrl.SetLabelString(szLabel);
}