// PaxDistListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "VehicleTagListCtrl.h"

#include "DlgTracerDensity.h"
#include "PaxFilterDlg.h"
#include "PaxShapeDefs.h"
#include "ShapesManager.h"
#include "../InputAirside/VehicleSpecifications.h"
#include "common\WinMsg.h"
#include ".\vehicletaglistctrl.h"
#include "../Landside/LandsideVehicleTags.h"
#include "../Landside/VehicleItemDetailList.h"
#include "DlgSelectLandsideVehicleType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CVehicleTagListCtrl

CVehicleTagListCtrl::CVehicleTagListCtrl(void) :m_vehicleTypeInPlaceComboBox(-1),m_nProjID( -1 )
{ 
}

CVehicleTagListCtrl::~CVehicleTagListCtrl()
{ 
}


BEGIN_MESSAGE_MAP(CVehicleTagListCtrl, CListBox)
	//{{AFX_MSG_MAP(CVehicleDispPropListCtrl) 
	//ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_LBUTTONDBLCLK() 
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVehicleDispPropListCtrl message handlers
/////////////////////////////////////////////////////////////////////////////
// CVehicleDispPropListCtrl message handlers

void CVehicleTagListCtrl::DrawItem( LPDRAWITEMSTRUCT lpdis)
{ 
	COLORREF clrTextSave, clrBkSave;
	CRect rcItem(lpdis->rcItem);
	CDC* pDC = CDC::FromHandle(lpdis->hDC);
	int nItem = lpdis->itemID;
	/*LVITEM lvi; 
	lvi.mask = LVIF_PARAM | LVIF_STATE; 
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;
	this->GetItem(&lvi); */

	BOOL bFocus = (GetFocus() == this);
	BOOL bSelected = (lpdis->itemState & ODS_SELECTED) || (lpdis->itemState & ODS_COMBOBOXEDIT);
	
	CRect rcAllLabels; 
	this->GetItemRect(nItem, rcAllLabels); 

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
	CVehicleTagItem* pItem = (CVehicleTagItem*)GetItemData(nItem);
	if(!pItem)return;

	//draw Case name 
	CString strName = pItem->m_vehicleItem.getName();
	pDC->DrawText(strName , rcAllLabels, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP); 
	if (lpdis->itemState & ODS_FOCUS) 
		pDC->DrawFocusRect(rcAllLabels); 

	if (bSelected) {
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
} 


void CVehicleTagListCtrl::InsertVehicleTagItem(int _nIdx, CVehicleTagItem* _pItem )
{
	int nIdx = InsertString( _nIdx, LPCTSTR(_pItem->m_vehicleItem.getName()) );
	SetItemData( nIdx, (DWORD)_pItem );
	ASSERT(nIdx == _nIdx);
}


//void CVehicleTagListCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
//{
//	// TODO: Add your control notification handler code here
//	CPoint ptMsg = GetMessagePos();
//	this->ScreenToClient( &ptMsg );
//	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
//	int nItem = pNMLW->iItem;
//	if(nItem != LB_ERR ) 
//	{
//		CVehicleTagItem* pItem = (CVehicleTagItem*)GetItemData(nItem);
//		m_nFlag_WM_INPLACE_COMBO = -1;
//		if(pNMLW->iSubItem == 0) 
//		{// vehicle type column
//			CRect rcVehicleType;
//			GetItemRect(nItem, rcVehicleType);

//			int nHeight = rcVehicleType.Height();
//			rcVehicleType.bottom += nHeight * 5;

//			m_nVehicleItem = nItem;
//			m_nFlag_WM_INPLACE_COMBO = 0;
//			int nSelComboBoxIndex = -1;
//			if (!m_vehicleTypeInPlaceComboBox.GetSafeHwnd())
//			{

//				DWORD dwStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST|CBS_DISABLENOSCROLL;

//				m_vehicleTypeInPlaceComboBox.Create( dwStyle, rcVehicleType, this, IDC_COMBO_LEVEL );
//				m_vehicleTypeInPlaceComboBox.SetItemHeight( -1, nHeight );
//				m_vehicleTypeInPlaceComboBox.SetHorizontalExtent( 300 );
//				CVehicleSpecifications vehicleSpe;
//				vehicleSpe.ReadData(m_nProjID);
//				size_t nVehicleSpeCount = vehicleSpe.GetElementCount();
//				COMBOBOXEXITEM cbItem;
//				cbItem.mask = CBEIF_TEXT;
//				for(size_t i = 0; i < nVehicleSpeCount; i++) {
//					cbItem.iItem = (int)i;
//					cbItem.pszText = (LPSTR)(LPCTSTR)(vehicleSpe.GetItem(i)->getName());
//					if(pItem->m_vehicleItem.getName() == vehicleSpe.GetItem(i)->getName() )
//						nSelComboBoxIndex = (int)i;
//					m_vehicleTypeInPlaceComboBox.InsertItem( &cbItem );
//					m_vehicleTypeInPlaceComboBox.SetItemData((int)i,vehicleSpe.GetItem(i)->GetID());
//				}
//			}
//			else
//			{
//				m_vehicleTypeInPlaceComboBox.MoveWindow(&rcVehicleType);
//			}

//			if(nSelComboBoxIndex == -1)nSelComboBoxIndex = 0;
//			m_vehicleTypeInPlaceComboBox.SetCurSel(nSelComboBoxIndex);
//			m_vehicleTypeInPlaceComboBox.ShowWindow(SW_SHOW);
//			m_vehicleTypeInPlaceComboBox.BringWindowToTop();
//			m_vehicleTypeInPlaceComboBox.SetFocus();		
//		}
//	} 
//	*pResult = 0;
//} 
LRESULT CVehicleTagListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message == WM_INPLACE_COMBO)
	{
		if(GetCount()==0) return CListBox::DefWindowProc(message, wParam, lParam);
		int nIdx = (int) lParam;
		UINT nID = (UINT) wParam;

		if(nID == -1) { //line type
			if(m_nFlag_WM_INPLACE_COMBO == 0)
			{
				CVehicleTagItem* pItemVehicle = (CVehicleTagItem*)GetItemData(m_nVehicleItem);
				if(pItemVehicle)
				{
					pItemVehicle->m_vehicleItem.DBElement::ReadData(m_vehicleTypeInPlaceComboBox.GetItemData(nIdx));
					int nItem = GetCurSel();
					if(nItem != LB_ERR ) 
					{
						DWORD_PTR pItemData = GetItemData(nItem);
						DeleteString(nItem);
						int nItemNew = -1;
						if( GetStyle() & LBS_SORT )
							nItemNew = AddString(pItemVehicle->m_vehicleItem.getName());
						else
							nItemNew = InsertString(nItem,pItemVehicle->m_vehicleItem.getName());
						SetItemData(nItemNew,pItemData);
						SetCurSel(nItemNew);
					}
				}
			}
		}
		SetFocus();
		return TRUE;
	}


	return CListBox::DefWindowProc(message, wParam, lParam);
}


