#pragma once
#include "../Results/AirsideFlightLogEntry.h"
#include <vector>
#include "CARC3DChart.h"
#include "../MFCExControl/XListCtrl.h"
#include "airsidebasereport.h"
#include <deque>
#include "Results\AirsideReportLog.h"
#include "Results\AirsideFlightNodeLog.h"
#include "Reports\StatisticalTools.h"
#include "TaxiwayUtilizationData.h"
#include <algorithm>

class CTaxiwayUtilizationData;
class CTaxiwayUtilizationSummaryData;
class CCTaxiwayUtilizationBaseChart;
class CTaxiwayUtilizationDetailData;
struct TaxiwayUtilzationSummaryData;
class CTaxiwayItem;

struct TaxiwayData
{
	CString m_sTaxiway;
	CString m_sStartNode;
	CString m_sEndNode;

	long m_intervalStart;
	long m_intervalEnd;
	long m_totalMovement;
	double m_avaSpeed;
	long m_lOccupiedTime;

	FlightInfoList m_vFlightInfoList;

	CStatisticalTools<double> m_statisticalTool;
};

struct TaxiwayFlightInfoData
{
	CString m_sTaxiway;
	CString m_sStartNode;
	CString m_sEndNode;

	FlightInfoList m_vFlightInfoList;

	bool operator == (const TaxiwayFlightInfoData& infoData)const
	{
		if (m_sTaxiway == infoData.m_sTaxiway && m_sStartNode == infoData.m_sStartNode && m_sEndNode == infoData.m_sEndNode)
		{
			return true;
		}

		return false;
	}

	void SortData()
	{
		std::sort(m_vFlightInfoList.begin(),m_vFlightInfoList.end(), std::greater<TaxiwayUtilizationFlightInfo>() );
	}
};
////////////base class////////////////////////////////////////////////////////////////////////
class CAirsideTaxiwayUtilizationBaseResult
{
public:
	CAirsideTaxiwayUtilizationBaseResult();
	virtual ~CAirsideTaxiwayUtilizationBaseResult();
	enum subReportType
	{
		TAXIWAY_UTILIZATION_SPEED = 0,
		TAXIWAY_UTILIZATION_OCCUPANCYTIME,
		TAXIWAY_UTILIZATION_MOVEMENT,
	};

	struct GroupInterval 
	{
		long lStart;
		long lEnd;
	};

	class TaxiwayResultItem
	{
	public:
		class GroupNodeInOut
		{
		public:
			GroupNodeInOut()
				:m_pInLog(NULL)
				,m_pOutLog(NULL)
			{

			}
			bool isVisit()const 
			{
				return (m_pInLog!= NULL && m_pOutLog != NULL);
			}
			bool exsit(FlightCrossNodeLog* pNodeLog)const
			{
				if (m_pInLog && m_pInLog == pNodeLog)
				{
					return true;
				}
				if (m_pOutLog && m_pOutLog == pNodeLog)
				{
					return true;
				}
				return false;
			}

			bool findNode(FlightCrossNodeLog* pNodeLog)const
			{
				if(isVisit())
					return false;

				FlightCrossNodeLog* pLog = getNode();
				if (pLog == NULL)
				{
					return false;
				}

				return (pLog->mNodeId == pNodeLog->mNodeId);
			}

			void setNode(FlightCrossNodeLog* pNodeLog)
			{
				ASSERT(pNodeLog);
				if (pNodeLog->meType == FlightCrossNodeLog::IN_NODE)
				{
					m_pInLog = pNodeLog;
				}
				else if (pNodeLog->meType == FlightCrossNodeLog::OUT_NODE)
				{
					m_pOutLog = pNodeLog;
				}
			}

			FlightCrossNodeLog* getNode()const
			{
				if (m_pInLog)
				{
					return m_pInLog;
				}
				
				if (m_pOutLog)
				{
					return m_pOutLog;
				}

				return NULL;
			};
			FlightCrossNodeLog* m_pInLog;
			FlightCrossNodeLog* m_pOutLog;
		};

	public:
		TaxiwayResultItem(CAirsideTaxiwayUtilizationBaseResult* pParent)
			:m_pParent(pParent)
			,m_bAreadyGenerateResult(false)
			,m_bWaitForGenerationResult(false)
		{
		}
		~TaxiwayResultItem()
		{
			
		}
		std::vector<GroupNodeInOut*> m_vVisitFlag;
		bool m_bAreadyGenerateResult;//this item already generation result
		bool m_bWaitForGenerationResult;
		void setGroupNode(FlightCrossNodeLog* pNodeLog);
		bool exsit(FlightCrossNodeLog* pNodeLog)const;
		bool isVisit()const;
		bool findNode(FlightCrossNodeLog* pNodeLog)const;
		bool BuiltDetailResult(AirsideFlightNodeLog *pNodeLog,const AirsideFlightLogItem& flightItem,AirsideFlightLogEntry& logEntry,CParameters *pParameter,int& nFlag);
		bool BuiltSummaryResult(AirsideFlightNodeLog *pNodeLog,const AirsideFlightLogItem& flightItem,AirsideFlightLogEntry& logEntry,CParameters *pParameter,std::vector<TaxiwayData*>& vResult,int& nFlag);
		GroupNodeInOut* getGroupNode(FlightCrossNodeLog* pNodeLog);

