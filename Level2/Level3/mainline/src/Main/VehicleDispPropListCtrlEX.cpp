#include "StdAfx.h"
#include ".\vehicledispproplistctrlex.h"

// CVehicleDispPropListCtrlEX.cpp : implementation file
//

#include "stdafx.h"
#include "VehicleDispPropListCtrlEX.h"

#include "DlgTracerDensity.h"
#include "PaxFilterDlg.h"
#include "PaxShapeDefs.h"
//#include "ShapesManager.h"
#include "../InputAirside/VehicleSpecifications.h"
#include "common\WinMsg.h"
#include "../Landside/LandSideVehicleProbDispDetail.h"
#include "../Landside/ShapeList.h"
#include "../landside/VehicleItemDetailList.h"
#include "../landside/LandsideVehicleTypeNode.h"
#include "DlgSelectLandsideVehicleType.h"
#include "common\WinMsg.h"
#include "DlgACScaleSize.h"
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

void CVehicleDispPropListCtrlEX::setDocument(CTermPlanDoc *tmpDoc)
	{
		pDoc=tmpDoc;
	}
CVehicleDispPropListCtrlEX::CVehicleDispPropListCtrlEX() :  m_linesInPlaceComboBox(-1),m_vehicleTypeInPlaceComboBox(-1)
	{
		CBitmap bitmap;
		VERIFY(bitmap.LoadBitmap(AFX_IDB_CHECKLISTBOX_95));
		BITMAP bm;
		bitmap.GetObject(sizeof(BITMAP), &bm);
		m_sizeCheck.cx = bm.bmWidth / 3;
		m_sizeCheck.cy = bm.bmHeight;
		m_hBitmapCheck = (HBITMAP) bitmap.Detach();


		//calculate the total number of shapes
		int nTotalShapes = CShapeList::getInstance()->getShapeCount();//shape set at 6 is vehicle
		m_shapesImageList.Create(10 , 10, ILC_COLOR8, 0, nTotalShapes);
		// load shape bitmap
		CBitmap* pBM;
		for(int nShape=0; nShape< nTotalShapes; nShape++) 
		{
		   pBM = CBitmap::FromHandle(CShapeList::getInstance()->GetShapeImgSmallBitmap( nShape,-1));
			m_shapesImageList.Add(pBM, pBM);
			for (int j=0;j<CShapeList::getInstance()->getShapeCount(nShape);j++)
			{
				pBM = CBitmap::FromHandle(CShapeList::getInstance()->GetShapeImgSmallBitmap( nShape, j));
				m_shapesImageList.Add(pBM, pBM);
			}
			
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

	}

CVehicleDispPropListCtrlEX::~CVehicleDispPropListCtrlEX()
	{
		if(m_hBitmapCheck != NULL)
			::DeleteObject(m_hBitmapCheck);
		//	if(m_hBitmapShape != NULL)
		//		::DeleteObject(m_hBitmapShape);
		//	for(int i=0;i< m_shapesImageList.GetImageCount();i++ )
		//		m_shapimagelist.Remove(i);
	}


	BEGIN_MESSAGE_MAP(CVehicleDispPropListCtrlEX, CListCtrl)
		//{{AFX_MSG_MAP(CVehicleDispPropListCtrlEX)
		ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
		ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
		ON_WM_CREATE()
		//}}AFX_MSG_MAP

	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CVehicleDispPropListCtrlEX message handlers
	/////////////////////////////////////////////////////////////////////////////
	// CVehicleDispPropListCtrlEX message handlers

void CVehicleDispPropListCtrlEX::DrawItem( LPDRAWITEMSTRUCT lpdis)
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

		CRect rcScalesize;
		this->GetSubItemRect(nItem, 7, LVIR_LABEL, rcScalesize);

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
		CLandSideVehicleProbDispDetail* pItem = (CLandSideVehicleProbDispDetail*)lvi.lParam;

		//draw Case name 
		if (pItem->getTypeName())
		{
			CString strName = pItem->getTypeName();
			pDC->DrawText(strName , rcLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP);

		}
		
		//draw Visible checkbox
		CDC bitmapDC;
		HBITMAP hOldBM;
		int nCheck = pItem->getVisible();
		if(bitmapDC.CreateCompatibleDC(pDC)) {
			hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);
			pDC->BitBlt(rcVisible.left, rcVisible.top, m_sizeCheck.cx, m_sizeCheck.cy, &bitmapDC,
				m_sizeCheck.cx * nCheck, 0, SRCCOPY);
			::SelectObject(bitmapDC.m_hDC, hOldBM);
			bitmapDC.DeleteDC();
		}

		//draw shape listcombox
		int nShapeImageIndex = 1;
		for(int nSet=0; nSet< pItem->getShape().first; nSet++) {
			nShapeImageIndex += CShapeList::getInstance()->getShapeCount(nSet)+1;
		}
		
		CPoint pt(rcShape.left+6, rcShape.top+2);
		int nCount = nShapeImageIndex+pItem->getShape().second;
		m_shapesImageList.Draw( pDC,nCount, pt, ILD_MASK );

		rcShape.left = rcShape.left + 10 + m_sizeCheck.cx;
		pDC->DrawText( CShapeList::getInstance()->GetShapeName( pItem->getShape().first, pItem->getShape().second ), rcShape, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP );

		//draw line listcombox
		CPoint pt2(rcLineType.left+10, rcLineType.top+2);
		m_linesImageList.Draw( pDC, pItem->getLineType(),pt2,ILD_NORMAL );
		rcLineType.left = rcLineType.left + 69 + m_sizeCheck.cx + 5;
		pDC->DrawText( szLineTypeLabel[pItem->getLineType()], rcLineType, DT_LEFT | DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP );


		//draw color box
		pDC->FillSolidRect( rcColor,pItem->getColor() );
		pDC->FrameRect( rcColor,&CBrush(::GetSysColor(COLOR_BTNTEXT)));

		//draw checkbox
		nCheck =  pItem->getTail();
		if(bitmapDC.CreateCompatibleDC(pDC)) {
			hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);
			pDC->BitBlt(rcLev.left, rcLev.top, m_sizeCheck.cx, m_sizeCheck.cy, &bitmapDC,
				m_sizeCheck.cx * nCheck, 0, SRCCOPY);
			::SelectObject(bitmapDC.m_hDC, hOldBM);
			bitmapDC.DeleteDC();
		}

		// draw density label
		CString sDensity;
		sDensity.Format("%.2f%%", pItem->getTraceDensity()*100.0);
		pDC->DrawText(sDensity , rcDensity, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP);

		// draw scale size
		CString sScaleSize;
		sScaleSize.Format("x%d",(int)pItem->GetScaleSize());
		pDC->DrawText(sScaleSize , rcScalesize, DT_CENTER | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP);

		if (lvi.state & LVIS_FOCUSED) 
			pDC->DrawFocusRect(rcAllLabels); 

		if (bSelected) {
			pDC->SetTextColor(clrTextSave);
			pDC->SetBkColor(clrBkSave);
		}
	}

