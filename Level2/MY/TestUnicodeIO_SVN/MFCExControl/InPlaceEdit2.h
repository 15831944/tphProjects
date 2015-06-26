#if !defined(AFX_INPLACEEDIT2_H__F6F2EFAD_82D8_49DC_A2C3_89E455396B0E__INCLUDED_)
#define AFX_INPLACEEDIT2_H__F6F2EFAD_82D8_49DC_A2C3_89E455396B0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InPlaceEdit2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit2 window

class CInPlaceEdit2 : public CEdit
{
// Construction
public:
	CInPlaceEdit2();

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
	virtual ~CInPlaceEdit2();

	// Generated message map functions
protected:
	bool m_bESC;   //To indicate whether ESC key was pressed

	//{{AFX_MSG(CInPlaceEdit2)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPLACEEDIT2_H__F6F2EFAD_82D8_49DC_A2C3_89E455396B0E__INCLUDED_)
