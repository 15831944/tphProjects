#if !defined(AFX_MOVESIDEWALKPROPDLG_H__F5301912_9043_49C4_ADD1_823D4198913D__INCLUDED_)
#define AFX_MOVESIDEWALKPROPDLG_H__F5301912_9043_49C4_ADD1_823D4198913D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MoveSideWalkPropDlg.h : header file
//

#include "..\MFCExControl\ListCtrlEx.h"
#include "ProcessorTreeCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CMoveSideWalkPropDlg dialog
class CSideMoveWalkProcData;

class CMoveSideWalkPropDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CMoveSideWalkPropDlg)

// Construction
public:
	CMoveSideWalkPropDlg(){};
	CMoveSideWalkPropDlg(CWnd* pView);
	~CMoveSideWalkPropDlg();

// Dialog Data
	//{{AFX_DATA(CMoveSideWalkPropDlg)
	enum { IDD = IDD_DIALOG_MOVING_SIDE_WALK_PROP };
	CButton	m_btnNew;
	CButton	m_btnDel;
	CListCtrlEx	m_listProperty;
	CProcessorTreeCtrl	m_treeProc;
	//}}AFX_DATA

	
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMoveSideWalkPropDlg)
	public:
	virtual void OnOK();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMoveSideWalkPropDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonNew();	
	afx_msg LRESULT OnCollumnIndex(  WPARAM wParam,  LPARAM lParam   ) ;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CWnd* m_pView;
	int m_iDbIndex;
	int m_iCollumnIdx;
	
	//ConstraintDatabase* GetConstraintDatabase( bool& _bIsOwn );
	//void ReloadListData( Constraint* _pSelCon );
	void ReloadDatabase();
	CString GetProjPath();
	InputTerminal* GetInputTerminal();
	void SetListCtrl();
	void ReloadListData( );
	CSideMoveWalkProcData * GetConstraintDatabase( bool& _bIsOwn );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOVESIDEWALKPROPDLG_H__F5301912_9043_49C4_ADD1_823D4198913D__INCLUDED_)
