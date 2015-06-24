#if !defined(AFX_MOBLIEELEMDLG_H__024555B6_7CA8_41BC_8238_0596B5A5D4F6__INCLUDED_)
#define AFX_MOBLIEELEMDLG_H__024555B6_7CA8_41BC_8238_0596B5A5D4F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MoblieElemDlg.h : header file
//

#include "..\inputs\in_term.h"
/////////////////////////////////////////////////////////////////////////////
// CMoblieElemDlg dialog

class CMoblieElemDlg : public CDialog
{
// Construction 
public:
	CMoblieElemDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMoblieElemDlg)
	enum { IDD = IDD_DIALOG_MOBILE_ELEM };
	CButton	m_butSave;
	CListBox	m_listElem;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMoblieElemDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	InputTerminal* GetInputTerminal();
	void EditItem( int _nIdx );
	void InitToolBar();
	void LoadList();

	CToolBar m_ToolBar;
	int m_nSelIdx;
	bool m_bStructureChanged;
	// Generated message map functions
	//{{AFX_MSG(CMoblieElemDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButElemNew();
	afx_msg void OnButElemDel();
	afx_msg void OnSelchangeListElem();
	afx_msg LONG OnEndEdit( WPARAM p_wParam, LPARAM p_lParam );
	afx_msg void OnButSave();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChangeName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool isNumString( const CString& _str) const;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOBLIEELEMDLG_H__024555B6_7CA8_41BC_8238_0596B5A5D4F6__INCLUDED_)
