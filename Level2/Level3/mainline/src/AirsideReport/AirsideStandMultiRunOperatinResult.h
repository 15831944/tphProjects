#pragma once
#include "airsidemultiplerunresult.h"
class CParameters;
class C2DChartData;

struct StandMultipleOperationData
{
	StandMultipleOperationData()
		:m_lSchedOccupancy(0)
        ,m_lOccupiedTime(0l)
		,m_lDelayEnter(0l)
		,m_lDelayLeaving(0l)
		,m_fltmode('A')
	{

	}

	bool IsConflict()const
	{
		if (!m_sSchedName.IsEmpty() && !m_sActualName.IsEmpty() && m_sSchedName != m_sActualName)
			return true;

		if (m_lDelayEnter > 0l)
			return true;

		return false;
	}

	bool IsDelay()const
	{
		if (m_lDelayEnter > 0l)
			return true;

		if (m_lDelayLeaving > 0l)
			return true;

		return false;
	}

	CString m_sSchedName;
	CString m_sActualName;
    long m_lSchedOccupancy;
	long m_lOccupiedTime;
	long m_lDelayEnter;
	long m_lDelayLeaving;
	char m_fltmode;
};

class CStandSummaryData
{
public:
	CStandSummaryData(void)
		:m_dTotal(0.0)
		,m_dMin(0.0)
		,m_dAverage(0.0)
		,m_dMax(0.0)
		,m_dQ1(0.0)
		,m_dQ2(0.0)
		,m_dQ3(0.0)
		,m_dP1(0.0)
		,m_dP5(0.0)
		,m_dP10(0.0)
		,m_dP90(0.0)
		,m_dP95(0.0)
		,m_dP99(0.0)
		,m_dSigma(0.0)
	{

	}
	~CStandSummaryData(void)
	{

	}

public:
	double   m_dTotal;
	double   m_dMin;
	double   m_dAverage;
	double   m_dMax;
	double   m_dQ1;
	double   m_dQ2;
	double   m_dQ3;
	double   m_dP1;
	double   m_dP5;
	double   m_dP10;
	double   m_dP90;
	double   m_dP95;
	double   m_dP99;
	double   m_dSigma;	
};
typedef std::map<CString,long> mapStandResult;
typedef std::map<CString,mapStandResult> mapStandOpResult;
typedef std::map<CString, CStandSummaryData> MultiStandRunSummaryMap;
typedef std::map<CString,std::vector<StandMultipleOperationData>> MapMultiRunStandOperationData;
class AIRSIDEREPORT_API CAirsideStandMultiRunOperatinResult :
	public CAirsideMultipleRunResult
{
public:
	CAirsideStandMultiRunOperatinResult(void);
	~CAirsideStandMultiRunOperatinResult(void);

	virtual void LoadMultipleRunReport(CParameters* pParameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl,CParameters * parameter,  int iType = 0,CSortableHeaderCtrl* piSHC=NULL);

    virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter, int iType = 0);


    virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType = 0);
	virtual BOOL WriteReportData( ArctermFile& _file );
	virtual BOOL ReadReportData( ArctermFile& _file );
	virtual CString GetReportFileName()const;

