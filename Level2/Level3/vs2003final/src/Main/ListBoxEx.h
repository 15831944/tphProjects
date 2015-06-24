#if !defined(AFX_LISTBOXEX_H__8370402B_C1E7_4036_80D7_BDA994D1E90D__INCLUDED_)
#define AFX_LISTBOXEX_H__8370402B_C1E7_4036_80D7_BDA994D1E90D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListBoxEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListBoxEx window

class CListBoxEx : public CListBox
{
// Construction
public:
	CListBoxEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListBoxEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListBoxEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListBoxEx)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CBrush m_br;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTBOXEX_H__8370402B_C1E7_4036_80D7_BDA994D1E90D__INCLUDED_)
