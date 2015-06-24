#if !defined(AFX_RESOURCEPOOLDLG_H__55290FD8_0248_4EF6_9E43_FD3184D857A1__INCLUDED_)
#define AFX_RESOURCEPOOLDLG_H__55290FD8_0248_4EF6_9E43_FD3184D857A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResourcePoolDlg.h : header file
//

// declare
class InputTerminal;
class CTermPlanDoc;
/////////////////////////////////////////////////////////////////////////////
// CResourcePoolDlg dialog
#include "..\MFCExControl\listctrlex.h"
class CResourcePoolDlg : public CDialog
{
// Construction
public:
	CResourcePoolDlg( CTermPlanDoc* _pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CResourcePoolDlg)
	enum { IDD = IDD_DIALOG_RESOURCE_POOL };
	CButton	m_btnOK;
	CStatic	m_staticToolbar;
	CListCtrlEx	m_listPool;
	CButton	m_btnSave;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResourcePoolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResourcePoolDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolbarbuttonAdd();
	afx_msg void OnToolbarbuttonDel();
	afx_msg void OnUpdateToolbarbuttonAdd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolbarbuttonDel(CCmdUI* pCmdUI);
	afx_msg void OnItemchangedListPool(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListPool(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnToolbarbuttonPickPoints();
	afx_msg void OnButtonSave();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnInplaceEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInplaceCombox( WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

private:
	CTermPlanDoc* m_pDoc;
	CToolBar m_ToolBar;
	CString  m_strDoubleClkItemName;
	CString  m_strSelectPoolName;
	int m_nSelectPoolIdx;
private:
	void InitListCtrl( void );
	void InitToolBar( void );
	InputTerminal* GetInputTerminal();
	CString GetProjPath();
	void ShowDialog(CWnd* parentWnd);
	void HideDialog(CWnd* parentWnd);
	void ReLoadDataset( void );
	bool CanSave(void);
	bool DoSave( void );
public:
	CString getSelectPoolName();
	int getSelectPoolIdx() { return m_nSelectPoolIdx; }

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESOURCEPOOLDLG_H__55290FD8_0248_4EF6_9E43_FD3184D857A1__INCLUDED_)
