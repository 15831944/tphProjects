// ResDispListCtrl.cpp: implementation of the CResDispListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResDispListCtrl.h"
#include "PaxShapeDefs.h"
#include "ShapesManager.h"
#include "TermPlanDoc.h"
#include "common\WinMsg.h"
#include "inputs\ResourcePool.h"


/////////////////////////////////////////////////////////////////////////////
// CPaxDispListCtrl

CResDispListCtrl::CResDispListCtrl(CTermPlanDoc* pDoc)
{
	m_pDoc = pDoc;
	ASSERT(m_pDoc);
	
	CBitmap bitmap;
	VERIFY(bitmap.LoadBitmap(AFX_IDB_CHECKLISTBOX_95));
	BITMAP bm;
	bitmap.GetObject(sizeof(BITMAP), &bm);
	m_sizeCheck.cx = bm.bmWidth / 3;
	m_sizeCheck.cy = bm.bmHeight;
	m_hBitmapCheck = (HBITMAP) bitmap.Detach();
	
	//calculate the total number of shapes
	int nTotalShapes = 0;
	for(int nSet=0; nSet<SHAPESMANAGER->GetShapeSetCount(); nSet++) {
		nTotalShapes+=SHAPESMANAGER->GetShapeCount(nSet);
	}

	m_shapesImageList.Create(10 , 10, ILC_COLOR8, 0, nTotalShapes);
	// load shape bitmap	
	for(int nSet=0; nSet<SHAPESMANAGER->GetShapeSetCount(); nSet++) {
		for(int nShape=0; nShape<SHAPESMANAGER->GetShapeCount(nSet); nShape++) {
			CBitmap* pBM = CBitmap::FromHandle(SHAPESMANAGER->GetShapeImgSmallBitmap(nSet, nShape));
			m_shapesImageList.Add(pBM, pBM);
		}
	}
}

CResDispListCtrl::~CResDispListCtrl()
{
	if(m_hBitmapCheck != NULL)
		::DeleteObject(m_hBitmapCheck);
}

