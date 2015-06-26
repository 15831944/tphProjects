#include "stdafx.h"
#include "FloatEdit.h"


IMPLEMENT_DYNAMIC(CFloatEdit, CEdit)

CFloatEdit::CFloatEdit(short nPrecision) : m_nPrecision(nPrecision)
{
}

CFloatEdit::~CFloatEdit()
{
}

BEGIN_MESSAGE_MAP(CFloatEdit, CEdit)
    ON_WM_CHAR()
END_MESSAGE_MAP()

void CFloatEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if(nChar == 8)  // backspace
    {
        CEdit::OnChar(nChar, nRepCnt, nFlags);
        return;
    }

    if(nChar=='.')
    {
        CString strEdit;
        GetWindowText(strEdit);
        int dotPos=strEdit.Find('.');

        int selStart=-1, selEnd=-1;
        this->GetSel(selStart,selEnd);
        ASSERT(selStart <= selEnd);
        if(dotPos == -1)
        {
            if(selStart==0 && selEnd==0)
            {
                if(strEdit[0] != '-')
                {
                    CEdit::OnChar(nChar, nRepCnt, nFlags);
                    return;
                }
                else
                {
                    return;
                }
            }
            else if((strEdit.GetLength()-selEnd) <= m_nPrecision)
            {
                CEdit::OnChar(nChar, nRepCnt, nFlags);
                return;
            }
            else
            {
                return;
            }
        }
        else
        {
            if(selStart<=dotPos && dotPos<selEnd)
            {
                CEdit::OnChar(nChar, nRepCnt, nFlags);
                return;
            }
            else
            {
                return;
            }
        }
    }

    if(nChar == '-')
    {
        CString strEdit;
        GetWindowText(strEdit);
        if(strEdit.IsEmpty())
        {
            CEdit::OnChar(nChar, nRepCnt, nFlags);
            return;
        }
        else
        {
            int selStart=-1, selEnd=-1;
            this->GetSel(selStart,selEnd);
            ASSERT(selStart <= selEnd);
            if(selStart==0 && selEnd!=0)
            {
                CEdit::OnChar(nChar, nRepCnt, nFlags);
                return;
            }
            else if(selStart==0 && selEnd==0)
            {
                if(strEdit[0] != '-')
                {
                    CEdit::OnChar(nChar, nRepCnt, nFlags);
                    return;
                }
                else
                {
                    return;
                }
            }
            else
            {
                return;
            }
        }
    }

    if('0'<=nChar && nChar<='9') // numeric
    {
        CString strEdit;
        GetWindowText(strEdit);
        int selStart=-1, selEnd=-1;
        this->GetSel(selStart,selEnd);
        ASSERT(selStart <= selEnd);
        if(strEdit[0] == '-' && selStart==0 && selEnd==0)
        {
            return;
        }

        int dotPos=strEdit.Find('.');
        if(dotPos == -1)
        {
            CEdit::OnChar(nChar, nRepCnt, nFlags);
            return;
        }
        else
        {
            if(selStart<selEnd)
            {
                CEdit::OnChar(nChar, nRepCnt, nFlags);
                return;
            }
            else
            {
                ASSERT(selStart == selEnd);
                if(selStart <= dotPos)
                {
                    CEdit::OnChar(nChar, nRepCnt, nFlags);
                    return;
                }
                else
                {
                    if((strEdit.GetLength()-dotPos) <= m_nPrecision)
                    {
                        CEdit::OnChar(nChar, nRepCnt, nFlags);
                        return;
                    }
                    else
                    {
                        return;
                    }
                }
            }
        }
    }
}