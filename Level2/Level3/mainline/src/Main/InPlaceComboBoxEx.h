#if !defined(AFX_INPLACECOMBOBOXEX_H__8DA66B2A_F9F5_4126_91E0_60A4B364D49E__INCLUDED_)
#define AFX_INPLACECOMBOBOXEX_H__8DA66B2A_F9F5_4126_91E0_60A4B364D49E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InPlaceComboBoxEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInPlaceComboBoxEx window

class CInPlaceComboBoxEx : public CComboBoxEx
{
// Construction
public:
	CInPlaceComboBoxEx();
	CInPlaceComboBoxEx(UINT nID);

// Attributes
public:
protected:
	UINT m_nID;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceComboBoxEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceComboBoxEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceComboBoxEx)
	afx_msg void OnCloseup();
	afx_msg void OnKillfocus();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPLACECOMBOBOXEX_H__8DA66B2A_F9F5_4126_91E0_60A4B364D49E__INCLUDED_)