private:
	void BuildDetailStandOperationOccupancy(CParameters* pParameter,MapMultiRunStandOperationData standOperationData);
	void BuildDetailStandOperationIdel(CParameters* pParameter,MapMultiRunStandOperationData standOperationData);
	void BuildDetailStandOperationPencentage(CParameters* pParameter,MapMultiRunStandOperationData standOperationData);
	void BuildDetailStandOperationDelay(CParameters* pParameter,MultiRunDetailMap& mapDetailData,MapMultiRunStandOperationData standOperationData,char fltMode);
	void BuildDetailStandOperationConflict(CParameters* pParameter,MultiRunDetailMap& mapDetailData,MapMultiRunStandOperationData standOperationData,char fltMode);

	void InitDetailListHead(CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData,CSortableHeaderCtrl* piSHC=NULL);
	void InitDetailListConfictHead(CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData,CSortableHeaderCtrl* piSHC=NULL);
	void InitDetailListPercentagetHead(CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData,CSortableHeaderCtrl* piSHC/* =NULL */);
	void FillDetailListCountContent(CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData);

	void Generate3DChartCountData(MultiRunDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter);
	void Generate3DChartPercentageData(MultiRunDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter);
	void Generate3DChartConflictData(MultiRunDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter,int iType);
	void Generate3DChartDelayData(MultiRunDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter,int iType);

	void SetDetail3DChartString( C2DChartData& c2dGraphData,CParameters *pParameter,int iType = 0 );
	void SetDetail3DConflictChartString(C2DChartData& c2dGraphData,CParameters *pParameter,int iType = 0);
	void SetDetail3DDelayChartString(C2DChartData& c2dGraphData, CParameters *pParameter,int iType = 0);
	void SetDetail3DIdleChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
	void SetDetail3DRatioChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
	void SetDetail3DOccupancyChartString(C2DChartData& c2dGraphData, CParameters *pParameter);

    void BuildSummaryScheduleUtilizationData(MapMultiRunStandOperationData& standOperationData);
    void BuildSummaryScheduleIdleData(MapMultiRunStandOperationData& standOperationData, CParameters *pParameter);
    void BuildSummaryActualUtilizationData(MapMultiRunStandOperationData& standOperationData);
    void BuildSummaryActualIdleData(MapMultiRunStandOperationData& standOperationData, CParameters *pParameter);
    void BuildSummaryDelayData(MapMultiRunStandOperationData& standOperationData);
    void BuildSummaryConflictData(MapMultiRunStandOperationData& standOperationData);

    void InitSummaryListHead(CXListCtrl &cxListCtrl, CSortableHeaderCtrl* piSHC);
    void FillSummaryListContent(CXListCtrl &cxListCtrl, MultiStandRunSummaryMap &MultiStandRunSummaryMap);
    void FillSummaryConflictListContent(CXListCtrl& cxListCtrl, MultiStandRunSummaryMap& MultiStandRunSummaryMap);

    void DrawSummary3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
    void GenerateSummary2DChartData(C2DChartData& c2dGraphData, MultiStandRunSummaryMap& MultiStandRunSummaryMap);
    void SetSummarySchedUtilize3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummarySchedIdle3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryActualUtilize3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryActualIdle3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryConflict3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);

//	int GetIntervalCount(long iStart, long iEnd, mapStandResult mapData,long iIgnore = 0)const;
	void ClearData();

	BOOL WriteDetailMap(MultiRunDetailMap mapDetailData, ArctermFile& _file );
	BOOL ReadDetailMap(MultiRunDetailMap& mapDetailData,ArctermFile& _file);

	BOOL WriteSummaryMap(MultiStandRunSummaryMap mapSummaryData,ArctermFile& _file);
	BOOL ReadSummayMap(MultiStandRunSummaryMap& mapSummaryData,ArctermFile& _file);

	BOOL WriteUnuseStandMap(std::map<CString,int> mapUnuseData,ArctermFile& _file);
	BOOL ReadUnuseStandMap(std::map<CString,int> mapUnuseData,ArctermFile& _file);

	CString PrintHMS( double dSecs );

private:
	MultiRunDetailMap m_standOccupMap;
	MultiRunDetailMap m_standidlemap;
	MultiRunDetailMap m_standRatiomap;
	MultiRunDetailMap m_standArrDelayMap;
	MultiRunDetailMap m_standDepDelayMap;
	MultiRunDetailMap m_standArrConflictsMap;
	MultiRunDetailMap m_standDepConfictsMap;

    MultiStandRunSummaryMap m_summarySchedUtilizeMap;
    MultiStandRunSummaryMap m_summarySchedIdleMap;
    MultiStandRunSummaryMap m_summaryActualUtilizeMap;
    MultiStandRunSummaryMap m_summaryActualIdleMap;
    MultiStandRunSummaryMap m_summaryDelayMap;
    MultiStandRunSummaryMap m_summaryConflictMap;

	std::map<CString,int> m_mapUnuseScheduleStand;
	std::map<CString,int> m_mapUnuseActualStand;
};

