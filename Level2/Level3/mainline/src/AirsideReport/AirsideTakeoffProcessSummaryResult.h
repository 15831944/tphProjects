#pragma once
#include "airsidetakeoffprocessresult.h"
#include "AirsideBaseReport.h"
#include "TakeoffProcessData.h"
#include <vector>

class CARC3DChart;
class CParameters;
class CXListCtrl;
class ArctermFile;
class CAirsideTakeoffProcessSummaryChart;

class CAirsideTakeoffProcessSummaryResult : public CAirsideTakeoffProcessResult
{
public:
	enum SummaryChartType
	{
		TakeOffQueueTime,
		TimeAtHoldShortLine,
		TaxiToPosition,
		TimeInPosition
	};
	CAirsideTakeoffProcessSummaryResult(void);
	virtual ~CAirsideTakeoffProcessSummaryResult(void);

	virtual void GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	void RefreshReport(CParameters * parameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg);
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg);
	virtual BOOL ExportListData(ArctermFile& _file,CParameters * parameter) ;

	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);
	
	const TakeoffSummaryDataList& GetResult()
	{
		return m_data;
	}
private:
	void BuiltResultData();
	void AddResultData(const CTakeoffProcessDetailData& detailData );

private:
	CAirsideTakeoffProcessSummaryChart* m_pChartResult;
	TakeoffSummaryDataList m_data;
};

class CAirsideTakeoffProcessSummaryChart
{
public:
	CAirsideTakeoffProcessSummaryChart();
	virtual ~CAirsideTakeoffProcessSummaryChart();

public:
	virtual void GenerateResult(TakeoffSummaryDataList& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
protected:
	std::vector<CString> m_vXAxisTitle;
	TakeoffSummaryDataList m_vResult;
};

class CAirsideTakeoffQueueSummaryChart : public CAirsideTakeoffProcessSummaryChart
{
public:
	CAirsideTakeoffQueueSummaryChart();
	~CAirsideTakeoffQueueSummaryChart();

	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
private:
};

class CAirsideTimeAtHoldShortLineSummaryChart : public CAirsideTakeoffProcessSummaryChart
{
public:
	CAirsideTimeAtHoldShortLineSummaryChart();
	~CAirsideTimeAtHoldShortLineSummaryChart();
	
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
private:
};

class CAirsideTaxiToPositionSummaryChart : public CAirsideTakeoffProcessSummaryChart
{
public:
	CAirsideTaxiToPositionSummaryChart();
	~CAirsideTaxiToPositionSummaryChart();

	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

private:
};

class CAirsideTimeInPositionSummaryChart : public CAirsideTakeoffProcessSummaryChart
{
public:
	CAirsideTimeInPositionSummaryChart();
	~CAirsideTimeInPositionSummaryChart();

	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
private:
};