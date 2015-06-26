#include "StdAfx.h"
#include ".\ListCtrlExEx.h"
#include "resource.h"


CListCtrlExEx::CListCtrlExEx(void)
{
	CBitmap bitmap;
	VERIFY(bitmap.LoadBitmap(IDB_BITMAP_ATCSEPARATION));
	BITMAP bm;
	bitmap.GetObject(sizeof(BITMAP), &bm);
	m_sizeCheck.cx = bm.bmWidth / 4;
	m_sizeCheck.cy = bm.bmHeight;
	m_hBitmapCheck = (HBITMAP) bitmap.Detach();
}

CListCtrlExEx::~CListCtrlExEx(void)
{
	if(m_hBitmapCheck != NULL)
		::DeleteObject(m_hBitmapCheck);
}


BEGIN_MESSAGE_MAP(CListCtrlExEx, CListCtrlEx)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

BOOL CListCtrlExEx::PreCreateWindow(CREATESTRUCT& cs)
{
	//set the list ctrl draw Item itself
	ModifyStyle(NULL,LVS_OWNERDRAWFIXED);

	return CListCtrlEx::PreCreateWindow(cs);
}

void CListCtrlExEx::DrawItem(LPDRAWITEMSTRUCT lpdis)
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

	//drat items
	CRect rcPBCount; 


	int nSubItemCount = static_cast<int>(ddStyleList.GetSize());
	for (int subItem = 0; subItem < nSubItemCount;subItem ++ )
	{
		if(((LVCOLDROPLIST*)ddStyleList[subItem])->Style == CHECKBOX)
		{
			CRect rcCheckBox;
			if (subItem == 0) 
				this->GetItemRect(nItem, rcPBCount, LVIR_LABEL);
			else
				this->GetSubItemRect( nItem,subItem,LVIR_LABEL,rcCheckBox );			
	
			rcCheckBox.left+=(rcCheckBox.Width()-m_sizeCheck.cx)/2;
			rcCheckBox.top+=(rcCheckBox.Height()-m_sizeCheck.cy)/2;

			CDC bitmapDC;
			HBITMAP hOldBM;
			int nCheck = GetItemStatus(lvi.iItem,subItem);
			if(bitmapDC.CreateCompatibleDC(pDC))
			{
				hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);

				pDC->BitBlt(rcCheckBox.left, rcCheckBox.top, m_sizeCheck.cx - 2, m_sizeCheck.cy, &bitmapDC,
					m_sizeCheck.cx * nCheck + 1 , 0, SRCCOPY);

				::SelectObject(bitmapDC.m_hDC, hOldBM);
				bitmapDC.Detach();
			}
		}
		else
		{
			if(subItem == 0)
				this->GetItemRect(nItem, rcPBCount, LVIR_LABEL);
			else
				this->GetSubItemRect(nItem,subItem,LVIR_LABEL,rcPBCount);

			rcPBCount.left+=2;
			CString strName=GetItemText(lvi.iItem,subItem);
			pDC->DrawText(strName , rcPBCount, DT_LEFT | DT_VCENTER | DT_SINGLELINE );		

		}
	}

	//draw lead runway


	//draw checkbox
	//CRect rcCheckBox;
	//this->GetSubItemRect( nItem,1,LVIR_LABEL,rcCheckBox );
	//rcCheckBox.left+=(rcCheckBox.Width()-m_sizeCheck.cx)/2;
	//rcCheckBox.top+=(rcCheckBox.Height()-m_sizeCheck.cy)/2;

	//CDC bitmapDC;
	//HBITMAP hOldBM;
	//int nCheck = GetItemStatus(lvi.iItem,1);
	//if(bitmapDC.CreateCompatibleDC(pDC)) {
	//	hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);

	//	pDC->BitBlt(rcCheckBox.left, rcCheckBox.top, m_sizeCheck.cx - 2, m_sizeCheck.cy, &bitmapDC,
	//		m_sizeCheck.cx * nCheck + 1 , 0, SRCCOPY);

	//	::SelectObject(bitmapDC.m_hDC, hOldBM);
	//	bitmapDC.Detach();
	//}


	//this->GetSubItemRect(nItem,2,LVIR_LABEL,rcPBCount);
	//rcPBCount.left+=2;
	//strName=GetItemText(lvi.iItem,2);
	//pDC->DrawText(strName , rcPBCount, DT_LEFT | DT_VCENTER | DT_SINGLELINE );



	//rcCheckBox.SetRect(0,0,0,0);
	//nCheck = 0;
	//this->GetSubItemRect( nItem,3,LVIR_LABEL,rcCheckBox );
	//rcCheckBox.left+=(rcCheckBox.Width()-m_sizeCheck.cx)/2;
	//rcCheckBox.top+=(rcCheckBox.Height()-m_sizeCheck.cy)/2;

	////	bitmapDC.;
	////	HBITMAP hOldBM;
	////	int 
	//nCheck = GetItemStatus(lvi.iItem,3);
	//if(bitmapDC.CreateCompatibleDC(pDC)) {
	//	hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);

	//	pDC->BitBlt(rcCheckBox.left, rcCheckBox.top, m_sizeCheck.cx - 2, m_sizeCheck.cy, &bitmapDC,
	//		m_sizeCheck.cx * nCheck +1, 0, SRCCOPY);

	//	::SelectObject(bitmapDC.m_hDC, hOldBM);
	//	bitmapDC.Detach();
	//}

	if (lvi.state & LVIS_FOCUSED) 
		pDC->DrawFocusRect(rcAllLabels); 

	if (bSelected) {
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}

}

