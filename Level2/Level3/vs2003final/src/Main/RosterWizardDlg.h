#if !defined(AFX_ROSTERWIZARDDLG_H__A994B51D_9B03_4E4E_A794_BED29B5B3D3E__INCLUDED_)
#define AFX_ROSTERWIZARDDLG_H__A994B51D_9B03_4E4E_A794_BED29B5B3D3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RosterWizardDlg.h : header file
//
#define ROSTER_COLUMN_NUM 9

#include "ListCtrlDrag.h"
#include "common\elaptime.h"
#include "TermPlanDoc.h"
class InputTerminal;
/////////////////////////////////////////////////////////////////////////////
// CRosterWizardDlg dialog

 class CRosterWizardDlg : public CXTResizeDialog 
{
// Construction
public:
	CRosterWizardDlg(InputTerminal* _pTerm, CTermPlanDoc* pDoc,const CString& _strPath, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRosterWizardDlg)
	enum { IDD = IDD_DIALOG_ROSTER_WIZARD };
	CButton	m_butTitle;
	CStatic	m_staticToolbar;
	CButton	m_butOK;
	CButton	m_butCancel;
	CStatic	m_staticText;
	CStatic	m_staticResult;
	CListCtrlDrag	m_listText;
	CListCtrlDrag	m_listResult;
	CEdit	m_editFileName;
	CButton	m_butFile;
	CButton	m_butClear;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRosterWizardDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRosterWizardDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonOpenfile();
	afx_msg void OnButtonClear();
	afx_msg void OnCreateMeList();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemchangedListResultSchedule(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPeoplemoverDelete();
	afx_msg void OnButtonTitle();
	afx_msg void OnDeleteRow();
	afx_msg void OnItemchangedListRostertext(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListResultSchedule(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LRESULT OnBegindrag(WPARAM wParam,LPARAM lParam);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
private:
	void setItemText( const CStringArray& _strArray );
	void addColumn(CListCtrl* _pListCtrl, int nColNum);
	int getColNumFromStr(CString strText,CString strSep,CStringArray& strArray);
	void deleteAllColumn( CListCtrl* _pListCtrl );
	bool ReadTextFromFile( const CString& _szFileName );
	void InitListCtrl();
	CImageList* CreateDragImageEx(int nColIndexDragged,CListCtrl *pList, LPPOINT lpPoint);
	bool createAssignEvent( int _indx );
	bool getTimeFromString( const CString& _strItem, ElapsedTime& _time );
	void setResourceNum( int _iItemIdx, int _iSubItem, const CString& _strProcName );
	bool isProcessor( const CString& _strProcName );
	bool isNumString( const CString& _strNum );
private:
	HCURSOR		m_hcArrow;
	HCURSOR		m_hcNo;
	CImageList* m_pDragImage;
	CWnd*		m_pDragWnd;
	bool		m_bColumnDropped[ROSTER_COLUMN_NUM];
	InputTerminal* m_pInTerm;
	CString		m_strProjPath;
	CToolBar	m_ToolBar;
	CToolBar    m_wndToolBarLeft;
	bool		m_bHaveAssignTitle;
	CTermPlanDoc* m_pTermDoc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROSTERWIZARDDLG_H__A994B51D_9B03_4E4E_A794_BED29B5B3D3E__INCLUDED_)
