#if !defined(AFX_DLGSTD2GATEMAPPING_H__050C1919_B6A8_456D_9C9E_C9AA3999D058__INCLUDED_)
#define AFX_DLGSTD2GATEMAPPING_H__050C1919_B6A8_456D_9C9E_C9AA3999D058__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GatePriorityDlg.h : header file
//

#include "ProcessorTreeCtrl.h"
#include "TermPlanDoc.h"
#include "ConDBListCtrl.h"
#include "..\MFCExControl\ListCtrlEx.h"
#include "GateSelectionDlg.h"
#include "../InputAirside/ALTObject.h"
#include "../Inputs/Stand2GateConstraint.h"
#include "afxwin.h"

class InputTerminal;
/////////////////////////////////////////////////////////////////////////////
// CGatePriorityDlg dialog

class CDlgStd2GateMapping : public CDialog
{	
public:
	enum EnumGateType
	{
		GateType_ARR = 1,
		GateType_DEP = 2,

	};
protected:
	void InitToolbar();
	void SetListCtrl();
	CString GetProjPath();
	void ShowList();
	void ResetOrder();
	// Construction
public:
	CDlgStd2GateMapping(InputTerminal* _pInTerm,int nAirportID,CString strProjectPath,EnumGateType enumGateType,CStand2GateConstraint * pConstraint, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgStd2GateMapping(void);

	// Dialog Data
	//{{AFX_DATA(CGatePriorityDlg)
	enum { IDD = IDD_DIALOG_GATE_PRIORITY };
	//}}AFX_DATA
protected:

	EnumGateType m_enumGateType;//flag of arrival gate(0x1) or departure gate(0x2)
	CListCtrlEx	m_listctrlData;
	CStatic	m_toolbarcontenter;
	CToolBar m_ToolBar;
	InputTerminal* m_pInTerm;
	int m_nAirportID;
	CString m_strProjectPath;
	CStand2GateConstraint * m_pConstraint;
	CStand2GateConstraint* m_constrTemp;//temporary

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGatePriorityDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGatePriorityDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPeoplemoverDelete();
	afx_msg void OnPeoplemoverNew();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonPreference() ;
	void OnCancel() ;
	virtual void OnOK();
	afx_msg void OnItemchangedListGatePriority(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListGatePriority(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_wndStatic;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSTD2GATEMAPPING_H__050C1919_B6A8_456D_9C9E_C9AA3999D058__INCLUDED_)
