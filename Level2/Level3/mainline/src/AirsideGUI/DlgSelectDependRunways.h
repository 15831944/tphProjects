#pragma once
#include "..\MFCExControl\SimpleToolTipListBox.h"
// CDlgSelectDependRunways dialog

class CDlgSelectDependRunways : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectDependRunways)

public:
	CDlgSelectDependRunways(std::vector<CString> &vAllRunways,
		std::vector<CString>& vSelectRunway,
		CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectDependRunways();

// Dialog Data
	enum { IDD = IDD_DIALOG_SELECTDEPENDRW };


	std::vector<CString> m_vAllRunways;
	std::vector<CString> m_vSelectedRunways;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CSimpleToolTipListBox m_listAllRunways;
	CSimpleToolTipListBox m_listSelectedRunways;
	afx_msg void OnBnClickedButtonAddrunways();
	afx_msg void OnBnClickedButtonDelrunways();
	virtual BOOL OnInitDialog();
	bool IsSelected(CString& strName);
protected:
	virtual void OnOK();
};
