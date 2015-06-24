#if !defined(AFX_ECONLISTCTRL_H__16706833_D75C_406A_A8C4_68E6147F8CF6__INCLUDED_)
#define AFX_ECONLISTCTRL_H__16706833_D75C_406A_A8C4_68E6147F8CF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EconListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEconListCtrl window

class CEconListCtrl : public CListCtrl
{
// Construction
public:
	CEconListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEconListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEconListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEconListCtrl)
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECONLISTCTRL_H__16706833_D75C_406A_A8C4_68E6147F8CF6__INCLUDED_)
