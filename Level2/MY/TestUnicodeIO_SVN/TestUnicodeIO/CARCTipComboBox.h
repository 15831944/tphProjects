#pragma once

#include "CARCTipListBox.h"

class CARCTipComboBox : public CComboBox
{
public:
    CARCTipComboBox();
    virtual void PreSubclassWindow();
protected:
    CARCTipListBox m_listbox;
    HWND m_hWndToolTip;
    TOOLINFO m_ToolInfo;
public:
    HINSTANCE m_hExeInstance;

protected:
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnDestroy();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT nIDEvent);

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
