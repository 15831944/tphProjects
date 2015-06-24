// PaxDistListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ACDispListCtrl.h"

#include "AircraftDispPropItem.h"
#include "DlgTracerDensity.h"
#include "PaxFilterDlg.h"
#include "PaxShapeDefs.h"
#include "FlightType.h"
#include "ShapesManager.h"
#include "common\WinMsg.h"
#include "DlgACScaleSize.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define IDC_BUTTON_DOWNLIST 0

static char szLineTypeLabel[][64] = {
	"solid1", "solid2", "solid3",
		"dash1", "dash2", "dash3",
		"dot1", "dot2", "dot3" };


/////////////////////////////////////////////////////////////////////////////
// CPaxDispListCtrl

CACDispListCtrl::CACDispListCtrl() :  m_linesInPlaceComboBox(-1)
{
	CBitmap bitmap;
	VERIFY(bitmap.LoadBitmap(AFX_IDB_CHECKLISTBOX_95));
	BITMAP bm;
	bitmap.GetObject(sizeof(BITMAP), &bm);
	m_sizeCheck.cx = bm.bmWidth / 3;
	m_sizeCheck.cy = bm.bmHeight;
	m_hBitmapCheck = (HBITMAP) bitmap.Detach();


	//calculate the total number of shapes
	//int nTotalShapes = 0;
	//for(int nSet=0; nSet<SHAPESMANAGER->GetShapeSetCount(); nSet++) {
	//	nTotalShapes+=SHAPESMANAGER->GetShapeCount(nSet);
	//}

	//m_shapesImageList.Create(10 , 10, ILC_COLOR8, 0, nTotalShapes);
	//// load shape bitmap	
	//for(int nSet=0; nSet<SHAPESMANAGER->GetShapeSetCount(); nSet++) {
	//	for(int nShape=0; nShape<SHAPESMANAGER->GetShapeCount(nSet); nShape++) {
	//		CBitmap* pBM = CBitmap::FromHandle(SHAPESMANAGER->GetShapeImgSmallBitmap(nSet, nShape));
	//		m_shapesImageList.Add(pBM, pBM);
	//	}
	//}

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

CACDispListCtrl::~CACDispListCtrl()
{
	if(m_hBitmapCheck != NULL)
		::DeleteObject(m_hBitmapCheck);
}

BEGIN_MESSAGE_MAP(CACDispListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaxDispListCtrl message handlers
/////////////////////////////////////////////////////////////////////////////
// CLayerListCtrl message handlers

void CACDispListCtrl::DrawItem( LPDRAWITEMSTRUCT lpdis)
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
	CRect rcColor;
	this->GetSubItemRect(nItem,1,LVIR_LABEL,rcColor);
	GetColorBoxRect(rcColor);
	CRect rcLogOnOff;
	this->GetSubItemRect(nItem,2,LVIR_LABEL,rcLogOnOff);
	GetVisibleRect(rcLogOnOff);
	CRect rcLineType;
	this->GetSubItemRect( nItem,3,LVIR_LABEL,rcLineType );
	CRect rcVisible;
	this->GetSubItemRect(nItem,4,LVIR_LABEL,rcVisible);
	GetVisibleRect(rcVisible);
	/*CRect rcShape;
	this->GetSubItemRect( nItem,2,LVIR_LABEL,rcShape );*/
	
	/*CRect rcLev;
	this->GetSubItemRect(nItem,5,LVIR_LABEL,rcLev);
	GetVisibleRect(rcLev);*/
	CRect rcDensity;
	this->GetSubItemRect(nItem, 5, LVIR_LABEL, rcDensity);

	CRect rcScalesize;
	this->GetSubItemRect(nItem, 6, LVIR_LABEL, rcScalesize);
	
	/*CRect rcDesc;
	this->GetSubItemRect( nItem,7,LVIR_LABEL,rcDesc );*/

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
	CAircraftDispPropItem* pItem = (CAircraftDispPropItem*)lvi.lParam;

	//draw Case name
	const CFlightType* pPT=pItem->GetFlightType();
	CString strName=pPT->GetName();
	pDC->DrawText(strName , rcLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP);

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

	////draw shape listcombox
	//CPoint pt(rcShape.left+6, rcShape.top+2);
	//m_shapesImageList.Draw( pDC, SHAPESMANAGER->SetAndShapeToIdx( pItem->GetShape().first, pItem->GetShape().second ), pt, ILD_NORMAL );

	//rcShape.left = rcShape.left + 10 + m_sizeCheck.cx;
	//pDC->DrawText( SHAPESMANAGER->GetShapeName( pItem->GetShape().first, pItem->GetShape().second ), rcShape, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP );

	//draw line listcombox
	CPoint pt2(rcLineType.left+10, rcLineType.top+2);
	m_linesImageList.Draw( pDC, pItem->GetLineType(),pt2,ILD_NORMAL );
	rcLineType.left = rcLineType.left + 69 + m_sizeCheck.cx + 5;
	pDC->DrawText( szLineTypeLabel[pItem->GetLineType()], rcLineType, DT_LEFT | DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP );
	
	//draw Log on/off chechbox
	//CDC bitmapDC;
	//HBITMAP hOldBM;
	nCheck = (int)pItem->IsLogoOn();
	if(bitmapDC.CreateCompatibleDC(pDC)) {
		hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);
		pDC->BitBlt(rcLogOnOff.left, rcLogOnOff.top, m_sizeCheck.cx, m_sizeCheck.cy, &bitmapDC,
			m_sizeCheck.cx * nCheck, 0, SRCCOPY);
		::SelectObject(bitmapDC.m_hDC, hOldBM);
		bitmapDC.DeleteDC();
	}

	//draw color box
	pDC->FillSolidRect( rcColor,pItem->GetColor() );
	pDC->FrameRect( rcColor,&CBrush(::GetSysColor(COLOR_BTNTEXT)));

	//draw checkbox
	/*nCheck = (int) pItem->IsLeaveTrail();
	if(bitmapDC.CreateCompatibleDC(pDC)) {
		hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);
		pDC->BitBlt(rcLev.left, rcLev.top, m_sizeCheck.cx, m_sizeCheck.cy, &bitmapDC,
			m_sizeCheck.cx * nCheck, 0, SRCCOPY);
		::SelectObject(bitmapDC.m_hDC, hOldBM);
		bitmapDC.DeleteDC();
	}*/

	 //draw density label
	CString sDensity;
	sDensity.Format("%.0f%%", pItem->GetDensity()*100.0);
	pDC->DrawText(sDensity , rcDensity, DT_CENTER | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP);

	// draw description
//	pDC->DrawText( pItem->GetFlightType()->ScreenPrint(), rcDesc, DT_LEFT | DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP);
	//draw scale size 
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

void CACDispListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint ptMsg = GetMessagePos();
	this->ScreenToClient( &ptMsg );
	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
	int nItem = pNMLW->iItem;
	if(nItem != -1) 
	{
		CAircraftDispPropItem* pItem = (CAircraftDispPropItem*)GetItemData(nItem);
		if(pNMLW->iSubItem == 4) 
		{// visible column
			CRect rcVisible;
			this->GetSubItemRect(nItem,4,LVIR_LABEL,rcVisible);
			GetVisibleRect(rcVisible);
			if(rcVisible.PtInRect(ptMsg))
			{
				pItem->SetVisible( !pItem->IsVisible() );
				GetParent()->SendMessage(AC_DISP_PROP_CHANGE1);
			}
		}
		else if(pNMLW->iSubItem == 2) 
		{// lev column
			CRect rcLogOnOff;
			this->GetSubItemRect(nItem,2,LVIR_LABEL,rcLogOnOff);
			GetVisibleRect(rcLogOnOff);
			if(rcLogOnOff.PtInRect(ptMsg)) 
			{
				pItem->SetLogoOn(!pItem->IsLogoOn());
				GetParent()->SendMessage(AC_DISP_PROP_CHANGE1);
			}
		}
		Invalidate();
	}

	*pResult = 0;
}

