#ifndef CARCTipListBox_H
#define CARCTipListBox_H

/////////////////////////////////////////////////////////////////////////////
// CARCTipListBox window

class CARCTipListBox : public CListBox
{
	// Construction
public:
	CARCTipListBox();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CARCTipListBox)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Implementation
protected:
	HWND			m_hWndToolTip;
	TOOLINFO		m_ToolInfo;
public:
	HWND GetToolTipHanlder(void){return (m_hWndToolTip);}
	// Generated message map functions
protected:
	//{{AFX_MSG(CARCTipListBox)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //CARCTipListBox_H