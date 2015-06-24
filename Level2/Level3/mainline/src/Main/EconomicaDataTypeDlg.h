#if !defined(AFX_DLGLABOURCOSTS_H__CB32EB1B_DFCE_4E2D_9B7F_B43A8BC34FB1__INCLUDED_)
#define AFX_DLGLABOURCOSTS_H__CB32EB1B_DFCE_4E2D_9B7F_B43A8BC34FB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EconomicaDataTypeDlg.h : header file
//

#include "ProcessorTreeCtrl.h"
#include "economic\EconomicManager.h"

/////////////////////////////////////////////////////////////////////////////
// CEconomicaDataTypeDlg dialog

enum ECONOMICDATATTYPE{ ECONOMIC_LABOUR,ECONOMIC_PROCESSOR,
                       ECONOMIC_GATEUSAGE,ECONOMIC_PROCESSORUSAGE,  };


class CEconomicaDataTypeDlg : public CDialog
{
// Construction
public:
	CEconomicaDataTypeDlg(enum ECONOMICDATATTYPE _enumType,CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEconomicaDataTypeDlg)
	enum { IDD = IDD_DIALOG_LABOURCOSTS };
	CButton	m_btnNew;
	CButton	m_btnApply;
	CStatic	m_staticLabel5;
	CEdit	m_editValue5;
	CStatic	m_staticTreeTitle;
	CStatic	m_staticLabel4;
	CStatic	m_staticLabel3;
	CStatic	m_staticLabel2;
	CStatic	m_staticLabel1;
	CEdit	m_editValue4;
	CEdit	m_editValue3;
	CEdit	m_editValue2;
	CEdit	m_editValue1;
	CButton	m_btnErase;
	CButton	m_btnSave;
	CProcessorTreeCtrl	m_treeProc;
	CString	m_strValue1;
	CString	m_strValue2;
	CString	m_strValue3;
	CString	m_strValue4;
	CString	m_strValue5;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEconomicaDataTypeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	// base on the processor tree selection, load the data in the edits and enable the edit
	void ShowItemData();

	// base on the tree selection
	// return the database index of selected item or its parents.
	// bOwn tells if its own database.
	void GetDBIdx( int& nDBIdx, BOOL& bOwn );

	// set the default value string in the edit fields.
	void ShowDefaultData();

	// when click new, add default value into database.
	void AddNewDefaultData();

	// update the data
	void SetData();

	CProcEconDatabase* m_pDB;
	CEconomicManager* GetEconomicManager();
	void GetDatabase();

	BOOL CheckInput(int _nID,CString& _str);
	InputTerminal* GetInputTerminal();
	CString GetProjPath();
	void ReloadDatabase();

    void SetDefaultValue();
	
	// Generated message map functions
	//{{AFX_MSG(CEconomicaDataTypeDlg)
	afx_msg void OnButtonErase();
	afx_msg void OnButtonSave();
	afx_msg void OnSelchangedTreeProc(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtonApply();
	afx_msg void OnSetfocusEditValue1();
	afx_msg void OnKillfocusEditValue1();
	afx_msg void OnChangeEditValue1();
	afx_msg void OnChangeEditValue2();
	afx_msg void OnChangeEditValue3();
	afx_msg void OnChangeEditValue4();
	afx_msg void OnChangeEditValue5();
	afx_msg void OnButNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
//	BOOL m_bOwn;
//	int m_nDBIdx;
/*    CString m_strOldValue1;
	CString m_strOldValue2;
	CString m_strOldValue3;
	CString m_strOldValue4;
	CString m_strOldValue5;*/
//	CString m_strValue1;
//	CString m_strValue2;
//	CString m_strValue3;
//	CString m_strValue4;
//	CString m_strValue5;
	enum ECONOMICDATATTYPE m_enumType;
	HTREEITEM m_hSelItem;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLABOURCOSTS_H__CB32EB1B_DFCE_4E2D_9B7F_B43A8BC34FB1__INCLUDED_)
