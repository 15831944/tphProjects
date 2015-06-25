#if !defined(AFX_MODELSELECTIONDLG_H__937C827C_F44F_49E9_8518_CB92737FF211__INCLUDED_)
#define AFX_MODELSELECTIONDLG_H__937C827C_F44F_49E9_8518_CB92737FF211__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelSelectionDlg.h : header file
//
#include "..\MFCExControl\ListCtrlEx.h"
#include "..\MFCExControl\CoolTree.h"
#include "../compare/ModelsManager.h"

/////////////////////////////////////////////////////////////////////////////
// CModelSelectionDlg dialo
class CModelSelectionDlg : public CDialog
{

	enum TREEITEMTYPE
	{	
		ITEM_HOST		= 1,
		ITEM_MODEL		= 10,
		ITEM_SIMRESULT	= 100,
		ITEM_SIMRESULT_LOAD		= 1000,
		ITEM_SIMRESULT_UNLOAD    = 10000	
	};	
public:
	void SaveHostInfo();

	CModelSelectionDlg(CModelsManager*	modelsManager,CWnd* pParent = NULL);   // standard constructor
	CModelsManager* GetModelsManager()
	{
		return m_modelsManager;
	}
// Dialog Data
	//{{AFX_DATA(CModelSelectionDlg)
	enum { IDD = IDD_DIALOG_MODELSSELECTION };
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

	CModelsManager*	m_modelsManager;

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
	DECLARE_MESSAGE_MAP()
private:
	int GetSelectionModelsFromTree();
	int GetSimCountOfModel(CString strPath);
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnTvnItemexpandingTree1(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELSELECTIONDLG_H__937C827C_F44F_49E9_8518_CB92737FF211__INCLUDED_)
