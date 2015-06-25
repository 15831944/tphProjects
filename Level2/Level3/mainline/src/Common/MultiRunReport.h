#pragma once
#include "CompareModel.h"
#include "reports/ReportType.h"
#include "MultiReportAndReportManager.h"
#include "Main/MultiRunsReportDataLoader.h"

class CReportParameter;
class CMultiRunReport
{
public:
	CMultiRunReport(void);
	virtual ~CMultiRunReport(void);


public:

	//void InitReport(ENUM_REPORT_TYPE enumRepType);

	CCompareModelList& GetCompareModelList(){ return m_comModelList;}

	void LoadReport(ENUM_REPORT_TYPE enumRepType,const CString& strProjPath);
	void SaveReport(ENUM_REPORT_TYPE enumRepType,const CString& strProjPath);

	void GenerateReport(ENUM_REPORT_TYPE enumRepType, int nDetailOrSummary);
	void GenerateReport(CReportParameter *pReportParam,const CString& strProjPath, Terminal *pTerminal, int nFloorCount);


	const MultiRunsReport::Detail::PaxDataList& GetReportResult_PaxDataList(){ return m_paxDataList;}
	const MultiRunsReport::Detail::TimeDataList& GetReportResult_TimeDataList(){ return m_timeDataList;}
	const MultiRunsReport::Detail::OccupnacyDataList& GetReportResult_OccupnacyDataList(){ return m_occupancyDataList;}

	const MultiRunsReport::Summary::SummaryQTimeList& GetReportResult_SummaryQueueTime(){  return m_summaryQTimeList;}
	const MultiRunsReport::Summary::SummaryCriticalQueueList& GetReportResult_SummaryCriticalQueue(){ return m_summaryCriticalQList;}
	const MultiRunsReport::Summary::SummaryTimeServiceList& GetReportResult_SummaryTimeService(){  return m_summaryTimeServiceList;}	
	//sum of all the simresult
	int GetAllSimResultCount();
	std::vector<CString> GetAllSimResultFullName();

protected:
	CCompareModelList m_comModelList;

protected:

	MultiRunsReport::Detail::PaxDataList m_paxDataList;
	MultiRunsReport::Detail::TimeDataList m_timeDataList;
	MultiRunsReport::Detail::OccupnacyDataList m_occupancyDataList;


	MultiRunsReport::Summary::SummaryCriticalQueueList m_summaryCriticalQList;
	MultiRunsReport::Summary::SummaryQTimeList m_summaryQTimeList;
	MultiRunsReport::Summary::SummaryTimeServiceList m_summaryTimeServiceList;
};
