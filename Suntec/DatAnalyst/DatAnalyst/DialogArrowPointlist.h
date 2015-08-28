#pragma once

class CDialogArrowPointlist : public CDialog
{
    DECLARE_DYNAMIC(CDialogArrowPointlist)

public:
    CDialogArrowPointlist(CWnd* pParent = NULL);
    virtual ~CDialogArrowPointlist();
    enum { IDD = IDD_ARROWPOINTLIST };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
};
