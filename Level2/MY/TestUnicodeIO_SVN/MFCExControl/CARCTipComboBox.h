#ifndef CARCTipComboBox_H
#define CARCTipComboBox_H

#include "CARCTipListBox.h"
#include "MFCExControlAPI.h"

/////////////////////////////////////////////////////////////////////////////
// CARCTipComboBox window

class MFCEXCONTROL_API CARCTipComboBox : public CComboBox
{
	// Construction
public:
	CARCTipComboBox();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CARCTipComboBox)
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Implementation
protected:
	CARCTipListBox	m_listbox;
	HWND			m_hWndToolTip;
	TOOLINFO		m_ToolInfo;
public:
	HINSTANCE m_hExeInstance;

	// Generated message map functions
protected:
	//{{AFX_MSG(CARCTipComboBox)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //CARCTipComboBox_H
