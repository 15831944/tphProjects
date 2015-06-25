#include "StdAfx.h"
#include ".\nodeblocklistctrl.h"
#include "../InputAirside/IntersectionNodeBlockList.h"

NodeBlockListCtrl::NodeBlockListCtrl(void)
{
	CBitmap bitmap;
	VERIFY(bitmap.LoadBitmap(AFX_IDB_CHECKLISTBOX_95));
	BITMAP bm;
	bitmap.GetObject(sizeof(BITMAP), &bm);
	m_sizeCheck.cx = bm.bmWidth / 3;
	m_sizeCheck.cy = bm.bmHeight;
	m_hBitmapCheck = (HBITMAP) bitmap.Detach();
}

NodeBlockListCtrl::~NodeBlockListCtrl(void)
{
	if(m_hBitmapCheck != NULL)
		::DeleteObject(m_hBitmapCheck);
}

BEGIN_MESSAGE_MAP(NodeBlockListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
END_MESSAGE_MAP()

void NodeBlockListCtrl::DrawItem( LPDRAWITEMSTRUCT lpdis )
{
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

	CRect rcName;
	this->GetSubItemRect(nItem,1,LVIR_LABEL,rcName);

	CRect rcBlock;
	this->GetSubItemRect(nItem,2,LVIR_LABEL,rcBlock);
	GetBlockRect(rcBlock);


	rcAllLabels.left = rcLabel.left;
	if(bSelected) 
	{ 
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT))); 
	} 
	else 
	{
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_WINDOW)));
	}

	IntersectionNodeBlock* pItem = (IntersectionNodeBlock*)GetItemData(nItem);

	//draw node id
	CString strName;
	strName.Format("%d", pItem->GetNodeID());
	pDC->DrawText(strName , rcLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP);

	////draw node name
	strName= " "+ pItem->GetNodeName();
	pDC->DrawText(strName, rcName, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX );//| DT_NOCLIP);


	//draw block check box
	CDC bitmapDC;
	HBITMAP hOldBM;
	int nCheck = pItem->IsBlock()?1:0;
	if(bitmapDC.CreateCompatibleDC(pDC))
	{
		hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);
		pDC->BitBlt(rcBlock.left, rcBlock.top, m_sizeCheck.cx, m_sizeCheck.cy, &bitmapDC,
			m_sizeCheck.cx * nCheck, 0, SRCCOPY);
		::SelectObject(bitmapDC.m_hDC, hOldBM);
		bitmapDC.DeleteDC();
	}

	if (lvi.state & LVIS_FOCUSED) 
		pDC->DrawFocusRect(rcAllLabels); 

	if (bSelected) 
	{
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

void NodeBlockListCtrl::InsertNodeBlockItem( int _nIdx, IntersectionNodeBlock* _pItem )
{
	CString strContent;
	strContent.Format("%d",_pItem->GetNodeID());
	int nIdx = InsertItem( _nIdx,strContent);
	SetItemText(_nIdx,1,_pItem->GetNodeName());
	strContent.Format("%d",_pItem->IsBlock()?1:0);
	SetItemText(_nIdx,2,strContent);

	SetItemData( nIdx, (DWORD_PTR)_pItem );
	ASSERT(nIdx == _nIdx);
}

void NodeBlockListCtrl::GetBlockRect( CRect& r )
{
	r.left = (r.left+r.right-m_sizeCheck.cx)/2;
	r.top = (r.bottom+r.top-m_sizeCheck.cy)/2;
	r.bottom = r.top+m_sizeCheck.cy;
	r.right = r.left+m_sizeCheck.cx;
}

void NodeBlockListCtrl::OnClick( NMHDR* pNMHDR, LRESULT* pResult )
{
	CPoint ptMsg = GetMessagePos();
	this->ScreenToClient( &ptMsg );
	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
	int nItem = pNMLW->iItem;
	if(nItem != -1) 
	{
		IntersectionNodeBlock* pItem = (IntersectionNodeBlock*)GetItemData(nItem);
		if(pNMLW->iSubItem == 2) 
		{// lev column
			CRect rcLogBlock;
			this->GetSubItemRect(nItem,2,LVIR_LABEL,rcLogBlock);
			GetBlockRect(rcLogBlock);
			if(rcLogBlock.PtInRect(ptMsg)) 
			{
				if( pItem->IsBlock() )
					pItem->SetBlock( false );
				else
					pItem->SetBlock( true );
			}
		}
		Invalidate();
	}

	*pResult = 0;
}