#pragma once

class CNumericEdit : public CEdit
{
private:
    struct _LocaleInfo
    {
        TCHAR chDecimalSymbol;  // character used for decimal separator
        TCHAR chNegationSymbol; // character used for negative sign

        _LocaleInfo()
        {
            chDecimalSymbol = _T('.');  // reasonable default!
            chNegationSymbol = _T('-'); // reasonable default!

            CString strT;   // temporary string
            int iResult = ::GetLocaleInfo(LOCALE_USER_DEFAULT, 
                LOCALE_SDECIMAL, 
                strT.GetBufferSetLength(2), 2);
            strT.ReleaseBuffer();
            if (iResult) chDecimalSymbol = strT[0];
            iResult = ::GetLocaleInfo(LOCALE_USER_DEFAULT, 
                LOCALE_SNEGATIVESIGN, 
                strT.GetBufferSetLength(2), 2);
            strT.ReleaseBuffer();
            if (iResult) chNegationSymbol = strT[0];
        }
    };

public:
    CNumericEdit() : CEdit(), m_bAllowNegativeValues(true), m_chDigitsAfterDecimal(6){}
public:
    virtual BOOL PreTranslateMessage(MSG *pMSG);
private:
    afx_msg LRESULT OnPaste(WPARAM wParam, LPARAM lParam);
private:
    CString GetClipboardText(void) const;
public:
    bool m_bAllowNegativeValues;    // determines whether negative values are allowed
    TCHAR m_chDigitsAfterDecimal;   // number of digits to allow after decimal
private:
    static const _LocaleInfo DefUserLocale;
    DECLARE_MESSAGE_MAP()
};

