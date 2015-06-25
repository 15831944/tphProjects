#pragma once
#include "afxwin.h"


// CDlgParametersAny dialog

class  CDlgParametersAny : public CDialog
{
	DECLARE_DYNAMIC(CDlgParametersAny)

public:
	CDlgParametersAny(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgParametersAny();

// Dialog Data
	//enum { IDD = IDD_DIALOG_REPORT_ANYEXT };
	void InitToolBar();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox m_wndListBox;
	CToolBar m_wndToolBar;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