void CVehicleTagListCtrl::SetProjID(int nProjID)
{
	m_nProjID = nProjID;
}



void CVehicleTagListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CPoint ptMsg = point;
	this->ScreenToClient( &ptMsg );
	int nItem = GetCurSel();
	if(nItem != LB_ERR ) 
	{
		CVehicleTagItem* pItem = (CVehicleTagItem*)GetItemData(nItem);
		m_nFlag_WM_INPLACE_COMBO = -1; 
		{// vehicle type column
			CRect rcVehicleType;
			GetItemRect(nItem, rcVehicleType);

			int nHeight = rcVehicleType.Height();
			rcVehicleType.bottom += nHeight * 5;

			m_nVehicleItem = nItem;
			m_nFlag_WM_INPLACE_COMBO = 0;
			int nSelComboBoxIndex = -1;
			if (!m_vehicleTypeInPlaceComboBox.GetSafeHwnd())
			{

				DWORD dwStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST|CBS_DISABLENOSCROLL;

				m_vehicleTypeInPlaceComboBox.Create( dwStyle, rcVehicleType, this, IDC_COMBO_LEVEL );
				m_vehicleTypeInPlaceComboBox.SetItemHeight( -1, nHeight );
				m_vehicleTypeInPlaceComboBox.SetHorizontalExtent( 300 );
				CVehicleSpecifications vehicleSpe;
				vehicleSpe.ReadData(m_nProjID);
				size_t nVehicleSpeCount = vehicleSpe.GetElementCount();
				COMBOBOXEXITEM cbItem;
				cbItem.mask = CBEIF_TEXT;
				for(size_t i = 0; i < nVehicleSpeCount; i++) {
					cbItem.iItem = (int)i;
					cbItem.pszText = (LPSTR)(LPCTSTR)(vehicleSpe.GetItem(i)->getName());
					if(pItem->m_vehicleItem.getName() == vehicleSpe.GetItem(i)->getName() )
						nSelComboBoxIndex = (int)i;
					m_vehicleTypeInPlaceComboBox.InsertItem( &cbItem );
					m_vehicleTypeInPlaceComboBox.SetItemData((int)i,vehicleSpe.GetItem(i)->GetID());
				}

				m_vehicleTypeInPlaceComboBox.MoveWindow(&rcVehicleType);
			}
			else
			{
				m_vehicleTypeInPlaceComboBox.MoveWindow(&rcVehicleType);
			}

			if(nSelComboBoxIndex == -1)nSelComboBoxIndex = 0;
			m_vehicleTypeInPlaceComboBox.SetCurSel(nSelComboBoxIndex);
			m_vehicleTypeInPlaceComboBox.ShowWindow(SW_SHOW);
			m_vehicleTypeInPlaceComboBox.BringWindowToTop();
			m_vehicleTypeInPlaceComboBox.SetFocus();	
			return;
		}
	} 

	CListBox::OnLButtonDblClk(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CLandsideVehicleTypeListCtrl::CLandsideVehicleTypeListCtrl(void) :m_vehicleTypeInPlaceComboBox(-1),m_nProjID( -1 )
{ 
}

CLandsideVehicleTypeListCtrl::~CLandsideVehicleTypeListCtrl()
{ 
}


BEGIN_MESSAGE_MAP(CLandsideVehicleTypeListCtrl, CListBox)
	//{{AFX_MSG_MAP(CVehicleDispPropListCtrl) 
	//ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_LBUTTONDBLCLK() 
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVehicleDispPropListCtrl message handlers
/////////////////////////////////////////////////////////////////////////////
// CVehicleDispPropListCtrl message handlers

void CLandsideVehicleTypeListCtrl::DrawItem( LPDRAWITEMSTRUCT lpdis)
{ 
	COLORREF clrTextSave, clrBkSave;
	CRect rcItem(lpdis->rcItem);
	CDC* pDC = CDC::FromHandle(lpdis->hDC);
	int nItem = lpdis->itemID;
	/*LVITEM lvi; 
	lvi.mask = LVIF_PARAM | LVIF_STATE; 
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;
	this->GetItem(&lvi); */

	BOOL bFocus = (GetFocus() == this);
	BOOL bSelected = (lpdis->itemState & ODS_SELECTED) || (lpdis->itemState & ODS_COMBOBOXEDIT);

	CRect rcAllLabels; 
	this->GetItemRect(nItem, rcAllLabels); 

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
	CVehicleTagItem* pItem = (CVehicleTagItem*)GetItemData(nItem);
	if(!pItem)return;

	//draw Case name 
	CString strName = pItem->m_vehicleItem.getName();
	pDC->DrawText(strName , rcAllLabels, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP); 
	if (lpdis->itemState & ODS_FOCUS) 
		pDC->DrawFocusRect(rcAllLabels); 

	if (bSelected) {
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
} 


void CLandsideVehicleTypeListCtrl::InsertVehicleTagItem(int _nIdx, CLandsideVehicleTypes* _pItem )
{
	int nIdx = InsertString( _nIdx, LPCTSTR(_pItem->m_vehicleItem.getName()) );
	SetItemData( nIdx, (DWORD)_pItem );
	ASSERT(nIdx == _nIdx);
}

LRESULT CLandsideVehicleTypeListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message == WM_INPLACE_COMBO)
	{
		if(GetCount()==0) return CListBox::DefWindowProc(message, wParam, lParam);
		int nIdx = (int) lParam;
		UINT nID = (UINT) wParam;

		if(nID == -1) { //line type
			if(m_nFlag_WM_INPLACE_COMBO == 0)
			{
				CLandsideVehicleTypes* pItemVehicle = (CLandsideVehicleTypes*)GetItemData(m_nVehicleItem);
				if(pItemVehicle)
				{
					pItemVehicle->m_vehicleItem.ReadData(m_vehicleTypeInPlaceComboBox.GetItemData(nIdx),NULL);
					int nItem = GetCurSel();
					if(nItem != LB_ERR ) 
					{
						DWORD_PTR pItemData = GetItemData(nItem);
						DeleteString(nItem);
						int nItemNew = -1;
						if( GetStyle() & LBS_SORT )
							nItemNew = AddString(pItemVehicle->m_vehicleName);
						else
							nItemNew = InsertString(nItem,pItemVehicle->m_vehicleName);
						SetItemData(nItemNew,pItemData);
						SetCurSel(nItemNew);
					}
				}
			}
		}
		SetFocus();
		return TRUE;
	}


	return CListBox::DefWindowProc(message, wParam, lParam);
}


