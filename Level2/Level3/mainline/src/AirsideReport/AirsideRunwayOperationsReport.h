#pragma once
#include "AirsideBaseReport.h"

class CAirsideRunwayOperationReportBaseResult;

class CAirsideReportBaseResult;
class AIRSIDEREPORT_API AirsideRunwayOperationsReport : public CAirsideBaseReport
{

public: 
	enum ChartType
	{
		ChartType_Detail = 0,
		ChartType_Detail_LandingsByRunway,
		ChartType_Detail_TakeOffByRunway,
		ChartType_Detail_LeadTrailByRunway,
		ChartType_Detail_MovementsPerRunway,


		ChartType_Detail_Count,


		ChartType_Summary = 100,
		ChartType_Summary_RunwayOperationalStatistic_Operations,
		ChartType_Summary_RunwayOperationalStatistic_TakeOff,
		ChartType_Summary_RunwayOperationalStatistic_Landing,
		ChartType_Summary_Count,

	};

public:
	AirsideRunwayOperationsReport(CBGetLogFilePath pFunc);
	~AirsideRunwayOperationsReport(void);


public:
	//load flight log files, and then generate report data
	virtual void GenerateReport(CParameters * parameter);

	//no need to load log, filter sub report data from the base data.
	virtual void RefreshReport(CParameters * parameter);

	virtual int GetReportType();

	virtual CString GetReportFileName();

public:
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);


protected:
	CAirsideRunwayOperationReportBaseResult *m_pBaseResult;

protected:
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);


public:
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);

	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

	CAirsideReportBaseResult *GetReportResult();
};


