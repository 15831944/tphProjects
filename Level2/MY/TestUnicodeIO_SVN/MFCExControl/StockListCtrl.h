#pragma once
#include "MFCExControlAPI.h"

class Stock
{
public:
    Stock(){}
    ~Stock(){}

public:
    int m_StockId;
};

class MFCEXCONTROL_API CStockListCtrl : public CListCtrl
{
    DECLARE_DYNAMIC(CStockListCtrl)

public:
    CStockListCtrl();
    virtual ~CStockListCtrl();
protected:
    virtual void PreSubclassWindow();
    afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg BOOL OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);

private:
    void InitListFont();
    void InvalidateItemRect(int nItem);
    void DrawSubItem(LPNMLVCUSTOMDRAW lpnmcd);
protected:
    DECLARE_MESSAGE_MAP()
};


