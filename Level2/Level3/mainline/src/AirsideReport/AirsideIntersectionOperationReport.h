#pragma once
#include "airsidebasereport.h"

class CAirsideIntersectionReportBaseResult;
class AIRSIDEREPORT_API CAirsideIntersectionOperationReport :
	public CAirsideBaseReport
{
public:
	CAirsideIntersectionOperationReport(CBGetLogFilePath pFunc);
	~CAirsideIntersectionOperationReport(void);

public:
	virtual void GenerateReport(CParameters * parameter);
	virtual int GetReportType(){return Airside_IntersectionOperation;}
	void RefreshReport(CParameters * parameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	//CAirsideRunwayCrossingsBaseResult *m_pResult;

public:
	BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
	BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);

	BOOL WriteReportData( ArctermFile& _file );
	BOOL ReadReportData( ArctermFile& _file );

	CString GetReportFileName();

public:
	CAirsideIntersectionReportBaseResult *m_pResult;
};
