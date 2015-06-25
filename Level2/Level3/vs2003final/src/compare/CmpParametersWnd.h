#pragma once

class CCmpReportManager;

class CCmpParametersWnd : public CWnd
{
	DECLARE_DYNAMIC(CCmpParametersWnd)

public:
	CCmpParametersWnd();
	virtual ~CCmpParametersWnd();

	CXTPPropertyGrid* GetPropGridWnd()
	{
		return &m_wndPropGrid;
	}
	void SetReportManager(CCmpReportManager* pRptManager)
	{
		m_pReportManager = pRptManager;
	}
	void InitParaWnd();
	void UpdateParaWnd();
protected:
	DECLARE_MESSAGE_MAP()
	CXTPPropertyGrid m_wndPropGrid;
	CButton m_btnRun;
	CButton m_btnCancel;
	CCmpReportManager* m_pReportManager;
	CFont m_font;

	CXTPPropertyGridItem* m_pSelItem;

	CXTPPropertyGridItem* m_pItemBasicInfo;
	CXTPPropertyGridItem* m_pItemName;
	CXTPPropertyGridItem* m_pItemDesc;

	CXTPPropertyGridItem* m_pItemModels;

	CXTPPropertyGridItem* m_pItemReports;
	CXTPPropertyGridItem* m_pItemPassenger;
	CXTPPropertyGridItem* m_pItemProcessor;
	CXTPPropertyGridItem* m_pItemSpace;


	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRun();
	afx_msg LRESULT OnGridNotify(WPARAM, LPARAM);

	afx_msg void OnCraAddNewModel();
	afx_msg void OnCrbDelete();
	afx_msg void OnCrcAddNewReport();
	afx_msg void OnCrdEditReport();
	afx_msg void OnCrdDelete();


	CString GetRegularDateTime(LPCTSTR elaptimestr, bool needsec = true);
	void RunCompareReport();
	BOOL CheckData();
	void RemoveItem(CXTPPropertyGridItem* pItem);
	void UpdateParaItem(CXTPPropertyGridItem* pItem);
	
	CMenu m_nMenu, *m_pSubMenu;

public:
	afx_msg void OnTimer(UINT nIDEvent);
};


