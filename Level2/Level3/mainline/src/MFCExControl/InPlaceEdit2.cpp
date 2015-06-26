// InPlaceEdit2.cpp : implementation file
//

#include "stdafx.h"
//#include "termplan.h"
#include "InPlaceEdit2.h"
#include "..\common\WinMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit2

CInPlaceEdit2::CInPlaceEdit2()
{
	m_bESC = FALSE;
}

CInPlaceEdit2::~CInPlaceEdit2()
{
}


BEGIN_MESSAGE_MAP(CInPlaceEdit2, CEdit)
	//{{AFX_MSG_MAP(CInPlaceEdit2)
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_SYSCHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit2 message handlers

void CInPlaceEdit2::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	ShowWindow(SW_HIDE);

	GetOwner()->Invalidate();

	CString csLabel;
	GetWindowText( csLabel );
	SetWindowText("");

	if( !m_bESC )
		GetParent()->SendMessage( WM_INPLACE_EDIT, 0, (LPARAM)&csLabel );		
}

int CInPlaceEdit2::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CFont* font = GetParent()->GetFont();
	SetFont( font );
	
	SetFocus();
	SetSel( 0, -1 );

	return 0;
}

void CInPlaceEdit2::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if( nChar == VK_ESCAPE )
			m_bESC = TRUE;
		GetParent()->SetFocus();
		return;
	}


	CEdit::OnChar(nChar, nRepCnt, nFlags);

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

BOOL CInPlaceEdit2::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->wParam == VK_RETURN)
	{
		GetParent()->SetFocus();
		return TRUE;
	}
	return CEdit::PreTranslateMessage(pMsg);
}