void CLandsideVehicleTypeListCtrl::SetProjID(int nProjID)
{
	m_nProjID = nProjID;
}



void CLandsideVehicleTypeListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CPoint ptMsg = point;
	this->ScreenToClient( &ptMsg );
	int nItem = GetCurSel();
	if(nItem != LB_ERR ) 
	{
		CLandsideVehicleTypes* pItem = (CLandsideVehicleTypes*)GetItemData(nItem);
		m_nFlag_WM_INPLACE_COMBO = -1; 
		{// vehicle type column
			CRect rcVehicleType;
			GetItemRect(nItem, rcVehicleType);

			int nHeight = rcVehicleType.Height();
			rcVehicleType.bottom += nHeight * 5;

			m_nVehicleItem = nItem;
			m_nFlag_WM_INPLACE_COMBO = 0;
			int nSelComboBoxIndex = -1;
			if (!m_vehicleTypeInPlaceComboBox.GetSafeHwnd())
			{

				if (pItem->m_vehicleName == "DEFAULT" || pItem->m_vehicleName == "OVERLAP")
					return;
				CDlgSelectLandsideVehicleType dlg;
				if(dlg.DoModal() == IDOK)
				{	
					DeleteString(nItem);
					CString strName;
					strName = dlg.GetName();
					if (strName.IsEmpty())
					{
						MessageBox(_T("Please define vehicle type name in Vehicle Type Definition Dialog!."),_T("Warning"),MB_OK);
					}
					pItem->m_vehicleName = strName;
					AddString(pItem->m_vehicleName);
					SetItemData(nItem,(DWORD_PTR)pItem);
				}
		   }
		}
	} 

	CListBox::OnLButtonDblClk(nFlags, point);
}