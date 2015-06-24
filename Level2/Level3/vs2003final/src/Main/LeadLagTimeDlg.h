#if !defined(AFX_LEADLAGTIMEDLG_H__81727036_29FF_4C33_8FC0_B1824F3837CE__INCLUDED_)
#define AFX_LEADLAGTIMEDLG_H__81727036_29FF_4C33_8FC0_B1824F3837CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LeadLagTimeDlg.h : header file
//
#include "conwithiddblistctrl.h"

class InputTerminal;
/////////////////////////////////////////////////////////////////////////////
// CLeadLagTimeDlg dialog
class CLeadLagTimeDlg : public CDialog
{
// Construction
public:
	CLeadLagTimeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLeadLagTimeDlg)
	enum { IDD = IDD_DIALOG_FLTPAX_DATA };
	CStatic	m_toolbarcontentor;
	CConWithIDDBListCtrl	m_listctrlData;
	CButton	m_btnSave;
	CButton m_btnImport;
	CButton m_btnOk;
	CButton m_btnCancel;
	CStatic m_Framebarcontenter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLeadLagTimeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLeadLagTimeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnEndlabeleditListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	afx_msg void OnItemchangedListData(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnListContentChanged( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// member
private:
	CToolBar m_ToolBar;

// function
private:
	void InitToolbar();
	InputTerminal* GetInputTerminal();
	CString GetProjPath();

	void SetListCtrl();
	//Reload data from database and select the item as parameter.
	void ReloadData( Constraint* _pSelCon );
	void ResortAndSaveData( bool _bNeedReloadList = false );
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEADLAGTIMEDLG_H__81727036_29FF_4C33_8FC0_B1824F3837CE__INCLUDED_)
