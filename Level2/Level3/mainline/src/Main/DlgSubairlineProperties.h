#if !defined(AFX_DLGSUBAIRLINEPROPERTIES_H__4FFEEB27_D70D_45F6_8C8E_2F8F4C17A605__INCLUDED_)
#define AFX_DLGSUBAIRLINEPROPERTIES_H__4FFEEB27_D70D_45F6_8C8E_2F8F4C17A605__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSubairlineProperties.h : header file
//

class CSubairline;
class CAirlinesManager;
/////////////////////////////////////////////////////////////////////////////
// CDlgSubairlineProperties dialog

class CDlgSubairlineProperties : public CDialog
{
// Construction
public:
	CDlgSubairlineProperties(CAirlinesManager* _pAirlineMan, CSubairline* pSubairline, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSubairlineProperties)
	enum { IDD = IDD_SUBAIRLINEPROPERTIES };
	CString	m_sAirlines;
	CString	m_sSubairlineName;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSubairlineProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    CSubairline*		m_pSubairline;
	CAirlinesManager*	m_pAirlineMan;
	// Generated message map functions
	//{{AFX_MSG(CDlgSubairlineProperties)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSUBAIRLINEPROPERTIES_H__4FFEEB27_D70D_45F6_8C8E_2F8F4C17A605__INCLUDED_)
