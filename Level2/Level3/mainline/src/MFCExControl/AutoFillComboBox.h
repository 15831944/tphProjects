#pragma once

#include "MFCExControlAPI.h"

class CComboChildEditBox : public CEdit
{
    DECLARE_DYNAMIC(CComboChildEditBox)

public:
    CComboChildEditBox();
    virtual ~CComboChildEditBox();

public:
    UINT GetLastPressKey() { return m_lastPressKey; }
protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
private:
    UINT m_lastPressKey;
};

class MFCEXCONTROL_API CAutoFillComboBox : public CComboBox
{
    DECLARE_DYNAMIC(CAutoFillComboBox)
public:
    CAutoFillComboBox();
    virtual ~CAutoFillComboBox();

public:
    UINT GetLastPressKey() { return m_edit.GetLastPressKey(); }
protected:
    DECLARE_MESSAGE_MAP()
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnDestroy();
protected:
    CComboChildEditBox m_edit;
};


