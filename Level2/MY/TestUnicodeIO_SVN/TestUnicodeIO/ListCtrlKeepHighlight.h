#pragma once
#include "ListCtrlEx.h"

class CListCtrlKeepHighlight : public CListCtrlEx
{
    DECLARE_DYNAMIC(CListCtrlKeepHighlight)
public:
    CListCtrlKeepHighlight();
    virtual ~CListCtrlKeepHighlight();
    DECLARE_MESSAGE_MAP()

protected:
    LOGFONT logfont;
    unsigned short LIST_ITEM_HEIGHT;
public:
    LOGFONT Logfont() const { return logfont; }
    void Logfont(LOGFONT val) { logfont = val; }
protected:
    virtual void PreSubclassWindow();
    virtual void InvalidateItem(int nItem);
    virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
    virtual void DrawSubItem(CDC *pDC, int nItem, int nSubItem, CRect &rSubItem, bool bSelected, bool bFocus);
    virtual void DrawRemainSpace(LPNMLVCUSTOMDRAW lpnmcd);
    virtual void draw_row_bg(CDC *pDC, RECT rc, bool bSelected, bool bFocus,int nRow);
    afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
private:
    void Init();
};