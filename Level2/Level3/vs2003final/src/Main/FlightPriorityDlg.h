#if !defined(AFX_FLIGHTPRIORITYDLG_H__7656395A_F788_45B2_97FD_272AC9A49EAF__INCLUDED_)
#define AFX_FLIGHTPRIORITYDLG_H__7656395A_F788_45B2_97FD_272AC9A49EAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlightPriorityDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFlightPriorityDlg dialog

#include "ProcessorTreeCtrl.h"
#include "ConDBListCtrl.h"
#include "TermPlanDoc.h"
#include "FlightGateSelection.h"
#include "vector"
#include "inputs\flightPriorityInfo.h"


class CFlightPriorityDlg : public CXTResizeDialog
{
public:
	std::vector<FlightGate> m_vectFlightGate;
	
protected:
		void InitToolbar();
		InputTerminal* GetInputTerminal();
		void SetListCtrl();
		CToolBar m_ToolBar;
		int GetSelectedListItem();
		CString GetProjPath();
		int GetSelectedItem();
		bool m_bPaintInit;
		void ResetOrder();

		int m_nprjID;
		int m_nAirportID;

// Construction
public:
	CFlightPriorityDlg(int nPrjID, int nAirportID, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFlightPriorityDlg)
	enum { IDD = IDD_DIALOG_FLIGHT_PRIORITY };
	CButton	m_btnSave;
	CConDBListCtrl	m_listctrlData;
	CProcessorTreeCtrl	m_treeProc;
	CStatic	m_toolbarcontenter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlightPriorityDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFlightPriorityDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPeoplemoverDelete();
	afx_msg void OnPeoplemoverNew();
	afx_msg void OnArrowUp();
	afx_msg void OnArrowDown();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListFlightPriority(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonSave();
	afx_msg void OnButtonEdit();
	virtual void OnOK();
	afx_msg void OnItemchangedListFlightPriority(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLIGHTPRIORITYDLG_H__7656395A_F788_45B2_97FD_272AC9A49EAF__INCLUDED_)
