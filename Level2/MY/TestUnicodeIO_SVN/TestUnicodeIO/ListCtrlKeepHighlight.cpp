#include "stdafx.h"
#include "ListCtrlKeepHighlight.h"

IMPLEMENT_DYNAMIC(CListCtrlKeepHighlight, CListCtrlEx)

    CListCtrlKeepHighlight::CListCtrlKeepHighlight()
{
    LIST_ITEM_HEIGHT = 13;
}
CListCtrlKeepHighlight::~CListCtrlKeepHighlight()
{
}

BEGIN_MESSAGE_MAP(CListCtrlKeepHighlight, CListCtrlEx)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
    //ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
END_MESSAGE_MAP()

void CListCtrlKeepHighlight::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVCUSTOMDRAW lpnmcd = (LPNMLVCUSTOMDRAW) pNMHDR;
    if (lpnmcd ->nmcd.dwDrawStage == CDDS_PREPAINT)
    {
        *pResult =  CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
        return;
    }
    else if (lpnmcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
    {
        /*CRect rSubItem, rectClient;
        GetItemRect(lpnmcd->nmcd.dwItemSpec, &rSubItem, LVIR_LABEL);
        GetClientRect(&rectClient);	
        rSubItem.left = 0;
        rSubItem.right = rectClient.right;
        rSubItem.NormalizeRect();
        bool bSelected = false;
        if (GetItemState(lpnmcd->nmcd.dwItemSpec, LVIS_SELECTED))
        {
        bSelected = true;
        }
        bool bFocus = false;
        HWND hWndFocus = ::GetFocus();
        if (::IsChild(m_hWnd,hWndFocus) || m_hWnd == hWndFocus)
        {
        bFocus = true;
        }
        CDC dc;
        dc.Attach(lpnmcd->nmcd.hdc);
        draw_row_bg(&dc, rSubItem, bSelected , bFocus, (int) lpnmcd->nmcd.dwItemSpec);
        dc.Detach();*/
        *pResult =  CDRF_NOTIFYSUBITEMDRAW;
        return;
    }
    else if (lpnmcd ->nmcd.dwDrawStage == (CDDS_SUBITEM | CDDS_ITEMPREPAINT))
    {
        int iItem = lpnmcd->nmcd.dwItemSpec;
        int iSubItem = lpnmcd->iSubItem;
        if (iItem >= 0 && iSubItem >= 0)
        {
            CRect rSubItem;
            HDC hDC = lpnmcd->nmcd.hdc;
            GetSubItemRect(iItem, iSubItem, LVIR_LABEL,rSubItem);
            if (iSubItem == 0)
            {
                rSubItem.left = 0;
            }

            bool bSelected = false;
            if (GetItemState(iItem, LVIS_SELECTED))
            {
                bSelected = true;
            }
            bool bFocus = false;
            CWnd *pWndFocus = GetFocus();
            if (IsChild(pWndFocus) || pWndFocus == this)
            {
                bFocus = true;
            }
            rSubItem.NormalizeRect();
            CDC dc;
            dc.Attach(lpnmcd->nmcd.hdc);
            DrawSubItem(&dc,iItem,iSubItem,rSubItem,bSelected,bFocus);
            dc.Detach();
            *pResult =  CDRF_SKIPDEFAULT;
            return;
        }
    }
    else if (lpnmcd ->nmcd.dwDrawStage == CDDS_POSTPAINT)
    {
        DrawRemainSpace(lpnmcd);
        *pResult =  CDRF_SKIPDEFAULT;
        return;
    }

    *pResult = 0;
}

void CListCtrlKeepHighlight::DrawSubItem(CDC *pDC, int nItem, int nSubItem, CRect &rSubItem, bool bSelected, bool bFocus)
{
    pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextColor(RGB(0, 0, 0));
    CFont font;
    font.CreateFont(12,            // nHeight
        0,                         // nWidth
        0,                         // nEscapement
        0,                         // nOrientation
        FW_NORMAL,                 // nWeight
        FALSE,                     // bItalic
        FALSE,                     // bUnderline
        0,                         // cStrikeOut
        ANSI_CHARSET,              // nCharSet
        OUT_DEFAULT_PRECIS,        // nOutPrecision
        CLIP_DEFAULT_PRECIS,       // nClipPrecision
        DEFAULT_QUALITY,           // nQuality
        DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
        _T("ËÎÌו"));
    pDC->SelectObject(&font);
    CString strText;
    strText = GetItemText(nItem, nSubItem);
    draw_row_bg(pDC, rSubItem, bSelected, bFocus, nItem);
    pDC->DrawText(strText, strText.GetLength(), &rSubItem, DT_SINGLELINE | DT_RIGHT | DT_VCENTER | DT_END_ELLIPSIS);

}

