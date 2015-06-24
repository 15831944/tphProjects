#if !defined(AFX_INTERESTONCAPITALDLG_H__D1C2157B_11F2_4863_9475_4885E0B7656E__INCLUDED_)
#define AFX_INTERESTONCAPITALDLG_H__D1C2157B_11F2_4863_9475_4885E0B7656E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InterestOnCapitalDlg.h : header file
//

#include "Economic\InterestOnCapitalDatabase.h"
#include "Economic\EconomicManager.h"
#include "..\MFCExControl\ListCtrlEx.h"


/////////////////////////////////////////////////////////////////////////////
// CInterestOnCapitalDlg dialog

class CInterestOnCapitalDlg : public CDialog
{
// Construction
public:
	CInterestOnCapitalDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInterestOnCapitalDlg)
	enum { IDD = IDD_DIALOG_INTERESTONCAPITAL };
	CListCtrlEx	m_List;
	CStatic	m_staticToolBar;
	CEdit	m_editLabel;
	CStatic	m_staticlisttoolbar;
	CButton	m_btnSave;
	CString	m_strLabel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInterestOnCapitalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CInterestOnCapitalDatabase* m_pInterestOnCapitalDB;

	CToolBar m_ListToolBar;

	int GetSelectedItem();

	void InitToolbar();
	void InitListCtrl();

	CEconomicManager* GetEconomicManager();

	CString GetProjPath();

	void ReloadDatabase();

	// refresh the sum of total value
	void RefreshEditLabel();

	// Generated message map functions
	//{{AFX_MSG(CInterestOnCapitalDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBtnSave();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnPeoplemoverNew();
	afx_msg void OnPeoplemoverDelete();
	afx_msg void OnItemchangedListInterests(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditListInterests(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INTERESTONCAPITALDLG_H__D1C2157B_11F2_4863_9475_4885E0B7656E__INCLUDED_)
