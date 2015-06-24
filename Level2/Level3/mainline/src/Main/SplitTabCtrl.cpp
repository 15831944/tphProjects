// SplitTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "SplitTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitTabCtrl

CSplitTabCtrl::CSplitTabCtrl()
{
}

CSplitTabCtrl::~CSplitTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CSplitTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CSplitTabCtrl)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplitTabCtrl message handlers
BOOL CSplitTabCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	//return CTabCtrl::OnEraseBkgnd(pDC);
	return TRUE;
}
