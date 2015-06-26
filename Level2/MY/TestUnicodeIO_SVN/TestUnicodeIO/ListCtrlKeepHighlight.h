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
    virtual void PreSubclassWindow();
    virtual void InvalidateItem(int nItem);
    virtual void DrawSubItem(CDC *pDC, int nItem, int nSubItem, CRect &rSubItem, bool bSelected, bool bFocus);
    virtual void DrawRowBackground(CDC *pDC, RECT rc, bool bSelected, bool bFocus,int nRow);
    virtual void DrawRemainSpace(LPNMLVCUSTOMDRAW lpnmcd);
    afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
private:
    void Init();
};