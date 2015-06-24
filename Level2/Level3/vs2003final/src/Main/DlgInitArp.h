#pragma once

#include "LLCtrl.h"
// CDlgInitArp dialog

class CDlgInitArp : public CDialog
{
	DECLARE_DYNAMIC(CDlgInitArp)

public:
	CDlgInitArp(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgInitArp();

	void SetDefineWayPoint(){ m_bDefineWayPointLL = true; }
	
public:	
	CLLCtrl m_pLongitude;
	CLLCtrl m_pLatitude;
	CString m_strLongitude;
	CString m_strLatitude;

	long m_lLongitude;
	long m_lLatitude;
//	double m_dElevation;

// Dialog Data
	enum { IDD = IDD_DIALOG_GETARP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	bool m_bDefineWayPointLL; 
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
