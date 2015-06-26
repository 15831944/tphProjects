#pragma once
#include <afxdtctl.h>

class CInPlaceDateTimeCtrl : public CDateTimeCtrl
{
public:
	CInPlaceDateTimeCtrl();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual ~CInPlaceDateTimeCtrl();

	// Generated message map functions
protected:

	//When focus killed, send message to its parent or grand-parent dialog.
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	//Initialization
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	//If ESC is pushed, etc
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	//Release the memory
	afx_msg void OnNcDestroy();

private:
	BOOL m_bESC;
	DECLARE_MESSAGE_MAP()
};