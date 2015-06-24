#pragma once


// CLicenseExpiredDlg dialog

class CLicenseExpiredDlg : public CDialog
{
	DECLARE_DYNAMIC(CLicenseExpiredDlg)

public:
	CLicenseExpiredDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLicenseExpiredDlg();

// Dialog Data
	enum { IDD = IDD_DLG_LICENSE_EXPIRE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCorpUrl();
	virtual BOOL OnInitDialog();
};