void CACDispListCtrl::GetColorBoxRect(CRect& r)
{
	//	r.DeflateRect(CSize(0,1));
	r.right = r.CenterPoint().x+r.Width()/4;
	r.left = r.CenterPoint().x-r.Width()/4;
	r.DeflateRect(3,2,3,2);
}

void CACDispListCtrl::GetVisibleRect(CRect& r)
{
	r.left = (r.left+r.right-m_sizeCheck.cx)/2;
	r.top = (r.bottom+r.top-m_sizeCheck.cy)/2;
	r.bottom = r.top+m_sizeCheck.cy;
	r.right = r.left+m_sizeCheck.cx;
}

void CACDispListCtrl::InsertACDispItem(int _nIdx, CAircraftDispPropItem* _pItem )
{
	int nIdx = InsertItem( _nIdx, LPCTSTR(_pItem->GetFlightType()->GetName()) );
	SetItemData( nIdx, (DWORD)_pItem );
	ASSERT(nIdx == _nIdx);
}


void CACDispListCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint ptMsg = GetMessagePos();
	this->ScreenToClient( &ptMsg );
	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
	int nItem = pNMLW->iItem;
	if(nItem != -1) 
	{
		CAircraftDispPropItem* pItem = (CAircraftDispPropItem*)GetItemData(nItem);
		if(pNMLW->iSubItem == 3) 
		{
			// line type column
			CRect rcLineType;
			this->GetSubItemRect( nItem, 3, LVIR_LABEL, rcLineType);

			int nHeight = rcLineType.Height();
			rcLineType.bottom += nHeight * 5;

			m_nLineSelItem = nItem;
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


		else if(pNMLW->iSubItem == 1) 
		{// color box column
			CRect rcColor;
			this->GetSubItemRect(nItem,1,LVIR_LABEL,rcColor);
			GetColorBoxRect(rcColor);
			if(rcColor.PtInRect(ptMsg)) 
			{
				CColorDialog dlg( pItem->GetColor(), CC_ANYCOLOR | CC_FULLOPEN, this);
				if(dlg.DoModal() == IDOK)
				{
					pItem->SetColor( dlg.GetColor() );
					GetParent()->SendMessage(AC_DISP_PROP_CHANGE1);
				}
			}
		}
		else if(pNMLW->iSubItem == 5)
		{ // density
			CDlgTracerDensity dlg(pItem->GetDensity()*100.0,this);
			if(dlg.DoModal() == IDOK) {
				pItem->SetDensity((float)dlg.GetDensity()/100.0f);
			}
		}
		else if(pNMLW->iSubItem == 6) 
		{
			CDlgACScaleSize dlg(this,20);
			dlg.SetScale(pItem->GetScaleSize());
			if(dlg.DoModal() == IDOK)
			{
				pItem->SetScaleSize(dlg.GetScale());
				GetParent()->SendMessage(AC_DISP_PROP_CHANGE1);
			}
		}
		Invalidate();
	}
	*pResult = 0;
}

LRESULT CACDispListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message == WM_INPLACE_COMBO)
	{
		if(GetItemCount()==0) return CListCtrl::DefWindowProc(message, wParam, lParam);
		int nIdx = (int) lParam;
		UINT nID = (UINT) wParam;

		if(nID == -1) { //line type
			CAircraftDispPropItem* pItem = (CAircraftDispPropItem*)GetItemData(m_nLineSelItem);
			pItem->SetLineType( ( CAircraftDispPropItem::ENUM_LINE_TYPE ) nIdx );
		}
		//else { //shape, nID == setID
		//	CAircraftDispPropItem* pItem = (CAircraftDispPropItem*)GetItemData(m_nShapeSelItem);
		//	pItem->SetShape( std::pair<int, int>( nID, nIdx ) );
		//}
		SetFocus();
		return TRUE;
	}

	return CListCtrl::DefWindowProc(message, wParam, lParam);
}