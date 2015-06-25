#include "stdafx.h"
#include "ItinerantFlightListCtrl.h"
#include "common\WinMsg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
CItinerantFlightListCtrl::CItinerantFlightListCtrl()
:m_strEntryValue(_T(""))
,m_strExitValue(_T(""))
{
	CBitmap bitmap;
	VERIFY(bitmap.LoadBitmap(AFX_IDB_CHECKLISTBOX_95));
	BITMAP bm;
	bitmap.GetObject(sizeof(BITMAP), &bm);
	m_sizeCheck.cx = bm.bmWidth / 3;
	m_sizeCheck.cy = bm.bmHeight;
	m_hBitmapCheck = (HBITMAP) bitmap.Detach();
}

CItinerantFlightListCtrl::~CItinerantFlightListCtrl()
{

}

BEGIN_MESSAGE_MAP(CItinerantFlightListCtrl, CListCtrlEx)
	//{{AFX_MSG_MAP(CItinerantFlightListCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

int CItinerantFlightListCtrl::InsertColumn(int nCol, const LV_COLUMNEX* pColumn)
{
	if(ddStyleList.GetSize() <= nCol)
		ddStyleList.SetSize(nCol + 1);
	LVCOLDROPLIST* DropList = new LVCOLDROPLIST;

	if(pColumn->fmt & LVCFMT_NOEDIT)
		DropList->Style = NO_EDIT;
	else if(pColumn->fmt & LVCFMT_DROPDOWN)
		DropList->Style = DROP_DOWN;
	else if(pColumn->fmt & LVCFMT_SPINEDIT)
		DropList->Style = SPIN_EDIT;
	else if(pColumn->fmt & LVCFMT_NUMBER)
		DropList->Style = NUMBER_ONLY;
	else if(pColumn->fmt & LVCFMT_DATETIME)
		DropList->Style = DATE_TIME;
	else if(pColumn->fmt & LVCFMT_POPUP_GATE_SELECTION)
		DropList->Style = POPUP_GATE_SELECTION;
	if( pColumn->fmt & LVCFMT_EDIT )
		DropList->Style = TEXT_EDIT;
	if (pColumn->fmt & LVCFMT_CHECK ) 
		DropList->Style = CHECKBOX; 
	if (pColumn->fmt & LVCFMT_BMP)
		DropList->Style = BMP_EDIT;

	DropList->List.AddTail(pColumn->csList);
	DropList->strDateFormat = pColumn->pszDateFormat;
	DropList->strShowDateFormat = pColumn->pszShowDateFormat;
	ddStyleList[nCol] = (void*)DropList;
	// Transfer the Relevent Fields to lvc and pass to base class
	LV_COLUMN lvc;
	memcpy(&lvc, pColumn, sizeof(tagLVCOLUMNA));
	return CListCtrl::InsertColumn(nCol, &lvc);
}

