#pragma once
#include "afxwin.h"
#include "TreeCtrlEx.h"
// CLoadFactorsDlg dialog

class CLoadFactorsDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoadFactorsDlg)

public:
	CLoadFactorsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoadFactorsDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_LOADFACTOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
protected:
	CStatic m_Static_tree;
	CTreeCtrlEx m_TreeFactor ;
protected:
	void initFactorsTree() ;
};
