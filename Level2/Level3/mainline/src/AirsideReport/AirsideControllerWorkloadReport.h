#pragma once
#include "airsidebasereport.h"


class CAirsideControllerWorkloadResult;
class CAirsideControllerWorkloadParam;


namespace AirsideControllerWorkload
{
	class CReportResult;
}

class CAirsideControllerWorkloadReport :
	public CAirsideBaseReport
{
public:
	CAirsideControllerWorkloadReport(CBGetLogFilePath pFunc);
	~CAirsideControllerWorkloadReport(void);



	virtual void GenerateReport(CParameters * parameter);
	//no need to load log, filter sub report data from the base data.
	virtual void RefreshReport(CParameters * parameter);

	virtual int GetReportType();

	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);

	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);



public:
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);


	AirsideControllerWorkload::CReportResult *m_pResult;
protected:

	CAirsideControllerWorkloadParam *m_pParam;


};
