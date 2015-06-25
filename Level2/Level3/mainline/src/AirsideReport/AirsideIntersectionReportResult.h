#pragma once
#include "../Results/AirsideFlightLogEntry.h"
#include <vector>
#include "CARC3DChart.h"
#include "../MFCExControl/XListCtrl.h"
#include "airsidebasereport.h"
#include "Reports\StatisticalTools.h"
#include <algorithm>
#include <queue>
#include <list>

class AirsideFlightLogItem;
class AirsideFlightNodeLog;
class CArisideIntersectionDetailBaseChartResult;
class CSortableHeaderCtrl;
class AIRSIDEREPORT_API CAirsideIntersectionReportBaseResult
{
public:
	CAirsideIntersectionReportBaseResult(void);
	virtual ~CAirsideIntersectionReportBaseResult(void);

	enum subReportType
	{
		DT_CROSSING = 0,
		DT_BUSYFACTOR,
		DT_DELAY,
		DT_CONFLICT,

		SM_CROSSING,
		SM_DELAY
	};
	class ConflictItem
	{
	public:
		ConflictItem()
		{

		}
		~ConflictItem()
		{

		}

		ElapsedTime m_eTimeDealy;
		CString m_strFlight; //conflict flight
		CString m_strType;//conflict type

		CString m_strNodeFrom;
		CString m_strOrigin;

		CString m_strNodeTo;
		CString m_strDest;

	};
	class ResultItem
	{
	public:
		ResultItem()
		{
			m_nNodeID = -1;
			m_nIndex = -1;
		}
		~ResultItem()
		{

		}
	public:
		int m_nIndex;
		//node index, start from 1. 1,2,3,4,5...

		int m_nNodeID;
		CString m_strNodeName;

		CString m_strFlight;
		CString m_strACType;
		
		ElapsedTime m_eTimeIn;

		ElapsedTime m_eTimeOut;

		ElapsedTime m_eTimeBuffer;

		CString m_strNodeFrom;
		CString m_strOrigin;

		CString m_strNodeTo;
		CString m_strDest;


		std::vector<ConflictItem> m_vConflicts;

	public:
		ElapsedTime getBufferTime() const{ return m_eTimeBuffer - m_eTimeOut;}
		ElapsedTime getDurationBusy() const{ return m_eTimeBuffer - m_eTimeIn;}

		ElapsedTime getTotalDelayTime() const
		{
			ElapsedTime eTotalDelayTime;
			
			for (int nDelay = 0; nDelay < static_cast<int>(m_vConflicts.size()); ++nDelay)
			{
				eTotalDelayTime += m_vConflicts[nDelay].m_eTimeDealy;
			}			
			return eTotalDelayTime;
		}

		int GetDelayCount(){return static_cast<int>(m_vConflicts.size());}
	private:

	};



	virtual void RefreshReport(CParameters * parameter) = 0;
	virtual void GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC) = 0;
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter) = 0;
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg) = 0 ;
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg) = 0 ;
	virtual BOOL ExportListData(ArctermFile& _file,CParameters * parameter) = 0;

	virtual BOOL WriteReportData(ArctermFile& _file) = 0;
	virtual BOOL ReadReportData(ArctermFile& _file) = 0;

	void setLoadFromFile(bool bLoad);
	bool IsLoadFromFile();
	void SetCBGetFilePath(CBGetLogFilePath pFunc);
	CBGetLogFilePath m_pCBGetLogFilePath;
protected:
	bool m_bLoadFromFile;

	std::vector<ResultItem> m_vResult;

protected:
	void HandleIntersectionNode(std::list<AirsideFlightNodeLog*>& vNodeLog,const AirsideFlightLogItem& flightItem, CParameters *pParameter);
};



class AIRSIDEREPORT_API CAirsideIntersectionDetailResult : public CAirsideIntersectionReportBaseResult
{
public:

	
public:
	CAirsideIntersectionDetailResult();
	~CAirsideIntersectionDetailResult();

public:
	virtual void RefreshReport(CParameters * parameter);
	virtual void GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg);
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg);
	virtual BOOL ExportListData(ArctermFile& _file,CParameters * parameter);

	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

protected:
	void InsertItemToList(CXListCtrl& cxListCtrl, int nIndex, const ResultItem& resultItem, const ConflictItem& conflictItem);



protected:
	CArisideIntersectionDetailBaseChartResult *m_pChartResult;
