#pragma once
#include "afxcmn.h"
#include "ListCtrlKeepHighlight.h"

class CTabSubView2 : public CDialog
{
    DECLARE_DYNAMIC(CTabSubView2)

public:
    CTabSubView2(CWnd* pParent = NULL);
    virtual ~CTabSubView2();
    enum { IDD = IDD_TABSUBVIEW2 };
protected:
    CListCtrlKeepHighlight m_mainList;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    DECLARE_MESSAGE_MAP()
private:
    int m_oldCx;
    int m_oldCy;
    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);
};
