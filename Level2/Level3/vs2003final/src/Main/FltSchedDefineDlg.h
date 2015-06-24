#pragma once
// FltSchedDefineDlg.h : header file
//

#include "inputs\flight.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CFltSchedDefineDlg dialog
class Flight;
class InputTerminal;
class CFltSchedDefineDlg : public CDialog
{
// Construction
public:
	CFltSchedDefineDlg( const Flight& _flight, InputTerminal* _pInTerm, CWnd* pParent = NULL);
	CFltSchedDefineDlg(InputTerminal* _pInTerm, CWnd* pParent = NULL);   // standard constructor

	BOOL SniffDateChanged();
	BOOL m_bStartDateChanged;
	void GetFlight( Flight& _flight );

	enum { IDD = IDD_DIALOG_FLTSCHDDEFINE };

	//Delete by adam 2007-04-24
	//CDateTimeCtrl	m_dtCtrlDepDate;
	//CDateTimeCtrl	m_dtCtrlArrDate;
	//CButton	m_btnDepDate;
	//CButton	m_btnArrDate;
	//End Delete by adam 2007-04-24

	//add by adam 2007-04-24
	CEdit  m_editDayOne;
	CEdit  m_editDayTwo;
	CSpinButtonCtrl m_sbtnDayOne;
	CSpinButtonCtrl m_sbtnDayTwo;
	//End add by adam 2007-04-24

	CDateTimeCtrl	m_gateOccTime;
	CDateTimeCtrl	m_dtCtrlDep;
	CDateTimeCtrl	m_dtCtrlArr;
	CButton	m_btnDep;
	CButton	m_btnArr;
	CEdit	m_editDepID;
	CEdit	m_editArrID;
	CComboBox	m_comboOrig;
	CComboBox   m_comboFrom;
	CComboBox   m_comboTo;
	CComboBox	m_comboDest;
	CComboBox	m_comboAirline;
	CComboBox	m_comboACType;
	CString	m_csAirline;
	CString	m_csDest;
	CString	m_csOrig;
	CString m_csFrom;
	CString m_csTo;
	CString	m_csArrID;
	CString	m_csDepID;
	CString	m_csACType;

	COleDateTime	m_arrTime;
	COleDateTime	m_depTime;

	//Add by adam 2007-04-24
	ElapsedTime m_estArrTime;
	ElapsedTime m_estDepTime;
	//End Add by adam 2007-04-24

	COleDateTime	m_oGateOccTime;

	//Delete by adam 2007-04-24
	//COleDateTime	m_arrDate;
	//COleDateTime	m_depDate;
	//End Delete by adam 2007-04-24

protected:
	DECLARE_MESSAGE_MAP()

	InputTerminal* m_pInTerm;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCheckArr();
	afx_msg void OnCheckDep();
	afx_msg void OnKillfocusDatetimepickerDep(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusDatetimepickerArr(NMHDR* pNMHDR, LRESULT* pResult);

	//delete by adam 2007-04-24
	//afx_msg void OnCheckArrdate();
	//afx_msg void OnCheckDepdate();
	//afx_msg void OnKillfocusDatetimepickerDepdate(NMHDR* pNMHDR, LRESULT* pResult);
	//afx_msg void OnKillfocusDatetimepickerArrdate(NMHDR* pNMHDR, LRESULT* pResult);
	//end delete by adam 2007-04-24

	afx_msg void OnCheckArrstopover();
	afx_msg void OnCheckDepstopover();

	// base on the current status, define data and read only feature.
	void SetGateOccupancyControl();
	
	bool IsStringAllAlpha( const CString& _str );
	bool IsStringAllDigit( const CString& _str );
	bool m_bNewFlight;
	Flight m_flight;
	CButton m_btnArrStopover;
	CButton m_btnDepStopover;
public:
	//CComboBox m_cmbStar;
	//CComboBox m_cmbSID;
	afx_msg void OnDeltaposSpinDayone(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinDaytwo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusEditDayone();
	afx_msg void OnEnKillfocusEditDaytwo();
	afx_msg void OnDtnDatetimechangeDatetimepickerArr(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepickerDep(NMHDR *pNMHDR, LRESULT *pResult);
};
