#pragma once
#include "repgraphviewbaseoperator.h"
#include "MultiRunsReportDataLoader.h"

class CRepGraphView;
class CRepGraphViewMultiRunReportOperator :
	public CRepGraphViewBaseOperator
{
public:
	CRepGraphViewMultiRunReportOperator(CRepGraphView *pGraphView);
	~CRepGraphViewMultiRunReportOperator(void);

public:
	virtual void LoadReport(LPARAM lHint, CObject* pHint);
	virtual void OnCbnSelChangeReportSubType();
	void OnSelchangeChartSelectCombo() ;
protected:
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



	MultiRunSummarySubReportType GetSummarySubReportType();

	//using strReplacePaxType to replace the paxtype text in parameter
	CString GetParameterFootString(CString strReplacePaxType = _T(""));

	void SetDetailOccupancyTimeWithNoRanger(MultiRunsReport::Detail::OccupnacyDataList& , std::vector<int>& vSimResults) ;
	int GetValueRangeFromDataList(const MultiRunsReport::Detail::PaxDataList& _datalist);
protected:
	RECTL		m_rclBounds;
};
