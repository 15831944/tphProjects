#include "stdafx.h"
#include "AirsideObjectTreeCtrl.h"

IMPLEMENT_DYNAMIC(CAirsideObjectTreeCtrl, CTreeCtrlEx)
CAirsideObjectTreeCtrl::CAirsideObjectTreeCtrl()
{
}

CAirsideObjectTreeCtrl::~CAirsideObjectTreeCtrl()
{
	ClearItemDatas();
}

void CAirsideObjectTreeCtrl::ClearItemDatas(void)
{
	//if(GetSafeHwnd())
	//{
	//	HTREEITEM hItem = GetFirstVisibleItem();
	//	int n = GetVisibleCount()+1;
	//	while( hItem && n--)
	//	{
	//		CTreeCtrlEx::SetItemData(hItem,0);
	//		hItem = GetNextVisibleItem( hItem ); 
	//	}
	//}
	for (std::vector<AirsideObjectTreeCtrlItemDataEx*>::iterator itrItemDataEx = m_vrItemDataEx.begin();\
	  itrItemDataEx != m_vrItemDataEx.end();++ itrItemDataEx) {
		  if(*itrItemDataEx)
			  delete (*itrItemDataEx);
	  }
	  m_vrItemDataEx.clear();
}

BEGIN_MESSAGE_MAP(CAirsideObjectTreeCtrl, CTreeCtrlEx)
	ON_WM_LBUTTONDBLCLK()
	//ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CAirsideObjectTreeCtrl message handlers
void CAirsideObjectTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	UINT flags;
	HTREEITEM hItem = HitTest(point, &flags);
	CTreeCtrlEx::SelectItem(hItem);
	GetParent()->SendMessage(WM_OBJECTTREE_DOUBLECLICK, 0L, (LPARAM)hItem);
}

//void CAirsideObjectTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
//{
//	UINT flags;
//	HTREEITEM hItem = HitTest(point, &flags);
//	CTreeCtrlEx::SelectItem(hItem);
//	GetParent()->SendMessage(WM_OBJECTTREE_LBUTTONDOWN, 0L, (LPARAM)hItem);
//}

BOOL CAirsideObjectTreeCtrl::SetItemData( HTREEITEM hItem, DWORD_PTR dwData )
{
	AirsideObjectTreeCtrlItemDataEx * pItemDataEx = 0;
	pItemDataEx = GetItemDataEx(hItem);
	if(pItemDataEx)
	{
		pItemDataEx->dwptrItemData = dwData;
		return TRUE;
	}
	else
	{
		pItemDataEx = new AirsideObjectTreeCtrlItemDataEx;
		pItemDataEx->lSize = sizeof(AirsideObjectTreeCtrlItemDataEx);
		pItemDataEx->dwptrItemData = dwData; 
		if(CTreeCtrlEx::SetItemData(hItem,(DWORD_PTR)pItemDataEx))
			m_vrItemDataEx.push_back(pItemDataEx);
		else
			delete pItemDataEx;
	}
	return FALSE;
}

DWORD_PTR CAirsideObjectTreeCtrl::GetItemData(HTREEITEM hItem)
{
	AirsideObjectTreeCtrlItemDataEx * pItemDataEx = 0;
	pItemDataEx = GetItemDataEx(hItem);
	if(pItemDataEx)
		return pItemDataEx->dwptrItemData;
	else
		return 0;
}

void CAirsideObjectTreeCtrl::SetItemDataEx(HTREEITEM hItem,const AirsideObjectTreeCtrlItemDataEx& aoidDataEx)
{
	AirsideObjectTreeCtrlItemDataEx * pItemDataEx = 0;
	pItemDataEx = GetItemDataEx(hItem);
	if(pItemDataEx)
		*pItemDataEx = aoidDataEx;
	else
	{
		pItemDataEx = new AirsideObjectTreeCtrlItemDataEx;
		*pItemDataEx = aoidDataEx; 
		if(CTreeCtrlEx::SetItemData(hItem,(DWORD_PTR)pItemDataEx))
			m_vrItemDataEx.push_back(pItemDataEx);
		else
			delete pItemDataEx;
	}
}

AirsideObjectTreeCtrlItemDataEx* CAirsideObjectTreeCtrl::GetItemDataEx(HTREEITEM hItem)
{
	AirsideObjectTreeCtrlItemDataEx * pItemDataEx = 0;
	pItemDataEx = (AirsideObjectTreeCtrlItemDataEx*)(CTreeCtrlEx::GetItemData(hItem));
	return pItemDataEx;
}

BOOL CAirsideObjectTreeCtrl::DeleteItem( HTREEITEM hItem )
{
	AirsideObjectTreeCtrlItemDataEx * pItemDataEx = 0;
	pItemDataEx = GetItemDataEx(hItem);
	BOOL bret = CTreeCtrlEx::DeleteItem(hItem);
	if(pItemDataEx)
	{
		CTreeCtrlEx::SetItemData(hItem,0);
		std::vector<AirsideObjectTreeCtrlItemDataEx*>::iterator itrFind = std::find(m_vrItemDataEx.begin(),m_vrItemDataEx.end(),pItemDataEx);
		if(itrFind != m_vrItemDataEx.end())
		{
			if(*itrFind)
				delete (*itrFind);
			m_vrItemDataEx.erase(itrFind);
		}
		/*else
			delete pItemDataEx;*/
	}
	return bret;
}

BOOL CAirsideObjectTreeCtrl::DeleteAllItems(void)
{
	CTreeCtrlEx::DeleteAllItems();
	ClearItemDatas();
	return TRUE;
}

void CAirsideObjectTreeCtrl::OnPaint() 
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
			AirsideObjectTreeCtrlItemDataEx * pItemDataEx = 0;
			int xRect = 0,yRect = 0;
			xRect = rect.left+2;
			yRect = rect.top+1;

			pItemDataEx = GetItemDataEx(hItem);		

			if(pItemDataEx){  ASSERT( IsItemInList(pItemDataEx) ); } 
			
			if(pItemDataEx && IsItemInList(pItemDataEx) )
			{			
				strRight = sItem; 				
				for(std::vector<ItemStringSectionColor>::iterator itrSection = pItemDataEx->vrItemStringSectionColorShow.begin();\
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

bool CAirsideObjectTreeCtrl::IsItemInList( AirsideObjectTreeCtrlItemDataEx* pEx ) const
{
	std::vector<AirsideObjectTreeCtrlItemDataEx*>::const_iterator itr = std::find(m_vrItemDataEx.begin(),m_vrItemDataEx.end(),pEx);
	if(itr!= m_vrItemDataEx.end())
		return true;
	return false;
}
