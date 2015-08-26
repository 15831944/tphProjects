#pragma once
#include "afxcmn.h"
#include "DialogMultiDatProcess.h"
#include "DialogSingleDatView.h"

class CDatAnalystDlg : public CDialog
{
public:
    CDatAnalystDlg(CWnd* pParent = NULL);
    enum { IDD = IDD_DATANALYST_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
protected:
    HICON m_hIcon;
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
private:
    CTabCtrl m_tab;
    CDialogMultiDatProcess m_mulDatProcDlg;
    CDialogSingleDatView m_singleDatViewDlg;

    void LayoutTabViews();
    afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);

    /* the resize adjustment manager. */
private:
    int m_oldCx;
    int m_oldCy;
    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};
