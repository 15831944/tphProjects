#pragma once
#include <afxcmn.h>
#include "TabSubView1.h"
#include "TabSubView2.h"
#include "TabSubView3.h"
#include "TabSubView4.h"

class CTestUnicodeIODlg : public CDialogEx
{
public:
    CTestUnicodeIODlg(CWnd* pParent = NULL);
    enum { IDD = IDD_TESTUNICODEIO_DIALOG };
protected:
    HICON m_hIcon;
    virtual BOOL OnInitDialog();

    void LayoutTabViews();

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnTcnSelchangeMainTab(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
protected:
    CTabCtrl m_mainTab;
    CTabSubView1 m_tabView1;
    CTabSubView2 m_tabView2;
    CTabSubView3 m_tabView3;
    CTabSubView4 m_tabView4;
private:
    int m_oldCx;
    int m_oldCy;
    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);
public:
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};
