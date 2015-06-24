#include "stdafx.h"
#include "MenuCH.h"
#include "NewMenu.h"
int strcfind(LPCSTR string,char ch,int nStart)
{
	int i=nStart;
   
	while( i < lstrlen(string) )
	{
		if( *(string+i) == ch )
			return i;
		i++;
	}
	return -1;
}
CMenuCH::CMenuCH()
{
	m_Width		  = 25;				// default menu width
	m_Height	  = 32;				// defautl menu height
	m_nType		  = MIT_ICON;		// default menu type (Icon Menu)
	m_nBmpHeight=32;
	m_nBmpWidth=32;
	m_imageList.Create(32,32,ILC_COLOR24|ILC_MASK,0,4);
	m_nMaxWidthStr=0;
}
CMenuCH::~CMenuCH()
{
	while( m_MenuList.GetCount() )	// release all menu item
	{
		CMenuItemEx* pItem = m_MenuList.GetHead();
		if( pItem->m_pszHotkey != NULL )
			delete pItem->m_pszHotkey;
		delete pItem->m_pBitmap;	// the menu include bitmap
		delete pItem;
		m_MenuList.RemoveHead();
	}
}
BOOL CMenuCH::AppendMenu(UINT nFlags,UINT nIDNewItem,LPCSTR lpszNewItem,
						 UINT nIDBitmap,HICON hIcon)
{
	CBitmap* pBitmap;

	if( nIDBitmap != NULL )				 // the menu has bitmap
	{
		pBitmap = new CBitmap;			 
		pBitmap->LoadBitmap(nIDBitmap);  // load bitmap from application resource
	}
	else
		pBitmap = NULL;					 // not has bitmap 

	// create a new menu item that associated bitmap and icon (if has)
	CMenuItemEx* pItem = new CMenuItemEx(lpszNewItem,pBitmap,hIcon);
	m_MenuList.AddTail(pItem);

	pItem->m_nHeight = m_Height;	// set current menu height
	pItem->m_nWidth  = m_Width;		// set current menu width
	if( !(nFlags & MF_POPUP) )		// not handled popup menu
		nFlags |= MF_OWNERDRAW;
	

	lstrcpy(pItem->m_szText,"");
	pItem->m_pszHotkey = NULL;
	int n;
	if( (n=strcfind(lpszNewItem,'\t',0)) != -1 )	// get item string and hotkey string
	{
		strncpy(pItem->m_szText,lpszNewItem,n);
		pItem->m_szText[n] = '\0';

		pItem->m_pszHotkey = new char[lpszNewItem+n+1,lstrlen(lpszNewItem)-(n+1)+1+1];
		strncpy(pItem->m_pszHotkey,lpszNewItem+n+1,lstrlen(lpszNewItem)-(n+1)+1);
		pItem->m_pszHotkey[lstrlen(lpszNewItem)-(n+1)+1] = '\0';
	}	
	else
		lstrcpy(pItem->m_szText,lpszNewItem);
	
	return CMenu::AppendMenu(nFlags,nIDNewItem,(LPCSTR)pItem);
}
void CMenuCH::SetMenuWidth(DWORD width)
{
	m_Width = width;
}
void CMenuCH::SetMenuHeight(DWORD height)
{
	m_Height = height;
}
void CMenuCH::SetMenuType(UINT nType)
{
	m_nType = nType;
}
void CMenuCH::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	UINT state = ::GetMenuState(m_hMenu,lpMIS->itemID,MF_BYCOMMAND);
 
	CMenuItemEx* pItem = reinterpret_cast<CMenuItemEx *>(lpMIS->itemData);
	if( state & MF_POPUP)
	{

	    int temp = GetSystemMetrics(SM_CYMENU);
	    lpMIS->itemHeight = temp>15+8 ? temp : 15+8;
		lpMIS->itemWidth=16+m_nMaxWidthStr+6 ;
		
	}
	else if( state & MF_SEPARATOR)		// item state is separator
	{
		lpMIS->itemWidth =  pItem->m_nWidth;
		int temp = GetSystemMetrics(SM_CYMENU);
		lpMIS->itemHeight = temp>15+8 ? temp : 15+8;
		//lpMIS->itemHeight = 6;
	}
	else							// other item state
	{
		lpMIS->itemWidth =  pItem->m_nWidth;
		lpMIS->itemHeight = pItem->m_nHeight;
	}
}
void CMenuCH::DrawColorMenu(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	CMenuItemEx* pItem = reinterpret_cast<CMenuItemEx *>(lpDIS->itemData);
	CRect rect(&lpDIS->rcItem); 

	if (lpDIS->itemAction & ODA_DRAWENTIRE)
	{
		// paint the brush and color item in requested
		pDC->FrameRect(rect,&CBrush(GetSysColor(COLOR_3DFACE)));
		rect.DeflateRect(3,3,3,3);
		pDC->FrameRect(rect,&CBrush(RGB(128,128,128)));

		rect.DeflateRect(1,1,1,1);
		// draw a rectangle palette
		pDC->FillSolidRect(rect,(COLORREF) atol(pItem->m_szText));
	}

	if ((lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		// item has been selected - raised frame
		pDC->DrawEdge(rect, EDGE_RAISED, BF_RECT);
		m_SelColor = (COLORREF) atol(pItem->m_szText);
		m_curSel = lpDIS->itemID;
	}

	if (!(lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & ODA_SELECT))
	{
		// item has been de-selected -- remove frame
		CBrush br(GetSysColor(COLOR_3DFACE));
		pDC->FrameRect(rect, &br);
		rect.DeflateRect(1,1,1,1);
		pDC->FrameRect(rect, &br);
	}
}
void CMenuCH::DrawIconMenu(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	CMenuItemEx* pItem = reinterpret_cast<CMenuItemEx *>(lpDIS->itemData);
	CRect rect(&lpDIS->rcItem); 
	
	UINT state = GetMenuState(lpDIS->itemID,MF_BYCOMMAND);
	if (lpDIS->itemAction & ODA_DRAWENTIRE)
	{
		// paint icon item in requested
		pDC->DrawIcon(rect.left+2,rect.top,pItem->m_hIcon);
		// the menu state is separator - draw a color title
		if( state & MF_SEPARATOR )
		{
			for(int i=0; i<rect.Width(); i++)
				pDC->FillRect(rect,&CBrush(RGB(216-i,216-i,235)));
		}
	}

	if ((lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		// item has been selected - draw hilite frame
		CBrush br(RGB(64,64,128));
		pDC->FrameRect(rect,&br);
		rect.DeflateRect(1,1,1,1);
		pDC->FrameRect(rect,&br);
		m_curSel = lpDIS->itemID;	// get current selected 
	}

	if (!(lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & ODA_SELECT))
	{
		// item has been de-selected - remove frame
		CBrush br(::GetSysColor(COLOR_3DFACE));
		pDC->FrameRect(rect,&br);
		rect.DeflateRect(1,1,1,1);
		pDC->FrameRect(rect,&br);
	}
}
void CMenuCH::DrawXPMenu(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	CMenuItemEx* pItem = reinterpret_cast<CMenuItemEx *>(lpDIS->itemData);
	CRect rectFull(&lpDIS->rcItem); 
	rectFull.left-=3;
	// get icon region and text region

	  COLORREF ColA = GetSysColor(COLOR_WINDOW);
	  COLORREF ColB = CNewMenu::GetMenuBarColor();

	  COLORREF colorMenu = MixedColor(ColA,ColB);
	  COLORREF colorBitmap = MixedColor(ColB,ColA);

	  COLORREF colorSel = MidColor(ColA,GetSysColor(COLOR_HIGHLIGHT));
	  if(pDC->GetNearestColor(colorMenu)==pDC->GetNearestColor(colorBitmap))
	  {
	    colorMenu = ColA;
	    colorBitmap = ColB;
	  }
	  
	  UINT state = ::GetMenuState(m_hMenu,lpDIS->itemID,MF_BYCOMMAND);
	pDC->FillRect(rectFull,&CBrush(colorMenu));
	rectFull.DeflateRect(4,4,4,4);
	CRect rectIcon(CPoint(rectFull.left+(rectFull.Width()-m_nBmpWidth)/2,rectFull.top+2),CSize(m_nBmpWidth,m_nBmpHeight));
	CRect rectText(rectFull.left,rectFull.top+m_nBmpHeight+2,rectFull.right,rectFull.bottom);
	if(CString(pItem->m_szText).IsEmpty())
	{
		pDC->FillRect(rectFull,&CBrush(colorMenu));
		return;
	}
	if (lpDIS->itemAction & ODA_DRAWENTIRE)
	{
		// paint the brush and icon item in requested
		CRect rect;
		// paint item background 
		pDC->FillRect(rectFull,&CBrush(colorMenu));
		pDC->FillRect(rectIcon,&CBrush(colorMenu));

		if( state & MF_SEPARATOR )	// item is separator
		{	// draw a etched edge
			rect.CopyRect(rectText);
			rect.OffsetRect(5,3);
			pDC->DrawEdge(rect,EDGE_ETCHED,BF_TOP);
		}
		if(state&MF_MENUBARBREAK)
		{
			pDC->FillRect(CRect(rectFull.left-4,rectFull.top,rectFull.left,rectFull.top+400),&CBrush(colorMenu));
		}
			
		if( state & MFS_CHECKED ) // item has been checked
		{
			// paint the frame and brush,then draw a check mark
			rect.CopyRect(rectIcon);
			rect.DeflateRect(1,1,1,1);
			pDC->FrameRect(rect,&CBrush(GetSysColor(COLOR_HIGHLIGHT)));

			rect.DeflateRect(1,1,1,1);
			pDC->FillRect(rect,&CBrush(colorMenu));
			DrawCheckMark(pDC,rect.left+5,rect.top+5,GetSysColor(COLOR_HIGHLIGHT));
		}
		if( pItem->m_nBmpIndex!=-1 )	// item has bitmap
		{	// draw a bitmap
			m_imageList.Draw(pDC,pItem->m_nBmpIndex,CPoint(rectIcon.left,rectIcon.top),ILD_NORMAL);
		}

		// draw display text
		pDC->SetBkMode(TRANSPARENT);
		rect.CopyRect(rectText);
		rect.OffsetRect(2,2);
		if( state & MFS_DISABLED )	// item has been disabled
			pDC->SetTextColor(GetSysColor(COLOR_3DFACE));
		
		pDC->DrawText(pItem->m_szText,lstrlen(pItem->m_szText),rect,DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);	
		if( pItem->m_pszHotkey != NULL )
		pDC->DrawText(pItem->m_pszHotkey,lstrlen(pItem->m_pszHotkey),
					  CRect(rect.right-60,rect.top,rect.right,rect.bottom),DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);	
	}

	if ((lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		// item has been selected
		if( state & MFS_DISABLED )	
			return;					// item has been disabled  
		CRect  rect;
		
		if( pItem->m_nBmpIndex!=-1 )
		{	// the item has bitmap - repaint icon region and bitmap
			pDC->FillRect(rectFull,&CBrush(colorSel));
			m_imageList.Draw(pDC,pItem->m_nBmpIndex,CPoint(rectIcon.left,rectIcon.top),ILD_NORMAL);

		}

		// draw hilite frame
		rect.SetRect(rectText.left,rectText.top,rectText.right,rectText.bottom);

		pDC->FrameRect(rectFull,&CBrush(GetSysColor(COLOR_HIGHLIGHT)));
		// draw hilite brush
		rect.DeflateRect(1,1,1,1);
		pDC->FillRect(rect,&CBrush(colorSel));
		// draw display text
		rect.CopyRect(rectText);
		rect.OffsetRect(2,2);

		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(pItem->m_szText,lstrlen(pItem->m_szText),rect,DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);	
		if( pItem->m_pszHotkey != NULL )
		pDC->DrawText(pItem->m_pszHotkey,lstrlen(pItem->m_pszHotkey),
					  CRect(rect.right-60,rect.top,rect.right,rect.bottom),DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);	
		m_curSel = lpDIS->itemID;	// get current selected
	}

	if (!(lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & ODA_SELECT))
	{
		// item has been de-selected
		CRect rect;
		rect.CopyRect(rectIcon);

		if( pItem->m_nBmpIndex!=-1 )
		{	// the item has bitmap - repaint icon region and bitmap
			pDC->FillRect(rectFull,&CBrush(colorMenu));
			m_imageList.Draw(pDC,pItem->m_nBmpIndex,CPoint(rectIcon.left,rectIcon.top),ILD_NORMAL);

		}

		// draw display text
		pDC->FillRect(rectText,&CBrush(colorMenu));
		rect.CopyRect(rectText);
		rect.OffsetRect(2,2);
		pDC->SetBkMode(TRANSPARENT);
		if( state & MFS_DISABLED ) // item has been disabled
			pDC->SetTextColor(GetSysColor(COLOR_3DFACE));

		pDC->DrawText(pItem->m_szText,lstrlen(pItem->m_szText),rect,DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);	
		if( pItem->m_pszHotkey != NULL )
		pDC->DrawText(pItem->m_pszHotkey,lstrlen(pItem->m_pszHotkey),
					  CRect(rect.right-60,rect.top,rect.right,rect.bottom),DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);	
	}
}
void CMenuCH::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	switch(m_nType)
	{
	case MIT_XP:
		DrawXPMenu(lpDIS);
		break;
	case MIT_ICON:
		DrawIconMenu(lpDIS);
		break;
	case MIT_COLOR:
		DrawColorMenu(lpDIS);
		break;
	}
}

void CMenuCH::DrawCheckMark(CDC* pDC,int x,int y,COLORREF color)
{
	CPen Pen;

	Pen.CreatePen(PS_SOLID,0,color);
	CPen *pOldPen = pDC->SelectObject(&Pen);
	
	pDC->MoveTo(x,y+2);
	pDC->LineTo(x,y+5);
	
	pDC->MoveTo(x+1,y+3);
	pDC->LineTo(x+1,y+6);
	
	pDC->MoveTo(x+2,y+4);
	pDC->LineTo(x+2,y+7);
	
	pDC->MoveTo(x+3,y+3);
	pDC->LineTo(x+3,y+6);
	
	pDC->MoveTo(x+4,y+2);
	pDC->LineTo(x+4,y+5);
	
	pDC->MoveTo(x+5,y+1);
	pDC->LineTo(x+5,y+4);
	
	pDC->MoveTo(x+6,y);
	pDC->LineTo(x+6,y+3);
	
	pDC->SelectObject(pOldPen);
	Pen.DeleteObject();
}


BOOL CMenuCH::AppendMenu(UINT nFlags, UINT nIDNewItem, LPCSTR lpszNewItem, HBITMAP hBitmap)
{
	int nBmpIndex=-1;
	if(hBitmap)
	{
		CBitmap Bitmap;
		Bitmap.Attach(hBitmap);  // load bitmap from application resource
		nBmpIndex=m_imageList.Add(&Bitmap,RGB(255,0,255));
		// create a new menu item that associated bitmap and icon (if has)
	}
	CMenuItemEx* pItem = new CMenuItemEx(lpszNewItem,NULL,NULL,nBmpIndex);
	m_MenuList.AddTail(pItem);
	
	pItem->m_nHeight = m_Height;	// set current menu height
	pItem->m_nWidth  = m_Width;		// set current menu width
	if( !(nFlags & MF_POPUP) )		// not handled popup menu
		nFlags |= MF_OWNERDRAW;
	

	lstrcpy(pItem->m_szText,"");
	pItem->m_pszHotkey = NULL;
	int n;
	if( (n=strcfind(lpszNewItem,'\t',0)) != -1 )	// get item string and hotkey string
	{
		strncpy(pItem->m_szText,lpszNewItem,n);
		pItem->m_szText[n] = '\0';

		pItem->m_pszHotkey = new char[lpszNewItem+n+1,lstrlen(lpszNewItem)-(n+1)+1+1];
		strncpy(pItem->m_pszHotkey,lpszNewItem+n+1,lstrlen(lpszNewItem)-(n+1)+1);
		pItem->m_pszHotkey[lstrlen(lpszNewItem)-(n+1)+1] = '\0';
	}	
	else
		lstrcpy(pItem->m_szText,lpszNewItem);
	
    CFont m_fontMenu;
    LOGFONT m_lf;
    ZeroMemory ((PVOID) &m_lf,sizeof (LOGFONT));
    NONCLIENTMETRICS nm;
    nm.cbSize = sizeof (NONCLIENTMETRICS);
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, nm.cbSize,&nm,0)); 
    m_lf =  nm.lfMenuFont;

    m_fontMenu.CreateFontIndirect (&m_lf);
    
    // DC of the desktop
    CClientDC myDC(NULL);
    
    // Select menu font in...
    CFont* pOldFont = myDC.SelectObject (&m_fontMenu);
    //Get pointer to text SK
    SIZE size = {0,0};
    VERIFY(::GetTextExtentPoint32(myDC.m_hDC,pItem->m_szText,(int)_tcslen(pItem->m_szText),&size));
    // Select old font in
    myDC.SelectObject(pOldFont);  
	if(size.cx>m_nMaxWidthStr)
		m_nMaxWidthStr=size.cx;
	
	return CMenu::AppendMenu(nFlags,nIDNewItem,(LPCSTR)pItem);

}

void CMenuCH::SetBmpSize(int cx, int cy)
{
	m_nBmpWidth=cx;
	m_nBmpHeight=cy;
}