void CListCtrlExEx::SetItemStatus(int nItem,int nSubItem,CHECKSTATUS nStatus)
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

CHECKSTATUS CListCtrlExEx::GetItemStatus(int nItem,int nSubItem)
{
	CString strCheck=GetItemText(nItem,nSubItem);
	if (strCheck == CString(""))
	{
		SetItemText(nItem,nSubItem,CString("0"));
		return DISABLEUNCHECKED;
	}
	if(strCheck==CString("3"))
		return CHECKED;
	if (strCheck == CString("2"))
		return UNCHECKED;
	if (strCheck == CString("1"))
		return DISABLECHECKED;

	return DISABLEUNCHECKED;
}

void CListCtrlExEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nItem=HitTest(point);
	if(nItem != -1) 
	{

		CRect rcVisible;
		int subItem = -1;
		for ( ; subItem < ddStyleList.GetSize() ; subItem ++) 
		{
			GetSubItemRect(nItem,subItem,LVIR_LABEL,rcVisible);
			if(rcVisible.PtInRect(point))
				break;
		}

		if (subItem != -1)
		{	
			if(((LVCOLDROPLIST*)ddStyleList[subItem])->Style == CHECKBOX)
			{
				CHECKSTATUS nStatus=GetItemStatus(nItem,subItem);

				if(nStatus != DISABLECHECKED && nStatus != DISABLEUNCHECKED)
				{
					if (nStatus == UNCHECKED)
						SetItemStatus( nItem,subItem,CHECKED);
					else if (nStatus == CHECKED) 
						SetItemStatus( nItem,subItem,UNCHECKED);
					else
						SetItemStatus(nItem,subItem,DISABLEUNCHECKED);
				}
			}
		}
		Invalidate();
	}

	CListCtrlEx::OnLButtonDown(nFlags, point);
}

void CListCtrlExEx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	Invalidate();
	CListCtrlEx::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CListCtrlExEx::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	Invalidate();
	CListCtrlEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CListCtrlExEx::SetDateTimeColumnFormat(int nColumn, LPCTSTR lpszFormat)
{
	if (nColumn >= ddStyleList.GetSize())
		return;

	LVCOLDROPLIST* pDropList = (LVCOLDROPLIST*)(ddStyleList[nColumn]);
	if (pDropList->Style != DATE_TIME)
		return;

	pDropList->strDateFormat = lpszFormat;
}

void CListCtrlExEx::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int colnum;
	int index = HitTestEx(point, &colnum);
	if (index == -1)
		return;

	LVCOLDROPLIST* pDropList = (LVCOLDROPLIST*)(ddStyleList[colnum]);
	if (pDropList->Style == DATE_TIME 
		&& !pDropList->strDateFormat.IsEmpty())
	{
		SYSTEMTIME dateTime;
		::SendMessage(GetParent()->GetSafeHwnd(), WM_LISTCTRL_SETDATETIME, WPARAM(&dateTime),LPARAM(colnum));	
		EditDateTimeLabelEx(index, colnum, pDropList->strDateFormat, &dateTime);
		return;
	}

	CListCtrlEx::OnLButtonDblClk(nFlags, point);
}

void CListCtrlExEx::EditDateTimeLabelEx(int nItem, int nCol, LPCTSTR lpszFormat, SYSTEMTIME* pDateTime)
{
	if(!EnsureVisible(nItem, TRUE)) 
		return;

	m_nTimeSelItem = nItem;
	m_nTimeSelCol = nCol;

	COleDateTime dt(*pDateTime);
	if (dt.GetStatus() == COleDateTime::invalid)
		dt.SetDateTime(1900, 1, 1, 0, 0, 0);

	// Create or move it
	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = GetHeaderCtrl();
	int nColumnCount = pHeader->GetItemCount();
	if(nCol >= nColumnCount || GetColumnWidth(nCol) < 5)
		return ;

	// Get the column offset
	int offset = 0;
	for(int i = 0; i < nCol; i++)
		offset += GetColumnWidth(i);

	CRect rect;
	GetItemRect(nItem, &rect, LVIR_BOUNDS);

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(&rcClient);
	if(offset + rect.left < 0 || offset + rect.left > rcClient.right)
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll(size);
		rect.left -= size.cx;
	}

	rect.left += offset+4;
	rect.right = rect.left + GetColumnWidth(nCol) - 3 ;
	if(rect.right > rcClient.right) 
		rect.right = rcClient.right;
	rect.bottom = rect.top + 20;

	if (!m_inPlaceDateTimeCtrl.GetSafeHwnd())
	{
		m_inPlaceDateTimeCtrl.Create( DTS_TIMEFORMAT, rect,this, ID_DATETIMEPICKER_TIME );
		m_inPlaceDateTimeCtrl.SetFormat(lpszFormat);
	}
	else
		m_inPlaceDateTimeCtrl.MoveWindow(&rect);

	m_inPlaceDateTimeCtrl.SetTime(dt);
	m_inPlaceDateTimeCtrl.ShowWindow(SW_SHOW);
	m_inPlaceDateTimeCtrl.BringWindowToTop();
	m_inPlaceDateTimeCtrl.SetFocus();

	return;  // USER CAN NOT CHARGE IT.
}
