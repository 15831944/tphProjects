// InPlaceDateTimeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "InPlaceDateTimeCtrl.h"
#include "WinMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CInPlaceDateTimeCtrl

CInPlaceDateTimeCtrl::CInPlaceDateTimeCtrl()
{
	m_bESC = FALSE;
}

CInPlaceDateTimeCtrl::~CInPlaceDateTimeCtrl()
{
}

BEGIN_MESSAGE_MAP(CInPlaceDateTimeCtrl, CDateTimeCtrl)
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceDateTimeCtrl message handlers

void CInPlaceDateTimeCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CDateTimeCtrl::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here	
	SetWindowText("");
	ShowWindow(SW_HIDE);

	GetOwner()->Invalidate();

	COleDateTime oTime;
	GetTime( oTime );

	// TRACE("Time:%d:%d:%d",oTime.GetHour(), oTime.GetMinute(), oTime.GetSecond() );
	GetParent()->SendMessage( WM_INPLACE_DATETIME, (WPARAM)m_bESC, (LPARAM)&oTime );	

	DestroyWindow();

}

int CInPlaceDateTimeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDateTimeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CFont* font = GetParent()->GetFont();
	SetFont( font );

	return 0;
}

void CInPlaceDateTimeCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if( nChar == VK_ESCAPE )
			m_bESC = TRUE;
		GetParent()->SetFocus();
		return;
	}

	CDateTimeCtrl::OnChar(nChar, nRepCnt, nFlags);

	// Resize edit control if needed

	// Get text extent
	CString str;

	GetWindowText( str );
	CWindowDC dc(this);
	CFont *pFont = GetParent()->GetFont();
	CFont *pFontDC = dc.SelectObject( pFont );
	CSize size = dc.GetTextExtent( str );
	dc.SelectObject( pFontDC );
   size.cx += 5; // add some extra buffer

	// Get client rect
	CRect rect, parentrect;
	GetClientRect( &rect );
	GetParent()->GetClientRect( &parentrect );

	// Transform rect to parent coordinates
	ClientToScreen( &rect );
	GetParent()->ScreenToClient( &rect );

	// Check whether control needs to be resized
	// and whether there is space to grow
	if( size.cx > rect.Width() )
	{
		if( size.cx + rect.left < parentrect.right )
			rect.right = rect.left + size.cx;
		else
			rect.right = parentrect.right;
		MoveWindow( &rect );
	}
}

BOOL CInPlaceDateTimeCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_RETURN
				|| pMsg->wParam == VK_DELETE
				|| pMsg->wParam == VK_ESCAPE
				|| GetKeyState( VK_CONTROL)
				)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
         return true;   // DO NOT process further
		}
	}
	return CDateTimeCtrl::PreTranslateMessage(pMsg);
}

void CInPlaceDateTimeCtrl::OnNcDestroy() 
{
	CDateTimeCtrl::OnNcDestroy();
}
