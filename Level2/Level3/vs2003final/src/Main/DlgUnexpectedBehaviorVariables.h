#pragma once
#include "../MFCExControl/ListCtrlEx.h"

class InputTerminal;
class UnexpectedBehaviorVariablesList;

class DlgUnexpectedBehaviorVariables: public CXTResizeDialog
{
	DECLARE_DYNAMIC(DlgUnexpectedBehaviorVariables)
public:
	DlgUnexpectedBehaviorVariables( CWnd* pParent = NULL);
	~DlgUnexpectedBehaviorVariables(void);

	// Dialog Data
	enum { IDD = IDD_DIALOG_MANTENANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNewItem();
	afx_msg void OnDelItem();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnSelComboBox(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnDbClickListItem( WPARAM wparam, LPARAM lparam);

	void SetListContent();

protected:
	void InitListCtrl();
	InputTerminal* GetInputTerminal();

protected:
	CToolBar m_wndToolbar;
	CListCtrlEx	m_wndListCtrl;

	int	m_nProjID;
	CString	m_strCaption;

	int m_nRowSel;
	int m_nColumnSel;

	UnexpectedBehaviorVariablesList* m_pUnexpectedBehaviorVariablesList;
	CWnd* m_pParentWnd;

};
