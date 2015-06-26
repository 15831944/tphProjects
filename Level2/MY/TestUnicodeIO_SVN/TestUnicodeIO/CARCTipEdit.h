#pragma once

class CARCTipEdit : public CEdit
{
public:
    CARCTipEdit();

protected:
    virtual void PreSubclassWindow();
protected:
    HWND m_hWndToolTip;
    TOOLINFO m_ToolInfo;

protected:
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT nIDEvent);
    DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

