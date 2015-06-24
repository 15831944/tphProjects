#pragma once
#include "afxwin.h"


// CDlgSelectArea dialog
class Terminal;
class CDlgSelectArea : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectArea)

public:
	CDlgSelectArea(Terminal* pTerminal, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectArea();

// Dialog Data
	enum { IDD = IDD_DIALOG_SELECTAREA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CString m_strArea;
	CListBox m_lstArea;
	Terminal *m_pTerminal;
	DECLARE_MESSAGE_MAP()
public:
	
	const CString& GetSelectArea(){return m_strArea;};
	virtual BOOL OnInitDialog();

	
protected:
	virtual void OnOK();
};
