#include "StdAfx.h"
#include ".\fuelspropertieslist.h"
#include "..\InputEnviroment\FuelProperties.h"

CFuelsPropertiesList::CFuelsPropertiesList(void)
{
	CBitmap bitmap;
	VERIFY(bitmap.LoadBitmap(AFX_IDB_CHECKLISTBOX_95));
	BITMAP bm;
	bitmap.GetObject(sizeof(BITMAP), &bm);
	m_sizeCheck.cx = bm.bmWidth / 3;
	m_sizeCheck.cy = bm.bmHeight;
	m_hBitmapCheck = (HBITMAP) bitmap.Detach();
	m_pAirPollutantList=NULL;
}

CFuelsPropertiesList::~CFuelsPropertiesList(void)
{
}
BEGIN_MESSAGE_MAP(CFuelsPropertiesList, CListCtrlEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT_EX(LVN_ENDLABELEDIT, OnEndlabeledit)
END_MESSAGE_MAP()


bool CFuelsPropertiesList::GetItemCheck(int nRow,int nColumn)
{
	CString strCheck=GetItemText(nRow,nColumn);
	if(strCheck==CString("1"))
		return TRUE;
	return FALSE;

}
void CFuelsPropertiesList::DrawItem( LPDRAWITEMSTRUCT lpdis)
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
	GetItem(&lvi); 

	BOOL bFocus = (GetFocus() == this);
	BOOL bSelected = (lvi.state & LVIS_SELECTED) || (lvi.state & LVIS_DROPHILITED);

	CRect rcAllLabels; 
	this->GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS); 



	rcAllLabels.left = 0;
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

	//draw Case name
	CRect rcPBCount; 
	CString strName;
	for (int i=0;i<GetHeaderCtrl()->GetItemCount();i++)
	{
		LVCOLDROPLIST* curStyle=GetColumnStyle(i);
		switch (curStyle->Style)
		{
		case CHECKBOX:
			{
				CRect rcCheckBox;
				this->GetSubItemRect( nItem,i,LVIR_LABEL,rcCheckBox );
				rcCheckBox.left+=(rcCheckBox.Width()-m_sizeCheck.cx)/2;
				rcCheckBox.top+=(rcCheckBox.Height()-m_sizeCheck.cy)/2;

				CDC bitmapDC;
				HBITMAP hOldBM;
				int nCheck = GetItemCheck(lvi.iItem,i);
				if(bitmapDC.CreateCompatibleDC(pDC)) {
					hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);
					pDC->BitBlt(rcCheckBox.left, rcCheckBox.top, m_sizeCheck.cx, m_sizeCheck.cy, &bitmapDC,
						m_sizeCheck.cx * nCheck, 0, SRCCOPY);
					::SelectObject(bitmapDC.m_hDC, hOldBM);
					bitmapDC.DeleteDC();
				}



				break;
			}
		default:
			{
				this->GetSubItemRect(nItem, i, LVIR_LABEL,rcPBCount);
				rcPBCount.left+=2;
				strName=GetItemText(lvi.iItem,i);
				pDC->DrawText(strName , rcPBCount, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

				break;
			}
		}
		
	}	
	
	if (lvi.state & LVIS_FOCUSED) 
		pDC->DrawFocusRect(rcAllLabels); 

	if (bSelected) {
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
	
}	

void CFuelsPropertiesList::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nColumn;
	int nItem=HitTestEx(point,&nColumn);
	if(nItem != -1&&nColumn>1) 
	{
		BOOL bCheck=GetItemCheck(nItem,nColumn);
		if(bCheck)
			SetItemCheck( nItem,nColumn,FALSE);
		else
			SetItemCheck( nItem,nColumn,TRUE);
		Invalidate();
	}
	CListCtrlEx::OnLButtonDown(nFlags,point);

}
void CFuelsPropertiesList::SetItemCheck(int nRow,int nColumn,BOOL bCheck)
{
	if(m_pAirPollutantList==NULL)
		return;
	int nType=m_pAirPollutantList->GetItem(nColumn-2)->GetID();
	FuelProperty *curProperty=(FuelProperty *)GetItemData(nRow);
	if(bCheck)
	{
		SetItemText(nRow,nColumn,"1");
		curProperty->setPollutantValueByType(nType,1);

	}
	else
	{
		SetItemText(nRow,nColumn,"0");
		curProperty->setPollutantValueByType(nType,0);
	}
}
void CFuelsPropertiesList::SetAirPollutantList(CAirPollutantList *pAirPollutantList)
{
	m_pAirPollutantList=pAirPollutantList;
}
BOOL CFuelsPropertiesList::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;
	int nRow=plvItem->iItem;
	if (nRow<0)
	{
		return false;
	}
	FuelProperty *curProperty=(FuelProperty *)GetItemData(nRow);
	int nColumn=plvItem->iSubItem;
	if (nColumn==0)
	{
		curProperty->setName(plvItem->pszText);	
		SetItemText(nRow,nColumn,plvItem->pszText);
	}else if (nColumn==1)
	{
		double dHvbtu=atof(plvItem->pszText);
		CString strHvbtu;
		strHvbtu.Format(_T("%.2f"),dHvbtu);
		curProperty->setHvbtu(dHvbtu);
		SetItemText(nRow,nColumn,strHvbtu);
	}
	
	return true;
}
void CFuelsPropertiesList::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	Invalidate();
	CListCtrlEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
void CFuelsPropertiesList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	Invalidate();
	CListCtrlEx::OnVScroll(nSBCode, nPos, pScrollBar);
}