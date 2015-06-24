#if !defined(AFX_ECONOMICDIALOG_H__CEDEB148_BB45_4551_8E12_015459BA27EB__INCLUDED_)
#define AFX_ECONOMICDIALOG_H__CEDEB148_BB45_4551_8E12_015459BA27EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// economicdialog.h : header file
//
#include "..\MFCExControl\ListCtrlEx.h"
#include "..\economic\EconomicManager.h"

/////////////////////////////////////////////////////////////////////////////
// CEconomicDialog dialog

class CEconomicDialog : public CDialog
{
// Construction
public:	
	CEconomicDialog(CWnd* pParent, int _nEconomicFileType );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEconomicDialog)
	enum { IDD = IDD_DIALOG_ECONOMIC };
	CButton	m_btnSave;
	CStatic	m_listtoolbarcontenter;
	CListCtrlEx	m_List;
	CEdit	m_editLabel;
	CString	m_strLabel;
	//}}AFX_DATA

	void SetStaticLabel(LPCTSTR szLabel)   { m_strLabel = szLabel;		   }
	void SetColNameArray(const CStringArray& arColName);

	int GetSelectedItem();
	void RefreshEditLabel();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEconomicDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitToolbar();
	void InitListCtrl();
	
	void ReloadDatabase();

	CEconomicManager* GetEconomicManager();

	void GetDatabase();


	void SelectList( int _nIdx );

	CString GetProjPath();

protected:
	CFixedCostRevDatabase* m_pDB;
	int m_nEconomicFileType;
	CToolBar m_ListToolBar;
	CStringArray m_arColName;
	
	// Generated message map functions
	//{{AFX_MSG(CEconomicDialog)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPeoplemoverNew();
	afx_msg void OnPeoplemoverDelete();
	afx_msg void OnEndlabeleditListEconomic(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnEconomicSave();
	afx_msg void OnItemchangedListEconomic(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECONOMICDIALOG_H__CEDEB148_BB45_4551_8E12_015459BA27EB__INCLUDED_)
