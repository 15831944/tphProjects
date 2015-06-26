#include "StdAfx.h"
#include ".\cooltreeedit.h"
#include "CoolTree.h"
CCoolTreeEdit::CCoolTreeEdit()
{
	m_bESC = FALSE;
}

CCoolTreeEdit::~CCoolTreeEdit()
{
}


BEGIN_MESSAGE_MAP(CCoolTreeEdit, CEdit)
	//{{AFX_MSG_MAP(CInPlaceEdit2)
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_SYSCHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit2 message handlers

void CCoolTreeEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
	ShowWindow(SW_HIDE);

	GetOwner()->Invalidate();

	CString csLabel;
	GetWindowText( csLabel );
	SetWindowText(_T(""));

	if( !m_bESC )
		GetParent()->SendMessage( UM_CEW_LABLE_END, 0, (LPARAM)&csLabel );		
}

void CCoolTreeEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
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

BOOL CCoolTreeEdit::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->wParam == VK_RETURN)
	{
		GetParent()->SetFocus();
		return TRUE;
	}
	return CEdit::PreTranslateMessage(pMsg);
}

