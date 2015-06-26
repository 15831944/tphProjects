#pragma once
#include "afxwin.h"
class Terminal;

class CDlgSelectPortal :
	public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectPortal)

public:
	CDlgSelectPortal(Terminal* pTermina, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectPortal();

	// Dialog Data
	enum { IDD = IDD_DIALOG_SELECTPORTAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CString m_strPortal;
	CListBox m_lstPortal;
	Terminal* m_pTerminal;
	DECLARE_MESSAGE_MAP()
public:

	const CString& GetSelectPortal(){return m_strPortal;};
	virtual BOOL OnInitDialog();


protected:
	virtual void OnOK();
};

