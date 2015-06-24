#if !defined(AFX_DLGAIRLINEPROPERTIES_H__17C5CC91_1453_46B8_93C9_EAAABE963237__INCLUDED_)
#define AFX_DLGAIRLINEPROPERTIES_H__17C5CC91_1453_46B8_93C9_EAAABE963237__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAirlineProperties.h : header file
//

class CAirline;
class CIPAddrCtl;
/////////////////////////////////////////////////////////////////////////////
// CDlgAirlineProperties dialog

class CDlgAirlineProperties : public CDialog
{
// Construction
public:
	CDlgAirlineProperties(CAirline* pAirline, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAirlineProperties)
	enum { IDD = IDD_AIRLINEPROPERTIES };
		CString	m_sAirlineIATACode;
	    CString	m_sLongName;
	    CString	m_sICAOCode;
	    CString	m_sOtherCodes;
	    CString	m_sStreetAddress;
	    CString	m_sCity;
	    CString	m_sCountry;
	    CString	m_sHQZipCode;
	    CString	m_sContinent;
	    CString	m_sTelephone;
	    CString	m_sFax;
	    CString	m_sEMail;
	    CString	m_sDirector;
	    CString	m_sFleet;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAirlineProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CAirline* m_pAirline;

	// Generated message map functions
	//{{AFX_MSG(CDlgAirlineProperties)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGAIRLINEPROPERTIES_H__17C5CC91_1453_46B8_93C9_EAAABE963237__INCLUDED_)
