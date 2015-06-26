#pragma once
#include "AirsideBaseReport.h"
#include "AirsideReportBaseResult.h"

class CAirsideFlightStandOperationBaseResult;
class CAirsideReportBaseResult;
///////////base class for generate report//////////////////////////////////////////////////////////////////
class CAirsideFlightStandOperationReport: public CAirsideBaseReport
{
public:
	enum ChartType
	{
		ChartType_Detail = 0,
		ChartType_Detail_Delay,
		ChartType_Detail_Conflict,
		ChartType_Detail_IdleTime,
		ChartType_Detail_Percentage,
		ChartType_Detail_Occupancy,


		ChartType_Summary = 100,
		ChartType_Summary_SchedUtilization,
		ChartType_Summary_ActualUtilization,
		ChartType_Summary_SchedIdle,
		ChartType_Summary_ActualIdle,
		ChartType_Summary_Delay,
		ChartType_Summary_Conflict
	};

	CAirsideFlightStandOperationReport(CBGetLogFilePath pFunc);
	virtual ~CAirsideFlightStandOperationReport();


public:
	//load flight log files, and then generate report data
	virtual void GenerateReport(CParameters * parameter);

	//no need to load log, filter sub report data from the base data.
	virtual void RefreshReport(CParameters * parameter);

	virtual int GetReportType(){return Airside_StandOperations;}

	virtual CString GetReportFileName();

	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);

	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

	CAirsideFlightStandOperationBaseResult* m_pBaseResult;//generate data for chart
protected:
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);




//	CAirsideReportBaseResult *GetReportResult();
};