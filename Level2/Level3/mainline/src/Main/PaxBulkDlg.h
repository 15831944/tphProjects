#if !defined(AFX_PAXBULKDLG_H__A3881E85_B8AB_4C73_86C0_F9B2FFCE7D2D__INCLUDED_)
#define AFX_PAXBULKDLG_H__A3881E85_B8AB_4C73_86C0_F9B2FFCE7D2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxBulkDlg.h : header file
//
#include "inputs\MobileElemConstraint.h"
#include "inputs\Paxbulkinfo.h"
/////////////////////////////////////////////////////////////////////////////
// CPaxBulkDlg dialog

class CPaxBulkDlg : public CDialog
{
// Construction
public:
	CPaxBulkDlg(CWnd* pParent = NULL);   // standard constructor

	CPaxBulk m_paxBulk;

// Dialog Data
	//{{AFX_DATA(CPaxBulkDlg)
	enum { IDD = IDD_DIALOG_BULK };
	CDateTimeCtrl	m_timeFrqctrl;
	CDateTimeCtrl	m_timeRangeEctrl;
	CDateTimeCtrl	m_timeRangeBctrl;
	CDateTimeCtrl	m_StarttimeCtr;
	CDateTimeCtrl	m_EndtimeCtr;
	CSpinButtonCtrl	m_spinCapacity;
	int		m_nCapacity;
	COleDateTime	m_timeEnd;
	COleDateTime	m_timeStart;
	CString	m_strPaxType;
	COleDateTime	m_timeRangeB;
	COleDateTime	m_timeRangeE;
	COleDateTime	m_timeFrq;	
	bool            m_bAdd;
	//}}AFX_DATA


	CMobileElemConstraint m_PaxType;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxBulkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bTypeChanged;

	// Generated message map functions
	//{{AFX_MSG(CPaxBulkDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBtselpaxtype();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAXBULKDLG_H__A3881E85_B8AB_4C73_86C0_F9B2FFCE7D2D__INCLUDED_)
