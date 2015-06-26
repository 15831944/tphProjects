#pragma once
#include "afxwin.h"

class CCoolTreeEdit : public CEdit
{
public:
    CCoolTreeEdit();

public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual ~CCoolTreeEdit();

protected:
    bool m_bESC;   // to indicate whether ESC key was pressed

    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

    DECLARE_MESSAGE_MAP()
};