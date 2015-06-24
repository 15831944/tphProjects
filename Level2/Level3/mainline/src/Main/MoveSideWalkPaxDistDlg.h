#if !defined(AFX_MOVESIDEWALKPAXDISTDLG_H__5A2DA047_3B48_4F62_921E_0B8E64EF52C7__INCLUDED_)
#define AFX_MOVESIDEWALKPAXDISTDLG_H__5A2DA047_3B48_4F62_921E_0B8E64EF52C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MoveSideWalkPaxDistDlg.h : header file
//
#include "ProcessorTreeCtrl.h"
#include "ConDBListCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CMoveSideWalkPaxDistDlg dialog

class CMoveSideWalkPaxDistDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CMoveSideWalkPaxDistDlg)

// Construction
public:
	CMoveSideWalkPaxDistDlg(){};
	CMoveSideWalkPaxDistDlg(CWnd* pView);
	~CMoveSideWalkPaxDistDlg();
	CToolBar m_ToolBar;
// Dialog Data
	//{{AFX_DATA(CMoveSideWalkPaxDistDlg)
	enum { IDD = IDD_DIALOG_MOVING_SIDE_WALK_PAX };
	CStatic	m_staticTool;
	CButton	m_btnNew;
	CButton	m_btnDel;
	CConDBListCtrl	m_listPax;
	CProcessorTreeCtrl	m_treeProc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMoveSideWalkPaxDistDlg)
	public:
	virtual void OnOK();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMoveSideWalkPaxDistDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButtonNew();
	afx_msg void OnButtonDel();
	afx_msg void OnItemchangedListPax(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedMovingsideTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnToolBarButtonNew();
	afx_msg void OnToolBarButtonDelete();
	afx_msg void OnEndlabeleditListPax(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CWnd* m_pView;
	ConstraintEntry* m_pDefaultEntry;
	void InitToolbar();
	void ReloadDatabase();
	InputTerminal* GetInputTerminal();
	void SetListCtrl();
	int GetSelectedListItem();
	void ReloadListData( Constraint* _pSelCon );
	ConstraintDatabase* GetConstraintDatabase( bool& _bIsOwn );
	CString GetProjPath();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOVESIDEWALKPAXDISTDLG_H__5A2DA047_3B48_4F62_921E_0B8E64EF52C7__INCLUDED_)
