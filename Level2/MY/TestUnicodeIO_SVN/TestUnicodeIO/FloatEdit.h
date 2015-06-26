#pragma once

class CFloatEdit : public CEdit
{
    DECLARE_DYNAMIC(CFloatEdit)

public:
    CFloatEdit(short nPrecision = 2);
    virtual ~CFloatEdit();

public:

    short GetPrecision() const { return m_nPrecision; }
    void SetPrecision(short val) { m_nPrecision = val; }

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) ;
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
private:
    short m_nPrecision;
};


