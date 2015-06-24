#if !defined(AFX_SIMTIMERANGEDLG_H__8A17D6D9_9358_4766_BFA5_EAEA28E46FE7__INCLUDED_)
#define AFX_SIMTIMERANGEDLG_H__8A17D6D9_9358_4766_BFA5_EAEA28E46FE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SimTimeRangeDlg.h : header file
//

#include "inputs\SimParameter.h"

/////////////////////////////////////////////////////////////////////////////
// CSimTimeRangeDlg dialog

class CSimTimeRangeDlg : public CDialog
{
// Construction
public:
	CSimTimeRangeDlg( CSimParameter* _pSimParam, CWnd* pParent = NULL);   // standard constructor
	void InitControl();


// Dialog Data
	//{{AFX_DATA(CSimTimeRangeDlg)
	enum { IDD = IDD_DIALOG_SIMULATE_TIMERANGE };
	CStatic	m_StaticDayCtrl;
	CStatic	m_StaticDayCtrl2;
	CSpinButtonCtrl	m_PlusDaySpinCtrl;
	CSpinButtonCtrl	m_PlusDaySpinCtrl2;
	CEdit	m_PlusDayEditCtrl;
	CEdit	m_PlusDayEditCtrl2;
	CDateTimeCtrl	m_dtCtrlEnd;
	CDateTimeCtrl	m_dtCtrlStart;
	CDateTimeCtrl	m_dtCtrlFollowupPeriod;
	CDateTimeCtrl	m_dtCtrlInitPeriod;
	COleDateTime	m_dtStart;
	COleDateTime	m_dtInitPeriod;
	COleDateTime	m_dtFollowupPeriod;
	int		m_PlusDayValue;
	int		m_PlusDayValue2;
	COleDateTime	m_dtEnd;
	//}}AFX_DATA
	
//	BOOL	m_bPlusDay;
//	CButton	m_btnPlusDay;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimTimeRangeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSimParameter* m_pSimParam;
	// Generated message map functions
	//{{AFX_MSG(CSimTimeRangeDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposPlusdayspin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDeltaposPlusdayspin2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusPlusdayedit2();
	afx_msg void OnEnKillfocusPlusdayedit();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMTIMERANGEDLG_H__8A17D6D9_9358_4766_BFA5_EAEA28E46FE7__INCLUDED_)
