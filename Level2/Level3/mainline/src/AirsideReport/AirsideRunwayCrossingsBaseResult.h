#pragma once
#include "../Results/AirsideFlightLogEntry.h"
#include <vector>
#include "CARC3DChart.h"
#include "../MFCExControl/XListCtrl.h"
#include "airsidebasereport.h"
#include "../Common/ALTObjectID.h"
class CRunwayCrossingBaseDetailResult;
class CRunwayCrossingBaseSummaryResult;
class CAirsideRunwayCrossingsBaseResult
{
public:
	CAirsideRunwayCrossingsBaseResult();
	virtual ~CAirsideRunwayCrossingsBaseResult();
	enum subReportType
	{
		DT_COUNT = 0,
		DT_WAITTIME,
		DT_TAXINODE,

		SM_COUNT,
		SM_WAITTIME
	};

	virtual void RefreshReport(CParameters * parameter) = 0;
	virtual void GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter) = 0;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType) = 0;
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
};

///////////////////////////////////////////////////////////////////////////////////////////////
class CAirsideRunwayCrossingBaseDetailResult : public CAirsideRunwayCrossingsBaseResult
{
public:
	CAirsideRunwayCrossingBaseDetailResult();
	~CAirsideRunwayCrossingBaseDetailResult();
public:
	class DetailRunwayCrossingsItem
	{
	public:
		DetailRunwayCrossingsItem()
			:m_sRunway(_T(""))
			,m_sEnterTaxiway(_T(""))
			,m_sExitTaxiway(_T(""))
			,m_sFlightID(_T(""))
			,m_sActype(_T(""))
			,m_sNodeName(_T(""))
			,m_lEnterRunwayTime(0L)
			,m_lExitRunwayTime(0L)
			,m_lOccupancyTime(0L)
			,m_lWaitTimeFroRunwayCrossings(0L)
			,m_dRunwayCrossingsSpeed(0.0)
		{

		}

		~DetailRunwayCrossingsItem()
		{

		}

		CString m_sRunway;
		CString m_sEnterTaxiway;
		CString m_sExitTaxiway;
		CString m_sFlightID;
		CString m_sActype;
		CString m_sNodeName;
		long m_lEnterRunwayTime;
		long m_lExitRunwayTime;
		long m_lOccupancyTime;
		long m_lWaitTimeFroRunwayCrossings;
		double m_dRunwayCrossingsSpeed;

	public:
		BOOL ExportFile(ArctermFile& _file);
		BOOL ImportFile(ArctermFile& _file);
	};
public:
	virtual void GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	void RefreshReport(CParameters * parameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg);
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg);
	virtual BOOL ExportListData(ArctermFile& _file,CParameters * parameter) ;

	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

private:
	CRunwayCrossingBaseDetailResult* m_pChartResult;
	std::vector<DetailRunwayCrossingsItem> m_vResult;
};

///////////////////////////////////////////////////////////////////////////////////////////////
class CAirsideRunwayCrossingsBaseSummaryResult : public CAirsideRunwayCrossingsBaseResult
{
public:
	CAirsideRunwayCrossingsBaseSummaryResult();
	~CAirsideRunwayCrossingsBaseSummaryResult();

public:
	class SummaryRunwayItem
	{
	public:
		SummaryRunwayItem()
			:m_sRunway(_T(""))
			,m_sNodeName(_T(""))
			,m_sEnterTaxiway(_T(""))
			,m_lWaitTime(0)
			,m_lEnterRunway(0)
		{

		}
		~SummaryRunwayItem()
		{

		}
	public:
		CString m_sRunway;
		CString m_sNodeName;
		CString m_sEnterTaxiway;
		long	m_lWaitTime;
		long	m_lEnterRunway;
	};

	class StatisticsSummaryRunwayCrossingsItem
	{
	public:
		StatisticsSummaryRunwayCrossingsItem()
		{
			//quarter
			m_nQ1 = 0;
			m_nQ2 = 0;
			m_nQ3 = 0;

			//percentage
			m_nP1 = 0;
			m_nP5 = 0;
			m_nP10 = 0;
			m_nP90 = 0;
			m_nP95 = 0;
			m_nP99 = 0;
			m_nStdDev = 0;
		}
		~StatisticsSummaryRunwayCrossingsItem()
		{

		}
	public:
		//quarter
		int m_nQ1;
		int m_nQ2;
		int m_nQ3;

