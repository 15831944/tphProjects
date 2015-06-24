#if !defined(AFX_ANNUALACTIVITYDEDUCTIONDLG_H__C1D38E92_7E24_4811_B65C_DA8BAAF08BE5__INCLUDED_)
#define AFX_ANNUALACTIVITYDEDUCTIONDLG_H__C1D38E92_7E24_4811_B65C_DA8BAAF08BE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnnualActivityDeductionDlg.h : header file
//


#include "..\economic\EconomicManager.h"

/////////////////////////////////////////////////////////////////////////////
// CAnnualActivityDeductionDlg dialog

class CAnnualActivityDeductionDlg : public CDialog
{
// Construction
public:
	CAnnualActivityDeductionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnnualActivityDeductionDlg)
	enum { IDD = IDD_DIALOG_ANNUALDED };
	CSpinButtonCtrl	m_spinWeekday;
	CSpinButtonCtrl	m_spinOccasionaly2;
	CSpinButtonCtrl	m_spinOccasionaly1;
	CEdit	m_editWeekday;
	CEdit	m_editOccasionaly2;
	CEdit	m_editOccasionaly1;
	CButton	m_btnSave;
	int		m_nSel;
	int		m_nOcPerc1;
	int		m_nOcPerc2;
	int		m_nWDPerc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnnualActivityDeductionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// base on the datamember, set the database
	void SetDatabase();

	void SetControl();

	CEconomicManager* GetEconomicManager();
	
	CString GetProjPath();
	
	void ReloadDatabase();

	void EnableWeekdayOption( BOOL _bEnable );

	void EnableOccasionalyOption( BOOL _bEnable );

	// Generated message map functions
	//{{AFX_MSG(CAnnualActivityDeductionDlg)
	afx_msg void OnRadioEveryday();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinOccasionaly1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinOccasionaly2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinWd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonSave();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANNUALACTIVITYDEDUCTIONDLG_H__C1D38E92_7E24_4811_B65C_DA8BAAF08BE5__INCLUDED_)
