#if !defined AFX_DLGCARRYONOWNERSHIP_H_
#define AFX_DLGCARRYONOWNERSHIP_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "resource.h"
#include "../MFCExControl/ListCtrlEx.h"

class DlgPaxLoadedStatesAndShapes;
class CCarryonOwnership;
class InputTerminal;

class DlgCarryonOwnership: public CXTResizeDialog
{
	DECLARE_DYNAMIC(DlgCarryonOwnership)
public:
	DlgCarryonOwnership(CWnd* pParent = NULL);
	~DlgCarryonOwnership(void);
	// Dialog Data

	enum { IDD = IDD_DIALOG_MOBILECOUNT };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void InitListBox();
	void InitToolbar();
	void InitListCtrl();

	//Get the current selected item index.
	int GetSelectedItem();

	//Get input terminal
	InputTerminal* GetInputTerminal();

	//Set styles and columns of the list control
	void SetListCtrl();

	// Generated message map functions

	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	afx_msg void OnAttachShape();

	afx_msg void OnItemchangedListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();

	afx_msg void OnSelchangeListBoxElement();
	afx_msg void OnButEdititem();
	afx_msg void OnCancel();

	afx_msg void OnSelComboBox(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnDbClickListItem( WPARAM wparam, LPARAM lparam);


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

protected:
	CListBox	m_listElem;
	CToolBar	m_toolbarcontenter;
	CButton	m_btnSave;
	CButton m_btnEditName;
	CButton m_btnAttachShapes;
	int m_nListPreIndex;
	CListCtrlEx	m_listctrlData;
	CCarryonOwnership* m_pCarryonOwnership;
	CWnd* m_pParentWnd;

	int m_nRowSel;
	int m_nColumnSel;
};

#endif