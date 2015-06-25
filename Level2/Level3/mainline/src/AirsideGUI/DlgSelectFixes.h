#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "..\MFCExControl\SimpleToolTipListBox.h"
// CDlgSelectFixes dialog

class CDlgSelectFixes : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectFixes)

public:
	CDlgSelectFixes(const std::vector<CString>& vstrAllFixes,
					std::vector<CString>& vstrSelectedFixes,
					LPCTSTR lpszCaption,
					CWnd* pParent = NULL);
	virtual ~CDlgSelectFixes();
	virtual BOOL OnInitDialog();

	enum { IDD = IDD_DIALOG_SELECTFIX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

	std::vector<CString> m_vstrAllFixes;
	std::vector<CString>& m_vstrSelectedFixes;
	CSimpleToolTipListBox m_wndListAllFixes;
	CTreeCtrl m_wndTreeSelectedFixes;
	CString m_strCaption;

	virtual void OnCancel();
	virtual void OnOK();

	afx_msg void OnBnClickedButtonAddfix();
	afx_msg void OnBnClickedButtonDelfix();
};
