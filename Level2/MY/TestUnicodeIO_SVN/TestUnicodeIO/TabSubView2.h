#pragma once

class CTabSubView2 : public CDialog
{
    DECLARE_DYNAMIC(CTabSubView2)

public:
    CTabSubView2(CWnd* pParent = NULL);
    virtual ~CTabSubView2();
    enum { IDD = IDD_TABSUBVIEW2 };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
};
