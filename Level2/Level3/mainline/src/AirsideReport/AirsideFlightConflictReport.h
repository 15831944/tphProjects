#pragma once
#include "AirsideBaseReport.h"
#include "AirsideReportBaseResult.h"

class FlightConflictReportData;
class AirsideFlightConflictBaseResult;
class AirsideFlightConflictPara;
class AIRSIDEREPORT_API AirsideFlightConflictReport: public CAirsideBaseReport
{
public:
	enum ChartType
	{
		ChartType_Detail = 0,
		ChartType_Detail_DistributionOfPerAC,
		ChartType_Detail_ByTimeOfDay,
		ChartType_Detail_ByAreaAndTimeOfDay,
		ChartType_Detail_ByLocationAndTimeOfDay,
		ChartType_Detail_ByOperationAndTimeOfDay,
		ChartType_Detail_ByActionAndTimeOfDay,
		ChartType_Detail_Count,


		ChartType_Summary = 100,

		ChartType_Summary_Count,
	};

public:
	AirsideFlightConflictReport(CBGetLogFilePath pFunc, AirsideFlightConflictPara* parameter);
	~AirsideFlightConflictReport(void);

	//load flight log files, and then generate report data
	virtual void GenerateReport(CParameters * parameter);

	//no need to load log, filter sub report data from the base data.
	virtual void RefreshReport(CParameters * parameter);

	virtual int GetReportType();

	virtual CString GetReportFileName();

	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);

	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

	CAirsideReportBaseResult *GetReportResult();

protected:
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

protected:
	AirsideFlightConflictBaseResult *m_pBaseResult;
	FlightConflictReportData* m_pReportConflictData;
	AirsideFlightConflictPara* m_pParameter;

};
