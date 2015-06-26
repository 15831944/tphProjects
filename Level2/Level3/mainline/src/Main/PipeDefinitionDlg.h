#if !defined(AFX_PIPEDEFINITIONDLG_H__2AF05CEB_A46F_4EE4_ADF6_EE4FD5BA084A__INCLUDED_)
#define AFX_PIPEDEFINITIONDLG_H__2AF05CEB_A46F_4EE4_ADF6_EE4FD5BA084A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PipeDefinitionDlg.h : header file
//
#include "..\Inputs\PipeDataSet.h"
#include "TreeCtrlEx.h"
#include "TermPlanDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CPipeDefinitionDlg dialog
#include "engine\CongestionAreaManager.h"

class CGuid;
class CPipeDefinitionDlg : public CDialog
{
// Construction
public:
	CPipeDefinitionDlg(Terminal* _pTerm,CTermPlanDoc* _pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPipeDefinitionDlg)
	enum { IDD = IDD_DIALOG_PIPE };
	CButton	m_butOk;
	CButton	m_butCancel;
	CButton	m_butSave;
	CButton	m_butNew;
	CButton	m_butDel;
	CTreeCtrlEx	m_treePipePro;
	CStatic	m_staticToolBarPos;
	CListBox	m_listPipe;
	CEdit	m_editPipeName;
	CString	m_strPipeName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPipeDefinitionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	void UpdateAllPipesInRender3D();
	void UpdatePipeInRender3D(const CGuid& guid);
// Implementation
protected:
	void SizeWindow(int cx,int cy);
	BOOL CheckPipeName();
	void ShowDialog(CWnd* parentWnd);
	void HideDialog(CWnd* parentWnd);
	void ShowPipePro( int nIndex );
	void InitTree();
	void LoadList( int _iSelIndex = -1 );
	void InitToolBar();
    CFloor2* GetPointFloor(const Point& pt);
	Terminal* m_pTerm;
	bool m_bPaxFlowChanged;

	CToolBar m_ToolBar;
	HTREEITEM m_hNode;
	HTREEITEM m_hPointRoot;
	HTREEITEM m_hMovingRoot;
	HTREEITEM m_hRClickItem;
	int m_iCurListSel;
	CPipeDataSet* m_pPipeDataSet;
	//CPipeDataSet  m_tempPipeDataSet;
	CTermPlanDoc* m_pDoc;
	int iPointIndex;
	int m_iButOption;
	CWnd* m_pParent;
	BOOL m_bHasShow;

	INTVECTOR* m_pPipes;
	INTVECTOR m_vPipes;
	// Generated message map functions
	//{{AFX_MSG(CPipeDefinitionDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListPipe();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnButtonDel();
	afx_msg void OnButtonNew();
	afx_msg void OnPickupFormMap();
	afx_msg void OnKillfocusPipename();
	afx_msg void OnPipePointDel();
	afx_msg void OnPipePointDelall();
	afx_msg void OnPointWidthAdjust();
	afx_msg void OnPointWidthAdjustAll();
	afx_msg void OnWalkAttach();
	afx_msg void OnWalkDettach();
	afx_msg void OnButPipeproNew();
	afx_msg void OnButPipeproDel();
	afx_msg void OnSelchangedTreePipepro(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonSave();
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIPEDEFINITIONDLG_H__2AF05CEB_A46F_4EE4_ADF6_EE4FD5BA084A__INCLUDED_)
