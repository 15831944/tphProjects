#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "..\common\EchoSystem.h"

// CLogDisplayDlg dialog

class CLogDisplayDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogDisplayDlg)

public:
	CLogDisplayDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLogDisplayDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_LOGDISPLAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void InitListBox();
	void InitListControl();
	void DisplayLogDetails(CString logTypeName);
private:
	CString filePath;

public:
	virtual BOOL OnInitDialog();
	CListBox m_lbLogType;
	afx_msg void OnLbnSelchangeListLogtype();
	CListCtrl m_lcLogDetail;
};
