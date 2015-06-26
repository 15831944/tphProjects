#if !defined(AFX_INPLACECOMBOBOX_H__D2CEF971_FE37_4347_9D65_9EB5BBC3D0E8__INCLUDED_)
#define AFX_INPLACECOMBOBOX_H__D2CEF971_FE37_4347_9D65_9EB5BBC3D0E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InPlaceComboBox.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CInPlaceComboBox window

class CInPlaceComboBox : public CComboBox
{
// Construction
public:
	CInPlaceComboBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceComboBox();

	// Generated message map functions
protected:
	int m_nSelIdx;
	//{{AFX_MSG(CInPlaceComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCloseup();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPLACECOMBOBOX_H__D2CEF971_FE37_4347_9D65_9EB5BBC3D0E8__INCLUDED_)
