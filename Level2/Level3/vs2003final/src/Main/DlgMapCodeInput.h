#pragma once


// CDlgMapCodeInput dialog

class CDlgMapCodeInput : public CDialog
{
	DECLARE_DYNAMIC(CDlgMapCodeInput)

public:
	CDlgMapCodeInput(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMapCodeInput();

// Dialog Data
	enum { IDD = IDD_DIALOG_MAPCODEINPUT };

public:
	CString GetCode()const {return m_strCode;}
	CString GetProcess()const {return m_strProcess;}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void OnOK();

	DECLARE_MESSAGE_MAP()

protected:
	CString m_strCode;
	CString m_strProcess;
};
