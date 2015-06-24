// CabinCrewDispListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "CabinCrewDispListCtrl.h"
#include "../InputOnBoard/CabinCrewGeneration.h"
#include "ShapesManager.h"
#include "common\WinMsg.h"
#include "../InputOnBoard/PaxPhysicalCharacteristics.h"
#include "../Common/ProbabilityDistribution.h"

CCabinCrewDispListCtrl::CCabinCrewDispListCtrl()
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

CCabinCrewDispListCtrl::~CCabinCrewDispListCtrl()
{
}


BEGIN_MESSAGE_MAP(CCabinCrewDispListCtrl, CListCtrlEx)
	//{{AFX_MSG_MAP(CPaxProDispListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCabinCrewDispListCtrl::ShowDropList(int nItem, int nCol)
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

int CCabinCrewDispListCtrl::InsertColumn(int nCol, const LV_COLUMNEX* pColumn)
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

void CCabinCrewDispListCtrl::DrawItem( LPDRAWITEMSTRUCT lpdis)
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



	CRect rcLabel, rcLabel1, rcLabel2, rcLabel3, rcLabel4; 
	this->GetItemRect(nItem, rcLabel, LVIR_LABEL);
	this->GetSubItemRect(nItem,1,LVIR_LABEL,rcLabel1);
	this->GetSubItemRect( nItem,2,LVIR_LABEL,rcLabel2);
	this->GetSubItemRect( nItem,3,LVIR_LABEL,rcLabel3);
	this->GetSubItemRect(nItem,4,LVIR_LABEL,rcLabel4);
	CRect rcVisible;
	this->GetSubItemRect(nItem,5,LVIR_LABEL,rcVisible);
	CRect rcVisible1;
	this->GetSubItemRect( nItem,6,LVIR_LABEL,rcVisible1 );
	CRect rcVisible2;
	this->GetSubItemRect( nItem,7,LVIR_LABEL,rcVisible2 );
	CRect rcVisible3;
	this->GetSubItemRect(nItem,8,LVIR_LABEL,rcVisible3);
	CRect rcVisible4;
	this->GetSubItemRect(nItem,9,LVIR_LABEL,rcVisible4);
	CRect rcVisible5;
	this->GetSubItemRect(nItem, 10, LVIR_LABEL, rcVisible5);
	CRect rcVisible6;
	this->GetSubItemRect( nItem,11,LVIR_LABEL,rcVisible6 );

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

	CabinCrewPhysicalCharacteristics* pItem = (CabinCrewPhysicalCharacteristics*)lvi.lParam;
	CString strText = "  ";
	pDC->DrawText(strText + pItem->getRank(), rcLabel, DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP);


	strText.Format("  %d", pItem->getID());
	pDC->DrawText( strText, rcLabel1,  DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP)  
	if (pItem->getSex() == Female)
		strText = "  F";
	else
		strText = "  M";
	pDC->DrawText( strText, rcLabel2,  DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP)  

	strText.Format("  %d", pItem->getVisibility());
	pDC->DrawText( strText, rcLabel3,  DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP)  

	if (pItem->getAwareness() == YES)
		strText = "  Yes";
	else
		strText = "  No";
	pDC->DrawText( strText, rcLabel4,  DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP)

	strText = "  ";
	pDC->DrawText( strText + pItem->getMajor()->getPrintDist(), rcVisible,  DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP)  
	pDC->DrawText( strText + pItem->getMinor()->getPrintDist(), rcVisible1, DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP)  ;
	pDC->DrawText( strText + pItem->getHeight()->getPrintDist(), rcVisible2, DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP);
	pDC->DrawText( strText + pItem->getUppReach()->getPrintDist(), rcVisible3, DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP);
	pDC->DrawText( strText + pItem->getArmLength()->getPrintDist(), rcVisible4, DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP);
	pDC->DrawText( strText + pItem->getFloorReach()->getPrintDist(), rcVisible5,DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP);
	pDC->DrawText( strText + pItem->getBackReach()->getPrintDist(), rcVisible6, DT_VCENTER | DT_SINGLELINE );// | DT_NOCLIP);


	//draw shape list combo
	CRect rcShape;
	this->GetSubItemRect( nItem,12,LVIR_LABEL,rcShape );		//upright shape
	int nShapeImageIndex = 0;
	for(int nSet=0; nSet< pItem->getUprightShape()->getShape().first; nSet++) 
		nShapeImageIndex += SHAPESMANAGER->GetShapeCount(nSet);

	CPoint pt(rcShape.left+6, rcShape.top+2);
	m_shapesImageList.Draw( pDC, nShapeImageIndex + pItem->getUprightShape()->getShape().second , pt, ILD_NORMAL );

	rcShape.left = rcShape.left + 10 + m_sizeCheck.cx;
	pDC->DrawText( SHAPESMANAGER->GetShapeName( pItem->getUprightShape()->getShape().first, pItem->getUprightShape()->getShape().second ), rcShape, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP );


	this->GetSubItemRect( nItem,13,LVIR_LABEL,rcShape );		//sit shape
	nShapeImageIndex = 0;
	for(int nSet=0; nSet< pItem->getSitShape()->getShape().first; nSet++) 
		nShapeImageIndex += SHAPESMANAGER->GetShapeCount(nSet);

	CPoint pt1(rcShape.left+6, rcShape.top+2);
	m_shapesImageList.Draw( pDC, nShapeImageIndex + pItem->getSitShape()->getShape().second , pt1, ILD_NORMAL );

	rcShape.left = rcShape.left + 10 + m_sizeCheck.cx;
	pDC->DrawText( SHAPESMANAGER->GetShapeName( pItem->getSitShape()->getShape().first, pItem->getSitShape()->getShape().second ), rcShape, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP );


	this->GetSubItemRect( nItem,14,LVIR_LABEL,rcShape );		//bend shape
	nShapeImageIndex = 0;
	for(int nSet=0; nSet< pItem->getBendShape()->getShape().first; nSet++) 
		nShapeImageIndex += SHAPESMANAGER->GetShapeCount(nSet);

	CPoint pt2(rcShape.left+6, rcShape.top+2);
	m_shapesImageList.Draw( pDC, nShapeImageIndex + pItem->getBendShape()->getShape().second , pt2, ILD_NORMAL );

	rcShape.left = rcShape.left + 10 + m_sizeCheck.cx;
	pDC->DrawText( SHAPESMANAGER->GetShapeName( pItem->getBendShape()->getShape().first, pItem->getBendShape()->getShape().second ), rcShape, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP );


	this->GetSubItemRect( nItem,15,LVIR_LABEL,rcShape );		//Reach shape
	nShapeImageIndex = 0;
	for(int nSet=0; nSet< pItem->getReachShape()->getShape().first; nSet++) 
		nShapeImageIndex += SHAPESMANAGER->GetShapeCount(nSet);

	CPoint pt3(rcShape.left+6, rcShape.top+2);
	m_shapesImageList.Draw( pDC, nShapeImageIndex + pItem->getReachShape()->getShape().second , pt3, ILD_NORMAL );

	rcShape.left = rcShape.left + 10 + m_sizeCheck.cx;
	pDC->DrawText( SHAPESMANAGER->GetShapeName( pItem->getReachShape()->getShape().first, pItem->getReachShape()->getShape().second ), rcShape, DT_LEFT | DT_VCENTER | DT_SINGLELINE );//| DT_NOCLIP );


	if (lvi.state & LVIS_FOCUSED) 
		pDC->DrawFocusRect(rcAllLabels); 

	if (bSelected) {
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

LRESULT CCabinCrewDispListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_INPLACE_COMBO)
	{
		if(GetItemCount()==0) return CListCtrl::DefWindowProc(message, wParam, lParam);
		int nIdx = (int) lParam;
		UINT nID = (UINT) wParam;

		CabinCrewPhysicalCharacteristics* pItem = (CabinCrewPhysicalCharacteristics*)GetItemData(m_nShapeSelItem);
		int nRow = m_nShapeSelSubItem - 12;
		switch(nRow)
		{
		case 0:
			pItem->setUprightShape( std::pair<int, int>( nID, nIdx ));
			break;
		case 1:
			pItem->setSitShape( std::pair<int, int>( nID, nIdx ));
			break;
		case 2:
			pItem->setBendShape( std::pair<int, int>( nID, nIdx ));
		    break;
		case 3:
			pItem->setReachShape( std::pair<int, int>( nID, nIdx ));
		    break;
		default:
		    break;
		}
		SetFocus();
		return TRUE;
	}
	return CListCtrlEx::DefWindowProc(message, wParam, lParam);
}


void CCabinCrewDispListCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint ptMsg = GetMessagePos();
	this->ScreenToClient( &ptMsg );
	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
	int nItem = pNMLW->iItem;
	int nSubItem = pNMLW->iSubItem;
	if (nItem < 0 ||nSubItem <12)
		return;

	CabinCrewPhysicalCharacteristics* pItem = (CabinCrewPhysicalCharacteristics*)GetItemData(nItem);
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

void CCabinCrewDispListCtrl::InsertCrewDispItem(int _nIdx, CabinCrewPhysicalCharacteristics* _pItem )
{
	int nIdx = InsertItem( _nIdx, LPCTSTR(""));
	SetItemData( nIdx, (DWORD_PTR)_pItem );
	ASSERT(nIdx == _nIdx);
}

int CCabinCrewDispListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrlEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	return 0;
}


