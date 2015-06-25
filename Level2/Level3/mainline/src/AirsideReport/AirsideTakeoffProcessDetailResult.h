#pragma once
#include "airsidetakeoffprocessresult.h"

class CAirsideTakeoffProcessDetailChart;
class CParameters;
class CXListCtrl;
class CARC3DChart;

class CAirsideTakeoffProcessDetailResult :
	public CAirsideTakeoffProcessResult
{
public:
	enum DetailChartType{
		TakeoffQueueDelay,
		TimeToPosition,
		TakeOffDelay
	};
	CAirsideTakeoffProcessDetailResult(void);
	virtual ~CAirsideTakeoffProcessDetailResult(void);

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

private:
	CAirsideTakeoffProcessDetailChart* m_pChartResult;
};


class CAirsideTakeoffProcessDetailChart
{
public:
	struct DetailChartData
	{
		std::vector<double> m_vData;//data of every legend
		CString m_sName;//legend
	};
	typedef std::vector<DetailChartData> detailChartDataList;
	CAirsideTakeoffProcessDetailChart();
	virtual ~CAirsideTakeoffProcessDetailChart();

public:
	virtual void GenerateResult(TakeoffDetailDataList& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
	virtual void InvertDetailToGraphData(TakeoffDetailDataList& vResult,detailChartDataList& detailData) = 0;

	void AddChartData(detailChartDataList& detailData,const CString& sName,double dData);

	virtual long GetMinData(TakeoffDetailDataList& dataList) = 0;
	virtual long GetMaxData(TakeoffDetailDataList& dataList) = 0;

protected:
	std::vector<CString> m_vXAxisTitle;//xis value
	detailChartDataList m_vData;//chart data
};

class CAirsideTakeoffQueueChart : public CAirsideTakeoffProcessDetailChart
{
public:
	CAirsideTakeoffQueueChart();
	~CAirsideTakeoffQueueChart();

	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

	virtual void InvertDetailToGraphData(TakeoffDetailDataList& vResult,detailChartDataList& detailData);
	long GetMinData(TakeoffDetailDataList& dataList);
	long GetMaxData(TakeoffDetailDataList& dataList);
private:
};

class CAirsideTimeToPositionChart : public CAirsideTakeoffProcessDetailChart
{
public:
	CAirsideTimeToPositionChart();
	~CAirsideTimeToPositionChart();

	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	virtual void InvertDetailToGraphData(TakeoffDetailDataList& vResult,detailChartDataList& detailData);
	long GetMinData(TakeoffDetailDataList& dataList);
	long GetMaxData(TakeoffDetailDataList& dataList);
private:
};

class CAirsideTakeoffDelayChart : public CAirsideTakeoffProcessDetailChart
{
public:
	CAirsideTakeoffDelayChart();
	~CAirsideTakeoffDelayChart();

	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	virtual void InvertDetailToGraphData(TakeoffDetailDataList& vResult,detailChartDataList& detailData);
	long GetMinData(TakeoffDetailDataList& dataList);
	long GetMaxData(TakeoffDetailDataList& dataList);
private:
};