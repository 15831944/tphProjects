#if !defined(AFX_DlgAirportProperties_H__081CEDEF_FDE3_4181_BC20_755ED6FF2EDC__INCLUDED_)
#define AFX_DlgAirportProperties_H__081CEDEF_FDE3_4181_BC20_755ED6FF2EDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAirportProperties.h : header file
//

class CAirport;
#include "LLCtrl.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgAirportProperties dialog

class CDlgAirportProperties : public CDialog
{
// Construction
public:
	CDlgAirportProperties(CAirport* pAirport, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAirportProperties)
	enum { IDD = IDD_AIRPORTPROPERTIES };
	CString	m_sIATACode;
	CString	m_sLongName;
    CString m_sICAOCode;
	CString	m_sARPCoordinates;
	CString	m_sCity;
	CString	m_sCountry;
	CString	m_sCountryCode;
	CString	m_sElevation;
	CString	m_sNumberOfRunways;
	CString	m_sMaximumRunwayLength;
	CString	m_sAlternateAirportICAOCode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAirportProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CAirport* m_pAirport;


//	CLLCtrl m_pLongitude;
//	CLLCtrl m_pLatitude;
	unsigned char m_nLongitude[4];
	unsigned char m_nLatitude[4];

	// Generated message map functions
	//{{AFX_MSG(CDlgAirportProperties)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CLLCtrl m_pLongitude;
	CLLCtrl m_pLatitude;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DlgAirportProperties_H__081CEDEF_FDE3_4181_BC20_755ED6FF2EDC__INCLUDED_)
