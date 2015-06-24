#if !defined(AFX_FLIGHTGATESELECTION_H__351C26CE_E1AD_4FCD_A31A_CB6C35119721__INCLUDED_)
#define AFX_FLIGHTGATESELECTION_H__351C26CE_E1AD_4FCD_A31A_CB6C35119721__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlightGateSelection.h : header file
//

//#include "FlightDialog.h"
#include "TermPlanDoc.h"
#include "ProcessorTreeCtrl.h"
#include "ConDBListCtrl.h"
#include "Inputs\ProcDB.h"
#include "ALTObjectTreeCtrl.h"
#include "../Common/AirsideFlightType.h"
/////////////////////////////////////////////////////////////////////////////
// CFlightGateSelectionDlg dialog

class CFlightGateSelectionDlg : public CDialog
{
public:
	bool m_bChanged;
	AirsideFlightType m_flight;
	
private:
	int m_nIndex;
	int m_nAirportID;

// Construction
public:
	CFlightGateSelectionDlg(CWnd* pParent, int nAirportID, int nIndex = -1);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFlightGateSelectionDlg)
	enum { IDD = IDD_DIALOG_FLIGHT_GATE_SELECTION };	
	CALTObjectTreeCtrl m_treeALTobject;
	CEdit	m_edtFlightSector;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlightGateSelectionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ReloadDatabase();
	InputTerminal* GetInputTerminal();
	// Generated message map functions
	//{{AFX_MSG(CFlightGateSelectionDlg)
	afx_msg void OnButtonFlightsectorSelection();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLIGHTGATESELECTION_H__351C26CE_E1AD_4FCD_A31A_CB6C35119721__INCLUDED_)
