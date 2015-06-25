#pragma once
#include "airsidebasereport.h"
class CAirsideReportBaseResult;
class CAirsideRunwayUtilizationReportBaseResult;
class AIRSIDEREPORT_API CAirsideRunwayUtilizationReport :
	public CAirsideBaseReport
{

public:
	enum ChartType
	{
		ChartType_Detail_RunwayOccupancy = 0,
		ChartType_Detail_RunwayUtilization,


		ChartType_Summary_RunwayOccupancy = 100,
		ChartType_Summary_RunwayUtilization
	};

public:
	CAirsideRunwayUtilizationReport(CBGetLogFilePath pFunc);
	~CAirsideRunwayUtilizationReport(void);

public:
	virtual void GenerateReport(CParameters * parameter);

	//no need to load log, filter sub report data from the base data.
	virtual void RefreshReport(CParameters * parameter);

	virtual int GetReportType();

	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);

	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

	//file operation,  use for export/import load/save report
public:
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);


	CAirsideReportBaseResult *GetReportResult();

	//file operation
	//write/read data to/from file
	CString GetReportFileName();
	BOOL WriteReportData( ArctermFile& _file );
	BOOL ReadReportData( ArctermFile& _file );








protected:
	CAirsideRunwayUtilizationReportBaseResult *m_pBaseResult;
};
