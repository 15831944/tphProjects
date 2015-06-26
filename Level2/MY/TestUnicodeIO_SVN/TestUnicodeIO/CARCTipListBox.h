#pragma once

class CARCTipListBox : public CListBox
{
public:
    CARCTipListBox();

protected:
    virtual void PreSubclassWindow();
protected:
    HWND m_hWndToolTip;
    TOOLINFO m_ToolInfo;
public:
    HWND GetToolTipHanlder(void){return (m_hWndToolTip);}
protected:
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT nIDEvent);
    DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
