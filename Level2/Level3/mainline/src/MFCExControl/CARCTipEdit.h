#pragma once

#include "MFCExControlAPI.h"


/////////////////////////////////////////////////////////////////////////////
// CARCTipListBox window

class MFCEXCONTROL_API CARCTipEdit : public CEdit
{
	// Construction
public:
	CARCTipEdit();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CARCTipEdit)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Implementation
protected:
	HWND			m_hWndToolTip;
	TOOLINFO		m_ToolInfo;

	// Generated message map functions
protected:
	//{{AFX_MSG(CARCTipEdit)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

