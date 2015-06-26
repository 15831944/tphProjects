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

typedef std::map<CString,MultiRunDetailMap> mapRunwayDetailDelay;
typedef std::map<CString, std::vector<AirsideFlightRunwayDelayData::RunwayDelayItem*>> mapRunwayDelayItem;
typedef std::map<CString, mapRunwayDelayItem> mapRunwayDelayResult;

class AIRSIDEREPORT_API CAirsideRunwayDelayMultiRunResult : public CAirsideMultipleRunResult
{
public:
    CAirsideRunwayDelayMultiRunResult(void);
    ~CAirsideRunwayDelayMultiRunResult(void);

    virtual void LoadMultipleRunReport(CParameters* pParameter);
    virtual void InitListHead(CXListCtrl& cxListCtrl, CParameters * parameter, int iType = 0,CSortableHeaderCtrl* piSHC=NULL);
    virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter, int iType = 0);
    virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType = 0);
    std::vector<CString> GetRunwayMarkList() const { return m_vRunwayMarkList; }
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

//    void InitSummaryListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);
//     void FillSummaryListContent(CXListCtrl& cxListCtrl,const MultiRunSummaryMap& mapSum);
//     void DrawSummary3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
//     void GenerateSummary2DChartData(C2DChartData& c2dGraphData, const MultiRunSummaryMap& multiRunSummaryMap);
//     void SetSummaryTakeoffQDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
//     void SetSummaryHoldShortLind3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
//     void SetSummaryToPositionTime3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
//     void SetSummaryArrivalPosition3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);

    void SetIntervalBegin2End(std::vector<MultipleRunReportData>& vec, long interval);
    void AddRunwayMarkIfNotExist(CString strRunwayMark);
    bool FindRunwayMark(CString strRunwayMark);
private:
    std::vector<CString> m_vRunwayMarkList;
    mapRunwayDetailDelay m_detailTotal;
    mapRunwayDetailDelay m_detailLandingRoll;
    mapRunwayDetailDelay m_detailExisting;
    mapRunwayDetailDelay m_detailInTakeoffQ;
    mapRunwayDetailDelay m_detailHoldShortL;
    mapRunwayDetailDelay m_detailInPosition;
};