private:
};

class CAirsideIntersectionSummaryBaseChartResult;
class AIRSIDEREPORT_API CAirsideIntersectionSummarylResult : public CAirsideIntersectionReportBaseResult
{
public:
	class StatisticsSummaryData
	{
	public:
		StatisticsSummaryData()
		{
			////quarter
			//m_nQ1 = 0;
			//m_nQ2 = 0;
			//m_nQ3 = 0;

			////percentage
			//m_nP1 = 0;
			//m_nP5 = 0;
			//m_nP10 = 0;
			//m_nP90 = 0;
			//m_nP95 = 0;
			//m_nP99 = 0;
			//m_nStdDev = 0;
		}
		~StatisticsSummaryData()
		{

		}
	public:
		ElapsedTime m_minTime;
		ElapsedTime m_avgTime;
		ElapsedTime m_maxTime;
		//quarter
		ElapsedTime m_nQ1;
		ElapsedTime m_nQ2;
		ElapsedTime m_nQ3;

		//percentage
		ElapsedTime m_nP1;
		ElapsedTime m_nP5;
		ElapsedTime m_nP10;
		ElapsedTime m_nP90;
		ElapsedTime m_nP95;
		ElapsedTime m_nP99;
		ElapsedTime m_nStdDev;


		void WirteFile(ArctermFile& _file)
		{
			_file.writeTime(m_minTime,TRUE);
			_file.writeTime(m_avgTime,TRUE);
			_file.writeTime(m_maxTime,TRUE);
			_file.writeTime(m_nQ1,TRUE);
			_file.writeTime(m_nQ2,TRUE);
			_file.writeTime(m_nQ3,TRUE);
			_file.writeTime(m_nP1,TRUE);
			_file.writeTime(m_nP5,TRUE);
			_file.writeTime(m_nP10,TRUE);
			_file.writeTime(m_nP90,TRUE);
			_file.writeTime(m_nP95,TRUE);
			_file.writeTime(m_nP99,TRUE);
			_file.writeTime(m_nStdDev,TRUE);
		}
		void ReadFile(ArctermFile& _file)
		{
			_file.getTime(m_minTime,TRUE);
			_file.getTime(m_avgTime,TRUE);
			_file.getTime(m_maxTime,TRUE);
			_file.getTime(m_nQ1,TRUE);
			_file.getTime(m_nQ2,TRUE);
			_file.getTime(m_nQ3,TRUE);
			_file.getTime(m_nP1,TRUE);
			_file.getTime(m_nP5,TRUE);
			_file.getTime(m_nP10,TRUE);
			_file.getTime(m_nP90,TRUE);
			_file.getTime(m_nP95,TRUE);
			_file.getTime(m_nP99,TRUE);
			_file.getTime(m_nStdDev,TRUE);
		}
	};



	//summary result
	class SummaryResultItem
	{
	public:
		SummaryResultItem()
		{
			m_nNodeID = -1;
			m_nCrossingCount = 0;
			m_nConflictCount = 0;
		}
		~SummaryResultItem(){}
	public:
		int m_nNodeID;
		CString m_strNodeName;

		ElapsedTime m_eStartTime;//interval from
		ElapsedTime m_eEndTime;//interval to

		int m_nCrossingCount;
		StatisticsSummaryData summaryCrossing;
		StatisticsSummaryData summaryDelay;
		int m_nConflictCount;

	public:
		CString GetInterval()
		{
			CString strInterval;
			strInterval.Format(_T("%s - %s"),m_eStartTime.PrintDateTime(), m_eEndTime.PrintDateTime());
			return strInterval;
		}
	protected:
	private:
	};

	class NodeSummaryResultItem
	{
	public:
		NodeSummaryResultItem()
		{

		}
		~NodeSummaryResultItem()
		{

		}
	public:
		int m_nNodeID;
		CString m_strNodeName;
		std::vector<SummaryResultItem> m_vIntervalItem;
	protected:
	private:
	};

	//summary middle result
	class IntervalSummaryMiddleResult
	{
	public:
		IntervalSummaryMiddleResult(){}
		~IntervalSummaryMiddleResult(){}

	public:
		ElapsedTime m_eStartTime;//interval from
		ElapsedTime m_eEndTime;//interval to

		std::vector<ResultItem> m_vResultItem;
	
