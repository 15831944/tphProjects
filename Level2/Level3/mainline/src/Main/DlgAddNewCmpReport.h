#pragma once

class DlgAddNewCmpReport : public CDialog
{
	DECLARE_DYNAMIC(DlgAddNewCmpReport)

public:
	DlgAddNewCmpReport(CWnd* pParent = NULL);
	virtual ~DlgAddNewCmpReport();

	enum { IDD = IDD_CMPREPORTADDNEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void LayoutAllControl(int cx, int cy);
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strName;
	CString m_strDesc;
};
