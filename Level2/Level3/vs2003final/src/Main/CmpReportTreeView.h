#pragma once
#include ".\MFCExControl\CoolTree.h"
#include "..\Compare\CmpReportManager.h"

class CCmpReportTreeView : public CFormView
{
	DECLARE_DYNCREATE(CCmpReportTreeView)

protected:
	CCmpReportTreeView();
	virtual ~CCmpReportTreeView();

public:
	virtual void OnDraw(CDC* pDC);
	enum { IDD = IDD_CMPREPORTTREEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:

	void SetCmpReport(CCmpReport* pCmpReport){ m_pCmpReport = pCmpReport; }
	void InitParaWnd();
	void UpdateParaWnd();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	CCoolTree m_propTree;
	CButton m_btnRun;
	CButton m_btnCancel;
	CFont m_font;

	HTREEITEM m_hBasicInfo;
	HTREEITEM m_hProjName;
	HTREEITEM m_hProjDesc;
	HTREEITEM m_hModelRoot;
	HTREEITEM m_hReportRoot;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRun();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChooseMenu(UINT nID);


	CString GetRegularDateTime(LPCTSTR elaptimestr, bool needsec = true);
	void RunCompareReport();
	void AddModel();
	void AddReport();
	void EditReport();
	void DeleteModel();
	void DeleteReport();
	BOOL CheckData();
	void RemoveSubItem(HTREEITEM pItem);
	void UpdateParaItem(HTREEITEM pItem);

	CMenu m_nMenu, *m_pSubMenu;

public:
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
private:
	void InitCooltreeNodeInfo(CWnd* pParent,COOLTREE_NODE_INFO& CNI,BOOL bVerify=TRUE);
protected:
	CCmpReport* m_pCmpReport;
};


