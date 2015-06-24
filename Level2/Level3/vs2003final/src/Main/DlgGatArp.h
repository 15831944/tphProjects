#pragma once

#include "LLCtrl.h"
// CDlgGatArp dialog
#include "TermPlanDoc.h"

class CDlgGatArp : public CDialog
{
	DECLARE_DYNAMIC(CDlgGatArp)

public:
	CDlgGatArp(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgGatArp();

public:	
	CLLCtrl m_pLongitude;
	CLLCtrl m_pLatitude;
	CString m_strLongitude;
	CString m_strLatitude;

	double m_dElevation;

// Dialog Data
	enum { IDD = IDD_DIALOG_GETARP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
