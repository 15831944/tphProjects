#pragma once


// DlgNewCarryonItemName dialog

class DlgNewCarryonItemName : public CDialog
{
	DECLARE_DYNAMIC(DlgNewCarryonItemName)

public:
	DlgNewCarryonItemName(CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgNewCarryonItemName();

// Dialog Data
	enum { IDD = IDD_DLGNEWCARRYONITEMNAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_strName;
	afx_msg void OnBnClickedCancel();
};
