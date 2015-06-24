// ResDispPropDlg.h: interface for the CResDispPropDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESDISPPROPDLG_H__5FCFBB5F_2BAF_45E6_B2C3_F72FED9AC1A6__INCLUDED_)
#define AFX_RESDISPPROPDLG_H__5FCFBB5F_2BAF_45E6_B2C3_F72FED9AC1A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "resource.h"
#include "ResDispListCtrl.h"


class InputTerminal;



class CResDispPropDlg : public CDialog
{
// Construction
public:
	CResDispPropDlg(CWnd* pParent, CTermPlanDoc* pDoc);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPaxDispPropDlg)
	enum { IDD = IDD_DIALOG_RESDISPPROP };
	CStatic	m_toolbarcontenter2;
	CButton	m_btnBarFrame2;
	CListCtrl	m_listNamedSets;
	CButton	m_btnBarFrame;
	CButton	m_btnOk;
	CButton	m_btnCancel;
	CButton	m_btnSave;
	CResDispListCtrl m_listctrlProp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxDispPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	void InitToolbar();
	//CToolBar m_ToolBar;
	CToolBar m_ToolBar2;

	InputTerminal* GetInputTerminal();
	CString GetProjPath();

	int m_nSelectedRDPSet;

	BOOL m_bNewRDPSet;

	void UpdateRDP();
	void UpdateRDPSetList();

	// Generated message map functions
	//{{AFX_MSG(CPaxDispPropDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickListDispprop(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnItemChangedListRDPSets(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRDPSetAdd();
	afx_msg void OnRDPSetDelete();
	afx_msg void OnEndLabelEditListRDPSets(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	//afx_msg void OnToolbarbuttonadd();
	//afx_msg void OnToolbarbuttondel();
	//afx_msg void OnToolbarbuttonedit();
	DECLARE_MESSAGE_MAP()


};

#endif // !defined(AFX_RESDISPPROPDLG_H__5FCFBB5F_2BAF_45E6_B2C3_F72FED9AC1A6__INCLUDED_)
