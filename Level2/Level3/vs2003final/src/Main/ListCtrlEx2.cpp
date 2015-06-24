// ListCtrlEx2.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "common\winmsg.h"
#include "ListCtrlEx2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx2

CListCtrlEx2::CListCtrlEx2()
{
}

CListCtrlEx2::~CListCtrlEx2()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEx2, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlEx2)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx2 message handlers
LRESULT CListCtrlEx2::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message==WM_INPLACE_COMBO)
		GetParent()->SendMessage(message, wParam, lParam);
	return CListCtrl::DefWindowProc(message, wParam, lParam);
}