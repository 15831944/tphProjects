#pragma once
#include "afxwin.h"


// CDlgSelectRunwayOperation dialog

class CDlgSelectRunwayOperation : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectRunwayOperation)

public:
	CDlgSelectRunwayOperation(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectRunwayOperation();

// Dialog Data
	enum { IDD = IDD_DIALOG_REPORTSELECTRUNWAYOPERATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	int m_nCursel;
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	CListBox m_lbOperation;
};
