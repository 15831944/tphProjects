#pragma once
#include "airsidebasereport.h"
#include <vector>
#include "../Results/AirsideFlightLog.h"
#include "../Results/AirsideFlightLogEntry.h"
#include "../Results/OutputAirside.h"
#include "../common/FLT_CNST.H"
#include "FlightActivityBaseResult.h"

class CAirsideReportBaseResult;

class AIRSIDEREPORT_API  CFlightActivityReport :
	public CAirsideBaseReport
{

public:
 	const static char* subReportName[];
public:
	CFlightActivityReport(CBGetLogFilePath pFunc);
	~CFlightActivityReport(void);

public:
	virtual void GenerateReport(CParameters * parameter);
	virtual int GetReportType(){return Airside_FlightActivity;}
	void RefreshReport(CParameters * parameter);
 	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	CString GetReportFileName();
 	CFlightActivityBaseResult *m_pResult;

protected:
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

 public:
 	BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
 	BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);

protected:
	BOOL ExportListData(ArctermFile& _file,CParameters * parameter) ;
};