		void GenerateSummaryResultItem(SummaryResultItem& summaryItem)
		{
			summaryItem.m_eStartTime = m_eStartTime;
			summaryItem.m_eEndTime = m_eEndTime;
			//crossing count
			summaryItem.m_nCrossingCount = (int)m_vResultItem.size();

			//crossing time, include buffer
			CStatisticalTools<ElapsedTime> statisticalToolCrossing;
			CStatisticalTools<ElapsedTime> statisticalToolDelay;
			
			ElapsedTime eMinCrossTime;
			ElapsedTime eMaxCrossTime;
			ElapsedTime eTotalCrossTime;


			ElapsedTime eMinDelayTime;
			ElapsedTime eMaxDelayTime;
			ElapsedTime eTotalDelayTime;
			for (int nResult = 0; nResult < static_cast<int>(m_vResultItem.size()); ++nResult)
			{
				ResultItem& resultItem = m_vResultItem[nResult];
				
				if(nResult == 0)
				{
					eMinCrossTime = resultItem.getDurationBusy();
					eMaxCrossTime = eMinCrossTime;



					eMinDelayTime = resultItem.getTotalDelayTime();
					eMaxDelayTime = eMinDelayTime;
		
				}

				eTotalCrossTime += resultItem.getDurationBusy();
				eTotalDelayTime += resultItem.getTotalDelayTime();


				statisticalToolCrossing.AddNewData(resultItem.getDurationBusy());

				//conflict count
				summaryItem.m_nConflictCount += resultItem.GetDelayCount();

				statisticalToolDelay.AddNewData(resultItem.getTotalDelayTime());

				if(resultItem.getDurationBusy() < eMinCrossTime)
					eMinCrossTime = resultItem.getDurationBusy();
				
				if(resultItem.getDurationBusy() > eMaxCrossTime)
					eMaxCrossTime = resultItem.getDurationBusy();

				if(resultItem.getTotalDelayTime() < eMinDelayTime && eMinDelayTime > ElapsedTime(0L))
					eMinDelayTime = resultItem.getTotalDelayTime();

				if(resultItem.getTotalDelayTime() > eMaxDelayTime)
					eMaxDelayTime = resultItem.getTotalDelayTime();
			}

			//crossing time
			statisticalToolCrossing.SortData();

			summaryItem.summaryCrossing.m_minTime = eMinCrossTime;
			ElapsedTime eAvgTime;
			if(summaryItem.m_nCrossingCount > 0)
			{
				long lAvgTime = eTotalCrossTime.getPrecisely()/summaryItem.m_nCrossingCount;
				eAvgTime.setPrecisely(lAvgTime);
			}
			summaryItem.summaryCrossing.m_avgTime =eAvgTime;
			summaryItem.summaryCrossing.m_maxTime =eMaxCrossTime;

			summaryItem.summaryCrossing.m_nQ1 = statisticalToolCrossing.GetPercentile(25);
			summaryItem.summaryCrossing.m_nQ2 = statisticalToolCrossing.GetPercentile(50);
			summaryItem.summaryCrossing.m_nQ3 = statisticalToolCrossing.GetPercentile(75);
			summaryItem.summaryCrossing.m_nP1 = statisticalToolCrossing.GetPercentile(1);
			summaryItem.summaryCrossing.m_nP5 = statisticalToolCrossing.GetPercentile(5);
			summaryItem.summaryCrossing.m_nP10 = statisticalToolCrossing.GetPercentile(10);
			summaryItem.summaryCrossing.m_nP90 = statisticalToolCrossing.GetPercentile(90);
			summaryItem.summaryCrossing.m_nP95 = statisticalToolCrossing.GetPercentile(95);
			summaryItem.summaryCrossing.m_nP99 = statisticalToolCrossing.GetPercentile(99);
			double dSigma =  statisticalToolCrossing.GetSigma();
			ElapsedTime eSigma;
			eSigma.setPrecisely(static_cast<long>(dSigma));
			summaryItem.summaryCrossing.m_nStdDev = eSigma;


			//delay time
			statisticalToolDelay.SortData();

			summaryItem.summaryDelay.m_minTime = eMinDelayTime;
			if(summaryItem.m_nCrossingCount > 0)
			{
				long lAvgTime = eTotalDelayTime.getPrecisely()/summaryItem.m_nCrossingCount;
				eAvgTime.setPrecisely(lAvgTime);
			}
			summaryItem.summaryDelay.m_avgTime =eAvgTime;
			summaryItem.summaryDelay.m_maxTime =eMaxDelayTime;


			summaryItem.summaryDelay.m_nQ1 = statisticalToolDelay.GetPercentile(25);
			summaryItem.summaryDelay.m_nQ2 = statisticalToolDelay.GetPercentile(50);
			summaryItem.summaryDelay.m_nQ3 = statisticalToolDelay.GetPercentile(75);
			summaryItem.summaryDelay.m_nP1 = statisticalToolDelay.GetPercentile(1);
			summaryItem.summaryDelay.m_nP5 = statisticalToolDelay.GetPercentile(5);
			summaryItem.summaryDelay.m_nP10 = statisticalToolDelay.GetPercentile(10);
			summaryItem.summaryDelay.m_nP90 = statisticalToolDelay.GetPercentile(90);
			summaryItem.summaryDelay.m_nP95 = statisticalToolDelay.GetPercentile(95);
			summaryItem.summaryDelay.m_nP99 = statisticalToolDelay.GetPercentile(99);
			
			dSigma =  statisticalToolDelay.GetSigma();
			eSigma.setPrecisely(static_cast<long>(dSigma));
			summaryItem.summaryDelay.m_nStdDev = eSigma;

		}
	};

