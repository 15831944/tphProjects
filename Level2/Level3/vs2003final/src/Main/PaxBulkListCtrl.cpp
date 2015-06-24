// PaxBulkListCtrl1.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PaxBulkListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaxBulkListCtrl

CPaxBulkListCtrl::CPaxBulkListCtrl()
{
}

CPaxBulkListCtrl::~CPaxBulkListCtrl()
{
}


BEGIN_MESSAGE_MAP(CPaxBulkListCtrl, CListCtrl)//Ex)
	//{{AFX_MSG_MAP(CPaxBulkListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaxBulkListCtrl message handlers


void CPaxBulkListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
//	SetFocus();
//	int index;
//	SetItemState( CurrentSelection, 0, LVIS_SELECTED ); 
//	if((index = HitTestEx(point, NULL)) != -1)
//	{
//		CurrentSelection = index;
//		SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
//	}	
//	CWnd* pWnd =GetParent();	
//	::SendMessage( pWnd->m_hWnd,WM_LBUTTONDOWN,0,0);
	CListCtrl::OnLButtonDown(nFlags, point);

}

void CPaxBulkListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
//	OnLButtonDown( nFlags,  point);
	CListCtrl::OnRButtonDown(nFlags, point);

}


