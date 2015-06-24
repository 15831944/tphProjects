#pragma once

#include ".\MFCExControl\CoolTree.h"

class CCmpReportManager;

class CCmpParametersWnd : public CWnd
{
	DECLARE_DYNAMIC(CCmpParametersWnd)

public:
	CCmpParametersWnd();
	virtual ~CCmpParametersWnd();

	void SetReportManager(CCmpReportManager* pRptManager)
	{
		m_pReportManager = pRptManager;
	}
	void InitParaWnd();
	void UpdateParaWnd();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	DECLARE_MESSAGE_MAP()
	CCoolTree m_propTree;
	CButton m_btnRun;
	CButton m_btnCancel;
	CCmpReportManager* m_pReportManager;
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
private:
	void InitCooltreeNodeInfo(CWnd* pParent,COOLTREE_NODE_INFO& CNI,BOOL bVerify=TRUE);
};


