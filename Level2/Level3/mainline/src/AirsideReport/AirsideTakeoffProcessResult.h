#pragma once
#include "AirsideBaseReport.h"
#include "ParameterCaptionDefine.h"
#include "TakeoffProcessData.h"
#include "AirsideReportBaseResult.h"

class CParameters;
class CARC3DChart;
class CXListCtrl;

class CAirsideTakeoffProcessResult : public CAirsideReportBaseResult
{
public:
	CAirsideTakeoffProcessResult(void);
	virtual ~CAirsideTakeoffProcessResult(void);

	virtual void GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter);
	virtual void RefreshReport(CParameters * parameter) = 0;
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg) = 0;
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg) = 0;
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC=NULL) = 0;
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter) = 0;
	virtual BOOL WriteReportData(ArctermFile& _file) = 0;
	virtual BOOL ReadReportData(ArctermFile& _file) = 0;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;

private:
	//-----------------------------------------------------------------------------------
	//Summary:
	//		calculate distance of flight event log list
	//-----------------------------------------------------------------------------------
	double CalculateDistance(std::vector<AirsideFlightEventLog*>& eventList)const;
	//-----------------------------------------------------------------------------------
	//Summary:
	//		check flight fit parameter that user input
	//-----------------------------------------------------------------------------------
	bool fits(CParameters * parameter,const AirsideFlightDescStruct& fltDesc,FlightConstraint& fltCons);
protected:
	TakeoffDetailDataList m_vResult;
};
