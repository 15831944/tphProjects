#if !defined(AFX_FLIGHTSCHEDULEADDNEWDLG_H__10A944B7_C3BF_409B_BC11_9252E333F89A__INCLUDED_)
#define AFX_FLIGHTSCHEDULEADDNEWDLG_H__10A944B7_C3BF_409B_BC11_9252E333F89A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlightScheduleAddNewDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFlightScheduleAddNewDlg dialog
#include "FlightScheduleAddUtil.h"
#include "inputs\StandAssignmentMgr.h"
#include "..\MFCExControl\ListCtrlEx.h"

class CTermPlanDoc;
class CFlightScheduleAddNewDlg : public CDialog
{
	//std::vector<CFlightScheduleAddUtil>*m_pAllSchduleUtil;
	InputTerminal* m_pTerm;
	CTermPlanDoc* m_pDoc;
	bool m_bAutoAssign;
	bool m_bReAssign;
	StandAssignmentMgr m_gateAssignmentMgr;
	CToolBar m_ToolBar;
// Construction
public:
	CFlightScheduleAddNewDlg(InputTerminal* _pTerm,CTermPlanDoc* _pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFlightScheduleAddNewDlg)
	enum { IDD = IDD_DIALOG_FLIGHTSCHDULE_ADDBYRANDOM };
	CButton	m_checkReAssign;
	CButton	m_checkAutoAssign;
	CStatic	m_toolbarcontenter;
	CListCtrlEx	m_listProperty;
	CButton	m_btSave;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlightScheduleAddNewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFlightScheduleAddNewDlg)
	afx_msg void OnSave();	
	afx_msg void OnCheckAutoassign();
	afx_msg void OnCheckReassign();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();	
	virtual void OnCancel();
	afx_msg void OnItemchangedListValue(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void GatePriorityAssign( bool _bReAssign );
	void SetListCtrl();
	void ReloadListData( CFlightScheduleAddUtil* _pSelect = NULL );
	void InitToolbar();
private:
	int m_iRowIdx;
	int m_iColumnIdx;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLIGHTSCHEDULEADDNEWDLG_H__10A944B7_C3BF_409B_BC11_9252E333F89A__INCLUDED_)
