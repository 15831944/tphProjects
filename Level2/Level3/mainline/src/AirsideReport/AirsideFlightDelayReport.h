#pragma once
#include "airsidebasereport.h"
#include "../Results/AirsideFlightLog.h"
#include "../Results/AirsideFlightLogEntry.h"
#include "../Results/OutputAirside.h"
#include "../common/FLT_CNST.H"
#include "AirsideReportNode.h"
#include "../Engine/Airside/CommonInSim.h"
#include "AirsideDelayDescLog.h"
#include "FlightConflictReportData.h"


class CDetailTotalDelayResult;
class CDetailAirDelayResult;
class CDetailTaxiDelayResult;
class CDetailStandDelayResult;
class CDetailServiceDelayResult;
class CDetailTakeOffDelayResult;
class CDetailSegmentDelayResult;
class CDetailComponentDelayResult;
class CARC3DChart;
class CAirsideFlightDelayParam;
class CAirsideFlightDelayBaseResult;
class FlightConflictReportData;
class ConflictDataItem;

class AIRSIDEREPORT_API  CAirsideFlightDelayReport :
	public CAirsideBaseReport
{
public:
	enum subReportType
	{
		SRT_DETAIL_SCHEDULEDELAY = 0,
		SRT_DETAIL_FLIGHTTOTALDELAY,
		SRT_DETAIL_SEGMENTDELAY,
		SRT_DETAIL_COMPONENTDELAY,
		SRT_DETAIL_AIRDELAY,
		SRT_DETAIL_TAXIDELAY,
		SRT_DETAIL_STANDDELAY,
		SRT_DETAIL_SERVICEDELAY,
		SRT_DETAIL_TAKOFFDELAY,

		//add detail report enumeration here
		SRT_DETAIL_COUNT,

		SRT_SUMMARY_SCHEDULEDELAY,
		SRT_SUMMARY_FLIGHTTOTALDELAY,
		SRT_SUMMARY_SEGMENTDELAY,
		SRT_SUMMARY_COMPONENTDELAY,
		SRT_SUMMARY_AIRDELAY,
		SRT_SUMMARY_TAXIDELAY,
		SRT_SUMMARY_STANDDELAY,
		SRT_SUMMARY_SERVICEDELAY,
		SRT_SUMMARY_TAKOFFDELAY,

		//add type before this line
		SRT_COUNT,

		SRT_DETAIL_NODEDELAY,
		SRT_SUMMARY_NODEDELAY,
		SRT_NODE_COUNT,

	};

	const static char * subReportName[];

	enum FltDelaySegment
	{
		FltDelaySegment_Unknown = 0,
		FltDelaySegment_Air ,
		FltDelaySegment_Taxi,
		FltDelaySegment_Stand,
		FltDelaySegment_Service,
		FltDelaySegment_TakeOff
	};



public:

	class FltNodeDelayItem
	{
	public:
		FltNodeDelayItem()
			:nSegment(FltDelaySegment_Unknown)
			,eArriveTime(0)
			,eActArriveTime(0)
			,nReason(FltDelayReason_Unknown)
			, delayTime(0l)
		{

		}
	public:
		long delayTime;
		// 		CAirsideReportNode::ReportNodeType nNodeType;
		// 		int nNodeID;
		CString m_strResName;
		FltDelaySegment nSegment;
		int eArriveTime;
		int eActArriveTime;
		FltDelayReason nReason;
		CString m_strDetailReason;
	public:
		BOOL ExportFile(ArctermFile& _file) ;
		BOOL ImportFile(ArctermFile& _file) ;
	};


	class FltDelayItem
	{
	public:
		FltDelayItem(CAirportDatabase* m_AirportDB)
		{
			totalDelayTime = 0;
			airDelayTime = 0;
			taxiDelayTime = 0;
			standDelayTime= 0;
			serviceDelayTime = 0;
			takeoffDelayTime = 0;
			bArrOrDeplDelay = true;
			bValidData = true;
			actStartTime = 0;
			actEndTime = 0;

			planSt = 0;

			ataTime = -1;
			atdTime = -1;

			memset(&fltDesc,0,sizeof(AirsideFlightDescStruct));
			//memset(&m_startNode,0,sizeof(CAirsideReportNode));
			//memset(&m_endNode,0,sizeof(CAirsideReportNode));
			//memset(&m_midNode,0,sizeof(CAirsideReportNode));
			m_fltCons.SetAirportDB(m_AirportDB) ;
		}

	public:
		AirsideFlightDescStruct fltDesc;

		FlightConstraint m_fltCons;
		long totalDelayTime;
		long airDelayTime;
		long taxiDelayTime;
		long standDelayTime;
		long serviceDelayTime;
		long takeoffDelayTime;
		bool bArrOrDeplDelay;// true, arrival delay, false, departure delay
		bool bValidData;

		long actStartTime;
		long actEndTime;

		long smtaTime;
		long smtdTime;

		long planSt;

		long ataTime;
		long atdTime;

		std::vector<FltNodeDelayItem> vNodeDelay;
		CAirsideReportNode m_startNode;
		CAirsideReportNode m_endNode;
		CAirsideReportNode m_midNode;

	public:
		BOOL ExportFile(ArctermFile& _file) ;
		BOOL ImportFile(ArctermFile& _file) ;
	};

	class FltTypeDelayItem
	{
	public:
		FltTypeDelayItem(FlightConstraint& fltCons)
		{
			m_fltCons = fltCons;
			m_fltCons.SetAirportDB(fltCons.GetAirportDB() );
		}
		FltTypeDelayItem(CAirportDatabase* m_AirportDB)
		{
			m_fltCons.SetAirportDB(m_AirportDB) ;
		}
		~FltTypeDelayItem()
		{
		}

		FlightConstraint m_fltCons;
		std::vector<FltDelayItem> m_vDelayData;
	public:
		BOOL ExportFile(ArctermFile& _file) ;
		BOOL ImportFile(ArctermFile& _file) ;
	};

public:
	CAirsideFlightDelayReport(CBGetLogFilePath pFunc);
	virtual ~CAirsideFlightDelayReport(void);

	const std::vector<FltTypeDelayItem>& GetResult(){ return m_vResult;}
	const std::vector<FltDelayItem>& GetTotalResult(){ return m_vTotalResult;}
	void InitResult();
	void AddToResult(FltDelayItem& item);

	virtual int GetReportType(){ return Airside_FlightDelay;}


public:
	void GenerateReport(CParameters * parameter);
	void HandleFlightLog(AirsideFlightLogEntry& fltlog,AirsideFlightLogItem& item,const FlightConflictReportData& ReportConflictData,CAirsideFlightDelayParam * parameter);
	long getHeldAtStandTime(AirsideFlightLogEntry& logEntry)const;
	bool setArrAirModeContent(FltDelayItem& fltArrivalDelayItem,FltNodeDelayItem& nodeDelayItem,long lDelayTime,long lAirTime,const FlightConflictReportData::ConflictDataItem* pData)const;
	bool setDepAirModeContent(FltDelayItem& fltDepartureDelayItem,FltNodeDelayItem& nodeDelayItem,long lDelayTime,long lAirTime,const FlightConflictReportData::ConflictDataItem* pData)const;
	bool setArrTaxiwayModeContent(FltDelayItem& fltArrivalDelayItem,FltNodeDelayItem& nodeDelayItem,long lDelayTime,const FlightConflictReportData::ConflictDataItem* pData)const;
	bool setDepTaxiwayModeContent(FltDelayItem& fltDepartureDelayItem,FltNodeDelayItem& nodeDelayItem,long lDelayTime,const FlightConflictReportData::ConflictDataItem* pData)const;
	bool setArrStandModeContent(FltDelayItem& fltArrivalDelayItem,FltNodeDelayItem& nodeDelayItem,long lDelayTime,const FlightConflictReportData::ConflictDataItem* pData)const;
	bool setDepStandModeContent(FltDelayItem& fltDepartureDelayItem,FltNodeDelayItem& nodeDelayItem,long lDelayTime,const FlightConflictReportData::ConflictDataItem* pData)const;
	bool setArrServiceModeContent(FltDelayItem& fltArrivalDelayItem,FltNodeDelayItem& nodeDelayItem,long lDelayTime,const FlightConflictReportData::ConflictDataItem* pData)const;
	bool setDepTakeoffModeContent(FltDelayItem& fltDepartureDelayItem,FltNodeDelayItem& nodeDelayItem,long lDelayTime,const FlightConflictReportData::ConflictDataItem* pData)const;

	bool calculateStartAndEndTime(bool Arrival,FltDelayItem& fltDelayItem,AirsideFlightLogEntry& logEntry,AirsideFlightLogItem& item);

	bool IsValidTime(AirsideFlightDescStruct& fltlog,CAirsideFlightDelayParam * parameter);

	bool IsFitConstraint(AirsideFlightDescStruct& fltlog,CAirsideFlightDelayParam * parameter,FLTCNSTR_MODE& fltConMode);	

	//return start log item and end item, and total delay time before the start pos
	bool IsPassObject(AirsideFlightLogEntry& logEntry,CAirsideFlightDelayParam * parameter,int& startpos,int& endpos,long& delayTime);
	bool setConflightBelongPosition(AirsideFlightLogEntry& logEntry,int startpos,int endpos,std::vector<FlightConflictReportData::ConflictDataItem*>& vConflictData);
	bool SetStartAndEndNode(AirsideFlightLogEntry& logEntry,CAirsideFlightDelayParam* pParam,FltDelayItem* pFltItem);
	void RefreshReport(CParameters * parameter);

	//init default list ctrl header
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	CAirsideFlightDelayBaseResult *GetReportResult(){ return m_pResult;}

	//when graph select change combox, call these method
	void InitResultListHead(CXListCtrl& cxListCtrl, CParameters * parameter,CSortableHeaderCtrl* piSHC);
	void FillResultListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
private:
	void FillDetailListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	void FillSummaryListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

	void InitDetailListHead(CXListCtrl& cxListCtrl, CParameters * parameter,CSortableHeaderCtrl* piSHC);
	void FillDetailTypeListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	bool fit(int nType,CParameters* parameter)const;

	void InitSummaryListHead(CXListCtrl& cxListCtrl, CParameters * parameter,CSortableHeaderCtrl* piSHC);
	void FillSummaryTypeListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

	//fill summary air delay list ctrl
	void FillAirDelaySummary(CXListCtrl& cxListCtrl, CParameters * parameter);
	//fill summary taxi delay list ctrl
	void FillTaxiDelaySummary(CXListCtrl& cxListCtrl, CParameters * parameter);
	//fill summary stand delay list ctrl
	void FillStandDelaySummary(CXListCtrl& cxListCtrl, CParameters * parameter);
	//fill summary service delay list ctrl
	void FillServiceDelaySummary(CXListCtrl& cxListCtrl, CParameters * parameter);
	//fill summary take off delay list ctrl
	void FillTakeoffDelaySummary(CXListCtrl& cxListCtrl, CParameters * parameter);
	//fill summary flight schedule list ctrl
	void FillSummaryScheduleListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

	bool GetAirsideFlightLogItem(AirsideFlightLogItem& item,AirsideFlightLogEntry logEntry,ARCBaseLog<AirsideFlightLogItem>& mFlightLogData)const;

public:
	virtual CString GetReportFileName();
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);
protected:
	std::vector<FltTypeDelayItem> m_vResult;
	//include all flights' delay time 
	std::vector<FltDelayItem> m_vTotalResult;

	CAirsideFlightDelayBaseResult *m_pResult;

	CAirsideDelayDescLog m_delayDescLog;

	//CDetailTotalDelayResult      *m_pDetailTotalDelayResult;
	//CDetailAirDelayResult        *m_pDetailAirDelayResult;
	//CDetailTaxiDelayResult       *m_pDetailTaxiDelayResult;
	//CDetailStandDelayResult      *m_pDetailStandDelayResult;
	//CDetailServiceDelayResult    *m_pDetailServiceDealyResult;
	//CDetailTakeOffDelayResult    *m_pDetailTakeOffDelayResult;
	//CDetailSegmentDelayResult    *m_pDetailSegmentDelayResult;
	//CDetailComponentDelayResult  *m_pDetailComponentDelayResult;
public:
	BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
	BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
protected:
	BOOL ExportListData(ArctermFile& _file,CParameters * parameter) ;

	void ExportListSummary(ArctermFile& _file,CParameters * parameter) ;
	void ExportListDetail(ArctermFile& _file,CParameters * parameter) ;
};
