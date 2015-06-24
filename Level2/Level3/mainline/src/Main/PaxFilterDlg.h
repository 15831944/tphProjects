#if !defined(AFX_PAXFILTERDLG_H__86F3143B_2618_46D1_A05C_2A7270C61DB9__INCLUDED_)
#define AFX_PAXFILTERDLG_H__86F3143B_2618_46D1_A05C_2A7270C61DB9__INCLUDED_

#include "PaxType.h"	// Added by ClassView
#include "afxcmn.h"
#include "afxwin.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxFilterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPaxFilterDlg dialog

class CPaxFilterDlg : public CDialog
{
// Construction
public:
	void SetPaxType( const CPaxType& _paxType );
	CPaxType& GetPaxType( ){ return m_paxType; }
	CPaxFilterDlg(CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPaxFilterDlg)
	enum { IDD = IDD_DIALOG_PAXTYPE };
	CButton	m_btnNotPaxCon;
	CButton	m_btnNotDeathTime;
	CButton	m_btnNotBirthTime;
	CButton	m_btnNotArea;
	CButton	m_btnDeathTime;
	CButton	m_btnBirthTime;
	CButton	m_radioPortal;
	CButton	m_radioArea;
	CButton	m_btnApplied;
	CComboBox	m_comboAreaPortal;
	CButton	m_btnAreaPortal;
	CString	m_csName;
	CString	m_csExpre;
	CString	m_csPaxCon;
	CDateTimeCtrl	m_dtCtrlAreaStart;
	CDateTimeCtrl	m_dtCtrlAreaEnd;
	CDateTimeCtrl	m_dtCtrlBirthStart;
	CDateTimeCtrl	m_dtCtrlBirthEnd;
	CDateTimeCtrl	m_dtCtrlDeathStart;
	CDateTimeCtrl	m_dtCtrlDeathEnd;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadPortal();
	void LoadArea();
	void ShowExpression();
	CPaxType m_paxType;

	void EnableAreaTimeControl(BOOL _bEnable);
	void EnableDeathTimeControl( BOOL _bEnable );
	void EnableBirthTimeControl( BOOL _bEnable );
	void EnableAreaPortalControl( BOOL _bEnable );
	void EnableNotDeathTimeControl( BOOL _bEnable );
	void EnableNotBirthTimeControl( BOOL _bEnable );
	void EnableNotAreaPortalControl( BOOL _bEnable );

	// Generated message map functions
	//{{AFX_MSG(CPaxFilterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckAreaportal();
	afx_msg void OnCheckBirth();
	afx_msg void OnCheckDeath();
	virtual void OnOK();
	afx_msg void OnCheckNotarea();
	afx_msg void OnCheckNotbirthtime();
	afx_msg void OnCheckNotdeathtime();
	afx_msg void OnCheckApplied();
	afx_msg void OnDatetimechangeDatetimepickerAreastart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeDatetimepickerAreaend(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeDatetimepickerBirthend(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeDatetimepickerBirthstart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeDatetimepickerDeathend(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeDatetimepickerDeathstart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadioArea();
	afx_msg void OnRadioPortal();
	afx_msg void OnButtonPaxcon();
	afx_msg void OnCheckNotpaxcon();
	afx_msg void OnCloseupComboAreaportal();
	afx_msg void OnKillfocusEditName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CSpinButtonCtrl m_spinAreaStartDay;
	CSpinButtonCtrl m_spinAreaEndDay;
	CSpinButtonCtrl m_spinBirthStartDay;
	CSpinButtonCtrl m_spinBirthEndDay;
	CSpinButtonCtrl m_spinDeathStartDay;
	CSpinButtonCtrl m_spinDeathEndDay;
	afx_msg void OnDeltaposSpinAreastartday(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinAreaendday(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinBirthstartday(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinBirthendday(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinDeathstartday(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinDeathendday(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditAreastartday();
	afx_msg void OnEnChangeEditAreaendday();
	afx_msg void OnEnChangeEditBirthstartday();
	afx_msg void OnEnChangeEditBirthendday();
	afx_msg void OnEnChangeEditDeathstartday();
	afx_msg void OnEnChangeEditDeathendday();
	CEdit m_editAreaStartDay;
	CEdit m_editAreaEndDay;
	CEdit m_editBirthStartDay;
	CEdit m_editBirthEndDay;
	CEdit m_editDeathStartDay;
	CEdit m_editDeathEndDay;
	void SafeShowExpression(void);

private:
	BOOL m_bInitilized;
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAXFILTERDLG_H__86F3143B_2618_46D1_A05C_2A7270C61DB9__INCLUDED_)
