#pragma once
#include "Resource.h"
#include "mschart.h"
#include "MultiRunsReportDataLoader.h"

// CMultiRunRepGraphView form view

class CMultiRunRepGraphView : public CFormView
{
public:
	enum { IDD = IDD_DIALOG_REPORT_GRAPH };

protected:
	DECLARE_DYNCREATE(CMultiRunRepGraphView)
	DECLARE_MESSAGE_MAP()

	CMultiRunRepGraphView();
	virtual ~CMultiRunRepGraphView();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnInitialUpdate();
	virtual void OnDraw(CDC* pDC);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelColorBtn();
	afx_msg void OnSelchangeChartTypeCombo();
	afx_msg void OnSelchangeChartSelectCombo();
	afx_msg void OnPrintBtn();

	void SetDetailedPaxDataContents(MultiRunsReport::Detail::PaxDataList& dataList, std::vector<int>& vSimResults);
	void SetDetailedTimeDataContents(MultiRunsReport::Detail::TimeDataList& dataList, std::vector<int>& vSimResults );
	void SetSummaryPaxDataContents(MultiRunsReport::Summary::PaxDataList& dataList, std::vector<int>& vSimResults);
	void SetHeadXYTitle(CString strTitle, CString strXAxisTitle, CString strYAxisTitle);
	void SetDetailOccupancyTimeDataContents(MultiRunsReport::Detail::OccupnacyDataList& dataList, std::vector<int>& vSimResults);
	void SetSummaryQueueTimeContents(const CString& strPaxType);
	void SetSummaryTimeServiceContents(const CString& strPaxType);

	void SetSummaryCriticalQueueLenContents();
	void SetSummaryCriticalQueueMaxQueueContents();
	void SetSummaryCriticalQueueMaxQueueTimeContents();

	CMSChart1	m_MSChartCtrl;
	CComboBox	m_comboChartType;
	CComboBox	m_comboChartSelect;
	CToolBar	m_wndToolbar;

	RECTL		m_rclBounds;
};


