#pragma once

class CTabSubView3 : public CDialog
{
    DECLARE_DYNAMIC(CTabSubView3)

public:
    CTabSubView3(CWnd* pParent = NULL);
    virtual ~CTabSubView3();
    enum { IDD = IDD_TABSUBVIEW3 };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()
};