void CItinerantFlightListCtrl::DrawItem( LPDRAWITEMSTRUCT lpdis)
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
	CRect rcVisible1;
	this->GetItemRect(nItem,rcVisible1,LVIR_LABEL);
	rcVisible1.left+=2;
	CRect rcVisible2;
	this->GetSubItemRect( nItem,1,LVIR_LABEL,rcVisible2 );
	CRect rcVisible3;
	this->GetSubItemRect(nItem,2,LVIR_LABEL,rcVisible3);
	rcVisible3.left+=2;
	CRect rcVisible4;
	this->GetSubItemRect(nItem,3,LVIR_LABEL,rcVisible4);
	rcVisible1.left+=4;
	CRect rcVisible5;
	this->GetSubItemRect(nItem, 4, LVIR_LABEL, rcVisible5);
	rcVisible5.left+=2;
	CRect rcVisible6;
	this->GetSubItemRect( nItem,5,LVIR_LABEL,rcVisible6 );
	rcVisible6.left+=2;
	CRect rcVisible7;
	this->GetSubItemRect( nItem,6,LVIR_LABEL,rcVisible7 );
	rcVisible7.left+=2;
	CRect rcVisible8;
	this->GetSubItemRect( nItem,7,LVIR_LABEL,rcVisible8 );
	rcVisible8.left+=2;
	CRect rcVisible9;
	this->GetSubItemRect( nItem,8,LVIR_LABEL,rcVisible9 );
	rcVisible9.left+=2;
	ItinerantFlight* pItem = (ItinerantFlight*)lvi.lParam;

	rcAllLabels.left = rcVisible1.left;
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

	pDC->DrawText( pItem->GetName(), rcVisible1, DT_LEFT| DT_VCENTER | DT_SINGLELINE );
	pDC->DrawText( m_strEntryValue, rcVisible3, DT_LEFT| DT_VCENTER | DT_SINGLELINE );
	pDC->DrawText( m_strExitValue, rcVisible4, DT_LEFT| DT_VCENTER | DT_SINGLELINE );
	CString strValue = _T("");
	CString strTimeRange = _T(""),strDay = _T("");

	ElapsedTime* etInsertStart = pItem->GetArrStartTime();
	long lSecond = etInsertStart->asSeconds();
	strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
	strTimeRange = strDay + _T(" - ");
	ElapsedTime* etInsertEnd = pItem->GetArrEndTime();
	if (*etInsertStart == 0L
		&& *etInsertEnd == 0L)
	{
		etInsertEnd->SetTime("23:59:59");
	}
	lSecond = etInsertEnd->asSeconds();
	strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
	strTimeRange += strDay;
	strValue = strTimeRange;
	pDC->DrawText( strValue, rcVisible5, DT_LEFT| DT_VCENTER | DT_SINGLELINE );
	etInsertStart = pItem->GetDepStartTime();
	lSecond = etInsertStart->asSeconds();
	strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
	strTimeRange = strDay + _T(" - ");
	etInsertEnd = pItem->GetDepEndTime();
	if (*etInsertStart == 0L
		&& *etInsertEnd == 0L)
	{
		etInsertEnd->SetTime("23:59:59");
	}
	lSecond = etInsertEnd->asSeconds();
	strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
	strTimeRange += strDay;
	strValue = strTimeRange;
	pDC->DrawText( strValue, rcVisible6, DT_LEFT| DT_VCENTER | DT_SINGLELINE );
	strValue.Format("%d", pItem->GetFltCount());
	pDC->DrawText( strValue, rcVisible7, DT_LEFT| DT_VCENTER | DT_SINGLELINE );
	pDC->DrawText( pItem->GetArrDistScreenPrint(), rcVisible8, DT_LEFT| DT_VCENTER | DT_SINGLELINE );
	pDC->DrawText( pItem->GetDepDistScreenPrint(), rcVisible9, DT_LEFT| DT_VCENTER | DT_SINGLELINE );

	//draw checkbox
	CDC bitmapDC;
	HBITMAP hOldBM;
	int nCheck = (int) pItem->GetEnRoute();
	rcVisible2.left+=(rcVisible2.Width()-m_sizeCheck.cx)/2;
	rcVisible2.top+=(rcVisible2.Height()-m_sizeCheck.cy)/2;
	if(bitmapDC.CreateCompatibleDC(pDC)) {
		hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);
		pDC->BitBlt(rcVisible2.left, rcVisible2.top, m_sizeCheck.cx, m_sizeCheck.cy, &bitmapDC,
			m_sizeCheck.cx * nCheck, 0, SRCCOPY);
		::SelectObject(bitmapDC.m_hDC, hOldBM);
		bitmapDC.DeleteDC();
	}

	
	if (lvi.state & LVIS_FOCUSED) 
		pDC->DrawFocusRect(rcAllLabels); 

	if (bSelected) {
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

void CItinerantFlightListCtrl::InsertItinerantFlightItem(int _nIdx, ItinerantFlight* _pItem )
{
	int nIdx = InsertItem( _nIdx, LPCTSTR(""));
	SetItemData( nIdx, (DWORD)_pItem );
	ASSERT(nIdx == _nIdx);
}

int CItinerantFlightListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrlEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	return 0;
}
