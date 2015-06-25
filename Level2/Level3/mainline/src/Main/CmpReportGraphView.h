#pragma once
#include "..\AirsideReport\CARC3DChart.h"
#include ".\compare\CmpReport.h"
#include "afxwin.h"
#include "Resource.h"

class CCmpReportGraphView : public CFormView
{
	DECLARE_DYNCREATE(CCmpReportGraphView)

protected:
	CCmpReportGraphView();
	virtual ~CCmpReportGraphView();

public:
	enum { IDD = IDD_CMPREPORTGRAPHVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:
	CARC3DChart	m_3DChart;
	CToolBar	m_ToolBar;
	CComboBox	m_comboChartType;
	CComboBox	m_comboReportList;
public:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
protected:
	CCmpReport* m_pCmpReport;
	int m_categoryType;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeReportListCombo();
	afx_msg void OnSelColorBtn();
	afx_msg void OnPrintBtn();
private:
	void Draw3DChartByReportName(CString &reportSelected);
public:
	afx_msg void OnCbnSelchangeChartTypeCombo();
};


