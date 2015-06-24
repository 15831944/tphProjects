//{{AFX_INCLUDES()
#include "gtchart.h"
//}}AFX_INCLUDES
#if !defined(AFX_AUTOROSTERDLG_H__F7630B8E_7E6E_4E1A_9B2B_0F070A711BB5__INCLUDED_)
#define AFX_AUTOROSTERDLG_H__F7630B8E_7E6E_4E1A_9B2B_0F070A711BB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoRosterDlg.h : header file
//
class ProcessorID;
class InputTerminal;
class CSimLevelOfService;

#include <vector>
#include "ColorTreeCtrl.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoRosterDlg dialog
class CTermPlanDoc;
class CAutoRosterDlg : public CDialog
{
// Construction
public:
	CAutoRosterDlg( InputTerminal* _inTerm,const CString& _strPath,CTermPlanDoc* _pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAutoRosterDlg)
	enum { IDD = IDD_DIALOG_AUTOROSTER };
	CButton	m_butOK;
	CButton	m_butCancel;
	CTreeCtrl	m_treeProc;
	CColorTreeCtrl	m_treeEvent;
	CStatic	m_staticResult;
	CStatic	m_staticEvent;
	CGTChart	m_gtCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoRosterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutoRosterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTreeProc(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	InputTerminal* m_pInTerm;
	CSimLevelOfService* m_pTLOSSim;
	int m_iPreSimResult;
	CString m_strPrjPath;
	std::vector<ProcessorID> m_vProcessID;
	CTermPlanDoc* m_pDoc;
	bool m_bMustSelectOneResult;
private:
	CString GetRunName( int _index );
	void InsertEventsOfAProcssor( const ProcessorID& _procID);
	void AddProcessorIDToList( const ProcessorID& _id );
	void ShowProcGroupRoster( const CString& _strProcName);
	void ShowAllRoster();
	void ReloadRosterData( int _index );
	void ShowAutoRoster( HTREEITEM _hParent, const CString& _strItem, BOOL _bIsTopItem );
	void AddProcToTree( HTREEITEM _hParent, const ProcessorID& _id );
	void InitProcTree();
	void InitGTCtrl();
	HTREEITEM GetTheTopItem( CTreeCtrl* _treeCtrl, HTREEITEM _hCurItem, BOOL& _bIsTop );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOROSTERDLG_H__F7630B8E_7E6E_4E1A_9B2B_0F070A711BB5__INCLUDED_)
