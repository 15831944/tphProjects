#pragma once

typedef struct tagSPINTEXT
{
    TCHAR szTitle[1024];
    int   iPercent;
    //new item, please make sure the value is valid before use it.
    //take care
    int nItem;
    int nColumn;
} SPINTEXT, FAR* LPSPINTEXT;


class CMySpin : public CWnd
{
    // Construction
public:
    CMySpin();

    void SetPercent(int iPercent);
    void SetTitle(CString strTitle);
    void SetDisplayType( int _iDisplayType );;
    virtual ~CMySpin();

    void SetSpinRange( int _nLower, int _nUpper );
    int m_nSpinEditLower;
    int m_nSpinEditUpper;

protected:
    CEdit m_Edit;
    CSpinButtonCtrl m_Spin;

    CString m_strTitle;
    int     m_iPercent;
    int     m_iDisplayType;
    bool    m_bSetPosFlag;

    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnKillfocusMyEdit();
    afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);

    DECLARE_MESSAGE_MAP()
};