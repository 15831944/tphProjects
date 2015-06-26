#pragma once
#include <vector>

class CTabSubView4 : public CDialog
{
    DECLARE_DYNAMIC(CTabSubView4)

public:
    CTabSubView4(CWnd* pParent = NULL);
    virtual ~CTabSubView4();
    enum { IDD = IDD_TABSUBVIEW4 };

protected:
    CListCtrl m_lstVirtual;
    std::vector<int> m_vInt;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    void InitData();
    DECLARE_MESSAGE_MAP()
private:
    int m_oldCx;
    int m_oldCy;
    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);
public:
    afx_msg void OnLvnGetdispinfoListVirtual(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedBtnRefresh();
    afx_msg void OnBnClickedBtnSort();
};
