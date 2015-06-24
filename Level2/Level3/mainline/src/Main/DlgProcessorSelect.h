#if !defined(AFX_DLGPROCESSORSELECT_H__F6E8A586_86FB_44B0_B02A_A9CD6763D11F__INCLUDED_)
#define AFX_DLGPROCESSORSELECT_H__F6E8A586_86FB_44B0_B02A_A9CD6763D11F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgProcessorSelect.h : header file
//
#include "Processor2.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgProcessorSelect dialog

class CDlgProcessorSelect : public CDialog
{
// Construction
public:
	CDlgProcessorSelect(CPROCESSOR2LIST* pProcList, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgProcessorSelect)
	enum { IDD = IDD_PROCNAMESELECT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProcessorSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList m_ilNodes;
	CPROCESSOR2LIST* m_pProcList;

	BOOL AddProcName(LPCTSTR lpszProcName);
	void FreeItemMemory();
	HTREEITEM FindName(LPCTSTR lpszProcName, HTREEITEM startItem = NULL);

	// Generated message map functions
	//{{AFX_MSG(CDlgProcessorSelect)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnUpdateEditProcName();
	afx_msg void OnChangeEditProcName();
	afx_msg void OnSelChangedTreeProcNames(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROCESSORSELECT_H__F6E8A586_86FB_44B0_B02A_A9CD6763D11F__INCLUDED_)
