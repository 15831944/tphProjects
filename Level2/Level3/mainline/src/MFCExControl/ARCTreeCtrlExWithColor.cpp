#include "StdAfx.h"
#include ".\arctreectrlexwithcolor.h"



IMPLEMENT_DYNAMIC(CARCTreeCtrlExWithColor, CARCTreeCtrlEx)
CARCTreeCtrlExWithColor::CARCTreeCtrlExWithColor(void)
{
}

CARCTreeCtrlExWithColor::~CARCTreeCtrlExWithColor(void)
{
	Clear();
}
void CARCTreeCtrlExWithColor::Clear(void)
{
	if(GetSafeHwnd())
	{
		HTREEITEM hItem = GetFirstVisibleItem();
		int n = GetVisibleCount()+1;
		while( hItem && n--)
		{
			CARCTreeCtrlEx::SetItemData(hItem,0);
			hItem = GetNextVisibleItem( hItem ); 
		}
	}
	for (std::vector<CItemData*>::iterator itrItemDataEx = m_vrItemDataEx.begin();\
		itrItemDataEx != m_vrItemDataEx.end();++ itrItemDataEx) {
			if(*itrItemDataEx)
				delete (*itrItemDataEx);
		}
		m_vrItemDataEx.clear();
}
BEGIN_MESSAGE_MAP(CARCTreeCtrlExWithColor, CARCTreeCtrlEx)
	ON_WM_LBUTTONDBLCLK()
	//ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CARCTreeCtrlExWithColor::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	UINT flags;
	HTREEITEM hItem = HitTest(point, &flags);
	CARCTreeCtrlEx::SelectItem(hItem);
	GetParent()->SendMessage(WM_OBJECTTREE_DOUBLECLICK, 0L, (LPARAM)hItem);
}
//void CARCTreeCtrlExWithColor::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	UINT flags;
//	HTREEITEM hItem = HitTest(point, &flags);
//	CARCTreeCtrlEx::SelectItem(hItem);
//	CARCTreeCtrlEx::OnLButtonDown(nFlags, point);
//	GetParent()->SendMessage(WM_OBJECTTREE_LBUTTONDOWN, 0L, (LPARAM)hItem);
//}
BOOL CARCTreeCtrlExWithColor::SetItemData( HTREEITEM hItem, DWORD_PTR dwData )
{
	CItemData * pItemDataEx = 0;
	pItemDataEx = GetItemDataEx(hItem);
	if(pItemDataEx)
	{
		pItemDataEx->dwptrItemData = dwData;
		return TRUE;
	}
	else
	{
		pItemDataEx = new CItemData;
		pItemDataEx->lSize = sizeof(CItemData);
		pItemDataEx->dwptrItemData = dwData; 
		if(CARCTreeCtrlEx::SetItemData(hItem,(DWORD_PTR)pItemDataEx))
			m_vrItemDataEx.push_back(pItemDataEx);
		else
			delete pItemDataEx;
	}
	return FALSE;
}

DWORD_PTR CARCTreeCtrlExWithColor::GetItemData(HTREEITEM hItem)
{
	CItemData * pItemDataEx = 0;
	pItemDataEx = GetItemDataEx(hItem);
	if(pItemDataEx)
		return pItemDataEx->dwptrItemData;
	else
		return 0;
}

void CARCTreeCtrlExWithColor::SetItemDataEx(HTREEITEM hItem,CItemData& aoidDataEx)
{
	CItemData * pItemDataEx = 0;
	pItemDataEx = GetItemDataEx(hItem);
	if(pItemDataEx)
		*pItemDataEx = aoidDataEx;
	else
	{
		pItemDataEx = new CItemData;
		*pItemDataEx = aoidDataEx; 
		if(CARCTreeCtrlEx::SetItemData(hItem,(DWORD_PTR)pItemDataEx))
			m_vrItemDataEx.push_back(pItemDataEx);
		else
			delete pItemDataEx;
	}
}

