#pragma once
#include "afxwin.h"

class MFCEXCONTROL_API CCoolTreeEdit :
	public CEdit
{
	// Construction
public:
	CCoolTreeEdit();

	// Attributes
public:

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceEdit2)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CCoolTreeEdit();

	// Generated message map functions
protected:
	bool m_bESC;   //To indicate whether ESC key was pressed

	//{{AFX_MSG(CInPlaceEdit2)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};