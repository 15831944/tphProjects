#include "stdafx.h"
#include "CoolTreeKeepHighlight.h"

IMPLEMENT_DYNAMIC(CCoolTreeKeepHighlight, CTreeCtrl)

CCoolTreeKeepHighlight::CCoolTreeKeepHighlight()
{
}

CCoolTreeKeepHighlight::~CCoolTreeKeepHighlight()
{
}

BEGIN_MESSAGE_MAP(CCoolTreeKeepHighlight, CCoolTree)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
END_MESSAGE_MAP()

void CCoolTreeKeepHighlight::PreSubclassWindow()
{
    CCoolTree::PreSubclassWindow();
}

void CCoolTreeKeepHighlight::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTVCUSTOMDRAW lpnmcd = (LPNMTVCUSTOMDRAW) pNMHDR;
    switch(lpnmcd ->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT:
        {
            *pResult =  CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
            return FALSE;
        }
        break;
    case CDDS_ITEMPREPAINT:
        {
            *pResult =  CDRF_NOTIFYSUBITEMDRAW;
            CDC dc;
            dc.Attach(lpnmcd->nmcd.hdc);
            int nSavedDC = dc.SaveDC();
            dc.SelectObject(::GetStockObject(SYSTEM_FONT));
            CBrush brush;
            brush.CreateSolidBrush(RGB(255, 0, 0));
            CRect rr(lpnmcd->nmcd.rc);
            dc.FillRect(&rr, &brush);
            dc.RestoreDC(nSavedDC);
            return TRUE;
        }
        break;
    case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
        {
            if (lpnmcd->nmcd.dwItemSpec >= 0)
            {
                *pResult =  CDRF_SKIPDEFAULT;
                return FALSE;
            }
        }
        break;
    case CDDS_POSTPAINT:
        {
            *pResult =  CDRF_SKIPDEFAULT;
            return FALSE;
        }
        break;
    default:
        break;
    }
    *pResult = 0;
    return FALSE;
}

BOOL CCoolTreeKeepHighlight::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTV = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    if (pNMTV)
    {
    }
    *pResult = 0;
    return FALSE;
}