CARCTreeCtrlExWithColor::CItemData* CARCTreeCtrlExWithColor::GetItemDataEx(HTREEITEM hItem)
{
	CItemData * pItemDataEx = 0;
	pItemDataEx = (CItemData*)(CARCTreeCtrlEx::GetItemData(hItem));
	return pItemDataEx;
}

BOOL CARCTreeCtrlExWithColor::DeleteItem( HTREEITEM hItem )
{
	CItemData * pItemDataEx = 0;
	pItemDataEx = GetItemDataEx(hItem);
	if(pItemDataEx)
	{
		CARCTreeCtrlEx::SetItemData(hItem,0);
		std::vector<CItemData*>::iterator itrFind = std::find(m_vrItemDataEx.begin(),m_vrItemDataEx.end(),pItemDataEx);
		if(itrFind != m_vrItemDataEx.end())
		{
			if(*itrFind)
				delete (*itrFind);
			m_vrItemDataEx.erase(itrFind);
		}
		else
			delete pItemDataEx;
	}
	return (CARCTreeCtrlEx::DeleteItem(hItem) );
}

BOOL CARCTreeCtrlExWithColor::DeleteAllItems(void)
{
	Clear();
	return (CARCTreeCtrlEx::DeleteAllItems() );
}

void CARCTreeCtrlExWithColor::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
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
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 ); 

	CFont *pFontDC;
	CFont fontDC;//,fontBoldDC;
	LOGFONT logfont;
	COLORREF clOld;

	HTREEITEM hItem = GetFirstVisibleItem();
	int n = GetVisibleCount()+1;

	CFont *pFont = GetFont();
	pFont->GetLogFont( &logfont );
	fontDC.CreateFontIndirect( &logfont );
	pFontDC = memDC.SelectObject( &fontDC);
	memDC.SetBkColor( GetSysColor( COLOR_WINDOW ));

	while( hItem && n--)
	{
		CRect rect;

		// Do not meddle with selected items or drop highlighted items
		UINT selflag = TVIS_DROPHILITED | TVIS_SELECTED;

		if ( !(GetItemState( hItem, selflag ) & selflag ) )
		{	
			CString sItem = GetItemText( hItem );
			GetItemRect( hItem, &rect, TRUE );
			memDC.FillSolidRect(rect,GetSysColor( COLOR_WINDOW ));
			int nPos = -1;
			CString strLeft,strRight; 
			CItemData * pItemDataEx = 0;
			int xRect = 0,yRect = 0;
			xRect = rect.left+2;
			yRect = rect.top+1;

			pItemDataEx = GetItemDataEx(hItem);			
			if(pItemDataEx)
			{		 
				strRight = sItem; 				
				for(std::vector<CColorStringSection>::iterator itrSection = pItemDataEx->vrItemStringSectionColorShow.begin();\
					itrSection != pItemDataEx->vrItemStringSectionColorShow.end();++ itrSection)
				{					
					nPos = strRight.Find((*itrSection).strSection);					
					if(nPos > -1)
					{
						strLeft = strRight.Left(nPos);
						memDC.TextOut(xRect,yRect,strLeft); 
						xRect += memDC.GetTextExtent(strLeft).cx;
						clOld = memDC.SetTextColor((*itrSection).colorSection);
						memDC.TextOut(xRect,yRect,(*itrSection).strSection);
						memDC.SetTextColor(clOld);
						xRect += memDC.GetTextExtent((*itrSection).strSection).cx;
						strRight = strRight.Mid(nPos + (*itrSection).strSection.GetLength());
					}
				}
				memDC.TextOut(xRect,yRect,strRight); 
			}
			else
				memDC.TextOut(xRect,yRect,sItem);
		}
		hItem = GetNextVisibleItem( hItem ); 
	}

	memDC.SelectObject( pFontDC );

	dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC, 
		rcClip.left, rcClip.top, SRCCOPY );

}