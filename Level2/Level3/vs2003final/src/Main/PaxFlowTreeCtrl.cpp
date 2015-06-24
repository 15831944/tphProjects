// PaxFlowTreeCtrl.cpp: implementation of the CPaxFlowTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "PaxFlowTreeCtrl.h"
#include "PaxFlowDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaxFlowTreeCtrl::CPaxFlowTreeCtrl()
{

}

CPaxFlowTreeCtrl::~CPaxFlowTreeCtrl()
{

}



BEGIN_MESSAGE_MAP(CPaxFlowTreeCtrl, CPrintableTreeCtrl)
	//{{AFX_MSG_MAP(CPaxFlowTreeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaxFlowTreeCtrl message handlers

void CPaxFlowTreeCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPrintableTreeCtrl::PreSubclassWindow();
	EnableToolTips(TRUE);
}


int CPaxFlowTreeCtrl::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
{
	RECT rect;

	UINT nFlags;
	CString strTipText ;
	HTREEITEM hitem = HitTest( point, &nFlags );
	TCHAR* MESSAGE = new TCHAR[1024] ;
	memset(MESSAGE,0,sizeof(TCHAR)*1024) ;
	if( nFlags & TVHT_ONITEMLABEL && hitem )
	{
	
		
		DWORD dw = GetItemData((HTREEITEM ) hitem); //get item data
		TREEITEMDATA* pTempData = (TREEITEMDATA*)dw;
		bool bShow=false;
		if (pTempData&&pTempData->m_arcInfo)
		{
			CWnd* pWnd=GetParent();
			CPaxFlowDlg* pDlg=(CPaxFlowDlg*)pWnd;
			strTipText= pDlg->GetArcInfoDescription(pTempData->m_arcInfo,true,&bShow);
			_tcsncpy(MESSAGE,strTipText.GetBuffer(),1023) ;
			strTipText.ReleaseBuffer() ;
		}

		GetItemRect( hitem, &rect, TRUE );
		pTI->hwnd = m_hWnd;
		pTI->uId = (UINT)hitem;
		pTI->lpszText = MESSAGE;
		pTI->rect = rect;
		return pTI->uId;
	}
	
	return -1;
}

//here we supply the text for the item 
BOOL CPaxFlowTreeCtrl::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	// need to handle both ANSI and UNICODE versions of the message
	//TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	//TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	//CString strTipText;
	//UINT nID = pNMHDR->idFrom;
	//// Do not process the message from built in tooltip 
	//if( nID == (UINT)m_hWnd &&
	//	(( pNMHDR->code == TTN_NEEDTEXTA && pTTTA->uFlags & TTF_IDISHWND ) ||
	//	( pNMHDR->code == TTN_NEEDTEXTW && pTTTW->uFlags & TTF_IDISHWND ) ) )
	//	return FALSE;

	//// Get the mouse position
	//const MSG* pMessage;
	//CPoint pt;
	//pMessage = GetCurrentMessage(); // get mouse pos 
	//ASSERT ( pMessage );
	//pt = pMessage->pt;
	//ScreenToClient( &pt );

	//UINT nFlags;
	//HTREEITEM hitem = HitTest( pt, &nFlags ); //Get item pointed by mouse

	//
	//DWORD dw = GetItemData((HTREEITEM ) nID); //get item data
	//TREEITEMDATA* pTempData = (TREEITEMDATA*)dw;
	//bool bShow=false;
	//if (pTempData&&pTempData->m_arcInfo)
	//{
	//	CWnd* pWnd=GetParent();
	//	CPaxFlowDlg* pDlg=(CPaxFlowDlg*)pWnd;
	//	strTipText= pDlg->GetArcInfoDescription(pTempData->m_arcInfo,true,&bShow);
	//}
	//if(bShow)
	//{
	//	if(pNMHDR->code == TTN_NEEDTEXTA)
	//		memset(pTTTA->szText,0,sizeof(char)*80) ;
	//	else
	//		memset(pTTTW->szText,0,sizeof(WCHAR)*80) ;
	//		#ifndef _UNICODE
	//			if (pNMHDR->code == TTN_NEEDTEXTA)
	//				lstrcpyn(pTTTA->szText, strTipText, 79);
	//			else
	//				_mbstowcsz(pTTTW->szText, strTipText, 79);
	//		#else
	//			if (pNMHDR->code == TTN_NEEDTEXTA)
	//				_wcstombsz(pTTTA->szText, strTipText, 79);
	//			else
	//				lstrcpyn(pTTTW->szText, strTipText, 79);
	//		#endif
	//}

	//*pResult = 0;

	return TRUE;    // message was handled
}

 