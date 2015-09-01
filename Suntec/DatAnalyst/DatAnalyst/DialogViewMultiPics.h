#pragma once

class DialogViewMultiPics : public CDialog
{
    DECLARE_DYNAMIC(DialogViewMultiPics)

public:
    DialogViewMultiPics(CWnd* pParent = NULL);
    virtual ~DialogViewMultiPics();
    enum { IDD = IDD_ARROWPOINTLIST };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
};