void CVehicleDispPropListCtrlEX::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		CPoint ptMsg = GetMessagePos();
		this->ScreenToClient( &ptMsg );
		NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
		int nItem = pNMLW->iItem;
		if(nItem != -1) 
		{
			CLandSideVehicleProbDispDetail* pItem = (CLandSideVehicleProbDispDetail*)GetItemData(nItem);
			if(pNMLW->iSubItem == 1) 
			{// visible column
				CRect rcVisible;
				this->GetSubItemRect(nItem,1,LVIR_LABEL,rcVisible);
				GetVisibleRect(rcVisible);
				if(rcVisible.PtInRect(ptMsg))
				{
					pItem->setVisible( !pItem->getVisible() );
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
					if( !pItem->getTail() )
						pItem->setLeaveTail( 1 );
					else
						pItem->setLeaveTail( 0 );
				}
			}
			Invalidate();
		}

		*pResult = 0;
	}

void CVehicleDispPropListCtrlEX::GetColorBoxRect(CRect& r)
	{
		//	r.DeflateRect(CSize(0,1));
		r.right = r.CenterPoint().x+r.Width()/4;
		r.left = r.CenterPoint().x-r.Width()/4;
		r.DeflateRect(3,2,3,2);
	}

void CVehicleDispPropListCtrlEX::GetVisibleRect(CRect& r)
	{
		r.left = (r.left+r.right-m_sizeCheck.cx)/2;
		r.top = (r.bottom+r.top-m_sizeCheck.cy)/2;
		r.bottom = r.top+m_sizeCheck.cy;
		r.right = r.left+m_sizeCheck.cx;
	}


	


void CVehicleDispPropListCtrlEX::InsertPaxDispItem(int _nIdx, CLandSideVehicleProbDispDetail* _pItem )
	{
		int nIdx = InsertItem(_nIdx,LPCTSTR(_pItem->getTypeName()) );
		SetItemData( nIdx, (DWORD)_pItem );
		//ASSERT(nIdx == _nIdx);
	}