		//percentage
		int m_nP1;
		int m_nP5;
		int m_nP10;
		int m_nP90;
		int m_nP95;
		int m_nP99;
		int m_nStdDev;
	};
	class SummaryRunwayCrossingsItem
	{
	public:
		SummaryRunwayCrossingsItem()
			:m_sRunway(_T(""))
			,m_sEnterTaxiway(_T(""))
			,m_lMinCrossings(0)
			,m_lIntervalMinCrossings(0)
			,m_lMaxCrossings(0)
			,m_dAverageCrosings(0.0)
			,m_lIntervalMaxCrossings(0)
			,m_lTotalCrossings(0)
			,m_lMinWaitTime(0L)
			,m_lIntervalMinWaitTime(0L)
			,m_lMaxWaitTime(0L)
			,m_lAverageWaitTime(0L)
			,m_lIntervalMaxWaitTime(0L)
			,m_lTotalWaitTime(0L)
		{

		}

		~SummaryRunwayCrossingsItem()
		{

		}

		enum SummaryRunwayCrossingType
		{
			SM_CROSSSINGS,
			SM_WAITTIME
		};
		CString m_sRunway;
		CString m_sEnterTaxiway;
		CString m_sNodeName;
		long m_lMinCrossings;
		long m_lIntervalMinCrossings;
		long m_lMaxCrossings;
		double m_dAverageCrosings;
		long m_lIntervalMaxCrossings;
		long m_lTotalCrossings;
		long m_lMinWaitTime;
		long m_lIntervalMinWaitTime;
		long m_lMaxWaitTime;
		long m_lAverageWaitTime;
		long m_lIntervalMaxWaitTime;
		long m_lTotalWaitTime;
		StatisticsSummaryRunwayCrossingsItem m_staCrossingCount;
		StatisticsSummaryRunwayCrossingsItem m_staWaitTime;
	public:
		BOOL ExportFile(ArctermFile& _file);
		BOOL ImportFile(ArctermFile& _file);
	};
public:
	virtual void GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	void RefreshReport(CParameters * parameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg);
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg);
	virtual BOOL ExportListData(ArctermFile& _file,CParameters * parameter) ;

	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

protected:
	void GetMaxCrossingsCountAndIntervalIndex(CParameters* pPara,SummaryRunwayCrossingsItem& item,std::vector<SummaryRunwayItem>&vResult);
	void GetMinCrossingsCountAndIntervalIndex(CParameters* pPara,SummaryRunwayCrossingsItem& item,std::vector<SummaryRunwayItem>&vResult);
	void GetAverageCrossingsCountAndTotal(CParameters* pPara,SummaryRunwayCrossingsItem& item,std::vector<SummaryRunwayItem>&vResult);

	void GetMaxWaitTimeAndItervalMaxIndex(CParameters* pPara,SummaryRunwayCrossingsItem& item,std::vector<SummaryRunwayItem>&vResult);
	void GetMinWaitTimeAndIntervalMinIndex(CParameters* pPara,SummaryRunwayCrossingsItem& item,std::vector<SummaryRunwayItem>&vResult);
	void GetAverageWaitTimeAndTotal(CParameters* pPara,SummaryRunwayCrossingsItem& item,std::vector<SummaryRunwayItem>&vResult);
	

	CString GetTaxiwayString(SummaryRunwayCrossingsItem& item,std::vector<SummaryRunwayItem>&vResult);
	long GetWaitTime(const ElapsedTime& startTime,const ElapsedTime& endTime,const ElapsedTime& eIntervalTime,const SummaryRunwayCrossingsItem& item,std::vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayItem>&vResult);
	long GetCrossingsCount(const ElapsedTime& startTime,const ElapsedTime& endTime,const ElapsedTime& eIntervalTime,const SummaryRunwayCrossingsItem& item,std::vector<SummaryRunwayItem>&vResult);
	long GetItemMaxWaitTime(const ElapsedTime& startTime,const ElapsedTime& endTime,const ElapsedTime& eIntervalTime,const SummaryRunwayCrossingsItem& item,std::vector<SummaryRunwayItem>&vResult);
	long GetItemMinWaitTime(const ElapsedTime& startTime,const ElapsedTime& endTime,const ElapsedTime& eIntervalTime,const SummaryRunwayCrossingsItem& item,std::vector<SummaryRunwayItem>&vResult);

	void InitStaticData(CParameters* pPara,SummaryRunwayCrossingsItem::SummaryRunwayCrossingType emType,SummaryRunwayCrossingsItem& item,std::vector<SummaryRunwayItem>&vResult);

	std::vector<std::pair<CString,ALTObjectID> > GetRunwayList(std::vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayItem>& vResult);
private:
	CRunwayCrossingBaseSummaryResult* m_pChartResult;
	std::vector<SummaryRunwayCrossingsItem> m_vResult;
};