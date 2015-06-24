#if !defined(AFX_SIMENGINEDLG_H__177C1BFC_E3C5_4A92_9304_983AD3FEE1D1__INCLUDED_)
#define AFX_SIMENGINEDLG_H__177C1BFC_E3C5_4A92_9304_983AD3FEE1D1__INCLUDED_

#pragma once

// SimEngineDlg.h : header file

// include
#include "resource.h"
#include "../MFCExControl/SortableHeaderCtrl.h"
#include "../MFCExControl/XListCtrl.h"
#include <vector>

#include "SimulationErrorMessagePanel.h"
#include "common\SelectedSimulationData.h"
#include "afxcmn.h"
#include "../MFCExControl/XTResizeDialog.h"
// forward declare
class DiagnoseEntry;
class CTermPlanDoc;
class InputTerminal;
class CARCportEngine;

/////////////////////////////////////////////////////////////////////////////
// CSimEngineDlg dialog
#define UM_SimEngineDlg_ProgressBar_SetPos	 WM_USER+256
class CSimEngineDlg : public CXTResizeDialog
{
// Construction
public:
	
	void setSelectedSimulationData(const SelectedSimulationData& simSelData);
	const SelectedSimulationData& getSelectedSimulationData(){ return m_simSelData; }
	SimulationErrorMessagePanel	m_wndErrorMsgPanel;

	CWnd* m_pUserMsgOwner;
	void SetUserMsgOwner(CWnd* pWnd);
	CSimEngineDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSimEngineDlg();
	void SetTermPlanDoc(CTermPlanDoc* pDoc){m_pTermPlanDoc = pDoc;}
	void ClearText();
// Dialog Data
	//{{AFX_DATA(CSimEngineDlg)
	enum { IDD = IDD_DIALOG_SIMENGINE };
	//CXListCtrl	m_listErrorMsg;
	CStatic	m_staticErrorMsg;
	CStatic	m_staticProgText2;
	CProgressCtrl	m_progSimStatus2;
	CStatic	m_butType;
	CStatic	m_butTime;
	CStatic	m_butNumber;
	CButton	m_butFrame2;
	CButton	m_butFrame1;
	CButton	m_btnCancel;
	CButton	m_btnFile;
	CButton	m_btnClose;
	CStatic	m_staticProgText;
	CEdit	m_editEventType;
	CEdit	m_editEventTime;
	CEdit	m_editEventNum;
	CProgressCtrl	m_progSimStatus;
	CListBox	m_listboxSimStatus;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimEngineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SelectedSimulationData m_simSelData;
	CString GetProjPath();
	CARCportEngine* GetEngine();
	void ShowCurrentSetting();
	CSortableHeaderCtrl m_ctlHeaderCtrl;
	int m_nCurProgPos;
	int m_nCurProgPos2;
	int m_nRunNumber;
	CTermPlanDoc* m_pTermPlanDoc;

	// Generated message map functions
	//{{AFX_MSG(CSimEngineDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonCancel();
	afx_msg void OnButtonFile();
// 	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnColumnclickListErrorMsg(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg LRESULT OnAppendDiagnose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSimStatusMsg( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSimStatusFormatExMsg(WPARAM wParam,LPARAM lParam); 
	afx_msg LRESULT OnSimStatusMsg2( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSimStatusEvent( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSimSortEventLog( WPARAM wParam, LPARAM lParam );		
	DECLARE_MESSAGE_MAP()
private:
	std::vector< DiagnoseEntry* > m_vDiagnoseEntryList;
	void clearDiagnoseEntryList( void );
	void reloadLogIfNeed( const DiagnoseEntry* _pEntry );
public:
	afx_msg void OnBnClickedCancel();

//	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMENGINEDLG_H__177C1BFC_E3C5_4A92_9304_983AD3FEE1D1__INCLUDED_)
