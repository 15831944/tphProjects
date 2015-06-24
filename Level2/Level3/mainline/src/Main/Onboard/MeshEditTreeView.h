#pragma once
#include "../../MFCExControl/CoolTree.h"
#include "afxcmn.h"
#include "afxwin.h"

class CTermPlanDoc;
class CComponentEditContext;
class CComponentEditOperation;
// CMeshEditTreeView form view

class CMeshEditTreeView : public CFormView
{
	DECLARE_DYNCREATE(CMeshEditTreeView)

protected:
	CMeshEditTreeView();           // protected constructor used by dynamic creation
	virtual ~CMeshEditTreeView();

	void InitTree();
	void InitCtrls();
public:
	enum { IDD = IDD_FORMVIEW_MESHEDIT_TREE };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:
	virtual void OnInitialUpdate();
protected:
	//virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnUpdateOperation(CComponentEditOperation* pCmd);

protected:
	void PumpMessages();
	void DeleteAllChildren(HTREEITEM hItem);

	CComponentEditContext* GetEditContext();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	//afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
	//afx_msg LRESULT OnItemExpanding(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonSaveas();
	afx_msg void OnBnClickedButtonsave();
public:
	//afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnAddsectionNewsection();
	afx_msg void OnEditsectionDelete();
	afx_msg void OnEditsectionEdit();
	afx_msg void OnUpdateAddsectionNewsection(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditsectionDelete(CCmdUI* pCmdUI);
	afx_msg void OnEnKillfocusEditLod();

	void UpdateUnit();

protected:
	CButton m_btnOK;
	CButton m_btnSave;
	CButton m_btnSaveAs;
	CButton m_btnCancel;

	CCoolTree m_wndTreeCtrl;
	CSliderCtrl	m_ctlLOD;

	int m_nLOD;
	double m_dHeight;	
	//BOOL m_bThickness;

	CToolBar m_wndToolbar;
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonApply();
};


