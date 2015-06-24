// PaxDistListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "VehicleDispPropListCtrl.h"

#include "DlgTracerDensity.h"
#include "PaxFilterDlg.h"
#include "PaxShapeDefs.h"
#include "ShapesManager.h"
#include "../InputAirside/VehicleSpecifications.h"
#include "common\WinMsg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static char szLineTypeLabel[][64] = {
	"solid1", "solid2", "solid3",
		"dash1", "dash2", "dash3",
		"dot1", "dot2", "dot3" };


	/////////////////////////////////////////////////////////////////////////////
	// CVehicleDispPropListCtrl

	CVehicleDispPropListCtrl::CVehicleDispPropListCtrl() :  m_linesInPlaceComboBox(-1),m_vehicleTypeInPlaceComboBox(-1)
	{
		CBitmap bitmap;
		VERIFY(bitmap.LoadBitmap(AFX_IDB_CHECKLISTBOX_95));
		BITMAP bm;
		bitmap.GetObject(sizeof(BITMAP), &bm);
		m_sizeCheck.cx = bm.bmWidth / 3;
		m_sizeCheck.cy = bm.bmHeight;
		m_hBitmapCheck = (HBITMAP) bitmap.Detach();


		//calculate the total number of shapes
		int nTotalShapes = SHAPESMANAGER->GetShapeCount(6);//shape set at 6 is vehicle
		m_shapesImageList.Create(10 , 10, ILC_COLOR8, 0, nTotalShapes);
		// load shape bitmap
		for(int nShape=0; nShape< nTotalShapes; nShape++) {
			CBitmap* pBM = CBitmap::FromHandle(SHAPESMANAGER->GetShapeImgSmallBitmap( 6, nShape));
			m_shapesImageList.Add(pBM, pBM);
		}

		//load line type bitmaps
		CBitmap bmIL;
		m_linesImageList.Create(69 , 10, ILC_COLOR16, 0, 9);
		bmIL.LoadBitmap( IDB_BITMAP_SOLID1 ); m_linesImageList.Add( &bmIL, &bmIL  ); bmIL.Detach();
		bmIL.LoadBitmap( IDB_BITMAP_SOLID2 ); m_linesImageList.Add( &bmIL, &bmIL  ); bmIL.Detach();
		bmIL.LoadBitmap( IDB_BITMAP_SOLID3 ); m_linesImageList.Add( &bmIL, &bmIL  ); bmIL.Detach();
		bmIL.LoadBitmap( IDB_BITMAP_DASH1 ); m_linesImageList.Add( &bmIL, &bmIL  ); bmIL.Detach();
		bmIL.LoadBitmap( IDB_BITMAP_DASH2 ); m_linesImageList.Add( &bmIL, &bmIL  ); bmIL.Detach();
		bmIL.LoadBitmap( IDB_BITMAP_DASH3 ); m_linesImageList.Add( &bmIL, &bmIL  ); bmIL.Detach();
		bmIL.LoadBitmap( IDB_BITMAP_DOT1 ); m_linesImageList.Add( &bmIL, &bmIL  ); bmIL.Detach();
		bmIL.LoadBitmap( IDB_BITMAP_DOT2 ); m_linesImageList.Add( &bmIL, &bmIL  ); bmIL.Detach();
		bmIL.LoadBitmap( IDB_BITMAP_DOT3 ); m_linesImageList.Add( &bmIL, &bmIL  ); bmIL.Detach();



		/*
		bitmap2 = new CBitmap;
		VERIFY(bitmap2->LoadBitmap( IDR_MAINFRAME ));

		m_shapimagelist.Create( 10, 10, ILC_COLOR8, 0, 20 );
		m_shapimagelist.Add( bitmap2,bitmap2);
		bitmap2->Detach();
		//	m_hBitmapShape = (HBITMAP) bitmap2.Detach();
		bitmap2->LoadBitmap(AFX_IDB_CHECKLISTBOX_95);
		m_shapimagelist.Add( bitmap2,bitmap2);
		bitmap2->Detach();
		*/
	}

	CVehicleDispPropListCtrl::~CVehicleDispPropListCtrl()
	{
		if(m_hBitmapCheck != NULL)
			::DeleteObject(m_hBitmapCheck);
		//	if(m_hBitmapShape != NULL)
		//		::DeleteObject(m_hBitmapShape);
		//	for(int i=0;i< m_shapesImageList.GetImageCount();i++ )
		//		m_shapimagelist.Remove(i);
	}


	BEGIN_MESSAGE_MAP(CVehicleDispPropListCtrl, CListCtrl)
		//{{AFX_MSG_MAP(CVehicleDispPropListCtrl)
		ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
		ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
		ON_WM_CREATE()
		//}}AFX_MSG_MAP

	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CVehicleDispPropListCtrl message handlers
	/////////////////////////////////////////////////////////////////////////////
	// CVehicleDispPropListCtrl message handlers

	void CVehicleDispPropListCtrl::DrawItem( LPDRAWITEMSTRUCT lpdis)
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
		CRect rcLineType;
		this->GetSubItemRect( nItem,3,LVIR_LABEL,rcLineType );
		CRect rcColor;
		this->GetSubItemRect(nItem,4,LVIR_LABEL,rcColor);
		GetColorBoxRect(rcColor);
		CRect rcLev;
		this->GetSubItemRect(nItem,5,LVIR_LABEL,rcLev);
		GetVisibleRect(rcLev);
		CRect rcDensity;
		this->GetSubItemRect(nItem, 6, LVIR_LABEL, rcDensity);

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
		CVehicleDispPropItem* pItem = (CVehicleDispPropItem*)lvi.lParam;

		//draw Case name 
		if (nItem == 0)
		{
			CString strName = _T("DEFAULT");
			pDC->DrawText(strName , rcLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP);
		} 
		else
		{
			CString strName = pItem->GetVehicleType()->getName();
			pDC->DrawText(strName , rcLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP);
		}

		//draw Visible checkbox
		CDC bitmapDC;
		HBITMAP hOldBM;
		int nCheck = (int)pItem->IsVisible();
		if(bitmapDC.CreateCompatibleDC(pDC)) {
			hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);
			pDC->BitBlt(rcVisible.left, rcVisible.top, m_sizeCheck.cx, m_sizeCheck.cy, &bitmapDC,
				m_sizeCheck.cx * nCheck, 0, SRCCOPY);
			::SelectObject(bitmapDC.m_hDC, hOldBM);
			bitmapDC.DeleteDC();
		}

		//draw shape listcombox
		int nOffset = SHAPESMANAGER->SetAndShapeToIdx(pItem->GetShape().first,0);
		CPoint pt(rcShape.left+6, rcShape.top+2);
		m_shapesImageList.Draw( pDC, SHAPESMANAGER->SetAndShapeToIdx( pItem->GetShape().first, pItem->GetShape().second )-nOffset, pt, ILD_NORMAL );

		rcShape.left = rcShape.left + 10 + m_sizeCheck.cx;
		pDC->DrawText( SHAPESMANAGER->GetShapeName( pItem->GetShape().first, pItem->GetShape().second ), rcShape, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP );

		//draw line listcombox
		CPoint pt2(rcLineType.left+10, rcLineType.top+2);
		m_linesImageList.Draw( pDC, pItem->GetLineType(),pt2,ILD_NORMAL );
		rcLineType.left = rcLineType.left + 69 + m_sizeCheck.cx + 5;
		pDC->DrawText( szLineTypeLabel[pItem->GetLineType()], rcLineType, DT_LEFT | DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP );


		//draw color box
		pDC->FillSolidRect( rcColor,pItem->GetColor() );
		pDC->FrameRect( rcColor,&CBrush(::GetSysColor(COLOR_BTNTEXT)));

		//draw checkbox
		nCheck = (int) pItem->IsLeaveTrail();
		if(bitmapDC.CreateCompatibleDC(pDC)) {
			hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);
			pDC->BitBlt(rcLev.left, rcLev.top, m_sizeCheck.cx, m_sizeCheck.cy, &bitmapDC,
				m_sizeCheck.cx * nCheck, 0, SRCCOPY);
			::SelectObject(bitmapDC.m_hDC, hOldBM);
			bitmapDC.DeleteDC();
		}

		// draw density label
		CString sDensity;
		sDensity.Format("%.2f%%", pItem->GetDensity()*100.0);
		pDC->DrawText(sDensity , rcDensity, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP);

		if (lvi.state & LVIS_FOCUSED) 
			pDC->DrawFocusRect(rcAllLabels); 

		if (bSelected) {
			pDC->SetTextColor(clrTextSave);
			pDC->SetBkColor(clrBkSave);
		}
	}

	void CVehicleDispPropListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		CPoint ptMsg = GetMessagePos();
		this->ScreenToClient( &ptMsg );
		NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
		int nItem = pNMLW->iItem;
		if(nItem != -1) 
		{
			CVehicleDispPropItem* pItem = (CVehicleDispPropItem*)GetItemData(nItem);
			if(pNMLW->iSubItem == 1) 
			{// visible column
				CRect rcVisible;
				this->GetSubItemRect(nItem,1,LVIR_LABEL,rcVisible);
				GetVisibleRect(rcVisible);
				if(rcVisible.PtInRect(ptMsg))
				{
					pItem->SetVisible( !pItem->IsVisible() );
					GetParent()->SendMessage(VEHICLE_DISP_PROP_CHANGE1);
				}
			}
			else if(pNMLW->iSubItem == 5) 
			{// lev column
				CRect rcLev;
				this->GetSubItemRect(nItem,5,LVIR_LABEL,rcLev);
				GetVisibleRect(rcLev);
				if(rcLev.PtInRect(ptMsg)) 
				{
					if( pItem->IsLeaveTrail() )
						pItem->SetLeaveTrail( FALSE );
					else
						pItem->SetLeaveTrail( TRUE );
				}
			}
			Invalidate();
		}

		*pResult = 0;
	}

	void CVehicleDispPropListCtrl::GetColorBoxRect(CRect& r)
	{
		//	r.DeflateRect(CSize(0,1));
		r.right = r.CenterPoint().x+r.Width()/4;
		r.left = r.CenterPoint().x-r.Width()/4;
		r.DeflateRect(3,2,3,2);
	}

	void CVehicleDispPropListCtrl::GetVisibleRect(CRect& r)
	{
		r.left = (r.left+r.right-m_sizeCheck.cx)/2;
		r.top = (r.bottom+r.top-m_sizeCheck.cy)/2;
		r.bottom = r.top+m_sizeCheck.cy;
		r.right = r.left+m_sizeCheck.cx;
	}


	/*
	BOOL CLayerListCtrl::UpdateFloorData()
	{
	BOOL bRet = FALSE;
	for(int i=0; i<this->GetItemCount(); i++) {
	_layerInfo* pdi = (_layerInfo*) this->GetItemData(i);
	ASSERT(pdi != NULL);
	if(pdi->pFloorLayer->bIsOn != pdi->bIsOn) {
	pdi->pFloorLayer->bIsOn = pdi->bIsOn;
	bRet = TRUE;
	}
	if(pdi->pFloorLayer->cColor != pdi->cColor) {
	pdi->pFloorLayer->cColor = pdi->cColor;
	bRet = TRUE;
	}
	if(pdi->pFloorLayer->sName.CompareNoCase(pdi->sName) != 0) {
	pdi->pFloorLayer->sName = pdi->sName;
	bRet = TRUE;
	}
	}
	return bRet;
	}
	*/



	void CVehicleDispPropListCtrl::InsertPaxDispItem(int _nIdx, CVehicleDispPropItem* _pItem )
	{
		int nIdx = InsertItem( _nIdx, LPCTSTR(_pItem->GetVehicleType()->getName()) );
		SetItemData( nIdx, (DWORD)_pItem );
		ASSERT(nIdx == _nIdx);
	}


	void CVehicleDispPropListCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		// TODO: Add your control notification handler code here
		CPoint ptMsg = GetMessagePos();
		this->ScreenToClient( &ptMsg );
		NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
		int nItem = pNMLW->iItem;
		if(nItem != -1) 
		{
			CVehicleDispPropItem* pItem = (CVehicleDispPropItem*)GetItemData(nItem);
			m_nFlag_WM_INPLACE_COMBO = -1;
			if(pNMLW->iSubItem == 0) 
			{// vehicle type column
					if (nItem == 0)
					{
						return;
					}
					CRect rcVehicleType;
					this->GetItemRect(nItem, rcVehicleType, LVIR_LABEL);

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
						vehicleSpe.ReadData(pItem->GetProjectID());
						size_t nVehicleSpeCount = vehicleSpe.GetElementCount();
						COMBOBOXEXITEM cbItem;
						cbItem.mask = CBEIF_TEXT;
						for(size_t i = 0; i < nVehicleSpeCount; i++) {
							cbItem.iItem = (int)i;
							cbItem.pszText = (LPSTR)(LPCTSTR)(vehicleSpe.GetItem(i)->getName());
							if(pItem->GetVehicleType()->getName() == vehicleSpe.GetItem(i)->getName() )
								nSelComboBoxIndex = (int)i;
							m_vehicleTypeInPlaceComboBox.InsertItem( &cbItem );
							m_vehicleTypeInPlaceComboBox.SetItemData((int)i,vehicleSpe.GetItem(i)->GetID());
						}
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
			}
		    else if(pNMLW->iSubItem == 2) 
			{// shape column
				///*
				//----------------------------------------Modified by Tim In 1/11/2003[Begin]-------------------------------------------------//
				CRect rcShape;
				this->GetSubItemRect( nItem, 2, LVIR_LABEL, rcShape);
				m_nShapeSelItem = nItem;
				rcShape.left +=(rcShape.Width()-16 );
				rcShape.bottom =rcShape.top +18;
				rcShape.right =rcShape.left +16;
				
				m_bbDropList.SetShapeSetIndex(6);
				if (!m_bbDropList.GetSafeHwnd())
					m_bbDropList.Create(rcShape,IDC_COMBO_LEVEL,this) ;
				else
					m_bbDropList.SetWindowPos(NULL,rcShape.left,rcShape.top,rcShape.Width(),rcShape.Height(),SWP_NOACTIVATE|SWP_NOZORDER|SWP_SHOWWINDOW);
				m_bbDropList.DisplayListWnd() ;
				//----------------------------------------Modified by Tim In 1/11/2003[End]-------------------------------------------------//
				//*/
				/*
				CRect rcShape;
				this->GetSubItemRect( nItem, 2, LVIR_LABEL, rcShape);

				int nHeight = rcShape.Height();
				rcShape.bottom += nHeight * 5;

				m_nShapeSelItem = nItem;
				if (!m_shapesInPlaceComboBox.GetSafeHwnd())
				{

				DWORD dwStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST|CBS_DISABLENOSCROLL;

				m_shapesInPlaceComboBox.Create( dwStyle, rcShape, this, IDC_COMBO_LEVEL );
				m_shapesInPlaceComboBox.SetItemHeight( -1, nHeight );
				m_shapesInPlaceComboBox.SetHorizontalExtent( 300 );
				m_shapesInPlaceComboBox.SetImageList( &m_shapesImageList );
				COMBOBOXEXITEM cbItem;
				cbItem.mask = CBEIF_TEXT|CBEIF_IMAGE|CBEIF_SELECTEDIMAGE;
				for(int i=0; i<PAXSHAPECOUNT; i++) {
				cbItem.iItem = i;
				cbItem.iImage = i;
				cbItem.iSelectedImage = i;
				cbItem.pszText = szShapeLabel[i];
				m_shapesInPlaceComboBox.InsertItem( &cbItem );
				}
				}
				else
				{
				m_shapesInPlaceComboBox.MoveWindow(&rcShape);
				}

				m_shapesInPlaceComboBox.SetCurSel( (int)pItem->GetShape() );
				m_shapesInPlaceComboBox.ShowWindow(SW_SHOW);
				m_shapesInPlaceComboBox.BringWindowToTop();
				m_shapesInPlaceComboBox.SetFocus();		
				*/
			}
			else if(pNMLW->iSubItem == 3) 
			{// line type column
				CRect rcLineType;
				this->GetSubItemRect( nItem, 3, LVIR_LABEL, rcLineType);

				int nHeight = rcLineType.Height();
				rcLineType.bottom += nHeight * 5;

				m_nLineSelItem = nItem;
				m_nFlag_WM_INPLACE_COMBO = 3;
				if (!m_linesInPlaceComboBox.GetSafeHwnd())
				{

					DWORD dwStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST|CBS_DISABLENOSCROLL;

					m_linesInPlaceComboBox.Create( dwStyle, rcLineType, this, IDC_COMBO_LEVEL );
					m_linesInPlaceComboBox.SetItemHeight( -1, nHeight );
					m_linesInPlaceComboBox.SetHorizontalExtent( 300 );
					m_linesInPlaceComboBox.SetImageList( &m_linesImageList );
					COMBOBOXEXITEM cbItem;
					cbItem.mask = CBEIF_TEXT|CBEIF_IMAGE|CBEIF_SELECTEDIMAGE;
					for(int i=0; i<9; i++) {
						cbItem.iItem = i;
						cbItem.iImage = i;
						cbItem.iSelectedImage = i;
						cbItem.pszText = szLineTypeLabel[i];
						m_linesInPlaceComboBox.InsertItem( &cbItem );
					}
				}
				else
				{
					m_linesInPlaceComboBox.MoveWindow(&rcLineType);
				}

				m_linesInPlaceComboBox.SetCurSel( (int)pItem->GetLineType() );
				m_linesInPlaceComboBox.ShowWindow(SW_SHOW);
				m_linesInPlaceComboBox.BringWindowToTop();
				m_linesInPlaceComboBox.SetFocus();		
			}


			else if(pNMLW->iSubItem == 4) 
			{// color box column
				CRect rcColor;
				this->GetSubItemRect(nItem,4,LVIR_LABEL,rcColor);
				GetColorBoxRect(rcColor);
				if(rcColor.PtInRect(ptMsg)) 
				{
					CColorDialog dlg( pItem->GetColor(), CC_ANYCOLOR | CC_FULLOPEN, this);
					if(dlg.DoModal() == IDOK)
					{
						pItem->SetColor( dlg.GetColor() );
						GetParent()->SendMessage(VEHICLE_DISP_PROP_CHANGE1);
					}
				}
			}
			else if(pNMLW->iSubItem == 6)
			{ // density
				CDlgTracerDensity dlg(pItem->GetDensity()*100.0,this);
				if(dlg.DoModal() == IDOK) {
					pItem->SetDensity(dlg.GetDensity()/100.0);
				}
			}
			
			Invalidate();
		}
		*pResult = 0;
	}

	int CVehicleDispPropListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{
		if (CListCtrl::OnCreate(lpCreateStruct) == -1)
			return -1;

		// TODO: Add your specialized creation code here


		return 0;
	}

	LRESULT CVehicleDispPropListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
	{
		if(message == WM_INPLACE_COMBO)
		{
			if(GetItemCount()==0) return CListCtrl::DefWindowProc(message, wParam, lParam);
			int nIdx = (int) lParam;
			UINT nID = (UINT) wParam;

			if(nID == -1) { //line type
				if(m_nFlag_WM_INPLACE_COMBO == 3)
				{
					CVehicleDispPropItem* pItemLineType = (CVehicleDispPropItem*)GetItemData(m_nLineSelItem);
					if(pItemLineType)
						pItemLineType->SetLineType( ( CVehicleDispPropItem::ENUM_LINE_TYPE ) nIdx );
				}
				else if(m_nFlag_WM_INPLACE_COMBO == 0)
				{
					CVehicleDispPropItem* pItemVehicle = (CVehicleDispPropItem*)GetItemData(m_nVehicleItem);
					if(pItemVehicle)
					{
						CVehicleSpecificationItem * pSpeItem = const_cast<CVehicleSpecificationItem *>(pItemVehicle->GetVehicleType());
						if(pSpeItem)
							pSpeItem->DBElement::ReadData(m_vehicleTypeInPlaceComboBox.GetItemData(nIdx));
						GetParent()->SendMessage(VEHICLE_DISP_PROP_CHANGE1);
					}
				}
			}
			else { //shape, nID == setID
				CVehicleDispPropItem* pItem = (CVehicleDispPropItem*)GetItemData(m_nShapeSelItem);
				pItem->SetShape( std::pair<int, int>( nID, nIdx ) );
				GetParent()->SendMessage(VEHICLE_DISP_PROP_CHANGE1);
			} 
			SetFocus();
			return TRUE;
		}


		return CListCtrl::DefWindowProc(message, wParam, lParam);
	}





