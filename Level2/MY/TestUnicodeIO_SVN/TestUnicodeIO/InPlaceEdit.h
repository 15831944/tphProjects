#pragma once


#define IPEDIT_TYPE_NORMAL    0x0001L
#define IPEDIT_TYPE_NUMBER    0x0002L
#define IPEDIT_TYPE_NUMBER2   0x0003L

class CInPlaceEdit : public CEdit
{
public:
    CInPlaceEdit(int iItem, int iSubItem, CString sInitText, int nUseType = IPEDIT_TYPE_NORMAL);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual ~CInPlaceEdit();

protected:
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnNcDestroy();
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()

private:
    int  m_iItem;
    int  m_iSubItem;
    CString m_sInitText;
    bool m_bESC;   //To indicate whether ESC key was pressed
    int  m_nUseType;
};