void CListCtrlKeepHighlight::DrawRemainSpace(LPNMLVCUSTOMDRAW lpnmcd)
{
    int nTop = lpnmcd->nmcd.rc.top;
    int nCount = GetItemCount();
    if (nCount > 0)
    {
        CRect rcItem;
        GetItemRect(nCount - 1, &rcItem, LVIR_LABEL);
        nTop = rcItem.bottom;
    }
    CRect rectClient;
    GetClientRect(&rectClient);
    if (nTop < lpnmcd->nmcd.rc.bottom)
    {
        CRect rcRemain = lpnmcd->nmcd.rc;
        rcRemain.top = nTop;
        rcRemain.right = rectClient.right;
        int nRemainItem = rcRemain.Height() / LIST_ITEM_HEIGHT;
        if (rcRemain.Height() % LIST_ITEM_HEIGHT != 0)
        {
            nRemainItem++;
        }
        int pos = GetScrollPos(SB_HORZ);
        CDC dc;
        dc.Attach(lpnmcd->nmcd.hdc);
        for (int i = 0; i < nRemainItem; ++i)
        {
            CRect rcItem;
            rcItem.top = rcRemain.top + i * LIST_ITEM_HEIGHT;
            rcItem.left = rcRemain.left;
            rcItem.right = rcRemain.right;
            rcItem.bottom = rcItem.top + LIST_ITEM_HEIGHT;
            int nColumnCount = GetHeaderCtrl()->GetItemCount();
            CRect  rcSubItem;
            for (int j = 0; j < nColumnCount; ++j)
            {
                GetHeaderCtrl()->GetItemRect(j, &rcSubItem);
                rcSubItem.top = rcItem.top;
                rcSubItem.bottom = rcItem.bottom;
                rcSubItem.OffsetRect(-pos, 0);
                if(rcSubItem.right < rcRemain.left || rcSubItem.left > rcRemain.right)
                    continue;
                draw_row_bg(&dc, rcSubItem, false, false, i + nCount);
            }
            /*if (rcSubItem.right<rectClient.right)
            {
            rcSubItem.left=rcSubItem.right;
            rcSubItem.right=rectClient.right;
            draw_row_bg(&dc, rcSubItem, false, false, i+nCount);
            }*/
        }
        dc.Detach();
    }
}

void CListCtrlKeepHighlight::draw_row_bg(CDC *pDC, RECT rc, bool bSelected, bool bFocus,int nRow)
{
    CRect rect = rc;
    if (rect.Width() == 0)
    {
        return;
    }

    int nSave = pDC->SaveDC();
    if (bSelected)
    {
        if (bFocus)
        {
            CBrush selectBrush;
            selectBrush.CreateSolidBrush(RGB(203, 223, 239));
            pDC->FillRect(&rc, &selectBrush);
        }
        else
        {
            CBrush selectNoFocusBrush;
            selectNoFocusBrush.CreateSolidBrush(RGB(206, 206, 206));
            pDC->FillRect(&rc, &selectNoFocusBrush);
        }
    }
    else if (nRow % 2 == 0)
    {
        CBrush oddBrush;
        oddBrush.CreateSolidBrush(RGB(255, 255, 255));
        pDC->FillRect(&rc, &oddBrush);
    }
    else
    {
        CBrush normalBrush;
        normalBrush.CreateSolidBrush(RGB(243, 243, 243));
        pDC->FillRect(&rc, &normalBrush);
    }


    CPen pen;
    pen.CreatePen(PS_SOLID, 1, RGB(218, 218, 218));
    pDC->SelectObject(&pen);
    pDC->MoveTo(rc.right - 1, rc.top);
    pDC->LineTo(rc.right - 1, rc.bottom);

    if (bSelected)
    {
        CPen bottomPen;
        bottomPen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        pDC->SelectObject(&bottomPen);
        pDC->MoveTo(rc.left, rc.bottom - 1);
        pDC->LineTo(rc.right, rc.bottom - 1);
    }
    pDC->RestoreDC(nSave);
}

void CListCtrlKeepHighlight::Init()
{
    LOGFONT logfont;
    memset(&logfont, 0, sizeof(logfont));
    logfont.lfWeight = FW_NORMAL;
    logfont.lfCharSet = GB2312_CHARSET;
    _tcscpy_s(logfont.lfFaceName, LF_FACESIZE, _T("ËÎÌו"));// TODO
    logfont.lfHeight = -(LIST_ITEM_HEIGHT-1);
    CFont font;
    font.CreateFontIndirect(&logfont);
    SetFont(&font);
    font.Detach();
}

void CListCtrlKeepHighlight::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    if (pNMLV->uChanged & LVIF_STATE)
    {
        if (((pNMLV->uOldState & LVIS_SELECTED) != (pNMLV->uNewState & LVIS_SELECTED)) 
            || ((pNMLV->uOldState & LVIS_STATEIMAGEMASK) != (pNMLV->uNewState & LVIS_STATEIMAGEMASK)))
        {
            InvalidateItem(pNMLV->iItem);
        }
    }
    *pResult = 0;
}

void CListCtrlKeepHighlight::InvalidateItem(int nItem)
{
    CRect rcClient;
    GetClientRect(&rcClient);
    CRect rcItem;
    GetItemRect(nItem, &rcItem, LVIR_BOUNDS);
    rcItem.left = rcClient.left;
    rcItem.right = rcClient.right;
    InvalidateRect(&rcItem,FALSE);
}

void CListCtrlKeepHighlight::PreSubclassWindow()
{
    Init();
    CListCtrl::PreSubclassWindow();
}

void CListCtrlKeepHighlight::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
}