	//summary middle result
	class NodeSummaryMiddleResult
	{
	public:
		NodeSummaryMiddleResult(){m_nNodeID = -1;}
		~NodeSummaryMiddleResult(){}

	public:
		int m_nNodeID;
		CString m_strNodeName;
		ElapsedTime m_eStartTime;//report start time
		ElapsedTime m_eEndTime;//report end time
		ElapsedTime m_eInterval;//report interval

		std::vector<IntervalSummaryMiddleResult> m_vResultItem;

	public:
		void InitResult(const ElapsedTime& eStartTime, const ElapsedTime& eEndTime, const ElapsedTime& eInterval)
		{
			m_eStartTime = eStartTime;
			m_eEndTime = m_eEndTime;
			m_eInterval = eInterval;

			
			for (ElapsedTime eTime = eStartTime; eTime < eEndTime; eTime += eInterval)
			{
				IntervalSummaryMiddleResult resultItem;
				resultItem.m_eStartTime = eTime;
				resultItem.m_eEndTime = eTime + eInterval;
				m_vResultItem.push_back(resultItem);
			}
		}

		void AddResultItem(const ResultItem& resultItem)
		{
			for (int nResult = 0; nResult < static_cast<int>(m_vResultItem.size()); ++nResult)
			{
				if(m_vResultItem[nResult].m_eStartTime >= resultItem.m_eTimeIn)
				{
					m_vResultItem[nResult].m_vResultItem.push_back(resultItem);
					break;
				}
			}
		}
		void GenerateSummaryResult(std::vector<NodeSummaryResultItem>& vSummaryItem)
		{		
			NodeSummaryResultItem NodeSummaryItem;
			NodeSummaryItem.m_nNodeID = m_nNodeID;
			NodeSummaryItem.m_strNodeName = m_strNodeName;

			for (int nResult = 0; nResult < static_cast<int>(m_vResultItem.size()); ++nResult)
			{	
				SummaryResultItem summaryItem;
				summaryItem.m_nNodeID = m_nNodeID;
				summaryItem.m_strNodeName = m_strNodeName;
				m_vResultItem[nResult].GenerateSummaryResultItem(summaryItem);
				NodeSummaryItem.m_vIntervalItem.push_back(summaryItem);
			}
			vSummaryItem.push_back(NodeSummaryItem);
		}
	};


public:
	CAirsideIntersectionSummarylResult();
	~CAirsideIntersectionSummarylResult();
public:
	virtual void RefreshReport(CParameters * parameter);
	virtual void GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg);
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg);
	virtual BOOL ExportListData(ArctermFile& _file,CParameters * parameter);

	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

	protected:
		CString FormatString(int nValue)
		{
			CString strValue;
			strValue.Format(_T("%d"),nValue);
			return strValue;
		}
public:
	std::map<int, NodeSummaryMiddleResult > m_mapNodeMiddleResult;

	std::vector<NodeSummaryResultItem> m_vSummaryResult;

	protected:
		void GenerateSummaryResult();

		CAirsideIntersectionSummaryBaseChartResult *m_pChartResult;
private:
};




