		double GetNodeSpeed(long startTime,long endTime,AirsideFlightLogEntry& logEntry);

	private:
		CAirsideTaxiwayUtilizationBaseResult* m_pParent;
	};


	virtual void RefreshReport(CParameters * parameter) = 0;
	virtual void GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter) = 0;
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC) = 0;
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter) = 0;
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg);
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg);
	virtual BOOL ExportListData(ArctermFile& _file,CParameters * parameter);
	virtual enumASReportType_Detail_Summary GetReportType()const = 0;

	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

	void setLoadFromFile(bool bLoad);
	bool IsLoadFromFile();
	void SetCBGetFilePath(CBGetLogFilePath pFunc);
	CBGetLogFilePath m_pCBGetLogFilePath;
	std::vector<CTaxiwayUtilizationData*>& GetResult(){return m_vResult;}

	void GenerateIntervalTime(CParameters *pParameter);
	void getIntervalTime(GroupInterval& groupSource,AirsideFlightNodeLog *pNodeLog,CParameters *pParameter);

	std::vector<std::pair<CString,std::pair<CString,CString> > > GetTaxiwayResultCount(std::vector<TaxiwayData*>& vResult);
protected:
	bool m_bLoadFromFile;
	CCTaxiwayUtilizationBaseChart* m_pChartResult;//for drawing graph
	std::vector<CTaxiwayUtilizationData*> m_vResult;
	std::vector<GroupInterval> m_vGroup;
	std::deque<TaxiwayResultItem*>m_vResultItem;
	std::vector<TaxiwayFlightInfoData*>m_vFlightInfoList;
	std::vector<TaxiwayResultItem::GroupNodeInOut*>m_vAllGroupNodeInOut;
};

/////////////////////////////////Detail result///////////////////////////////////////////////
class CAirsideTaxiwayUtilizationDetailResult : public CAirsideTaxiwayUtilizationBaseResult
{
public:
	CAirsideTaxiwayUtilizationDetailResult();
	~CAirsideTaxiwayUtilizationDetailResult();

public:
	void RefreshReport(CParameters * parameter);
	void GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter);
	void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC);
	void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	CTaxiwayUtilizationDetailData* getResultItem(CTaxiwayItem& taxiwayItem,AirsideFlightNodeLog *pNodeLog,CParameters *pParameter);
	bool haveData(CTaxiwayUtilizationDetailData& data);
	TaxiwayFlightInfoData* getFlightInfoData(CTaxiwayUtilizationDetailData* pDetail);

	enumASReportType_Detail_Summary GetReportType()const{return ASReportType_Detail;}

protected:
	void HandleIntersectionNode(AirsideFlightLogEntry& logEntry,AirsideFlightNodeLog *pNodeLog,const AirsideFlightLogItem& flightItem, CParameters *pParameter);
	void GenerateAllDetailaData();
	void BuiltFlightInfoList();
	TaxiwayFlightInfoData* exsitFlightInfo(const TaxiwayFlightInfoData* pData);
};

/////////////////////////////summary result///////////////////////////////////////////////////
class CAirsideTaxiwayUtilizationSummaryResult : public CAirsideTaxiwayUtilizationBaseResult
{
public:
	CAirsideTaxiwayUtilizationSummaryResult();
	~CAirsideTaxiwayUtilizationSummaryResult();

public:
	void RefreshReport(CParameters * parameter);
	void GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter);
	void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC);
	void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	void HandleSummaryData(AirsideFlightLogEntry& logEntry,AirsideFlightNodeLog *pNodeLog,const AirsideFlightLogItem& flightItem,std::vector<TaxiwayData*>& vResult,CParameters *pParameter);
	TaxiwayData* getResultItem(CTaxiwayItem& taxiwayItem,AirsideFlightNodeLog *pNodeLog,std::vector<TaxiwayData*>& vResult,CParameters *pParameter);
	long GetMovements(const ElapsedTime& startTime,const ElapsedTime& endTime,const ElapsedTime& eIntervalTime,const CTaxiwayUtilizationSummaryData* item,std::vector<TaxiwayData*>&vResult);
	double GetSpeed(const ElapsedTime& startTime,const ElapsedTime& endTime,const ElapsedTime& eIntervalTime,const CTaxiwayUtilizationSummaryData* item,std::vector<TaxiwayData*>&vResult);
	long GetOccupancyTime(const ElapsedTime& startTime,const ElapsedTime& endTime,const ElapsedTime& eIntervalTime,const CTaxiwayUtilizationSummaryData* item,std::vector<TaxiwayData*>&vResult);
	void InitStaticData(CParameters* pPara,subReportType emType,CTaxiwayUtilizationSummaryData* item,std::vector<TaxiwayData*>& vResult);
	void InitToolData(TaxiwayUtilzationSummaryData& data,CStatisticalTools<double>& statisticalTool);	
	enumASReportType_Detail_Summary GetReportType()const{return ASReportType_Summary;}
};