#pragma once
#include "afxwin.h"
#include "../MFCExControl/CARCTipComboBox.h"

class CInplaceComboBox2 :
	public CARCTipComboBox
{
public:
	CInplaceComboBox2(void);
	virtual ~CInplaceComboBox2(void);
	int m_nSelIdx;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKillComboFocus();
};
