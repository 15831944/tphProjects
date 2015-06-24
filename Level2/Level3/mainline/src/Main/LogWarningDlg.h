#pragma once


// CLogWarningDlg dialog

class CLogWarningDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogWarningDlg)

public:
	CLogWarningDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLogWarningDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_LOGWARNING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTurnoff();
	afx_msg void OnBnClickedButtonKeepon();
	afx_msg void OnBnClickedButtonLogsetting();
};
