#if !defined(AFX_MATHEMATICVIEW_H__6CBD4770_737D_4995_AFC9_5297B6523794__INCLUDED_)
#define AFX_MATHEMATICVIEW_H__6CBD4770_737D_4995_AFC9_5297B6523794__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MathematicView.h : header file
//
#include "PaxFlowDlg.h"

#include "MathFlow.h"
#include "MathProcess.h" 
#include "XPStyle\NewMenu.h"

/////////////////////////////////////////////////////////////////////////////
// CMathematicView view
class InputTerminal;
class CTermPlanDoc;

class CMathematicView : public CView
{
protected:
	CMathematicView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMathematicView)

// Attributes
public:
// Form Data
public:
//	enum { IDD = IDD_FORMVIEW_MATHEMATIC };
	CTreeCtrl	m_treeModels;

// Operations
public:
	CTermPlanDoc* GetDocument();
	InputTerminal* GetInputTerminal() const;
	BOOL InitTreeModels();
	void SetProcessPtr(std::vector<CMathProcess> *pProcess)
	{
		m_pProcVect = pProcess;
	}

	void SetFlowPtr(std::vector<CMathFlow> *pFlow)
	{
		m_pFlowVect = pFlow;
	}
	void SetSelectItem(const CString& strName);
	void DeleteItem(const CString& strName);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMathematicView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMathematicView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMathematicView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnOk();
	afx_msg void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnRclickTreeModels(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTreeModels(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnMmflightsComments();
	afx_msg void OnMmflightsHelp();
	afx_msg void OnDblclkTreeModels(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangingTreeModels(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMmprocAdd();
	afx_msg void OnMmprocComments();
	afx_msg void OnMmprocHelp();
	afx_msg void OnMmflowAdd();
	afx_msg void OnMmflowComments();
	afx_msg void OnMmflowHelp();
	afx_msg void OnMmitemEdit();
	afx_msg void OnMmitemDelete();
	afx_msg void OnMmitemComments();
	afx_msg void OnMmitemHelp();
	afx_msg void OnBeginlabeleditTreeModels(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditTreeModels(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginDragTreeModels(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL RunSimulation();

	HTREEITEM HasChild(HTREEITEM hItem, const CString& strText);

	BOOL ShowProcessFlowView(CView** pView = NULL);

protected:
	CFont m_Font;
	CImageList m_ImageList;
	CPaxFlowDlg* m_pDlgFlow;
	HTREEITEM m_hTreeItem[6];
	CNewMenu m_nMenu, *m_pSubMenu;
	HTREEITEM m_hSelecting;

	CString m_strOldText;
	int m_nIterval;

	std::vector<CMathFlow> *m_pFlowVect;
	std::vector<CMathProcess> *m_pProcVect;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MATHEMATICVIEW_H__6CBD4770_737D_4995_AFC9_5297B6523794__INCLUDED_)
