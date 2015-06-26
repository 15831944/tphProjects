#pragma once
#include "ListCtrlEx.h"

class CListCtrlKeepHighlight : public CListCtrlEx
{
    DECLARE_DYNAMIC(CListCtrlKeepHighlight)
public:
    CListCtrlKeepHighlight();
    virtual ~CListCtrlKeepHighlight();
protected:
    DECLARE_MESSAGE_MAP()
    virtual void PreSubclassWindow();
    void InvalidateItemRect(int nItem);
    void DrawSubItem(LPNMLVCUSTOMDRAW lpnmcd);
    void DrawRemainSpace(LPNMLVCUSTOMDRAW lpnmcd);
    afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg BOOL OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
private:
    void Init();
};