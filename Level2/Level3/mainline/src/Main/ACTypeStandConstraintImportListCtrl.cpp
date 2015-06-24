#include "StdAfx.h"
#include ".\actypestandconstraintimportlistctrl.h"
#include "resource.h"

IMPLEMENT_DYNAMIC(ACTypeStandConstraintImportListCtrl, CXListCtrl)
ACTypeStandConstraintImportListCtrl::ACTypeStandConstraintImportListCtrl(void)
{
	m_mapElementIcon.insert( std::make_pair(0, LoadIcon(IDI_ICON_NULL) ) );
	m_mapElementIcon.insert( std::make_pair(1, LoadIcon(IDI_ICON_FAIL) ) );
	m_mapElementIcon.insert( std::make_pair(2, LoadIcon(IDI_ICON_SUCCESS) ) );

	m_hDefaultIcon = LoadIcon(IDI_ICON_NULL);//
	DWORD n = ::GetLastError();
}

ACTypeStandConstraintImportListCtrl::~ACTypeStandConstraintImportListCtrl(void)
{
	for(std::map<int, HICON>::iterator iter = m_mapElementIcon.begin();
		iter != m_mapElementIcon.end(); iter++)
	{
		if(iter->second)
			DestroyIcon(iter->second);
	}
	if(m_hDefaultIcon)
		DestroyIcon(m_hDefaultIcon);
}

BEGIN_MESSAGE_MAP(ACTypeStandConstraintImportListCtrl, CListCtrl)
END_MESSAGE_MAP()

HICON ACTypeStandConstraintImportListCtrl::LoadIcon(UINT uIDRes)
{
	return (HICON) LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(uIDRes), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
}

HICON ACTypeStandConstraintImportListCtrl::GetIcon(int nVal)
{
	std::map<int, HICON>::iterator iter = m_mapElementIcon.begin();
	for(; iter!= m_mapElementIcon.end(); iter++)
	{
		if(iter->first == nVal)
			return iter->second;
	}

	return m_hDefaultIcon;
}

void ACTypeStandConstraintImportListCtrl::DrawItem(LPDRAWITEMSTRUCT lpdis)
{
	//COLORREF clrTextSave, clrBkSave;
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
	CRect rcACType;
	this->GetSubItemRect(nItem,1,LVIR_LABEL,rcACType);
	CRect rcSymbol;
	this->GetSubItemRect(nItem,2,LVIR_LABEL,rcSymbol);
	CRect rcReason;
	this->GetSubItemRect(nItem,3,LVIR_LABEL,rcReason);

	//AdjustColorBoxRect(rcColor);
	rcAllLabels.left = rcLabel.left;
	COLORREF clrTextSave, clrBkSave;
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
	int nVal = GetItemData(nItem);
	CString strStandName = GetItemText(nItem, 0);
	CString strACTypes = GetItemText(nItem,1);
	CString strReason = GetItemText(nItem,3);

	pDC->DrawText(strStandName,rcLabel, DT_LEFT | DT_VCENTER /*| DT_SINGLELINE | DT_NOCLIP*/);

	rcACType.left += 5;
	pDC->DrawText(strACTypes,rcACType, DT_LEFT | DT_VCENTER /*| DT_SINGLELINE | DT_NOCLIP*/);

	HICON hDrawIcon = GetIcon(nVal);

	ASSERT(hDrawIcon != NULL);

	::DrawIconEx(lpdis->hDC, rcSymbol.left+ 17, rcSymbol.top, hDrawIcon, rcSymbol.Height(), 
		rcSymbol.Height()-2, 0, NULL, DI_NORMAL);

	rcReason.left += 5;
	pDC->DrawText(strReason,rcReason, DT_LEFT | DT_VCENTER /*| DT_SINGLELINE | DT_NOCLIP*/);

	if (lvi.state & LVIS_FOCUSED) 
		pDC->DrawFocusRect(rcAllLabels); 

	if (bSelected) {
		pDC->SetTextColor(RGB(100,145,111));
		pDC->SetBkColor(RGB(234,11,9));
	}

}