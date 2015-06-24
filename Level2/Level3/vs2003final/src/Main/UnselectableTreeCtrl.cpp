// UnselectableTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "UnselectableTreeCtrl.h"
#include "common\WinMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnselectableTreeCtrl

CUnselectableTreeCtrl::CUnselectableTreeCtrl()
{
}

CUnselectableTreeCtrl::~CUnselectableTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CUnselectableTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CUnselectableTreeCtrl)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnselectableTreeCtrl message handlers

void CUnselectableTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	// TODO: Add your message handler code here and/or call default
	HTREEITEM hItem = GetSelectedItem();
	CRect rt;
	if( hItem && GetItemRect( hItem,rt,TRUE ) )
	{		
		if( rt.PtInRect( point ) )
		{
			SelectItem( NULL );
			//GetParent()->SendMessage( WM_UNSELECTABLELTREE_SELCHANGE, 0, 0 );
			return;
		}
	}	
	CTreeCtrl::OnLButtonDown(nFlags, point);
}
