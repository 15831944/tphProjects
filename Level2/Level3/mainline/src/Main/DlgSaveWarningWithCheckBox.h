#pragma once
#include "afxwin.h"


// CDlgSaveWarningWithCheckBox dialog

class CDlgSaveWarningWithCheckBox : public CDialog
{
	DECLARE_DYNAMIC(CDlgSaveWarningWithCheckBox)

public:
	CDlgSaveWarningWithCheckBox(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSaveWarningWithCheckBox();

// Dialog Data
	enum { IDD = IDD_DIALOG_WARNINGWITH_CHECKBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CButton m_CheckBox;
public:
	BOOL IsCheck() ;
protected:
	BOOL m_IsCheck ;
protected:
	void OnOK() ;
};
