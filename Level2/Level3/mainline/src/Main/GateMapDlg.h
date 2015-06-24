#if !defined(AFX_GATEMAPDLG_H__10656748_3257_49BD_B16C_3F99C25B6040__INCLUDED_)
#define AFX_GATEMAPDLG_H__10656748_3257_49BD_B16C_3F99C25B6040__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GateMapDlg.h : header file
//
class InputTerminal;
#include "..\MFCExControl\ListCtrlEx.h"
#include "inputs\GateMappingDB.h"
/////////////////////////////////////////////////////////////////////////////
// CGateMapDlg dialog

class CGateMapDlg : public CDialog
{
// Construction
public:
	CGateMapDlg(InputTerminal* _pInTerm,const CString& _strPath,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGateMapDlg)
	enum { IDD = IDD_DIALOG_GATEMAPPING };
	CButton	m_butOK;
	CButton	m_butCancel;
	CButton	m_butSave;
	CStatic	m_staticToolBar;
	CListCtrlEx	m_listMap;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGateMapDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGateMapDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	afx_msg void OnButSave();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnItemchangedListMap(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListMap(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditListMap(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	InputTerminal* m_pInTerm;
	CString m_strProjPath;
	CToolBar m_ToolBar;
	
private:
	void InitToolBar();
	void InitListCtrl();
	void ReloadMapData();
	void AddAMappingToList( const CGateMapping& gatemap,int _index );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GATEMAPDLG_H__10656748_3257_49BD_B16C_3F99C25B6040__INCLUDED_)
