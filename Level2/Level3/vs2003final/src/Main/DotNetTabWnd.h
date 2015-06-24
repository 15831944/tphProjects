#if !defined(AFX_DOTNETTABWND_H__42BFCC71_1E21_4FAF_9A8C_BCD82EBF539D__INCLUDED_)
#define AFX_DOTNETTABWND_H__42BFCC71_1E21_4FAF_9A8C_BCD82EBF539D__INCLUDED_

#pragma once
#import "..\..\bin\SplitTabCtrl.dll"

// DotNetTabWnd.h : header file
//
#include "MathFlow.h"
#include "FlowChartPane.h"
#include <vector>
/////////////////////////////////////////////////////////////////////////////
// DotNetTabWnd view

class CDotNetTabWnd : public CWnd
{
public:
	CDotNetTabWnd();
	virtual ~CDotNetTabWnd();

	bool Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	SplitTabCtrlLib::ISplitPanePtr& GetSplitPanePtr() { return m_pSplitPane; }

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	SplitTabCtrlLib::ISplitPanePtr m_pSplitPane;

protected:
	//{{AFX_MSG(CDotNetTabWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	std::vector<CFlowChartPane*> m_vpWndPane;
	std::vector<CMathFlow>* m_pFlowVect;

	BOOL m_bDeleted;
public:
	BOOL UpdateProcess(const CString &strOld, const CString &strNew);
	BOOL DeleteProcess(const CString &strName);
	BOOL ShowChildWnd(const CString& strName);
	afx_msg LRESULT OnChildDelete(WPARAM wParam, LPARAM lParam);
	const std::vector<CFlowChartPane*>& GetPaneList() const
	{
		return m_vpWndPane;
	}
	void SetPaneList(const std::vector<CFlowChartPane*>& vpWndPane)
	{
		m_vpWndPane = vpWndPane;
	}
	void SetMathFlowPtr(std::vector<CMathFlow>* pFlowVect)
	{
		m_pFlowVect = pFlowVect;
	}
	void AddChartPane(const CString& strName);

	BOOL DeleteChartPane(const CString& strName);

	BOOL DeleteAllPane();

	BOOL ModifyChartPane(const CString& strOldName, const CString& strNewName);

	BOOL SaveFlowChartData();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOTNETTABWND_H__42BFCC71_1E21_4FAF_9A8C_BCD82EBF539D__INCLUDED_)
