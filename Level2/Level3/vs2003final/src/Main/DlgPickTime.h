#if !defined(AFX_DLGPICKTIME_H__1F34D504_3E6C_41F8_8576_B03CC816CBE3__INCLUDED_)
#define AFX_DLGPICKTIME_H__1F34D504_3E6C_41F8_8576_B03CC816CBE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPickTime.h : header file
//

#include "../common/elaptime.h"
#include "../common/StartDate.h"
#include "AnimationData.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgPickTime dialog

enum Time_Category{TC_ABSOLUTE, TC_RELATIVE};

class CDlgPickTime : public CDialog
{
// Construction
public:
	Time_Category& GetTimeCategory();
	void GetDateTime(ElapsedTime& _eTime, int& _nDayIndex);
	void GetDateTime(CStartDate& _sDate, ElapsedTime& _eTime);
	void InitControl();
//	void SetDateTime(int _dayIndex, const ElapsedTime& _eTime, );
	//void SetDateTime(const CStartDate& _sDate, const ElapsedTime& _eTime);
	void  SetDateTime(const CStartDate& _sDate, AnimationData& animData);
	CDlgPickTime( CWnd* pParent = NULL );   // standard constructor
    long m_EndTime;
	ElapsedTime GetTime();// { return m_nTime; }

// Dialog Data
	//{{AFX_DATA(CDlgPickTime)
	enum { IDD = IDD_ANIMPICKTIME };
	CDateTimeCtrl	m_dtCtrlAnimDate;
	CComboBox	m_cmbAnimDateIndex;
	COleDateTime	m_dtAnimPickTime;
	COleDateTime	m_dtAnimPickDate;
	int		m_nDayIndex;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPickTime)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	long m_nTime; //in 1/100s of a sec
	Time_Category m_tc;
	CStartDate		m_startDate;

	// Generated message map functions
	//{{AFX_MSG(CDlgPickTime)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	void SetDateTime(COleDateTime _dtDateTime);
	void GetDateTime(COleDateTime& _dtDateTime);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPICKTIME_H__1F34D504_3E6C_41F8_8576_B03CC816CBE3__INCLUDED_)
