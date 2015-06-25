#pragma once
#include "airsidebasereport.h"
#include "../Results/OutputAirside.h"
#include "AirsideReportNode.h"
#include "AirsideReportBaseResult.h"
#include "../Common/FLT_CNST.H"
class CAirsideNodeDelayReportParameter;

class CAirsideNodeDelayReport :
	public CAirsideBaseReport
{
public:
	class nodeDelayItem
	{
	public:
		nodeDelayItem(CAirportDatabase* _AirportDB)
			:delayTime(0L)
		{
			memset(&fltDesc,0,sizeof(AirsideFlightDescStruct));
			m_fltCons.SetAirportDB(_AirportDB) ;
		}

		FlightConstraint m_fltCons;
		AirsideFlightDescStruct fltDesc;
		long delayTime;
		long lEventTime;  //the delay occured time
		long lPlanArrivalTime;      //plan time

		BOOL ExportFile(ArctermFile& _file) ;
		BOOL ImportFile(ArctermFile& _file) ;
	};

	class nodeDelay
	{
	public:
		nodeDelay(CAirportDatabase* _AirportDB)
			:delayTime(0L)
		{
          m_AirportDB = _AirportDB ;
		}

		CAirsideReportNode node;
		long delayTime;

		std::vector<nodeDelayItem> vDelayItem;

		BOOL ExportFile(ArctermFile& _file) ;
		BOOL ImportFile(ArctermFile& _file) ;
	CAirportDatabase* m_AirportDB ;
	};

public:
	CAirsideNodeDelayReport(CBGetLogFilePath pFunc);
	~CAirsideNodeDelayReport(void);

public:
	void GenerateReport(CParameters * parameter);
	void RefreshReport(CParameters * parameter);
	int GetReportType() {return Airside_NodeDelay;}
	CAirsideReportBaseResult *GetReportResult(){ return m_pNodeDelayResult;}

	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

	void HandleFlightLog(AirsideFlightLogEntry& fltlog,CAirsideNodeDelayReportParameter * parameter);
	bool IsValidTime(AirsideFlightDescStruct& fltlog,CAirsideNodeDelayReportParameter * parameter);
	bool IsFitConstraint(AirsideFlightDescStruct& fltlog,CAirsideNodeDelayReportParameter * parameter, FlightConstraint& fltConstraint);	
	//if node is equal return true, haven't selected return false
	bool IsNodeEqual(AirsideFlightEventStruct& event, CAirsideReportNode& airsideReportNode);

private:
	void FillDetailListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	void FillSummaryListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

protected:

	std::vector<nodeDelay > m_vResult;
	CAirsideReportBaseResult *m_pNodeDelayResult;
public:
	BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
	BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
protected:
	BOOL ExportListData(ArctermFile& _file,CParameters * parameter) ;
protected:
	void ExportListDetail(ArctermFile& _file, CParameters * parameter);
	void ExportListSummary(ArctermFile& _file, CParameters * parameter) ;
};
