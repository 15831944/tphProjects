#pragma once

#include "afxwin.h"
class InputLandside;
class LandsideVehicleTypeList;
class CVehicleTypeDefDlg : public CDialog
{
public:
	CVehicleTypeDefDlg(CWnd* pParent);
	~CVehicleTypeDefDlg(void);

	// Dialog Data
	//{{AFX_DATA(CPaxTypeDefDlg)
	enum { IDD = IDD_DIALOG_VEHICLETYPEDEF };
	CStatic	m_toolbarcontenter;
	CButton	m_btnSave;
	CTreeCtrl	m_treePax;
	CComboBox	m_comboLevel;
	CListBox	m_listboxName;
	//}}AFX_DATA


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxTypeDefDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	void InitToolbar();
	CToolBar m_ToolBar;
	bool m_bStructureChanged;
	void InsertVehicleItem( HTREEITEM _hParent, int _nIdxMinLevel );
	void DeleteAllChildren(HTREEITEM hItem);
	void ReloadLevelCombo();
	void ReloadTree();
	void ReloadName( int _nIdxLevel );
	void EditItem( int _nIdx );

	CString GetProjPath();
	InputLandside* GetInputLandside();
	int m_nSelIdx;
	// Generated message map functions
	//{{AFX_MSG(CPaxTypeDefDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListVehiclename();
	afx_msg LONG OnEndEdit( WPARAM p_wParam, LPARAM p_lParam );
	afx_msg void OnSelchangeComboLevel();
	afx_msg void OnDropdownComboLevel();
	afx_msg void OnSelchangeListVehiclename();
	afx_msg void OnButtonSave();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnVehicleTypeNew();
	afx_msg void OnVehicleTypeDelete();
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusEditName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CEdit m_EditLevelName;

private:
	LandsideVehicleTypeList* m_pVehicleTypeList;
};
