#pragma once
#include <vector>
#include "CARC3DChart.h"
#include "../MFCExControl/XListCtrl.h"
#include "AirsideBaseReport.h"
#include "../Common/termfile.h"
class CParameters;
class CStandOperationReportData;
class CFlightStandOperationChartResult;
class StandOperationDataCollection;
class CSummaryStandOperationData;
class CFlightStandOperationParameter;
class StandOperationDataProcessor;
//////////////////generate data for result//////////////////////////////////////////////////////////////////
///////base class///////////////////////////////////////////////////////////////////////////////////////////
class CAirsideFlightStandOperationBaseResult
{
public:
	CAirsideFlightStandOperationBaseResult();
	virtual ~CAirsideFlightStandOperationBaseResult();

public:
	virtual void RefreshReport(CParameters * parameter) = 0;
	virtual void GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC=NULL) = 0;
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter) = 0;

	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg);
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg);
	virtual BOOL ExportListData(ArctermFile& _file,CParameters * parameter);

	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

	void setLoadFromFile(bool bLoad);
	bool IsLoadFromFile();
	void SetCBGetFilePath(CBGetLogFilePath pFunc);
	CBGetLogFilePath m_pCBGetLogFilePath;

	//void clear();
	void SetCBSecheduleStand(CBCScheduleStand pFunc){m_pScheduleStand = pFunc;}
    std::vector<CStandOperationReportData*> GetResult() const { return m_vResult; }
    void SetResult(std::vector<CStandOperationReportData*> val) { m_vResult = val; }

	int GetUnuseActualStandCount()const {return m_nUnuseActualStandCount;}
	int GetUnuseScheduleStandCount()const {return m_nUnuseScheduleStandCount;}
protected:
	bool m_bLoadFromFile;
	CFlightStandOperationChartResult* m_pChartResult; //draw graph
	std::vector<CStandOperationReportData*> m_vResult;
	int m_nUnuseActualStandCount;
	int m_nUnuseScheduleStandCount;
    StandOperationDataProcessor* m_pStandOperationProcessor;
	CBCScheduleStand  m_pScheduleStand;

public:
	static inline CString FormatDHMS(long lSecs); // format Day.Hour:Minute:Second
	static inline CString FormatHMS(long lSecs);  // format Hour:Minute:Second
	static inline CString PrintHMS(double dSecs);//

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////for detail result generate data/////////////////////////////////////////////////////////////////////////
class CDetailStandOperationResult : public CAirsideFlightStandOperationBaseResult
{
public:
	CDetailStandOperationResult();
	virtual ~CDetailStandOperationResult();

public:
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	void RefreshReport(CParameters * parameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////for summary result generate data////////////////////////////////////////////////////////////////////////
class CSummaryStandOperationResult : public CAirsideFlightStandOperationBaseResult
{
public:
	CSummaryStandOperationResult();
	virtual ~CSummaryStandOperationResult();

public:
	void RefreshReport(CParameters * parameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

protected:
	void RebuildResultData(CFlightStandOperationParameter* pParam, const std::vector<CStandOperationReportData*>& vStandOpData, const CString& strStandName);
	void clearData();
	std::vector<CSummaryStandOperationData*> m_vSummaryData;
};