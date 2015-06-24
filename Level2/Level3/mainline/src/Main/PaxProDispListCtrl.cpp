#include "stdafx.h"
#include "PaxProDispListCtrl.h"
#include "../InputOnBoard/PaxPhysicalCharacteristics.h"
#include "ShapesManager.h"
#include "common\WinMsg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
CPaxProDispListCtrl::CPaxProDispListCtrl()
{
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

CPaxProDispListCtrl::~CPaxProDispListCtrl()
{

}

BEGIN_MESSAGE_MAP(CPaxProDispListCtrl, CListCtrlEx)
	//{{AFX_MSG_MAP(CPaxProDispListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

void CPaxProDispListCtrl::ShowDropList(int nItem, int nCol)
{
	CRect rcShape;
	this->GetSubItemRect( nItem, nCol, LVIR_LABEL, rcShape);
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

int CPaxProDispListCtrl::InsertColumn(int nCol, const LV_COLUMNEX* pColumn)
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

void CPaxProDispListCtrl::DrawItem( LPDRAWITEMSTRUCT lpdis)
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
	CRect rcVisible;
	this->GetSubItemRect(nItem,1,LVIR_LABEL,rcVisible);
	CRect rcVisible1;
	this->GetSubItemRect( nItem,2,LVIR_LABEL,rcVisible1 );
	CRect rcVisible2;
	this->GetSubItemRect( nItem,3,LVIR_LABEL,rcVisible2 );
	CRect rcVisible3;
	this->GetSubItemRect(nItem,4,LVIR_LABEL,rcVisible3);
	CRect rcVisible4;
	this->GetSubItemRect(nItem,5,LVIR_LABEL,rcVisible4);
	CRect rcVisible5;
	this->GetSubItemRect(nItem, 6, LVIR_LABEL, rcVisible5);
	CRect rcVisible6;
	this->GetSubItemRect( nItem,7,LVIR_LABEL,rcVisible6 );
	PaxPhysicalCharatieristicsItem* pItem = (PaxPhysicalCharatieristicsItem*)lvi.lParam;

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
	
	pDC->DrawText( pItem->getMobileElemCnt().PrintStringForShow(), rcLabel, DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP);
	pDC->DrawText( pItem->getMajor().getPrintDist(), rcVisible,  DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP)  
	pDC->DrawText( pItem->getMinor().getPrintDist(), rcVisible1, DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP)  ;
	pDC->DrawText( pItem->getHeight().getPrintDist(), rcVisible2, DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP);
	pDC->DrawText( pItem->getUppReach().getPrintDist(), rcVisible3, DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP);
	pDC->DrawText( pItem->getArmLength().getPrintDist(), rcVisible4, DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP);
	pDC->DrawText( pItem->getFloorReach().getPrintDist(), rcVisible5,DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP);
	pDC->DrawText( pItem->getBackReach().getPrintDist(), rcVisible6, DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP);
	
	
	//draw shape listcombox
	for (int i = 0; i < (int)pItem->m_val.size(); i++)
	{
		CRect rcShape;
		this->GetSubItemRect( nItem,i+8,LVIR_LABEL,rcShape );
		int nShapeImageIndex = 0;
		for(int nSet=0; nSet< pItem->m_val.at(i).getShape().first; nSet++) {
			nShapeImageIndex += SHAPESMANAGER->GetShapeCount(nSet);
		}
		CPoint pt(rcShape.left+6, rcShape.top+2);
		m_shapesImageList.Draw( pDC, nShapeImageIndex + pItem->m_val.at(i).getShape().second , pt, ILD_NORMAL );

		rcShape.left = rcShape.left + 10 + m_sizeCheck.cx;
		pDC->DrawText( SHAPESMANAGER->GetShapeName( pItem->m_val.at(i).getShape().first, pItem->m_val.at(i).getShape().second ), rcShape, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP );
	}

	if (lvi.state & LVIS_FOCUSED) 
		pDC->DrawFocusRect(rcAllLabels); 

	if (bSelected) {
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

LRESULT CPaxProDispListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_INPLACE_COMBO)
	{
		if(GetItemCount()==0) return CListCtrl::DefWindowProc(message, wParam, lParam);
		int nIdx = (int) lParam;
		UINT nID = (UINT) wParam;

		PaxPhysicalCharatieristicsItem* pItem = (PaxPhysicalCharatieristicsItem*)GetItemData(m_nShapeSelItem);
		pItem->m_val.at(m_nShapeSelSubItem - 8).setShape( std::pair<int, int>( nID, nIdx ) );

		SetFocus();
		return TRUE;
	}
	return CListCtrlEx::DefWindowProc(message, wParam, lParam);
}


void CPaxProDispListCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint ptMsg = GetMessagePos();
	this->ScreenToClient( &ptMsg );
	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
	int nItem = pNMLW->iItem;
	int nSubItem = pNMLW->iSubItem;
	if(nItem != -1 && nSubItem > 7) 
	{
		PaxPhysicalCharatieristicsItem* pItem = (PaxPhysicalCharatieristicsItem*)GetItemData(nItem);
		CRect rcShape;
		this->GetSubItemRect( nItem, nSubItem, LVIR_LABEL, rcShape);
		m_nShapeSelItem = nItem;
		m_nShapeSelSubItem = nSubItem;
		rcShape.left +=(rcShape.Width()-16 );
		rcShape.bottom =rcShape.top +18;
		rcShape.right =rcShape.left +16;

		if (!m_bbDropList.GetSafeHwnd())
			m_bbDropList.Create(rcShape,IDC_COMBO_LEVEL,this) ;
		else
			m_bbDropList.SetWindowPos(NULL,rcShape.left,rcShape.top,rcShape.Width(),rcShape.Height(),SWP_NOACTIVATE|SWP_NOZORDER|SWP_SHOWWINDOW);
		m_bbDropList.DisplayListWnd() ;

		Invalidate();
	}
	*pResult = 0;
}

void CPaxProDispListCtrl::InsertPaxDispItem(int _nIdx, PaxPhysicalCharatieristicsItem* _pItem )
{
	int nIdx = InsertItem( _nIdx, LPCTSTR(""));
	SetItemData( nIdx, (DWORD)_pItem );
	ASSERT(nIdx == _nIdx);
}

int CPaxProDispListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrlEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	return 0;
}
