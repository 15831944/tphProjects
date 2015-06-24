#pragma once
#include "afxwin.h"

#include "..\common\EchoSystem.h"

// CLogClearingDlg dialog

class CLogClearingDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogClearingDlg)

public:
	CLogClearingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLogClearingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_LOGCLEARING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void InitList();
private:
	CString filePath;

public:
	CCheckListBox m_clbLogFiles;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonAll();
	afx_msg void OnBnClickedButtonSelected();
	afx_msg void OnBnClickedButtonClose();
};
