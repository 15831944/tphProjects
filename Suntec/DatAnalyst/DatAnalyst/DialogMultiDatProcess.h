#pragma once

class CDialogMultiDatProcess : public CDialog
{
    DECLARE_DYNAMIC(CDialogMultiDatProcess)

public:
    CDialogMultiDatProcess(CWnd* pParent = NULL);
    virtual ~CDialogMultiDatProcess();
    enum { IDD = IDD_MULTIDATPROCESS };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();
};
