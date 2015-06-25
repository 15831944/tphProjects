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
typedef std::map<CString,long> mapStandResult;
typedef std::map<CString,mapStandResult> mapStandOpResult;
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
    void FillSummaryListContent(CXListCtrl &cxListCtrl, MultiRunSummaryMap &multiRunSummaryMap);

    void DrawSummary3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
    void GenerateSummary2DChartData(C2DChartData& c2dGraphData, MultiRunSummaryMap& multiRunSummaryMap);
    void SetSummarySchedUtilize3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummarySchedIdle3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryActualUtilize3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryActualIdle3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryConflict3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);

	int GetIntervalCount(long iStart, long iEnd, mapStandResult mapData,long iIgnore = 0)const;
	void ClearData();
private:
	MultiRunDetailMap m_standOccupMap;
	MultiRunDetailMap m_standidlemap;
	MultiRunDetailMap m_standRatiomap;
	MultiRunDetailMap m_standArrDelayMap;
	MultiRunDetailMap m_standDepDelayMap;
	MultiRunDetailMap m_standArrConflictsMap;
	MultiRunDetailMap m_standDepConfictsMap;

    MultiRunSummaryMap m_summarySchedUtilizeMap;
    MultiRunSummaryMap m_summarySchedIdleMap;
    MultiRunSummaryMap m_summaryActualUtilizeMap;
    MultiRunSummaryMap m_summaryActualIdleMap;
    MultiRunSummaryMap m_summaryDelayMap;
    MultiRunSummaryMap m_summaryConflictMap;
};

