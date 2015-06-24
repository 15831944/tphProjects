#pragma once
#include "afxcmn.h"
#include "..\MFCExControl\CoolTree.h"
#include "afxwin.h"



// CLogSettingDlg dialog

class CLogSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogSettingDlg)

public:
	CLogSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLogSettingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_LOGSETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// list the Log Type data
	CCheckListBox m_lbLogs;
	CButton* pm_MasterOn;
	CButton* pm_MasterOff;

protected:
	void InitList();
	void LoadData();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();	
	afx_msg void OnBnClickedRadioMasteron();
	afx_msg void OnBnClickedRadioMasteroff();
};
