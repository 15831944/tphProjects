#if !defined(AFX_MOBILECOUNTDLG_H__C73D26D5_B6DE_4F74_AE4F_F92E19943954__INCLUDED_)
#define AFX_MOBILECOUNTDLG_H__C73D26D5_B6DE_4F74_AE4F_F92E19943954__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MobileCountDlg.h : header file
//
#include "ConDBListCtrlWithCheckBox.h"
#include "MoblieElemTips.h"
#include "..\inputs\in_term.h"
#include "..\inputs\con_db.h"


/////////////////////////////////////////////////////////////////////////////
// CMobileCountDlg dialog

class CMobileCountDlg : public CXTResizeDialog
{
// Construction
public:
	CMobileCountDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMobileCountDlg)
	enum { IDD = IDD_DIALOG_MOBILECOUNT };
	CListBox	m_listElem;
	CStatic	m_toolbarcontenter;
	CButton	m_btnSave;
	CButton m_btnNonpax;
	CConDBListCtrlWithCheckBox	m_listctrlData;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMobileCountDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitListBox();
	void ShowTips( int iItemData );
	//set styles & show the toolbar
	void InitToolbar();

	CToolBar m_ToolBar;
	//CToolTipCtrl m_toolTips;
	CMoblieElemTips m_toolTips;
	int m_nListPreIndex;
	//Get the current selected item index.
	int GetSelectedItem();

	//Get constraint databases for various flight type pointed out in m_enumType.
	ConstraintDatabase* GetConstraintDatabase();

	ConstraintDatabase* GetImpactSpeedDatabase();
	ConstraintDatabase* GetSideStepDatabase();
	ConstraintDatabase* GetInStepDatabase();
	ConstraintDatabase* GetEmerDatabase();

	//Get the project path 
	CString GetProjPath();

	//Get input terminal
	InputTerminal* GetInputTerminal();
	
	//Reload data from database and select the item as parameter.
	void ReloadData( Constraint* _pSelCon );
	
	//Set styles and columns of the list control
	void SetListCtrl();

	// Generated message map functions
	//{{AFX_MSG(CMobileCountDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnEndlabeleditListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	afx_msg void OnToolbarbuttonedit();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSelchangeListBoxElement();
	afx_msg void OnButEditmobile();
	afx_msg LRESULT OnCheckStateChanged( WPARAM wParam, LPARAM lParam );
	afx_msg void OnBnClickPosSpec();
	afx_msg void OnBnClickHostSpec();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOBILECOUNTDLG_H__C73D26D5_B6DE_4F74_AE4F_F92E19943954__INCLUDED_)
