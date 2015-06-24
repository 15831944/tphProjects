#if !defined(AFX_MAINTENANCEDIALOG_H__FD97D353_7CA9_4744_BEC7_CFA189BD4CDA__INCLUDED_)
#define AFX_MAINTENANCEDIALOG_H__FD97D353_7CA9_4744_BEC7_CFA189BD4CDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MaintenanceDialog.h : header file
//
#include "Economic\MaintenanceCostDatabase.h"
#include "Economic\EconomicManager.h"
#include "..\MFCExControl\ListCtrlEx.h"

/////////////////////////////////////////////////////////////////////////////
// CMaintenanceDialog dialog

class CMaintenanceDialog : public CDialog
{
// Construction
public:
	CMaintenanceDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMaintenanceDialog)
	enum { IDD = IDD_DIALOG_MANTENANCE };
	CStatic	m_listtoolbarcontenter;
	CListCtrlEx	m_List;
	CButton	m_btnSave;
	//}}AFX_DATA
	
	void SetColNameArray(const CStringArray& arColName);
	void SetTitle(LPCTSTR szTitle);
	int GetSelectedItem();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMaintenanceDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitToolbar();
	void InitListCtrl();

	CEconomicManager* GetEconomicManager();

	CString GetProjPath();


	void ReloadDatabase();

protected:
	CMaintenanceCostDatabase* m_pMaintenanceCostDB;

	CToolBar m_ListToolBar;
	CStringArray m_arColName;
	CString m_strTitle;

	// Generated message map functions
	//{{AFX_MSG(CMaintenanceDialog)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEndlabeleditListMaintenance(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListMaintenance(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnMaintenanceSave();
	afx_msg void OnPeoplemoverNew();
	afx_msg void OnPeoplemoverDelete();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINTENANCEDIALOG_H__FD97D353_7CA9_4744_BEC7_CFA189BD4CDA__INCLUDED_)
