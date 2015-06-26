#pragma once
#include "AirsideBaseReport.h"
#include "AirsideReportBaseResult.h"

class AirsideFlightRunwayDelayData;
class AirsideFlightRunwayDelayBaseResult;
class AirsideFlightRunwayDelayReportPara;

class AIRSIDEREPORT_API AirsideFlightRunwayDelayReport: public CAirsideBaseReport
{
public:
	enum ChartType
	{
		ChartType_Detail = 0,
		ChartType_Detail_Total,
		ChartType_Detail_ByLandingRoll,
		ChartType_Detail_ByExiting,
		ChartType_Detail_ByInTakeoffQueue,
		ChartType_Detail_ByHoldShort,
		ChartType_Detail_ByInPosition,
		ChartType_Detail_Count,


		ChartType_Summary = 100,
		ChartType_Summary_Total,
		ChartType_Summary_ByLandingRoll,
		ChartType_Summary_ByExiting,
		ChartType_Summary_ByInTakeoffQueue,
		ChartType_Summary_ByHoldShort,
		ChartType_Summary_ByInPosition,
		ChartType_Summary_Count,
	};

public:
	AirsideFlightRunwayDelayReport(CBGetLogFilePath pFunc, AirsideFlightRunwayDelayReportPara* parameter);
	~AirsideFlightRunwayDelayReport(void);

	//load flight log files, and then generate report data
	virtual void GenerateReport(CParameters * parameter);

	//no need to load log, filter sub report data from the base data.
	virtual void RefreshReport(CParameters * parameter);

	virtual int GetReportType();

	virtual CString GetReportFileName();

	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);

	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl * piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

	CAirsideReportBaseResult *GetReportResult();
    AirsideFlightRunwayDelayBaseResult* GetBaseResult() const { return m_pBaseResult; }
protected:
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

protected:
	AirsideFlightRunwayDelayBaseResult *m_pBaseResult;
	AirsideFlightRunwayDelayData* m_pReportData;
	AirsideFlightRunwayDelayReportPara* m_pParameter;

};
