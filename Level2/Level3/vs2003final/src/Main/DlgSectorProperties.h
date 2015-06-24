#if !defined(AFX_DlgSectorProperties_H__8D9902F2_25B9_4820_90B4_B09D292843B7__INCLUDED_)
#define AFX_DlgSectorProperties_H__8D9902F2_25B9_4820_90B4_B09D292843B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSectorProperties.h : header file
//

class CSector;
class CAirportsManager;

/////////////////////////////////////////////////////////////////////////////
// CDlgSectorProperties dialog

class CDlgSectorProperties : public CDialog
{
// Construction
public:
	CDlgSectorProperties(CAirportsManager* _pAirportMan, CSector* pSector, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSectorProperties)
	enum { IDD = IDD_SECTORPROPERTIES };
	CString	m_sAirports;
	CString	m_sSectorName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSectorProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSector*			m_pSector;
	CAirportsManager*	m_pAirportMan;
	// Generated message map functions
	//{{AFX_MSG(CDlgSectorProperties)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DlgSectorProperties_H__8D9902F2_25B9_4820_90B4_B09D292843B7__INCLUDED_)
