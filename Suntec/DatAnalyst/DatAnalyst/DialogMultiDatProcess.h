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
    afx_msg void OnBnClickedGo();
    afx_msg void OnBnClickedBtnGetinputpath();
    afx_msg void OnBnClickedBtnGetoutputpath();
    afx_msg void OnSize(UINT nType, int cx, int cy);

private:
    /* the resize adjustment manager. */
    int m_oldCx;
    int m_oldCy;
    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);

};
