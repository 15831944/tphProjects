#pragma once
#include "airsidebasereport.h"
#include <vector>
#include "../Results/AirsideFlightLog.h"
#include "../Results/AirsideFlightLogEntry.h"
#include "../common/FLT_CNST.H"

class CAirsideReportBaseResult;
class CAirsideRunwayCrossingsBaseResult;
class AIRSIDEREPORT_API  CAirsideRunwayCrossingsReport :
	public CAirsideBaseReport
{

public:
	const static char* subReportName[];
public:
	CAirsideRunwayCrossingsReport(CBGetLogFilePath pFunc);
	~CAirsideRunwayCrossingsReport(void);

public:
	virtual void GenerateReport(CParameters * parameter);
	virtual int GetReportType(){return Airside_RunwayCrossings;}
	void RefreshReport(CParameters * parameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	CAirsideRunwayCrossingsBaseResult *m_pResult;

public:
	BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
	BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);

	BOOL WriteReportData( ArctermFile& _file );
	BOOL ReadReportData( ArctermFile& _file );

	CString GetReportFileName();
};