BEGIN_MESSAGE_MAP(CResDispListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CResDispListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResDispListCtrl message handlers

void CResDispListCtrl::DrawItem( LPDRAWITEMSTRUCT lpdis)
{
	//if(lpdis->hDC)
	COLORREF clrTextSave, clrBkSave;
	CRect rcItem(lpdis->rcItem);
	CDC* pDC = CDC::FromHandle(lpdis->hDC);
	int nItem = lpdis->itemID;
	LVITEM lvi; 
    lvi.mask = LVIF_PARAM | LVIF_STATE; 
    lvi.iItem = nItem;
    lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;
    this->GetItem(&lvi); 
	
	BOOL bFocus = (GetFocus() == this);
	BOOL bSelected = (lvi.state & LVIS_SELECTED) || (lvi.state & LVIS_DROPHILITED);
	
	CRect rcAllLabels; 
    this->GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS); 
    CRect rcLabel; 
    this->GetItemRect(nItem, rcLabel, LVIR_LABEL);
	CRect rcVisible;
	this->GetSubItemRect(nItem,1,LVIR_LABEL,rcVisible);
	GetVisibleRect(rcVisible);
	CRect rcShape;
	this->GetSubItemRect( nItem,2,LVIR_LABEL,rcShape );
	CRect rcColor;
	this->GetSubItemRect(nItem,3,LVIR_LABEL,rcColor);
	GetColorBoxRect(rcColor);

	rcAllLabels.left = rcLabel.left;
    if(bSelected) { 
        clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
        clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
        pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT))); 
    } 
    else {
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
        pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_WINDOW)));
	}
	CResourceDispPropItem* pItem = (CResourceDispPropItem*)lvi.lParam;
	
	//draw resource pool name
	int nResPoolIdx = pItem->GetResourcePoolIndex();
	
	if(nResPoolIdx >= 0)
		pDC->DrawText(m_pDoc->GetTerminal().m_pResourcePoolDB->getResourcePoolByIndex(nResPoolIdx)->getPoolName() , rcLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	else
		pDC->DrawText(_T("undefined"), rcLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	
	//draw Visible checkbox
	CDC bitmapDC;
	HBITMAP hOldBM;
	int nCheck = (int)pItem->IsVisible();
	if(bitmapDC.CreateCompatibleDC(pDC)) {
		hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);
		pDC->BitBlt(rcVisible.left, rcVisible.top, m_sizeCheck.cx, m_sizeCheck.cy, &bitmapDC,
			m_sizeCheck.cx * nCheck, 0, SRCCOPY);
		::SelectObject(bitmapDC.m_hDC, hOldBM);
		bitmapDC.Detach();
	}
	
	//draw shape listcombox
	CPoint pt(rcShape.left+6, rcShape.top+2);
	m_shapesImageList.Draw( pDC, SHAPESMANAGER->SetAndShapeToIdx( pItem->GetShape().first, pItem->GetShape().second ), pt, ILD_NORMAL );

	rcShape.left = rcShape.left + 10 + m_sizeCheck.cx;
	pDC->DrawText( SHAPESMANAGER->GetShapeName( pItem->GetShape().first, pItem->GetShape().second ), rcShape, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP );

	
	//draw color box
	pDC->FillSolidRect( rcColor,pItem->GetColor() );
	pDC->FrameRect( rcColor,&CBrush(::GetSysColor(COLOR_BTNTEXT)));
		
	if (lvi.state & LVIS_FOCUSED) 
        pDC->DrawFocusRect(rcAllLabels); 
	
    if (bSelected) {
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

void CResDispListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint ptMsg = GetMessagePos();
	this->ScreenToClient( &ptMsg );
	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
	int nItem = pNMLW->iItem;
	if(nItem != -1) 
	{
		CResourceDispPropItem* pItem = (CResourceDispPropItem*)GetItemData(nItem);
		if(pNMLW->iSubItem == 1) 
		{// visible column
			CRect rcVisible;
			this->GetSubItemRect(nItem,1,LVIR_LABEL,rcVisible);
			GetVisibleRect(rcVisible);
			if(rcVisible.PtInRect(ptMsg))
			{
				if( pItem->IsVisible() )
					pItem->IsVisible( FALSE );
				else
				pItem->IsVisible( TRUE );
			}
		}
		Invalidate();
	}
	
	*pResult = 0;
}

void CResDispListCtrl::GetColorBoxRect(CRect& r)
{
	r.right = r.CenterPoint().x+r.Width()/4;
	r.left = r.CenterPoint().x-r.Width()/4;
	r.DeflateRect(3,2,3,2);
}

void CResDispListCtrl::GetVisibleRect(CRect& r)
{
	r.left = (r.left+r.right-m_sizeCheck.cx)/2;
	r.top = (r.bottom+r.top-m_sizeCheck.cy)/2;
	r.bottom = r.top+m_sizeCheck.cy;
	r.right = r.left+m_sizeCheck.cx;
}

void CResDispListCtrl::InsertResDispItem(int _nIdx, CResourceDispPropItem* _pItem )
{
	LVITEM lvi; 
	lvi.mask = LVIF_PARAM | LVIF_TEXT;
	lvi.iSubItem = 0;
	lvi.pszText = LPSTR_TEXTCALLBACK;
	lvi.iItem = _nIdx;
	lvi.lParam = (LPARAM) _pItem;
	int nIdx = InsertItem(&lvi);
	//int nIdx = InsertItem( _nIdx );//, m_pDoc->GetTerminal().m_pResourcePoolDB->getResourcePoolByIndex(_pItem->GetResourcePoolIndex())->getPoolName() );
	SetItemData( nIdx, (DWORD)_pItem );
	ASSERT(nIdx == _nIdx);
}


void CResDispListCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint ptMsg = GetMessagePos();
	this->ScreenToClient( &ptMsg );
	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
	int nItem = pNMLW->iItem;
	if(nItem != -1) 
	{
		CResourceDispPropItem* pItem = (CResourceDispPropItem*)GetItemData(nItem);
		if(pNMLW->iSubItem == 2) 
		{// shape column
			///*
			//----------------------------------------Modified by Tim In 1/11/2003[Begin]-------------------------------------------------//
			CRect rcShape;
			this->GetSubItemRect( nItem, 2, LVIR_LABEL, rcShape);
			m_nShapeSelItem = nItem;
			rcShape.left +=(rcShape.Width()-16 );
			rcShape.bottom =rcShape.top +18;
			rcShape.right =rcShape.left +16;
			
			if (!m_bbDropList.GetSafeHwnd())
				m_bbDropList.Create(rcShape,IDC_COMBO_LEVEL,this) ;
			else
				m_bbDropList.SetWindowPos(NULL,rcShape.left,rcShape.top,rcShape.Width(),rcShape.Height(),SWP_NOACTIVATE|SWP_NOZORDER|SWP_SHOWWINDOW);
			m_bbDropList.DisplayListWnd() ;
		}
		else if(pNMLW->iSubItem == 3) 
		{// color box column
			CRect rcColor;
			this->GetSubItemRect(nItem,3,LVIR_LABEL,rcColor);
			GetColorBoxRect(rcColor);
			if(rcColor.PtInRect(ptMsg)) 
			{
				CColorDialog dlg( pItem->GetColor(), CC_ANYCOLOR | CC_FULLOPEN, this);
				if(dlg.DoModal() == IDOK)
				{
					pItem->SetColor( dlg.GetColor() );
				}
			}
		} 
		Invalidate();
	}
	*pResult = 0;
}

int CResDispListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	
	return 0;
}

LRESULT CResDispListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_INPLACE_COMBO)
	{
		if(GetItemCount()==0) return CListCtrl::DefWindowProc(message, wParam, lParam);
		int nIdx = (int) lParam;
		UINT nID = (UINT) wParam;

		if(nID != -1) { ///shape, nID == setID
			CResourceDispPropItem* pItem = (CResourceDispPropItem*)GetItemData(m_nShapeSelItem);
			pItem->SetShape( std::make_pair(nID, nIdx) );
		}

		SetFocus();
		return TRUE;
	}
	
	
	return CListCtrl::DefWindowProc(message, wParam, lParam);
}

