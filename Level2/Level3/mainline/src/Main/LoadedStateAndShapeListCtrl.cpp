#include "StdAfx.h"
#include ".\loadedstateandshapelistctrl.h"
#include "../InputOnBoard/LoadedStateAndShapData.h"
#include "ShapesManager.h"
#include "..\InputOnBoard\PaxPhysicalCharacteristics.h"
#include "..\Common\WinMsg.h"

LoadedStateAndShapeListCtrl::LoadedStateAndShapeListCtrl(void)
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
	for(int nSet=0; nSet<SHAPESMANAGER->GetShapeSetCount(); nSet++) 
	{
		nTotalShapes+=SHAPESMANAGER->GetShapeCount(nSet);
	}

	m_shapesImageList.Create(10 , 10, ILC_COLOR8, 0, nTotalShapes);
	// load shape bitmap	
	for(int nSet=0; nSet<SHAPESMANAGER->GetShapeSetCount(); nSet++) 
	{
		for(int nShape=0; nShape<SHAPESMANAGER->GetShapeCount(nSet); nShape++) 
		{
			CBitmap* pBM = CBitmap::FromHandle(SHAPESMANAGER->GetShapeImgSmallBitmap(nSet, nShape));
			m_shapesImageList.Add(pBM, pBM);
		}
	}
}

LoadedStateAndShapeListCtrl::~LoadedStateAndShapeListCtrl(void)
{
}

BEGIN_MESSAGE_MAP(LoadedStateAndShapeListCtrl, CListCtrlEx)
	//{{AFX_MSG_MAP(CPaxProDispListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

void LoadedStateAndShapeListCtrl::ShowDropList(int nItem, int nCol)
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

//int LoadedStateAndShapeListCtrl::InsertColumn(int nCol, const LV_COLUMNEX* pColumn)
//{
//	if(ddStyleList.GetSize() <= nCol)
//		ddStyleList.SetSize(nCol + 1);
//	LVCOLDROPLIST* DropList = new LVCOLDROPLIST;
//
//	if(pColumn->fmt & LVCFMT_NOEDIT)
//		DropList->Style = NO_EDIT;
//	else if(pColumn->fmt & LVCFMT_DROPDOWN)
//		DropList->Style = DROP_DOWN;
//	else if(pColumn->fmt & LVCFMT_SPINEDIT)
//		DropList->Style = SPIN_EDIT;
//	else if(pColumn->fmt & LVCFMT_NUMBER)
//		DropList->Style = NUMBER_ONLY;
//	else if(pColumn->fmt & LVCFMT_DATETIME)
//		DropList->Style = DATE_TIME;
//	else if(pColumn->fmt & LVCFMT_POPUP_GATE_SELECTION)
//		DropList->Style = POPUP_GATE_SELECTION;
//	if( pColumn->fmt & LVCFMT_EDIT )
//		DropList->Style = TEXT_EDIT;
//	if (pColumn->fmt & LVCFMT_CHECK ) 
//		DropList->Style = CHECKBOX; 
//	if (pColumn->fmt & LVCFMT_BMP)
//		DropList->Style = BMP_EDIT;
//
//	DropList->List.AddTail(pColumn->csList);
//	DropList->strDateFormat = pColumn->pszDateFormat;
//	DropList->strShowDateFormat = pColumn->pszShowDateFormat;
//	ddStyleList[nCol] = (void*)DropList;
//	// Transfer the Relevent Fields to lvc and pass to base class
//	LV_COLUMN lvc;
//	memcpy(&lvc, pColumn, sizeof(tagLVCOLUMNA));
//	return CListCtrl::InsertColumn(nCol, &lvc);
//}

void LoadedStateAndShapeListCtrl::DrawItem( LPDRAWITEMSTRUCT lpdis)
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


	LoadedStateAndShapData* pItem = (LoadedStateAndShapData*)lvi.lParam;
	CString strName;
	strName.Format("  PACK%d",pItem->getPackID());
	pDC->DrawText( strName, rcLabel, DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP);

	std::vector<CarryonItemState> vDataList = pItem->getStateList();
	int nCount = vDataList.size();
	for (int i =0; i < nCount; i++)
	{
		CarryonItemState state = vDataList.at(i);
		CRect rcVisible;
		this->GetSubItemRect(nItem,(int)state.m_eItemType+1,LVIR_LABEL,rcVisible);
		CString strText;
		strText.Format("  %d",state.m_nItemCount);
		if (state.m_nItemCount)
		{
			pDC->DrawText( strText, rcVisible,  DT_VCENTER | DT_SINGLELINE );
		}
	}

	//draw shape
	CRect rcShape;
	this->GetSubItemRect( nItem,10,LVIR_LABEL,rcShape );
	int nShapeImageIndex = 0;
	for(int nSet=0; nSet< pItem->GetPaxShape()->getShape().first; nSet++) 
	{
		nShapeImageIndex += SHAPESMANAGER->GetShapeCount(nSet);
	}
	CPoint pt(rcShape.left+6, rcShape.top+2);
	m_shapesImageList.Draw( pDC, nShapeImageIndex + pItem->GetPaxShape()->getShape().second , pt, ILD_NORMAL );

	rcShape.left = rcShape.left + 10 + m_sizeCheck.cx;
	pDC->DrawText( SHAPESMANAGER->GetShapeName( pItem->GetPaxShape()->getShape().first, pItem->GetPaxShape()->getShape().second ), rcShape, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP );
	
	if (lvi.state & LVIS_FOCUSED) 
		pDC->DrawFocusRect(rcAllLabels); 

	if (bSelected) 
	{
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

LRESULT LoadedStateAndShapeListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_INPLACE_COMBO)
	{
		if(GetItemCount()==0) 
			return CListCtrl::DefWindowProc(message, wParam, lParam);

		int nIdx = (int) lParam;
		UINT nID = (UINT) wParam;

		LoadedStateAndShapData* pItem = (LoadedStateAndShapData*)GetItemData(m_nShapeSelItem);
		pItem->GetPaxShape()->setShape( std::pair<int, int>( nID, nIdx ) );

		SetFocus();
		return TRUE;
	}
	return CListCtrlEx::DefWindowProc(message, wParam, lParam);
}


void LoadedStateAndShapeListCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint ptMsg = GetMessagePos();
	this->ScreenToClient( &ptMsg );
	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
	int nItem = pNMLW->iItem;
	int nSubItem = pNMLW->iSubItem;
	if(nItem == -1 || nSubItem <10) 
		return;
	
	LoadedStateAndShapData* pItem = (LoadedStateAndShapData*)GetItemData(nItem);
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
	
	*pResult = 0;
}

void LoadedStateAndShapeListCtrl::InsertDataItem(int _nIdx, LoadedStateAndShapData* pItem )
{
	int nIdx = InsertItem( _nIdx, LPCTSTR(""));
	SetItemData( nIdx, (DWORD)pItem );
	ASSERT(nIdx == _nIdx);
}

int LoadedStateAndShapeListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrlEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	return 0;
}