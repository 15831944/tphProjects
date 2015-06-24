// ShapesListBox.cpp : implementation file
//

#include "stdafx.h"

#include "termplan.h"
#include "common\WinMsg.h"
#include "ShapesListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShapesListBox

CShapesListBox::CShapesListBox()
{
}

CShapesListBox::~CShapesListBox()
{
}


BEGIN_MESSAGE_MAP(CShapesListBox, CListBox)
	//{{AFX_MSG_MAP(CShapesListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShapesListBox message handlers

BOOL CShapesListBox::PreCreateWindow(CREATESTRUCT& cs) 
{
	if(!CListBox::PreCreateWindow(cs))
		return FALSE;

	cs.style &= ~(LBS_OWNERDRAWFIXED | LBS_SORT);
	cs.style |= LBS_OWNERDRAWVARIABLE | LBS_NOTIFY;
	return TRUE;
}

void CShapesListBox::MeasureItem(LPMEASUREITEMSTRUCT lpmis) 
{
	int nIdx = lpmis->itemID;
	if(nIdx != (UINT) -1) {
		/*
		CShape* pShape = (CShape*) GetItemData(nIdx);
		ASSERT(pShape != NULL);
		CBitmap* pBM = pShape->GetShapeImageBitmap();
		ASSERT(pBM != NULL);
		BITMAP bm;
		VERIFY(pBM->GetBitmap(&bm));
		*/
		lpmis->itemHeight = m_vImgSizes[nIdx].cy+2;
	}
	else {
		lpmis->itemHeight = 32;
	}
}

void CShapesListBox::DrawItem(LPDRAWITEMSTRUCT lpdis) 
{
	CDC dc;
	dc.Attach(lpdis->hDC);
	CRect rect = lpdis->rcItem;
	int nIdx = lpdis->itemID;

	BOOL bDisabled = !IsWindowEnabled();
	BOOL bFocus = (GetFocus() == this) &&  (lpdis->itemState & ODS_FOCUS);
	BOOL bSelected = (lpdis->itemState & ODS_SELECTED) || (lpdis->itemState & LVIS_DROPHILITED);

	CBrush* pBrush = new CBrush;
	pBrush->CreateSolidBrush(::GetSysColor(bSelected ? COLOR_HIGHLIGHT : COLOR_BTNFACE));
	dc.FillRect(rect, pBrush);
	delete pBrush;

	if(bFocus)
		dc.DrawFocusRect(rect);

	if(nIdx != (UINT) -1) {
		CShape* pShape = (CShape*) GetItemData(nIdx);
		ASSERT(pShape != NULL);
		CBitmap* pBM = pShape->GetShapeImageBitmap();
		ASSERT(pBM != NULL);
		DIBSECTION ds;
		VERIFY(pBM->GetObject(sizeof(DIBSECTION), &ds));
		CDC memDC;
		VERIFY(memDC.CreateCompatibleDC(&dc));
		CBitmap* pOldBM = (CBitmap*) memDC.SelectObject(pBM);
		VERIFY(::TransparentBlt(dc.GetSafeHdc(), rect.left+2,rect.top+1,ds.dsBm.bmWidth,ds.dsBm.bmHeight,memDC.GetSafeHdc(),0,0,ds.dsBm.bmWidth,ds.dsBm.bmHeight,RGB(255,0,255)));
		memDC.SelectObject(pOldBM);

		//now draw the shapes's name as text
		int nMode = dc.GetBkMode();
		COLORREF tcol = dc.GetTextColor();
		if(bSelected)
			dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		else
			dc.SetTextColor(::GetSysColor(COLOR_BTNTEXT));
		dc.SetBkMode(TRANSPARENT);
		rect.left += (m_vImgSizes[nIdx].cx+5);
		dc.DrawText(pShape->Name(), rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		dc.SetBkMode(nMode);
		dc.SetTextColor(tcol);
	}

	dc.Detach();
}

int CShapesListBox::AddItem(CShape* pShape)
{
	ASSERT(pShape != NULL);
	int nIdx = AddString(pShape->Name());
	if((nIdx != LB_ERR) && (nIdx != LB_ERRSPACE))
		SetItemData(nIdx, (DWORD) pShape);
	return nIdx;
}
void CShapesListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CListBox::OnLButtonDown(nFlags, point);
	int idx = GetCurSel();
	// TRACE("idx=%d\n",idx);
	ReleaseCapture();
	GetParent()->SendMessage( WM_SLB_SELCHANGED, (WPARAM) idx, (LPARAM) NULL);
}

int CShapesListBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}
