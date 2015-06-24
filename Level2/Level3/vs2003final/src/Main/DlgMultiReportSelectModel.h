#pragma once
#include "..\MFCExControl\CoolTree.h"
#include "../Common/CompareModel.h"
#include "../compare/ModelsManager.h"
// CDlgMultiReportSelectModel dialog

class CDlgMultiReportSelectModel : public CDialog
{
	DECLARE_DYNAMIC(CDlgMultiReportSelectModel)

public:
	CDlgMultiReportSelectModel(const CCompareModelList& lstCmpModel,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMultiReportSelectModel();

// Dialog Data
	enum { IDD = IDD_DIALOG_MULTIREPORTSELECTMODEL };

	// Construction
public:
	HTREEITEM m_hLocal;
	HTREEITEM m_hLocalModel1;
	HTREEITEM m_hLocalModel2;
	HTREEITEM m_hHostModel1;
	HTREEITEM m_hHostModel2;
	HTREEITEM m_hHost;

public:
	void SaveHostInfo();

	CCompareModelList m_lstCompareModel;
	// Dialog Data
	//{{AFX_DATA(CModelSelectionDlg)
	CCoolTree m_cooltree;
	CStatic	m_ToolBarText;
	CString	m_strFileName;
	//}}AFX_DATA


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelSelectionDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	void InitSelectedNodes();
	void RemoveTreeItem(const CString& strHostName);
	void AddTreeItem(const CString& strPath, BOOL bAdd = TRUE);
	CString GetHostName(const CString& strFolder);
	BOOL GetLocalProjectPath(CString& strPath);
	void InitToolBar();
	void InitTree();
	CToolBar m_ToolBar;
	CString	m_Path;


	CModelDataSet	m_dsModel;

	// Generated message map functions
	//{{AFX_MSG(CModelSelectionDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAddModel();
	afx_msg void OnDeleteModel();
	virtual void OnCancel();
	afx_msg void OnSetfocusTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

private:
	int GetSelectionModelsFromTree();
public:
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
};
