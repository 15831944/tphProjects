#if !defined(AFX_DLGAIRPORTDBNAMEINPUT_H__8336A0BA_7819_4A07_99A0_9DF6EA7877D5__INCLUDED_)
#define AFX_DLGAIRPORTDBNAMEINPUT_H__8336A0BA_7819_4A07_99A0_9DF6EA7877D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAirportDBNameInput.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAirportDBNameInput dialog

class CDlgAirportDBNameInput : public CDialog
{
// Construction
public:
	CDlgAirportDBNameInput( const CString& szTile,bool bShowDropDown=true, CWnd* pParent = NULL );   // standard constructor
		
// Dialog Data
	//{{AFX_DATA(CDlgAirportDBNameInput)
	enum { IDD = IDD_DIALOG_AIRPORTDB_NAME };
	CString	m_sAirportDBName;
	CString	m_sStaticTitle;
	CString	m_sAirportDBName1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAirportDBNameInput)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bShowDropDown;

	// Generated message map functions
	//{{AFX_MSG(CDlgAirportDBNameInput)
	virtual BOOL OnInitDialog();
	afx_msg void OnEditchangeComboAirportdbName();
	afx_msg void OnSelchangeComboAirportdbName();
	virtual void OnOK();
	afx_msg void OnChangeEditAirportdbName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void EnableChildControl();
		
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGAIRPORTDBNAMEINPUT_H__8336A0BA_7819_4A07_99A0_9DF6EA7877D5__INCLUDED_)
