#if !defined(AFX_SINGLEEVENTSDATADLG_H__22A6A6E7_2773_4DCB_8926_5D0AFA06BE3E__INCLUDED_)
#define AFX_SINGLEEVENTSDATADLG_H__22A6A6E7_2773_4DCB_8926_5D0AFA06BE3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// singleEventsDataDlg.h : header file
//
#include "..\inputs\assign.h"
#include "afxdtctl.h"
#include "atlcomtime.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// CsingleEventsDataDlg dialog

class CsingleEventsDataDlg : public CDialog
{
// Construction
public:
	bool GetDlgData(ProcessorRoster& _procAssign);
	void SetDlgData(CString _strProc, ProcessorRoster& _procAssign);
	CsingleEventsDataDlg(CWnd* pParent, BOOL _bCanModify ,Processor* pro,ProcessorRosterSchedule* p_scedule);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CsingleEventsDataDlg)
	enum { IDD = IDD_DIALOG_EVENTSDATA };
	CButton	m_butDefPaxType;
	CComboBox	m_combState;
	CDateTimeCtrl	m_timeBeginCtrl;
	CDateTimeCtrl	m_timeEndCtrl;
	CString	m_strPaxType;
	COleDateTime	m_timeEnd;
	COleDateTime	m_timeBegin;
	int		m_iRelationRadio;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CsingleEventsDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CsingleEventsDataDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButDefPaxtype();
	afx_msg void OnSelchangeComboState();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	InputTerminal* GetInputTerminal();
	ProcessorRoster m_procAssign;
	CString strProcessorName;
	CString m_strWindowTitle;
	BOOL m_bCanModifyFlag;
	Processor* P_proc ;
	ProcessorRosterSchedule* P_Schedule ;
public:
	CDateTimeCtrl m_timeBeginDateCtrl;
	CDateTimeCtrl m_timeEndDateCtrl;
	COleDateTime m_startDate;
	COleDateTime m_endDate;
	CComboBox m_comboStartDay;
	CComboBox m_comboEndDay;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnDtnDatetimechangeDatetimepickerEnddate(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CButton m_checkDaily;
	BOOL isDaily ;
	BOOL AssigmentData() ;
	void InitComboBeginAndEndDay(int n_day,	int nStartDays ,int nenddays) ;
	void HideComboBoxTime();
	void showComboBoxTime() ;
public:
	afx_msg void OnBnClickedCheckDaily();
protected:
    CRect pos_starttimeCtr ;
	CRect pos_endtimeCtr ;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SINGLEEVENTSDATADLG_H__22A6A6E7_2773_4DCB_8926_5D0AFA06BE3E__INCLUDED_)
