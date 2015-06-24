#pragma once
#include "../Common/elaptime.h"
#include "afxcmn.h"

class CDlgTimeRange : public CDialog
{
public:
	CDlgTimeRange(const ElapsedTime& startTime, const ElapsedTime& endTime, BOOL bDaily = FALSE, CWnd* pParent = NULL);
	virtual ~CDlgTimeRange();

	enum { IDD = IDD_TIMERANGE };

	ElapsedTime GetStartTime() ;
	ElapsedTime GetEndTime() ;
	CString GetStartTimeString();
	CString GetEndTimeString();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();

	//CComboBox m_wndCmbStartDay;
	//CComboBox m_wndCmbEndDay;
	int m_nStartDay;
	int m_nEndDay;
	COleDateTime m_startTime;
	COleDateTime m_endTime;

	CDateTimeCtrl m_StarTimeCtrl;
	CDateTimeCtrl m_EndTimeCtrl;

	void ConvertElapsedTime(int& nDay, COleDateTime& time, const ElapsedTime& elapsedTime);
	void UpdateDataEx();
	afx_msg void OnSelChangeStartDay();
	afx_msg void OnSelChangeEndDay();

public:
	afx_msg void OnDeltaposSpinStartday(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinEndday(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CSpinButtonCtrl m_spinStartDay; 
	CSpinButtonCtrl m_spinEndDay;

	BOOL m_bDaily;
};