void CVehicleDispPropListCtrlEX::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		// TODO: Add your control notification handler code here
		CPoint ptMsg = GetMessagePos();
		this->ScreenToClient( &ptMsg );
		NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
		int nItem = pNMLW->iItem;
		if(nItem != -1) 
		{
			CLandSideVehicleProbDispDetail* pItem = (CLandSideVehicleProbDispDetail*)GetItemData(nItem);
			m_nFlag_WM_INPLACE_COMBO = -1;
			if(pNMLW->iSubItem == 0) 
			{// vehicle Name column		
				if (pItem->getTypeName() == "DEFAULT" || pItem->getTypeName() == "OVERLAP")
					return;
				CDlgSelectLandsideVehicleType dlg;
				if(dlg.DoModal() == IDOK)
				{	
					CString strName;
					strName = dlg.GetName();
					if (strName.IsEmpty())
					{
						MessageBox(_T("Please define vehicle type name in Vehicle Type Definition Dialog!."),_T("Warning"),MB_OK);
					}
					/*else if (m_landsideVehicleGroupProperties.ExistSameName(strName))
					{
						MessageBox(_T("Exist the same group name!."),_T("Warning"),MB_OK);
					}*/
					pItem->setVehicleTypeName(strName);
				}
			}
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
				
				m_bbDropList.SetShapeSetIndex(-1);
				if (!m_bbDropList.GetSafeHwnd())
					m_bbDropList.Create(rcShape,IDC_COMBO_LEVEL,this) ;
				else
					m_bbDropList.SetWindowPos(NULL,rcShape.left,rcShape.top,rcShape.Width(),rcShape.Height(),SWP_NOACTIVATE|SWP_NOZORDER|SWP_SHOWWINDOW);
				m_bbDropList.DisplayListWnd() ;
				
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

				m_linesInPlaceComboBox.SetCurSel( (int)pItem->getLineType() );
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
					CColorDialog dlg( pItem->getColor(), CC_ANYCOLOR | CC_FULLOPEN, this);
					if(dlg.DoModal() == IDOK)
					{
						pItem->setColor( dlg.GetColor() );
						GetParent()->SendMessage(VEHICLE_DISP_PROP_CHANGE1);
					}
				}
			}
			else if(pNMLW->iSubItem == 6)
			{ // density
				CDlgTracerDensity dlg(pItem->getTraceDensity()*100.0,this);
				if(dlg.DoModal() == IDOK) {
					pItem->setTraceDensity(dlg.GetDensity()/100.0);
				}
			}
			else if(pNMLW->iSubItem == 7) 
			{
				CDlgACScaleSize dlg(this,1000);
				dlg.SetScale(pItem->GetScaleSize());
				if(dlg.DoModal() == IDOK)
				{
					pItem->SetScaleSize(dlg.GetScale());
					GetParent()->SendMessage(VEHICLE_DISP_PROP_CHANGE1);
				}
			}
			Invalidate();
		}
		*pResult = 0;
	}

	int CVehicleDispPropListCtrlEX::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{
		if (CListCtrl::OnCreate(lpCreateStruct) == -1)
			return -1;

		// TODO: Add your specialized creation code here


		return 0;
	}

LRESULT CVehicleDispPropListCtrlEX::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
	{
		if(message == WM_INPLACE_COMBO)
		{
			if(GetItemCount()==0) return CListCtrl::DefWindowProc(message, wParam, lParam);
			int nIdx = (int) lParam;
			UINT nID = (UINT) wParam;

			if(nID == -1) { //line type
				if(m_nFlag_WM_INPLACE_COMBO == 3)
				{
					CLandSideVehicleProbDispDetail* pItemLineType = (CLandSideVehicleProbDispDetail*)GetItemData(m_nLineSelItem);
					if(pItemLineType)
						pItemLineType->setLineType( ( CLandSideVehicleProbDispDetail::ENUM_LINE_TYPE ) nIdx );
				}
				else if(m_nFlag_WM_INPLACE_COMBO == 0)
				{
					CLandSideVehicleProbDispDetail* pItemVehicle = (CLandSideVehicleProbDispDetail*)GetItemData(m_nVehicleItem);
					/*CVehicleItemDetail *tmpDetail=(CVehicleItemDetail*)m_vehicleTypeInPlaceComboBox.GetItemData(nID);
					pItemVehicle->setVehicleTypeID(tmpDetail->GetID());
					pItemVehicle->setVehicleTypeName(tmpDetail->getName());
					pItemVehicle->SetVehicleItemObj(*tmpDetail);*/
				//	if(pItemVehicle)
				//	{
					//	
						//CVehicleItemDetail * pSpeItem = const_cast<CVehicleItemDetail *>(pItemVehicle->getVehicleItemObj());
					//	if(pSpeItem)
					//		pSpeItem->ReadData(m_vehicleTypeInPlaceComboBox.GetItemData(nIdx),NULL);
					
						//pItemVehicle->SetVehicleItemObj(*pSpeItem);
						pItemVehicle->setVehicleTypeName(pItemVehicle->getTypeName());
						pItemVehicle->setVehicleTypeID(pItemVehicle->getTypeID());
						GetParent()->SendMessage(VEHICLE_DISP_PROP_CHANGE1);
					//}
				}
			}
			else { //shape, nID == setID
				CLandSideVehicleProbDispDetail* pItem = (CLandSideVehicleProbDispDetail*)GetItemData(m_nShapeSelItem);
				nIdx = nIdx < 0 ? 0 : nIdx;
				pItem->setShape( std::pair<int, int>( nID, nIdx ) );
				GetParent()->SendMessage(VEHICLE_DISP_PROP_CHANGE1);
			} 
			SetFocus();
			return TRUE;
		}


		return CListCtrl::DefWindowProc(message, wParam, lParam);
	}





