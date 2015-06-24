#if !defined(AFX_DLGACTIVITYDENSITY_H__7C0635F8_E3AA_462F_853A_15B7A700BD34__INCLUDED_)
#define AFX_DLGACTIVITYDENSITY_H__7C0635F8_E3AA_462F_853A_15B7A700BD34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

// DlgActivityDensity.h : header file
//

class CLOSColorsListCtrl : public CListCtrl
{
public:
	CLOSColorsListCtrl();
	virtual ~CLOSColorsListCtrl();

	void FreeItemMemory();

	virtual void DrawItem(LPDRAWITEMSTRUCT lpdis);

	//afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};

struct _LOSinfo {
	CString sTag;
	COLORREF cColor;
};


/////////////////////////////////////////////////////////////////////////////
// CDlgActivityDensity dialog

class CDlgActivityDensity : public CDialog
{

// Construction
public:
	CDlgActivityDensity(CWnd* pParent = NULL);   // standard constructor

	int DoFakeModal();
	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);

// Dialog Data
	//{{AFX_DATA(CDlgActivityDensity)
	enum { IDD = IDD_DIALOG_ACTIVITYDENSITY };
	CComboBox	m_cmbFloor;
	CLOSColorsListCtrl	m_lstColors;
	CButton	m_btnSave;
	double	m_dCPx;
	double	m_dCPy;
	double	m_dSizeX;
	double m_dSizeY;
	double	m_dLOSint;
	int		m_nLOSlevels;
	double	m_dLOSmin;
	int		m_nSubdivsX;
	int		m_nSubdivsY;
	CString	m_sUnits;
	CString	m_sUnits2;
	CString	m_sUnits3;
	CString	m_sUnits4;
	CString	m_sUnits5;
	CString	m_sUnits6;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgActivityDensity)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void RebuildLOSColorsList();

	// Generated message map functions
	//{{AFX_MSG(CDlgActivityDensity)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKillfocusEditLOSlvl();
	afx_msg void OnSelchangeCmbFloor();
	afx_msg void OnDblclkListLoscolors(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditLosmin();
	afx_msg void OnKillfocusEditLosint();
	virtual void OnCancel();
	afx_msg void OnButtonPfm();
	//}}AFX_MSG
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGACTIVITYDENSITY_H__7C0635F8_E3AA_462F_853A_15B7A700BD34__INCLUDED_)
