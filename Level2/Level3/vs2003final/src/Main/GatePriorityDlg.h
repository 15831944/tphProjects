#if !defined(AFX_GATEPRIORITYDLG_H__050C1919_B6A8_456D_9C9E_C9AA3999D058__INCLUDED_)
#define AFX_GATEPRIORITYDLG_H__050C1919_B6A8_456D_9C9E_C9AA3999D058__INCLUDED_

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

class InputTerminal;
/////////////////////////////////////////////////////////////////////////////
// CGatePriorityDlg dialog

class CGatePriorityDlg : public CDialog
{

public:
	ALTObjectIDList m_standIDList;
	int GetSelectedItem();


protected:
		void InitToolbar();
		InputTerminal* GetInputTerminal();
		void SetListCtrl();
		CToolBar m_ToolBar;
		int GetSelectedListItem();
		CString GetProjPath();
		void ShowList();
		bool m_bPaintInit;
		void ResetOrder();
		InputTerminal* m_pInTerm;
		int m_nAirportID;
		int m_nProjID;
// Construction
public:
	CGatePriorityDlg(InputTerminal* _pInTerm,int nAirportID, int nProjectID,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGatePriorityDlg)
	enum { IDD = IDD_DIALOG_GATE_PRIORITY };
	CListCtrlEx	m_listctrlData;
	CStatic	m_toolbarcontenter;
	//}}AFX_DATA


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
	afx_msg void OnArrowUp();
	afx_msg void OnArrowDown();
	afx_msg void OnButtonSave();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnItemchangedListGatePriority(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonEdit();
	afx_msg void OnDblclkListGatePriority(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GATEPRIORITYDLG_H__050C1919_B6A8_456D_9C9E_C9AA3999D058__INCLUDED_)
