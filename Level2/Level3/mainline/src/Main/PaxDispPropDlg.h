#if !defined(AFX_PAXDISPPROPDLG_H__2D990C4A_08FF_4F78_826C_6329FEFA25D7__INCLUDED_)
#define AFX_PAXDISPPROPDLG_H__2D990C4A_08FF_4F78_826C_6329FEFA25D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxDispPropDlg.h : header file
//

#include "PaxDispListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CPaxDispPropDlg dialog
class CTermPlanDoc;
class CPaxDispPropDlg : public CDialog
{
// Construction
public:
	CPaxDispPropDlg(CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPaxDispPropDlg)
	enum { IDD = IDD_DIALOG_PAXDISPPROP };
	CStatic	m_toolbarcontenter2;
	CButton	m_btnBarFrame2;
	CListCtrl	m_listNamedSets;
	CButton	m_btnBarFrame;
	CButton	m_btnOk;
	CButton	m_btnCancel;
	CButton	m_btnSave;
	CStatic	m_toolbarcontenter;
	CPaxDispListCtrl m_listctrlProp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxDispPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTermPlanDoc* GetDocument() const;
	void UpdatePaxDispPropIn3D() const;

	void InitToolbar();
	CToolBar m_ToolBar;
	CToolBar m_ToolBar2;

	InputTerminal* GetInputTerminal();
	CString GetProjPath();

	int m_nSelectedPDPSet;

	BOOL m_bNewPDPSet;

	void UpdatePDP();
	void UpdatePDPSetList();

	// Generated message map functions
	//{{AFX_MSG(CPaxDispPropDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	afx_msg void OnToolbarbuttonedit();
	afx_msg void OnClickListDispprop(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnPaxProcEdit( WPARAM wParam, LPARAM lParam );
	afx_msg void OnClose();
	afx_msg void OnItemChangedListPDPSets(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPDPSetAdd();
	afx_msg void OnPDPSetDelete();
	afx_msg void OnEndLabelEditListPDPSets(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAXDISPPROPDLG_H__2D990C4A_08FF_4F78_826C_6329FEFA25D7__INCLUDED_)
