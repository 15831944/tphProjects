// ListBoxEx.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ListBoxEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListBoxEx

CListBoxEx::CListBoxEx()
{
	m_br.CreateSolidBrush( ::GetSysColor(COLOR_BTNFACE) );
}

CListBoxEx::~CListBoxEx()
{
}


BEGIN_MESSAGE_MAP(CListBoxEx, CListBox)
	//{{AFX_MSG_MAP(CListBoxEx)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBoxEx message handlers

HBRUSH CListBoxEx::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	pDC->SetTextColor( RGB(0,128,0) );
	pDC->SetBkColor( ::GetSysColor(COLOR_BTNFACE) );
	pDC->SetBkMode( TRANSPARENT );
	
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return (HBRUSH)m_br.GetSafeHandle();
//	return NULL;
}

void CListBoxEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CListBox::OnLButtonDown(nFlags, point);
	
	ClientToScreen( &point );
	DWORD lParam;
	lParam = ((DWORD) (((WORD) (point.x)) | ((DWORD) ((WORD) (point.y))) << 16));
//	LOWORD(lParam) = point.x;  // horizontal position of cursor 
//  (lParam) = HIWORD(point.y);  // vertical position of cursor 
 
	GetParent()->SendMessage( WM_LBUTTONDOWN,nFlags,lParam );
}

void CListBoxEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
//	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}
