// LayerListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "LayerListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLayerListCtrl::_layerInfo::_layerInfo(CCADLayer* pLayer)
{
	pFloorLayer = pLayer;
	if(pFloorLayer)
	{
		bIsOn = pLayer->bIsOn;
		cColor = pLayer->cColor;
		sName = pLayer->sName;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CLayerListCtrl

CLayerListCtrl::CLayerListCtrl()
{
	CBitmap bitmap;
	VERIFY(bitmap.LoadBitmap(AFX_IDB_CHECKLISTBOX_95));
	BITMAP bm;
	bitmap.GetObject(sizeof(BITMAP), &bm);
	m_sizeCheck.cx = bm.bmWidth / 3;
	m_sizeCheck.cy = bm.bmHeight;
	m_hBitmapCheck = (HBITMAP) bitmap.Detach();
}

CLayerListCtrl::~CLayerListCtrl()
{
	if(m_hBitmapCheck != NULL)
		::DeleteObject(m_hBitmapCheck);
}


BEGIN_MESSAGE_MAP(CLayerListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CLayerListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CLayerListCtrl::PopulateLayerList(CCADFileContent::CFloorLayerList& vList)
{
	SetIconSpacing(CSize(50, 50));
	FreeItemMemory();
	for(int i=0; i<static_cast<int>(vList.size()); i++) {
		_layerInfo* li = new _layerInfo(vList[i]);
		LV_ITEM lvi;
		lvi.mask = LVIF_PARAM;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.iImage = 0;
		lvi.lParam = (LPARAM) li;
		InsertItem(&lvi);
	}
}

void CLayerListCtrl::PopulateLayerList( const LayerInfoList& vList )
{
	SetIconSpacing(CSize(50, 50));
	FreeItemMemory();

	for(int i=0; i<static_cast<int>(vList.size()); i++) {
		_layerInfo* li = new _layerInfo(0);
		li->bIsOn = vList.at(i).bIsOn;
		li->cColor = vList.at(i).cColor;
		li->sName = vList.at(i).sName;

		LV_ITEM lvi;
		lvi.mask = LVIF_PARAM;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.iImage = 0;
		lvi.lParam = (LPARAM) li;
		InsertItem(&lvi);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CLayerListCtrl message handlers
void CLayerListCtrl::DrawItem( LPDRAWITEMSTRUCT lpdis)
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
	CRect rcColor;
	this->GetSubItemRect(nItem,1,LVIR_LABEL,rcColor);
	GetColorBoxRect(rcColor);
	CRect rcVisible;
	this->GetSubItemRect(nItem,2,LVIR_LABEL,rcVisible);
	GetVisibleRect(rcVisible);
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
	_layerInfo* li = (_layerInfo*) lvi.lParam;

	//draw layer name
    pDC->DrawText(li->sName,rcLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	//draw color box
	pDC->FillSolidRect(rcColor,li->cColor);
	//draw checkbox
	int nCheck = (int) li->bIsOn;
	CDC bitmapDC;
	if(bitmapDC.CreateCompatibleDC(pDC)) {
		HBITMAP hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);
		pDC->BitBlt(rcVisible.left, rcVisible.top, m_sizeCheck.cx, m_sizeCheck.cy, &bitmapDC,
			m_sizeCheck.cx * nCheck, 0, SRCCOPY);
		::SelectObject(bitmapDC.m_hDC, hOldBM);
	}

	if (lvi.state & LVIS_FOCUSED) 
        pDC->DrawFocusRect(rcAllLabels); 

    if (bSelected) {
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

void CLayerListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/*
	CPoint ptMsg = GetMessagePos();
	ScreenToClient( &ptMsg );
	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
	int nItem = pNMLW->iItem;
	if(nItem != -1) {
		_layerInfo* li = (_layerInfo*) this->GetItemData(nItem);
		if(pNMLW->iSubItem == 1) {// color box column
			CRect rcColor;
			GetSubItemRect(nItem,1,LVIR_LABEL,rcColor);
			GetColorBoxRect(rcColor);
			if(rcColor.PtInRect(ptMsg)) {
				CColorDialog dlg(li->cColor, CC_ANYCOLOR | CC_FULLOPEN, this);
				if(dlg.DoModal() == IDOK) {
					li->cColor = dlg.GetColor();
				}
			}
		}
		else if(pNMLW->iSubItem == 2) {// visible column
			CRect rcVisible;
			this->GetSubItemRect(nItem,2,LVIR_LABEL,rcVisible);
			GetVisibleRect(rcVisible);
			if(rcVisible.PtInRect(ptMsg)) {
				BOOL b;
				if(li->bIsOn)
					b = FALSE;
				else
					b = TRUE;
				POSITION pos = GetFirstSelectedItemPosition();
				while(pos != NULL) {
					int nSelItem = GetNextSelectedItem(pos);
					_layerInfo* selli = (_layerInfo*) GetItemData(nSelItem);
					selli->bIsOn = b;
				}
			}
		}
		Invalidate();
	}
*/
	
	*pResult = 0;
}

void CLayerListCtrl::GetColorBoxRect(CRect& r)
{
	r.DeflateRect(CSize(0,1));
	r.right = r.CenterPoint().x;
	r.left++;
}

void CLayerListCtrl::GetVisibleRect(CRect& r)
{
	r.left++;
	r.top = (r.bottom+r.top-m_sizeCheck.cy)/2;
	r.bottom = r.top+m_sizeCheck.cy;
	r.right = r.left+m_sizeCheck.cx;
}

void CLayerListCtrl::FreeItemMemory()
{
	for(int i=0; i<this->GetItemCount(); i++) {
		_layerInfo* li = (_layerInfo*) this->GetItemData(i);
		ASSERT(li != NULL);
		delete li;
	}
}

BOOL CLayerListCtrl::UpdateFloorData()
{
	BOOL bRet = FALSE;
	for(int i=0; i<this->GetItemCount(); i++) {
		_layerInfo* li = (_layerInfo*) this->GetItemData(i);
		ASSERT(li != NULL);
		if(li->pFloorLayer->bIsOn != li->bIsOn) {
			li->pFloorLayer->bIsOn = li->bIsOn;
			bRet = TRUE;
		}
		if(li->pFloorLayer->cColor != li->cColor) {
			li->pFloorLayer->cColor = li->cColor;
			bRet = TRUE;
		}
		if(li->pFloorLayer->sName.CompareNoCase(li->sName) != 0) {
			li->pFloorLayer->sName = li->sName;
			bRet = TRUE;
		}
	}
	return bRet;
}
void CLayerListCtrl::GetLayersData( LayerInfoList& newList )
{	
	for(int i=0; i<this->GetItemCount(); i++) {
		_layerInfo* li = (_layerInfo*) this->GetItemData(i);
		ASSERT(li != NULL);
		CCADLayerInfo info;
		info.bIsOn = li->bIsOn;
		info.sName = li->sName;
		info.cColor = li->cColor;
		newList.push_back(info);		
	}	
}


void CLayerListCtrl::OnDestroy() 
{	
	FreeItemMemory();
	CListCtrl::OnDestroy();	
}

void CLayerListCtrl::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;
	
	if ((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
		return;

	if(pnmv->uOldState & LVIS_SELECTED) {
		if(!(pnmv->uNewState & LVIS_SELECTED)) {
			// TRACE("Unselected Item %d...\n", pnmv->iItem);
		}
	}
	else if(pnmv->uNewState & LVIS_SELECTED) {
		//a new item has been selected
		// TRACE("Selected Item %d...\n", pnmv->iItem);
	}
	else
		ASSERT(0);
}

void CLayerListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	LVHITTESTINFO lvhti;
	lvhti.pt = point;
	SubItemHitTest(&lvhti);

	int nItem = lvhti.iItem;

	BOOL bForwardMessage = TRUE;

	if(nItem != -1) {
		_layerInfo* li = (_layerInfo*) this->GetItemData(nItem);
		
		int nSubItem = lvhti.iSubItem;
		if(nSubItem == 1) {// color box column
			CRect rcColor;
			GetSubItemRect(nItem,1,LVIR_LABEL,rcColor);
			GetColorBoxRect(rcColor);
			if(rcColor.PtInRect(point)) {

				CColorDialog dlg(li->cColor, CC_ANYCOLOR | CC_FULLOPEN, this);
				COLORREF col;
				if(dlg.DoModal() == IDOK) {
					col = dlg.GetColor();
					UINT nState = GetItemState(nItem, LVIS_SELECTED);
					if(nState & LVIS_SELECTED) {
						POSITION pos = GetFirstSelectedItemPosition();
						while(pos != NULL) {
							int nSelItem = GetNextSelectedItem(pos);
							_layerInfo* selli = (_layerInfo*) GetItemData(nSelItem);
							selli->cColor = col;
						}
						bForwardMessage = FALSE;
					}
					else {
						li->cColor = col;
					}
				}
			}
		}
		else if(nSubItem == 2) {// visible column
			CRect rcVisible;
			this->GetSubItemRect(nItem,2,LVIR_LABEL,rcVisible);
			GetVisibleRect(rcVisible);
			if(rcVisible.PtInRect(point)) {
				BOOL b;
				if(li->bIsOn)
					b = FALSE;
				else
					b = TRUE;

				UINT nState = GetItemState(nItem, LVIS_SELECTED);
				if(nState & LVIS_SELECTED) {
					POSITION pos = GetFirstSelectedItemPosition();
					while(pos != NULL) {
						int nSelItem = GetNextSelectedItem(pos);
						_layerInfo* selli = (_layerInfo*) GetItemData(nSelItem);
						selli->bIsOn = b;
					}
					bForwardMessage = FALSE;
				}
				else {
					li->bIsOn = b;
				}
			}
		}
		Invalidate();
	}

	if(bForwardMessage)
		CListCtrl::OnLButtonDown(nFlags, point);
}


