#pragma once


// CFloarEdit

class MFCEXCONTROL_API CFloarEdit : public CEdit
{
	DECLARE_DYNAMIC(CFloarEdit)

public:
	CFloarEdit();
	virtual ~CFloarEdit();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};


