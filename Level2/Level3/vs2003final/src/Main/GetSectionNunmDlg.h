#pragma once


// GetSectionNunmDlg dialog

class GetSectionNunmDlg : public CDialog
{
	DECLARE_DYNAMIC(GetSectionNunmDlg)

public:
	GetSectionNunmDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~GetSectionNunmDlg();

	int m_point_num;

// Dialog Data
	enum { IDD = IDD_SET_SECTION_POINT_NUM_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
