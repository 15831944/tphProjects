#include "StdAfx.h"
#include ".\atcrwpairlistctrl.h"
#include "resource.h"

CATCRWPairListCtrl::CATCRWPairListCtrl(void)
{
	CBitmap bitmap;
	VERIFY(bitmap.LoadBitmap(IDB_BITMAP_ATCSEPARATION));
	BITMAP bm;
	bitmap.GetObject(sizeof(BITMAP), &bm);
	m_sizeCheck.cx = bm.bmWidth / 4;
	m_sizeCheck.cy = bm.bmHeight;
	m_hBitmapCheck = (HBITMAP) bitmap.Detach();
}

CATCRWPairListCtrl::~CATCRWPairListCtrl(void)
{
}


BEGIN_MESSAGE_MAP(CATCRWPairListCtrl, CListCtrlEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

void CATCRWPairListCtrl::DrawItem( LPDRAWITEMSTRUCT lpdis)
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
	GetItem(&lvi); 

	BOOL bFocus = (GetFocus() == this);
	BOOL bSelected = (lvi.state & LVIS_SELECTED) || (lvi.state & LVIS_DROPHILITED);

	CRect rcAllLabels; 
	this->GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS); 



	rcAllLabels.left = 0;
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

	//draw lead runway
	CRect rcPBCount; 
	this->GetItemRect(nItem, rcPBCount, LVIR_LABEL);
	rcPBCount.left+=2;
	CString strName=GetItemText(lvi.iItem,0);
	pDC->DrawText(strName , rcPBCount, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	//draw checkbox
	CRect rcCheckBox;
	this->GetSubItemRect( nItem,1,LVIR_LABEL,rcCheckBox );
	rcCheckBox.left+=(rcCheckBox.Width()-m_sizeCheck.cx)/2;
	rcCheckBox.top+=(rcCheckBox.Height()-m_sizeCheck.cy)/2;

	CDC bitmapDC;
	HBITMAP hOldBM;
	int nCheck = GetItemStatus(lvi.iItem,1);
	if(bitmapDC.CreateCompatibleDC(pDC)) {
		hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);

		pDC->BitBlt(rcCheckBox.left, rcCheckBox.top, m_sizeCheck.cx - 2, m_sizeCheck.cy, &bitmapDC,
			m_sizeCheck.cx * nCheck + 1 , 0, SRCCOPY);

		::SelectObject(bitmapDC.m_hDC, hOldBM);
		bitmapDC.Detach();
	}


	this->GetSubItemRect(nItem,2,LVIR_LABEL,rcPBCount);
	rcPBCount.left+=2;
	strName=GetItemText(lvi.iItem,2);
	pDC->DrawText(strName , rcPBCount, DT_LEFT | DT_VCENTER | DT_SINGLELINE );



	rcCheckBox.SetRect(0,0,0,0);
	nCheck = 0;
	this->GetSubItemRect( nItem,3,LVIR_LABEL,rcCheckBox );
	rcCheckBox.left+=(rcCheckBox.Width()-m_sizeCheck.cx)/2;
	rcCheckBox.top+=(rcCheckBox.Height()-m_sizeCheck.cy)/2;

	//	bitmapDC.;
	//	HBITMAP hOldBM;
	//	int 
	nCheck = GetItemStatus(lvi.iItem,3);
	if(bitmapDC.CreateCompatibleDC(pDC)) {
		hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);

			pDC->BitBlt(rcCheckBox.left, rcCheckBox.top, m_sizeCheck.cx - 2, m_sizeCheck.cy, &bitmapDC,
				m_sizeCheck.cx * nCheck +1, 0, SRCCOPY);
	
		::SelectObject(bitmapDC.m_hDC, hOldBM);
		bitmapDC.Detach();
	}

	if (lvi.state & LVIS_FOCUSED) 
		pDC->DrawFocusRect(rcAllLabels); 

	if (bSelected) {
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}


}
void CATCRWPairListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nItem=HitTest(point);
	if(nItem != -1) 
	{
		CRect rcVisible;
		GetSubItemRect(nItem,1,LVIR_LABEL,rcVisible);
		if(rcVisible.PtInRect(point))
		{
			CHECKSTATUS nStatus=GetItemStatus(nItem,1);

			if(nStatus != DISABLECHECKED && nStatus != DISABLEUNCHECKED)
			{
				if (nStatus == UNCHECKED)
					SetItemStatus( nItem,1,CHECKED);
				else if (nStatus == CHECKED) 
					SetItemStatus( nItem,1,UNCHECKED);
				else
					SetItemStatus(nItem,1,DISABLEUNCHECKED);
			}
//			SendMessage(UM_ITEM_CHECK_STATUS_CHANGED,(WPARAM)nItem,(LPARAM)!bCheck);
		}

		GetSubItemRect(nItem,3,LVIR_LABEL,rcVisible);
		if(rcVisible.PtInRect(point))
		{
			CHECKSTATUS nStatus=GetItemStatus(nItem,3);

			if(nStatus != DISABLECHECKED && nStatus != DISABLEUNCHECKED)
			{
				if (nStatus == UNCHECKED)
					SetItemStatus( nItem,3,CHECKED);
				else if (nStatus == CHECKED) 
					SetItemStatus( nItem,3,UNCHECKED);
				else
					SetItemStatus(nItem,3,DISABLEUNCHECKED);
			}
//			SendMessage(UM_ITEM_CHECK_STATUS_CHANGED,(WPARAM)nItem,(LPARAM)!bCheck);
		}

		Invalidate();
	}
	CListCtrlEx::OnLButtonDown(nFlags, point);
}
void CATCRWPairListCtrl::SetItemStatus(int nItem,int nSubItem,CHECKSTATUS nStatus)
{
	if(nStatus == CHECKED)
		SetItemText(nItem,nSubItem,"3");
	else if (nStatus == UNCHECKED ) 
		SetItemText(nItem,nSubItem,"2");
	else if (nStatus == DISABLECHECKED) //DISABLE
		SetItemText(nItem,nSubItem,"1");
	else
		SetItemText(nItem,nSubItem,"0");
}

CHECKSTATUS CATCRWPairListCtrl::GetItemStatus(int nItem,int nSubItem)
{
	CString strCheck=GetItemText(nItem,nSubItem);
	if(strCheck==CString("3"))
		return CHECKED;
	if (strCheck == CString("2"))
		return UNCHECKED;
	if (strCheck == CString("1"))
		return DISABLECHECKED;

	return DISABLEUNCHECKED;
}

void CATCRWPairListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	Invalidate();

	CListCtrlEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CATCRWPairListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	Invalidate();

	CListCtrlEx::OnVScroll(nSBCode, nPos, pScrollBar);
}
