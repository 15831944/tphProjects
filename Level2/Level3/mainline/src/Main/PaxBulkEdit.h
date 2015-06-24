#include "afxwin.h"
#include "afxcmn.h"
#if !defined(AFX_PAXBULKEDIT_H__A1D57914_A7FD_44F8_AF0A_EA0DA8FA262B__INCLUDED_)
#define AFX_PAXBULKEDIT_H__A1D57914_A7FD_44F8_AF0A_EA0DA8FA262B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxBulkEdit.h : header file
//
#include "../MFCExControl/ListCtrlEx.h"
/////////////////////////////////////////////////////////////////////////////
// CPaxBulkEdit dialog

class CPaxBulkEdit : public CDialog
{
// Construction
public:
	CPaxBulkEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPaxBulkEdit)
	enum { IDD = IDD_DIALOG_EDITBULK };
	CDateTimeCtrl	m_frequencyCtrl;
	CDateTimeCtrl	m_startTimeCtrl;
	CDateTimeCtrl	m_endtimeCtrl;
	CDateTimeCtrl	m_endRangeCtrl;
	CDateTimeCtrl	m_beginrangeCtrl;
	CSpinButtonCtrl	m_Spin;
	BOOL	m_BeforeCheck;
	int	m_Capacity;
	COleDateTime	m_FrequencyTime;
	//}}AFX_DATA

protected:
	// do not access time values from external, because they have not plus the day-value
	COleDateTime	m_FreqStartTime;
	COleDateTime	m_FreqEndTime;


public:
	virtual BOOL OnInitDialog();
	void InitListCtrlHeader();
	void SetListCtrlContent();
	void UpdatePercentData();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxBulkEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPaxBulkEdit)
	virtual void OnOK();
	afx_msg void OnRADIOafter();
	afx_msg void OnRADIObefore();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnEndlabeleditPercent(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusmFrequencytime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusBeginrangetime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusEndrangetime(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editFreqStartDay;
	CEdit m_editFreqEndDay;
	CSpinButtonCtrl m_spinFreqStartDay;
	CSpinButtonCtrl m_spinFreqEndDay;

	// please access these time values from external
	COleDateTime m_freqStartTime;
	COleDateTime m_freqEndTime;
	COleDateTime m_BeginRangeTime;
	COleDateTime m_EndRangeTime;
	CListCtrlEx m_wndListCtrl;
	std::vector<int> m_vBulkPercent;

	void InitTimeValues(int& dayFreqStart, int& dayFreqEnd);
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAXBULKEDIT_H__A1D57914_A7FD_44F8_AF0A_EA0DA8FA262B__INCLUDED_)
