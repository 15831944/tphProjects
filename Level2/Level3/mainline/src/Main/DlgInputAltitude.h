#pragma once
#include "afxwin.h"


// CDlgInputAltitude dialog

class CDlgInputAltitude : public CDialog
{
	DECLARE_DYNAMIC(CDlgInputAltitude)

public:
	CDlgInputAltitude(double dalt, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgInputAltitude();

// Dialog Data
	enum { IDD = IDD_DIALOG_ALT };

	double getAlt()const;
	//BOOL remberAlt()const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editAlt;
	//CButton m_chkRember;
	//BOOL m_bCheckRem;
	double m_dAlt;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
