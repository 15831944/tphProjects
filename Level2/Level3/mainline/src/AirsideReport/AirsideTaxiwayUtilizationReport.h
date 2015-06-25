#pragma once
#include "airsidebasereport.h"
#include <vector>


class CAirsideReportBaseResult;
class CAirsideTaxiwayUtilizationBaseResult;
class CParameters;
class AIRSIDEREPORT_API  CAirsideTaxiwayUtilizationReport :
	public CAirsideBaseReport
{

public:
	const static char* subReportName[];
public:
	CAirsideTaxiwayUtilizationReport(CBGetLogFilePath pFunc);
	~CAirsideTaxiwayUtilizationReport(void);

public:
	virtual void GenerateReport(CParameters * parameter);
	virtual int GetReportType(){return Airside_TaxiwayUtilization;}
	void RefreshReport(CParameters * parameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	CAirsideTaxiwayUtilizationBaseResult *m_pResult;

public:
	BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
	BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);

	BOOL WriteReportData( ArctermFile& _file );
	BOOL ReadReportData( ArctermFile& _file );

	CString GetReportFileName();
};
