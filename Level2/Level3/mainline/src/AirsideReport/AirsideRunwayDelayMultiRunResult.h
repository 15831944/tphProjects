#pragma once
#include "AirsideMultipleRunResult.h"
#include "MFCExControl\XListCtrl.h"
#include "MFCExControl\SortableHeaderCtrl.h"
#include <vector>
#include <map>
#include "CARC3DChart.h"
#include "Parameters.h"
#include <atlstr.h>
#include "AirsideFlightRunwayDelayData.h"
#include "Reports\StatisticalTools.h"

class TimeInterval
{
public:
    TimeInterval():m_tStart(0l), m_tEnd(0l){}
    ~TimeInterval(){}

    ElapsedTime m_tStart;
    ElapsedTime m_tEnd;

public:
    bool IsTimeInMe(const ElapsedTime& time) const
    {
        if(m_tStart<=time && time<m_tEnd)
            return true;
        return false;
    }

    bool operator==(const TimeInterval& other)
    {
        if(m_tStart == other.m_tStart && m_tEnd == other.m_tEnd)
            return true;
        return false;
    }

    // return true if *p1 < *p2.
    static bool CompareTimeInterval(TimeInterval* p1, TimeInterval* p2)
    {
        if((*p1) < (*p2))
            return true;
        return false;
    }

    bool operator<(const TimeInterval& other) const
    {
        if(m_tStart < other.m_tStart)
        {
            return true;
        }
        else if(m_tStart == other.m_tStart)
        {
            if(m_tEnd < other.m_tEnd)
                return true;
        }
        return false;
    }

    TimeInterval& operator=(const TimeInterval& other)
    {
        m_tStart = other.m_tStart;
        m_tEnd = other.m_tEnd;
        return *this;
    }
};

class RunwayDelaySummaryData : public CSummaryData
{
public:
    RunwayDelaySummaryData():m_nDataCount(0){}
    ~RunwayDelaySummaryData(){}
    void Clear(){ m_nDataCount = 0; }
public:
    int m_nDataCount;
};

typedef std::map<CString,MultiRunDetailMap> mapRunwayDetailDelay;
typedef std::map<CString, std::vector<AirsideFlightRunwayDelayData::RunwayDelayItem*>> mapRunwayDelayItem;
typedef std::map<CString, mapRunwayDelayItem> mapRunwayDelayResult;
typedef std::map<TimeInterval, RunwayDelaySummaryData> mapSummaryDataOfInterval;
typedef std::map<CString, mapSummaryDataOfInterval> mapRunwaySummaryDelay;

class AIRSIDEREPORT_API CAirsideRunwayDelayMultiRunResult : public CAirsideMultipleRunResult
{
public:
    CAirsideRunwayDelayMultiRunResult(void);
    ~CAirsideRunwayDelayMultiRunResult(void);

    virtual void LoadMultipleRunReport(CParameters* pParameter);
    virtual void InitListHead(CXListCtrl& cxListCtrl, CParameters * parameter, int iType = 0,CSortableHeaderCtrl* piSHC=NULL);
    virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter, int iType = 0);
    virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType = 0);
    virtual BOOL WriteReportData( ArctermFile& _file );
    virtual BOOL ReadReportData( ArctermFile& _file );

    virtual CString GetReportFileName()const;

    std::vector<CString> GetDelayedRunwayMarkList(CParameters *pParameter) const;
    std::vector<TimeInterval*> GetDelayedIntervalList(CParameters *pParameter);
    void ClearData();
private:
    void BuildDetailRunwayDelayData(mapRunwayDetailDelay& mapToBuild, const mapRunwayDelayResult& oriData, CParameters* parameter);
    void InitDetailListHead(CXListCtrl& cxListCtrl, const mapRunwayDetailDelay& mapDetailData, CSortableHeaderCtrl* piSHC=NULL);
    void FillDetailListContent(CXListCtrl& cxListCtrl, const mapRunwayDetailDelay& mapDetailData);
    void DrawDetail3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType);
    void GenerateDetail2DChartData(C2DChartData& c2dGraphData, const mapRunwayDetailDelay& mapDetailData, int iType);
    void SetDetailTotalDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetDetailLandingRoll3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetDetailExisting3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetDetailInTakeoffQ3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetDetailHoldShortLine3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetDetailInPosition3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);

    void BuildSummaryRunwayDelayData(mapRunwaySummaryDelay& mapToBuild, const mapRunwayDelayResult& oriData, 
        const std::vector<TimeInterval>& vTimeInterval, CParameters* parameter);
    void InitSummaryListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);
    void FillSummaryListContent(CXListCtrl& cxListCtrl,const mapRunwaySummaryDelay& mapSum, CString strPosition);
    void FillListItemData(mapSummaryDataOfInterval::const_iterator intItor, 
        int idx, CXListCtrl &cxListCtrl, CString strSimResult, CString strPosition );
    void DrawSummary3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType);
    void GenerateSummary2DChartData(C2DChartData& c2dGraphData, const mapRunwaySummaryDelay& mapSummaryData, int iType);
    void SetSummaryTotalDelay3DChartString(C2DChartData& c2dGraphData, TimeInterval* pTimeInterval);
    void SetSummaryLandingRoll3DChartString(C2DChartData& c2dGraphData, TimeInterval* pTimeInterval);
    void SetSummaryExisting3DChartString(C2DChartData& c2dGraphData, TimeInterval* pTimeInterval);
    void SetSummaryInTakeoffQ3DChartString(C2DChartData& c2dGraphData, TimeInterval* pTimeInterval);
    void SetSummaryHoldShortLine3DChartString(C2DChartData& c2dGraphData, TimeInterval* pTimeInterval);
    void SetSummaryInPosition3DChartString(C2DChartData& c2dGraphData, TimeInterval* pTimeInterval);

    void SetIntervalBegin2End(std::vector<MultipleRunReportData>& vec, long interval);
    bool IsAvailableRunwayMark(CString strRunwayMark, CParameters* pParameter);
    bool FindRunwayMark(CString strRunwayMark);
    bool FindTimeInterval(const std::vector<TimeInterval>& vTimeInterval, ElapsedTime time, TimeInterval& result);

	BOOL WriteDetailMap(mapRunwayDetailDelay mapDetailData, ArctermFile& _file );
	BOOL ReadDetailMap(mapRunwayDetailDelay& mapDetailData,ArctermFile& _file);

	BOOL WriteSummaryMap(mapRunwaySummaryDelay mapSummaryResult,ArctermFile& _file);
	BOOL ReadSummayMap(mapRunwaySummaryDelay& mapSummaryResult,ArctermFile& _file);
private:
    mapRunwayDetailDelay m_detailTotal;
    mapRunwayDetailDelay m_detailLandingRoll;
    mapRunwayDetailDelay m_detailExiting;
    mapRunwayDetailDelay m_detailInTakeoffQ;
    mapRunwayDetailDelay m_detailHoldShortL;
    mapRunwayDetailDelay m_detailInPosition;

    std::vector<TimeInterval*> m_vIntervalList; // for summary report: generate intervals 
    mapRunwaySummaryDelay m_summaryTotal;
    mapRunwaySummaryDelay m_summaryLandingRoll;
    mapRunwaySummaryDelay m_summaryExiting;
    mapRunwaySummaryDelay m_summaryInTakeoffQ;
    mapRunwaySummaryDelay m_summaryHoldShortL;
    mapRunwaySummaryDelay m_summaryInPosition;
};

