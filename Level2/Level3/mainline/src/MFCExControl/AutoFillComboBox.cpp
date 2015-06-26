#include "stdafx.h"
#include "AutoFillComboBox.h"

IMPLEMENT_DYNAMIC(CComboChildEditBox, CEdit)
CComboChildEditBox::CComboChildEditBox() : m_lastPressKey(0){}
CComboChildEditBox::~CComboChildEditBox(){}


BEGIN_MESSAGE_MAP(CComboChildEditBox, CEdit)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void CComboChildEditBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    m_lastPressKey = nChar;
    CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
    return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CAutoFillComboBox, CComboBox)
CAutoFillComboBox::CAutoFillComboBox(){}
CAutoFillComboBox::~CAutoFillComboBox(){}

BEGIN_MESSAGE_MAP(CAutoFillComboBox, CComboBox)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

HBRUSH CAutoFillComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    if (nCtlColor == CTLCOLOR_EDIT)
    {
        //Edit control 
        if (m_edit.GetSafeHwnd() == NULL)
        {
            m_edit.SubclassWindow(pWnd->GetSafeHwnd());
        }
    }
    HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
    return hbr;
}

void CAutoFillComboBox::OnDestroy()
{
    if (m_edit.GetSafeHwnd() != NULL)
        m_edit.UnsubclassWindow();
    CComboBox::OnDestroy();
}






