#pragma once
#include "../Reports/ReportType.h"
#include "../MFCExControl/CoolTree.h"
#include "../Common/MultiRunReport.h"

// CMultiRunRepControlView form view

class CMultiRunRepControlView : public CFormView
{
	enum TreeItemType
	{
		ITEM_MODEL	= 0,
		ITEM_RUNRES =1

	
	};
protected:
	DECLARE_DYNCREATE(CMultiRunRepControlView)
	DECLARE_MESSAGE_MAP()

	CMultiRunRepControlView();
	virtual ~CMultiRunRepControlView();

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

	void InitToolBar();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCheckAllSimResults();
	afx_msg void OnApply();

	void UpdateControlContents();
	void GenerateParametersText(CString& strParameters);
	int  GetSimCountOfModel(CString strPath);

	CMultiRunReport m_MultiRunReport;

	CStatic					m_wndReportParasGroupBox;
	CEdit					m_wndReportParameters;
	CStatic					m_wndSimResultsGroupBox;
	CCoolTree				m_wndTreeSimResults;
	CToolBar				m_toolBar;
	//CCheckListBox			m_wndSimResultsList;
	//CButton					m_wndAllSimResultsCheckBox;
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolBarAddProject();
	afx_msg void OnToolBarDelProject();
	afx_msg void OnTvnSelchangedTreeSimresults(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnToolBarBtnDel(CCmdUI* pCmdUI);
	afx_msg void OnToolBarBtnAdd(CCmdUI* pCmdUI);
};


