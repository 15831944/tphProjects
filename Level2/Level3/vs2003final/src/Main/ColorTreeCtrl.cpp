// ColorTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ColorTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorTreeCtrl

CColorTreeCtrl::CColorTreeCtrl()
{
	m_DefColor = DEFCOLOR;
}

CColorTreeCtrl::~CColorTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CColorTreeCtrl, CTreeCtrlEx)
	//{{AFX_MSG_MAP(CColorTreeCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorTreeCtrl message handlers

void CColorTreeCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Create a memory DC compatible with the paint DC
	CDC memDC;
	memDC.CreateCompatibleDC( &dc );
	
	CRect rcClip, rcClient;
	dc.GetClipBox( &rcClip );
	GetClientRect(&rcClient);
	
	// Select a compatible bitmap into the memory DC
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
	memDC.SelectObject( &bitmap );
	
	// Set clip region to be same as that in paint DC
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcClip );
	memDC.SelectClipRgn(&rgn);
	rgn.DeleteObject();
	
	// First let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, (LPARAM)0 );

	//draw item color
	HTREEITEM hItem = GetFirstVisibleItem();
	int n = GetVisibleCount()+1;
	while( hItem && n--)
	{
		CRect rect;
		// Do not meddle with selected items or drop highlighted items
		UINT selflag = TVIS_DROPHILITED | TVIS_SELECTED;
		Color_Font cf;
		
		if ( !(GetItemState( hItem, selflag ) & selflag ) 
			 && m_mapColorFont.Lookup( hItem, cf ))
		{
			CFont *pFontDC;
			CFont fontDC;
			LOGFONT logfont;
			
			if( cf.logfont.lfFaceName[0] != '\0' ) 
			{
				logfont = cf.logfont;
			}
			else
			{
				// No font specified, so use window font
				CFont *pFont = GetFont();
				pFont->GetLogFont( &logfont );
			}
			if( GetItemBold( hItem ) )
				logfont.lfWeight = 700;

			fontDC.CreateFontIndirect( &logfont );
			pFontDC = memDC.SelectObject( &fontDC );
			
			if( cf.color != (COLORREF)-1 )
				memDC.SetTextColor( cf.color );
			
			CString sItem = GetItemText( hItem );
			GetItemRect( hItem, &rect, TRUE );
			memDC.SetBkColor( GetSysColor( COLOR_WINDOW ) );
			memDC.TextOut( rect.left+2, rect.top+1, sItem );
			
			memDC.SelectObject( pFontDC );
		}
		hItem = GetNextVisibleItem( hItem );
	}
	
	
	dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC, 
		       rcClip.left, rcClip.top, SRCCOPY );	
}

//////////////////////////////////////////////////////////////////////////
//
// set and get!
//
//////////////////////////////////////////////////////////////////////////
//set hItem's color
void CColorTreeCtrl::SetItemColor(HTREEITEM hItem, COLORREF color)
{
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		cf.logfont.lfFaceName[0] = '\0';
	if( color ==-1 )
		cf.color = m_DefColor;
	else
		cf.color = color;	
	m_mapColorFont[hItem] = cf;
}

//set hItem's font
void CColorTreeCtrl::SetItemFont(HTREEITEM hItem, LOGFONT &logfont)
{
	Color_Font cf;
	if( !m_mapColorFont.Lookup(hItem,cf) )
		cf.color = (COLORREF)-1;
	cf.logfont = logfont;
	m_mapColorFont[hItem] = cf;
}

//set hItem's bold
void CColorTreeCtrl::SetItemBold(HTREEITEM hItem, BOOL bBold)
{
	SetItemState( hItem, bBold ? TVIS_BOLD: 0, TVIS_BOLD );
}

//set default color of color item 
void CColorTreeCtrl::SetTreeDefColor(COLORREF color)
{
	m_DefColor = color;
}
//////////////////////////////////////////////////////////////////////////
//get hItem's color
COLORREF CColorTreeCtrl::GetItemColor(HTREEITEM hItem)
{
	Color_Font cf;
	if( !m_mapColorFont.Lookup(hItem,cf) )
		//return GetSysColor(COLOR_WINDOW);
		return (COLORREF)-1;
	return cf.color;
} 

//get hItem's font
BOOL CColorTreeCtrl::GetItemFont(HTREEITEM hItem, LOGFONT *plogfont)
{
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem,cf ))
		return FALSE;
	if( cf.logfont.lfFaceName == '\0' )
		return FALSE;
	*plogfont = cf.logfont;
	return TRUE;
}

//get hItem's bold
BOOL CColorTreeCtrl::GetItemBold(HTREEITEM hItem)
{
	return GetItemState( hItem, TVIS_BOLD ) & TVIS_BOLD;
}

//get default color of color item 
COLORREF CColorTreeCtrl::GetTreeDefColor()
{
	return m_DefColor;
}

////////////////////////////////////////////////////////////////////////////////
// when rebuilt the tree,must empty the map
BOOL CColorTreeCtrl::DeleteAllItems()
{
	m_mapColorFont.RemoveAll();
	return CTreeCtrlEx::DeleteAllItems();
}

//when delete a item, must remove the item from map
BOOL CColorTreeCtrl::DeleteItem(HTREEITEM hItem)
{
	m_mapColorFont.RemoveKey( hItem );
	return CTreeCtrlEx::DeleteItem( hItem );
}


