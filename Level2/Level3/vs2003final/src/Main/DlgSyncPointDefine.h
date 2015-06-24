#pragma once


// CDlgSyncPointDefine dialog

class CDlgSyncPointDefine : public CDialog
{
	DECLARE_DYNAMIC(CDlgSyncPointDefine)

public:
	CDlgSyncPointDefine(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSyncPointDefine();

// Dialog Data
	enum { IDD = IDD_DIALOG_SYNCPOINTDEFINE };

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	CString GetSyncName()const;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CString m_syncName;
};
