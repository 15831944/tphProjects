#if !defined(AFX_PROCTORESPOOLDLG_H__1AEE562B_5AC0_4299_AFE8_47ABC3536AC4__INCLUDED_)
#define AFX_PROCTORESPOOLDLG_H__1AEE562B_5AC0_4299_AFE8_47ABC3536AC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcToResPoolDlg.h : header file
//

#include "..\MFCExControl\listctrlex.h"
#include "afxwin.h"
// declare
class InputTerminal;
class CTermPlanDoc;
/////////////////////////////////////////////////////////////////////////////
// CProcToResPoolDlg dialog

class CProcToResPoolDlg : public CDialog
{
// Construction
public:
	CProcToResPoolDlg(CTermPlanDoc* _pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProcToResPoolDlg)
	enum { IDD = IDD_DIALOG_PROCTORESOURCEPOOL };
	CStatic	m_staticToolbar;
	CListCtrlEx	m_listData;
	CButton	m_btnSave;
	CButton m_btnOk;
	CButton m_btnCancel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcToResPoolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProcToResPoolDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	afx_msg void OnItemchangedListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnDblclkListData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	afx_msg void OnButtonSave();
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg LRESULT OnInplaceCombox( WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	CToolBar m_ToolBar;
	CTermPlanDoc* m_pDoc;
private:
	void InitListCtrl( void );
	void InitToolBar( void );
	void InitListString( CStringList& _strDistList, CStringList& _strPoolList );
	InputTerminal* GetInputTerminal();
	CString GetProjPath();

	void ReloadData( void );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCTORESPOOLDLG_H__1AEE562B_5AC0_4299_AFE8_47ABC3536AC4__INCLUDED_)
