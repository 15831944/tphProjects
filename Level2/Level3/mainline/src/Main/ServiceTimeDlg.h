#if !defined(AFX_SERVICETIMEDLG_H__B3959FEA_26B1_4EC7_9AD8_28391F673652__INCLUDED_)
#define AFX_SERVICETIMEDLG_H__B3959FEA_26B1_4EC7_9AD8_28391F673652__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServiceTimeDlg.h : header file
//

#include <CommonData/procid.h>
#include "engine\proclist.h"
#include "..\inputs\in_term.h"
#include "..\MFCExControl\ListCtrlEx.h"
#include "..\inputs\con_db.h"
#include "ProcessorTreeCtrl.h"
#include "ConDBExListCtrl.h"
#include "MoblieElemTips.h"

/////////////////////////////////////////////////////////////////////////////
// CServiceTimeDlg dialog

class CServiceTimeDlg : public CDialog
{
// Construction
public:
	CServiceTimeDlg(CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CServiceTimeDlg)
	enum { IDD = IDD_DIALOG_SERVICETIME };
	CButton	m_btnProcNew;
	CConDBExListCtrl	m_listctrlData;
	CStatic	m_toolbarcontenter;
	CProcessorTreeCtrl	m_treeProc;
	CButton	m_btnSave;
	CButton	m_btnPaxEdit;
	CButton	m_btnProcDel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServiceTimeDlg)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ShowTips( int iItemData );

	ConstraintEntry* m_pDefaultEntry;

	CMoblieElemTips m_toolTips;
	int m_nListPreIndex;

	void InitToolbar();
	CToolBar m_ToolBar;
	void ReloadDatabase();
	int GetSelectedListItem();
	ConstraintDatabase* GetConstraintDatabase( bool& _bIsOwn );
	void ReloadListData( Constraint* _pSelCon );
	void SetListCtrl();
	int m_nProcType;
	std::vector<ProcessorID> m_vectID;
	CString GetProjPath();
	InputTerminal* GetInputTerminal();
	void SelectPaxDist( int _nIdx );
	void ReloadPaxDistList( int _nDBIdx );

	// Generated message map functions
	//{{AFX_MSG(CServiceTimeDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonNewproc();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonEditpax();
	afx_msg void OnButtonSave();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnEndlabeleditListPax(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedListPax(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPeoplemoverDelete();
	afx_msg void OnPeoplemoverNew();
	afx_msg void OnClickListPax(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonNew();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnListContentChanged( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVICETIMEDLG_H__B3959FEA_26B1_4EC7_9AD8_28391F673652__INCLUDED_)
