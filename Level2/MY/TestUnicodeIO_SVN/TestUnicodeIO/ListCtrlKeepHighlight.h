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
    void InvalidateItemRect(int nItem);
    void DrawSubItem( LPNMLVCUSTOMDRAW lpnmcd );
    void DrawSubItemBackground(CDC& dc, RECT rc, bool bSelected, bool bFocus);
    void DrawRemainSpace(LPNMLVCUSTOMDRAW lpnmcd);
    afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
private:
    void Init();
};