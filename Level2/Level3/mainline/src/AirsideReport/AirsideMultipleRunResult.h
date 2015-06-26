#pragma once
#include "ParameterCaptionDefine.h"
#include "SummaryData.h"
#include <map>
#include <vector>
class CARC3DChart;
class CXListCtrl;
class CParameters;
class CSortableHeaderCtrl;
class CAirsideBaseReport;
struct MultipleRunReportData
{
	MultipleRunReportData()
		:m_iStart(0)
		,m_iEnd(0)
		,m_iData(0)
	{

	}
	long m_iStart;
	long m_iEnd;
	long m_iData;
};

typedef std::map<CString,std::vector<long>> mapLoadResult;
typedef std::map<CString,std::vector<MultipleRunReportData>> MultiRunDetailMap;
typedef std::map<CString, CSummaryData> MultiRunSummaryMap;
typedef std::map<CString,CAirsideBaseReport*> mapSimReport;
class AIRSIDEREPORT_API CAirsideMultipleRunResult
{
public:
	CAirsideMultipleRunResult(void);
	~CAirsideMultipleRunResult(void);

	virtual void LoadMultipleRunReport(CParameters* pParameter) = 0;
	virtual void InitListHead(CXListCtrl& cxListCtrl, CParameters * parameter, int iType = 0,CSortableHeaderCtrl* piSHC=NULL) = 0;
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter,int iType = 0) = 0;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter,int iType = 0) = 0;

	void AddSimReport(const CString& strSimResult, CAirsideBaseReport* pReport);
	void ClearSimReport();

protected:
	long GetMapMinValue(mapLoadResult mapData);
	long GetMapMaxValue(mapLoadResult mapData);
	long GetMinValue(std::vector<long> vData)const;
	long GetMaxValue(std::vector<long> vData)const;

	int GetIntervalCount(long iStart, long iEnd, std::vector<long> vData,long iIgnore = 0)const;
	long ClacTimeRange(ElapsedTime& eMaxTime,ElapsedTime& eMinValue,ElapsedTime& eInterval);
protected:
	mapSimReport m_mapSimReport;
};

