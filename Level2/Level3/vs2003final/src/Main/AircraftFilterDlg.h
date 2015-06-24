#pragma once

#include "FlightType.h"	// Added by ClassView
#include "resource.h"
// AircraftFilterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAircraftFilterDlg dialog

class CAircraftFilterDlg : public CDialog
{
public:
	CAircraftFilterDlg(CWnd* pParent);

	void SetFlightType( const CFlightType& m_flightType );
	CFlightType& GetFlightType( ){ return m_flightType; }
	
	enum { IDD = IDD_DIALOG_FLIGHTTYPE };

protected:
	CButton	m_btnNotPaxCon;
	CButton	m_btnNotDeathTime;
	CButton	m_btnNotBirthTime;
	CButton	m_btnNotArea;
	CDateTimeCtrl	m_dtCtrlDeathStart;
	CDateTimeCtrl	m_dtCtrlDeathEnd;
	CDateTimeCtrl	m_dtCtrlBirthStart;
	CDateTimeCtrl	m_dtCtrlBirthEnd;
	CButton	m_btnDeathTime;
	CButton	m_btnBirthTime;
	CButton	m_radioPortal;
	CButton	m_radioArea;
	CDateTimeCtrl	m_dtCtrlAreaEnd;
	CDateTimeCtrl	m_dtCtrlAreaStart;
	CButton	m_btnApplied;
	CComboBox	m_comboAreaPortal;
	CButton	m_btnAreaPortal;
	CString	m_csName;
	CString	m_csExpre;
	CString	m_csPaxCon;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void LoadPortal();
	void LoadArea();
	void ShowExpresion();
	CFlightType m_flightType;

	void EnableDeathTimeControl( BOOL _bEnable );
	void EnableBirthTimeControl( BOOL _bEnable );
	void EnableAreaPortalControl( BOOL _bEnable );
	void EnableNotDeathTimeControl( BOOL _bEnable );
	void EnableNotBirthTimeControl( BOOL _bEnable );
	void EnableNotAreaPortalControl( BOOL _bEnable );

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
	DECLARE_MESSAGE_MAP()
};