#include "stdafx.h"
#include "AirsideRunwayDelayMultiRunResult.h"
#include "Common\TERMFILE.H"
#include "AirsideFlightRunwayDelayReport.h"
#include <vector>
#include "AirsideFlightRunwayDelayData.h"
#include "AirsideFlightRunwayDelayBaseResult.h"
#include "AirsideFlightRunwayDelayReportPara.h"

static const char* strSummaryListTitle[] = 
{
    "Min",
    "Avg",
    "Max",
    "Q1",
    "Q2",
    "Q3",
    "P1",
    "P5",
    "P10",
    "P90",
    "P95",
    "P99",
    "Std dev"
};

CAirsideRunwayDelayMultiRunResult::CAirsideRunwayDelayMultiRunResult( void )
{
}

CAirsideRunwayDelayMultiRunResult::~CAirsideRunwayDelayMultiRunResult( void )
{
}

void CAirsideRunwayDelayMultiRunResult::LoadMultipleRunReport(CParameters* pParameter)
{
    ClearData();
    mapRunwayDelayResult mapResultTotal,mapResultLandingRoll,mapResultExiting,mapResultTakeOffQ,mapResultHoldShort,mapResultInPosition;
    mapSimReport::iterator iter = m_mapSimReport.begin();
    for (; iter != m_mapSimReport.end(); ++iter)
    {
        CString strSimResult = iter->first;
        AirsideFlightRunwayDelayReport* pReport = (AirsideFlightRunwayDelayReport*)(iter->second);
        AirsideFlightRunwayDelayBaseResult* pBaseResult = pReport->GetBaseResult();
        AirsideFlightRunwayDelayData* runwayDelayData = pBaseResult->GetReportData();
        const std::vector<AirsideFlightRunwayDelayData::RunwayDelayItem*>& vRunwayDelayItem = runwayDelayData->GetDataItemList();
        int nCount = (int)vRunwayDelayItem.size();
        for (int i=0; i<nCount; ++i)
        {
            AirsideFlightRunwayDelayData::RunwayDelayItem* pItem = vRunwayDelayItem.at(i);
            CString strRunwayMark = pItem->m_strRunwayMark;
            AddRunwayMarkIfNotExist(strRunwayMark);
            mapResultTotal[strSimResult][strRunwayMark].clear();
            mapResultLandingRoll[strSimResult][strRunwayMark].clear();
            mapResultExiting[strSimResult][strRunwayMark].clear();
            mapResultTakeOffQ[strSimResult][strRunwayMark].clear();
            mapResultHoldShort[strSimResult][strRunwayMark].clear();
            mapResultInPosition[strSimResult][strRunwayMark].clear();
        }
        for (int i=0; i<nCount; ++i)
        {
            AirsideFlightRunwayDelayData::RunwayDelayItem* pItem = vRunwayDelayItem.at(i);
            CString strRunwayMark = pItem->m_strRunwayMark;
            mapResultTotal[strSimResult][strRunwayMark].push_back(pItem);
            switch(pItem->m_PositionType+2)
            {
            case AirsideFlightRunwayDelayReport::ChartType_Detail_ByLandingRoll:
                mapResultLandingRoll[strSimResult][strRunwayMark].push_back(pItem);
                break;
            case AirsideFlightRunwayDelayReport::ChartType_Detail_ByExiting:
                mapResultExiting[strSimResult][strRunwayMark].push_back(pItem);
                break;
            case AirsideFlightRunwayDelayReport::ChartType_Detail_ByInTakeoffQueue:
                mapResultTakeOffQ[strSimResult][strRunwayMark].push_back(pItem);
                break;
            case AirsideFlightRunwayDelayReport::ChartType_Detail_ByHoldShort:
                mapResultHoldShort[strSimResult][strRunwayMark].push_back(pItem);
                break;
            case AirsideFlightRunwayDelayReport::ChartType_Detail_ByInPosition:
                mapResultInPosition[strSimResult][strRunwayMark].push_back(pItem);
                break;
            default:
                break;
            }
        }
    }

    BuildDetailRunwayDelayData(m_detailTotal, mapResultTotal, pParameter);
    BuildDetailRunwayDelayData(m_detailLandingRoll, mapResultLandingRoll, pParameter);
    BuildDetailRunwayDelayData(m_detailExiting, mapResultExiting, pParameter);
    BuildDetailRunwayDelayData(m_detailInTakeoffQ, mapResultTakeOffQ, pParameter);
    BuildDetailRunwayDelayData(m_detailHoldShortL, mapResultHoldShort, pParameter);
    BuildDetailRunwayDelayData(m_detailInPosition, mapResultInPosition, pParameter);

    // calculate how many time intervals have been set in ui.
    m_vIntervalList.clear();
    ElapsedTime tempTime = pParameter->getStartTime();
    for(; tempTime<pParameter->getEndTime(); tempTime+=pParameter->getInterval())
    {
        TimeInterval* pInt = new TimeInterval;
        pInt->m_tStart = tempTime;
        pInt->m_tEnd = tempTime + (pParameter->getInterval() - 1l);
        if(pInt->m_tEnd >= pParameter->getEndTime())
            pInt->m_tEnd = pParameter->getEndTime() -1l;
        m_vIntervalList.push_back(pInt);
    }

    BuildSummaryRunwayDelayData(m_summaryTotal, mapResultTotal, pParameter);
    BuildSummaryRunwayDelayData(m_summaryLandingRoll, mapResultLandingRoll, pParameter);
    BuildSummaryRunwayDelayData(m_summaryExiting, mapResultExiting, pParameter);
    BuildSummaryRunwayDelayData(m_summaryInTakeoffQ, mapResultTakeOffQ, pParameter);
    BuildSummaryRunwayDelayData(m_summaryHoldShortL, mapResultHoldShort, pParameter);
    BuildSummaryRunwayDelayData(m_summaryInPosition, mapResultInPosition, pParameter);
}

void CAirsideRunwayDelayMultiRunResult::BuildDetailRunwayDelayData(mapRunwayDetailDelay& mapToBuild, 
    const mapRunwayDelayResult& oriData, CParameters* parameter)
{
    mapRunwayDelayResult::const_iterator itor;

    // find the maximum delayed item in all simulation results.
    long maxDelayValue = 0;
    for(itor = oriData.begin(); itor!=oriData.end(); ++itor)
    {
        mapRunwayDelayItem::const_iterator itor2 = itor->second.begin();
        for(; itor2!=itor->second.end(); ++itor2)
        {
            std::vector<AirsideFlightRunwayDelayData::RunwayDelayItem*>::const_iterator itor3 = itor2->second.begin();
            for(; itor3!=itor2->second.end(); ++itor3)
            {
                if(maxDelayValue<(*itor3)->m_lTotalDelay)
                    maxDelayValue = (*itor3)->m_lTotalDelay;
            }
        }
    }

    // calculate how many columns should be showed in the list view and the graph view.
    int maxIntervalCount = -1;
    if(maxDelayValue%parameter->getInterval() == 0)
    {
        maxIntervalCount = maxDelayValue/parameter->getInterval();
        if(maxIntervalCount == 0)
            maxIntervalCount = 1;
    }
    else
    {
        maxIntervalCount = maxDelayValue/parameter->getInterval() + 1;
    }

    // calculation.
    for(itor = oriData.begin(); itor!=oriData.end(); ++itor)
    {
        CString strSimResult = itor->first;
        mapRunwayDelayItem::const_iterator itor2 = itor->second.begin();
        for(; itor2!=itor->second.end(); ++itor2)
        {
            CString strRunwayMark = itor2->first;
            mapToBuild[strSimResult][strRunwayMark].resize(maxIntervalCount);
            SetIntervalBegin2End(mapToBuild[strSimResult][strRunwayMark], parameter->getInterval());
            std::vector<AirsideFlightRunwayDelayData::RunwayDelayItem*>::const_iterator itor3 = itor2->second.begin();
            for(; itor3!=itor2->second.end(); ++itor3)
            {
                size_t nIntervalIdx = (size_t)(*itor3)->GetDelayInIntervalIndex(parameter->getInterval());
                if((*itor3)->m_lTotalDelay>0)
                    mapToBuild[strSimResult][strRunwayMark][nIntervalIdx].m_iData++;
            }
        }
    }
}

void CAirsideRunwayDelayMultiRunResult::InitListHead(CXListCtrl& cxListCtrl,CParameters * parameter, int iType,CSortableHeaderCtrl* piSHC)
{
    cxListCtrl.DeleteAllItems();

    while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
        cxListCtrl.DeleteColumn(0);

    AirsideFlightRunwayDelayReportPara* pPara = (AirsideFlightRunwayDelayReportPara*)parameter;
    if (parameter->getReportType() == ASReportType_Detail)
    {
        AirsideFlightRunwayDelayReportPara* pOpPara = (AirsideFlightRunwayDelayReportPara*)parameter;
        switch (pOpPara->getSubType())
        {
        case AirsideFlightRunwayDelayReport::ChartType_Detail_Total:
            InitDetailListHead(cxListCtrl, m_detailTotal, piSHC);
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Detail_ByLandingRoll:
            InitDetailListHead(cxListCtrl, m_detailLandingRoll, piSHC);
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Detail_ByExiting:
            InitDetailListHead(cxListCtrl, m_detailExiting, piSHC);
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Detail_ByInTakeoffQueue:
            InitDetailListHead(cxListCtrl, m_detailInTakeoffQ, piSHC);
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Detail_ByHoldShort:
            InitDetailListHead(cxListCtrl, m_detailHoldShortL, piSHC);
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Detail_ByInPosition:
            InitDetailListHead(cxListCtrl, m_detailInPosition, piSHC);
            break;
        default:
            break;
        }
    }
    else if(parameter->getReportType() == ASReportType_Summary)
    {
        InitSummaryListHead(cxListCtrl, piSHC);
    }
}

void CAirsideRunwayDelayMultiRunResult::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter, int iType)
{
    cxListCtrl.DeleteAllItems();
    ASSERT(parameter);
    if (parameter->getReportType() == ASReportType_Detail)
    {
        AirsideFlightRunwayDelayReportPara* pPara = (AirsideFlightRunwayDelayReportPara*)parameter;
        switch (pPara->getSubType())
        {
        case AirsideFlightRunwayDelayReport::ChartType_Detail_Total:
            FillDetailListContent(cxListCtrl, m_detailTotal);
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Detail_ByLandingRoll:
            FillDetailListContent(cxListCtrl, m_detailLandingRoll);
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Detail_ByExiting:
            FillDetailListContent(cxListCtrl, m_detailExiting);
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Detail_ByInTakeoffQueue:
            FillDetailListContent(cxListCtrl, m_detailInTakeoffQ);
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Detail_ByHoldShort:
            FillDetailListContent(cxListCtrl, m_detailHoldShortL);
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Detail_ByInPosition:
            FillDetailListContent(cxListCtrl, m_detailInPosition);
            break;
        default:
            break;
        }
    }
    else if(parameter->getReportType() == ASReportType_Summary)
    {
        AirsideFlightRunwayDelayReportPara* pPara = (AirsideFlightRunwayDelayReportPara*)parameter;
        switch (pPara->getSubType())
        {
        case AirsideFlightRunwayDelayReport::ChartType_Summary_Total:
            FillSummaryListContent(cxListCtrl, m_summaryTotal, "Total");
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_ByLandingRoll:
            FillSummaryListContent(cxListCtrl, m_summaryLandingRoll, FlightRunwayDelay::POSITIONTYPE[0]);
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_ByExiting:
            FillSummaryListContent(cxListCtrl, m_summaryExiting, FlightRunwayDelay::POSITIONTYPE[1]);
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_ByInTakeoffQueue:
            FillSummaryListContent(cxListCtrl, m_summaryInTakeoffQ, FlightRunwayDelay::POSITIONTYPE[2]);
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_ByHoldShort:
            FillSummaryListContent(cxListCtrl, m_summaryHoldShortL, FlightRunwayDelay::POSITIONTYPE[3]);
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_ByInPosition:
            FillSummaryListContent(cxListCtrl, m_summaryInPosition, FlightRunwayDelay::POSITIONTYPE[4]);
            break;
        default:
            break;
        }
    }
}

void CAirsideRunwayDelayMultiRunResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType)
{
    ASSERT(pParameter);
    if (pParameter->getReportType() == ASReportType_Detail)
    {
        DrawDetail3DChart(chartWnd, pParameter, iType);
    }
    else if(pParameter->getReportType() == ASReportType_Summary)
    {
        DrawSummary3DChart(chartWnd, pParameter, iType);
    }
}

void CAirsideRunwayDelayMultiRunResult::ClearData()
{
    m_detailTotal.clear();
    m_detailLandingRoll.clear();
    m_detailExiting.clear();
    m_detailInTakeoffQ.clear();
    m_detailHoldShortL.clear();
    m_detailInPosition.clear();
    m_vRunwayMarkList.clear();

    m_summaryTotal.clear();
    m_summaryLandingRoll.clear();
    m_summaryExiting.clear();
    m_summaryInTakeoffQ.clear();
    m_summaryHoldShortL.clear();
    m_summaryInPosition.clear();
    m_vIntervalList.clear();
}

void CAirsideRunwayDelayMultiRunResult::InitDetailListHead(CXListCtrl& cxListCtrl, const mapRunwayDetailDelay& mapDetailData, CSortableHeaderCtrl* piSHC)
{
    if(mapDetailData.empty())
        return;
    int nCurCol = 0;
    DWORD headStyle = LVCFMT_CENTER;
    headStyle &= ~HDF_OWNERDRAW;
    cxListCtrl.InsertColumn(nCurCol,"",headStyle,20);
    nCurCol++;

    headStyle = LVCFMT_LEFT;
    headStyle &= ~HDF_OWNERDRAW;
    cxListCtrl.InsertColumn(nCurCol, _T("SimResult"), headStyle, 80);
    if(piSHC)
    {
        piSHC->SetDataType(nCurCol, dtSTRING);
    }
    nCurCol++;

    cxListCtrl.InsertColumn(nCurCol, _T("Runway"), headStyle, 80);
    if(piSHC)
    {
        piSHC->SetDataType(nCurCol, dtSTRING);
    }
    nCurCol++;

    if(mapDetailData.empty())
    {
        return;
    }
    else
    {
        mapRunwayDetailDelay::const_iterator itor = mapDetailData.begin();
        const std::vector<MultipleRunReportData>& vData = itor->second.begin()->second;
        size_t nCount = vData.size();
        for(size_t i=0; i<nCount; i++)
        {
            const MultipleRunReportData& data = vData.at(i);
            CString strRange;
            ElapsedTime eStartTime;
            ElapsedTime eEndTime;
            eStartTime.setPrecisely(data.m_iStart);
            eEndTime.setPrecisely(data.m_iEnd);
            strRange.Format(_T("%s - %s"),eStartTime.printTime(), eEndTime.printTime());

            DWORD dwStyle = LVCFMT_LEFT;
            dwStyle &= ~HDF_OWNERDRAW;
            cxListCtrl.InsertColumn(nCurCol, strRange, dwStyle, 80);
            if(piSHC)
            {
                piSHC->SetDataType(nCurCol, dtINT);
            }
            nCurCol++;
        }
    }
}

void CAirsideRunwayDelayMultiRunResult::FillDetailListContent(CXListCtrl& cxListCtrl, const mapRunwayDetailDelay& mapDetailData)
{
    if(mapDetailData.empty())
        return;

    mapRunwayDetailDelay::const_iterator itor = mapDetailData.begin();
    int nColCount = cxListCtrl.GetColumns();
    ASSERT(nColCount == (int)itor->second.begin()->second.size()+3); // col0: index, col1: run name, col2: runway name, col3+: intervals.

    int idx = 0;
    for(; itor!=mapDetailData.end(); ++itor)
    {
        MultiRunDetailMap::const_iterator itor2 = itor->second.begin();
        for(; itor2!=itor->second.end(); ++itor2)
        {
            CString strIndex;
            strIndex.Format(_T("%d"),idx+1);
            cxListCtrl.InsertItem(idx,strIndex);

            CString strSimName = itor->first;
            int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
            CString strRun = _T("");
            strRun.Format(_T("Run%d"),nCurSimResult+1);
            cxListCtrl.SetItemText(idx,1,strRun);
            cxListCtrl.SetItemData(idx,idx);

            CString strRunway = itor2->first;
            cxListCtrl.SetItemText(idx,2,strRunway);

            std::vector<MultipleRunReportData> vec = itor2->second;
            for(size_t i=0; i<vec.size(); i++)
            {
                const MultipleRunReportData data = vec.at(i);
                CString strCount;
                strCount.Format(_T("%d"), data.m_iData);
                cxListCtrl.SetItemText(idx, i+3, strCount);
            }
            idx++;
        }
    }
}

void CAirsideRunwayDelayMultiRunResult::SetIntervalBegin2End(std::vector<MultipleRunReportData>& vec, long interval)
{
    size_t nCount = vec.size();
    for(size_t i=0; i<nCount; i++)
    {
        vec.at(i).m_iStart = i * interval * TimePrecision;
        vec.at(i).m_iEnd = (i+1) * interval * TimePrecision;
    }
    return;
}

void CAirsideRunwayDelayMultiRunResult::DrawDetail3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType)
{
    C2DChartData c2dGraphData;

    AirsideFlightRunwayDelayReportPara* pPara = (AirsideFlightRunwayDelayReportPara*)pParameter;
    int nSubType = pPara->getSubType();
    switch (nSubType)
    {
    case AirsideFlightRunwayDelayReport::ChartType_Detail_Total:
        SetDetailTotalDelay3DChartString(c2dGraphData, pParameter);
        GenerateDetail2DChartData(c2dGraphData, m_detailTotal, iType);
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Detail_ByLandingRoll:
        SetDetailLandingRoll3DChartString(c2dGraphData, pParameter);
        GenerateDetail2DChartData(c2dGraphData, m_detailLandingRoll, iType);
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Detail_ByExiting:
        SetDetailExisting3DChartString(c2dGraphData, pParameter);
        GenerateDetail2DChartData(c2dGraphData, m_detailExiting, iType);
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Detail_ByInTakeoffQueue:
        SetDetailInTakeoffQ3DChartString(c2dGraphData, pParameter);
        GenerateDetail2DChartData(c2dGraphData, m_detailInTakeoffQ, iType);
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Detail_ByHoldShort:
        SetDetailHoldShortLine3DChartString(c2dGraphData, pParameter);
        GenerateDetail2DChartData(c2dGraphData, m_detailHoldShortL, iType);
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Detail_ByInPosition:
        SetDetailInPosition3DChartString(c2dGraphData, pParameter);
        GenerateDetail2DChartData(c2dGraphData, m_detailInPosition, iType);
        break;
    default:
        break;
    }

    chartWnd.DrawChart(c2dGraphData);
}

void CAirsideRunwayDelayMultiRunResult::GenerateDetail2DChartData(C2DChartData& c2dGraphData, const mapRunwayDetailDelay& mapDetailData, int iType)
{
    ASSERT(iType);
    CString strRunwayMark = *((CString*)iType);

    int simCount = (int)mapDetailData.size();
    c2dGraphData.m_vr2DChartData.resize(simCount);
    mapRunwayDetailDelay::const_iterator itor = mapDetailData.begin();
    for(int simIdx = 0; itor!=mapDetailData.end(); ++itor, ++simIdx)
    {
        CString strSimName = itor->first;
        int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
        CString strRun = _T("");
        strRun.Format(_T("Run%d"),nCurSimResult+1);
        c2dGraphData.m_vrLegend.push_back(strRun);

        const std::vector<MultipleRunReportData>& vData2 = itor->second.at(strRunwayMark);
        size_t nCount2 = vData2.size();
        for(size_t i=0; i<nCount2; i++)
        {
            const MultipleRunReportData& data = vData2.at(i);
            c2dGraphData.m_vr2DChartData[simIdx].push_back((double)data.m_iData);
        }
    }

    const std::vector<MultipleRunReportData>& vData = mapDetailData.begin()->second.at(strRunwayMark);
    size_t nCount = vData.size();
    for(size_t i=0; i<nCount; i++)
    {
        const MultipleRunReportData& data = vData.at(i);
        CString strXTickTitle;
        ElapsedTime eStartTime;
        ElapsedTime eEndTime;
        eStartTime.setPrecisely(data.m_iStart);
        eEndTime.setPrecisely(data.m_iEnd);
        strXTickTitle.Format(_T("%d -%d"),eStartTime.asSeconds(), eEndTime.asSeconds());
        c2dGraphData.m_vrXTickTitle.push_back(strXTickTitle);
    }
}

void CAirsideRunwayDelayMultiRunResult::SetDetailTotalDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter)
{
    c2dGraphData.m_strChartTitle = _T(" Total Runway Delay ");
    c2dGraphData.m_strYtitle = _T("Number of aircraft");
    c2dGraphData.m_strXtitle = _T("Delay time(secs)");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Total Runway Delay %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetDetailLandingRoll3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay Landing Roll ");
    c2dGraphData.m_strYtitle = _T("Number of aircraft");
    c2dGraphData.m_strXtitle = _T("Delay time(secs)");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Runway Delay Landing Roll %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetDetailExisting3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay Exiting ");
    c2dGraphData.m_strYtitle = _T("Number of aircraft");
    c2dGraphData.m_strXtitle = _T("Delay time(secs)");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Runway Delay Exiting %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetDetailInTakeoffQ3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay In Takeoff Queue ");
    c2dGraphData.m_strYtitle = _T("Number of aircraft");
    c2dGraphData.m_strXtitle = _T("Delay time(secs)");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Runway Delay In Takeoff Queue %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetDetailHoldShortLine3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay At Hold Short ");
    c2dGraphData.m_strYtitle = _T("Number of aircraft");
    c2dGraphData.m_strXtitle = _T("Delay time(secs)");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Runway Delay At Hold Short %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetDetailInPosition3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay In Position ");
    c2dGraphData.m_strYtitle = _T("Number of aircraft");
    c2dGraphData.m_strXtitle = _T("Delay time(secs)");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Runway Delay In Position %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::AddRunwayMarkIfNotExist(CString strRunwayMark)
{
    if(!FindRunwayMark(strRunwayMark))
        m_vRunwayMarkList.push_back(strRunwayMark);
}

bool CAirsideRunwayDelayMultiRunResult::FindRunwayMark( CString strRunwayMark )
{
    size_t nCount = m_vRunwayMarkList.size();
    for(size_t i=0; i<nCount; i++)
    {
        if(m_vRunwayMarkList.at(i).CompareNoCase(strRunwayMark) == 0)
            return true;
    }
    return false;
}

void CAirsideRunwayDelayMultiRunResult::BuildSummaryRunwayDelayData(mapRunwaySummaryDelay& mapToBuild, 
    const mapRunwayDelayResult& oriData, CParameters* parameter)
{
    // initialize map.
    mapRunwayDelayResult::const_iterator itor = oriData.begin();
    for(; itor!=oriData.end(); ++itor)
    {
        CString strSimResult = itor->first;
        std::vector<TimeInterval*>::iterator intItor = m_vIntervalList.begin();
        for(; intItor!=m_vIntervalList.end(); ++intItor)
        {
            mapToBuild[strSimResult][*(*intItor)].Clear();
        }
    }

    // calculation.
    for(itor = oriData.begin(); itor!=oriData.end(); ++itor)
    {
        CString strSimResult = itor->first;
        std::map<TimeInterval, CStatisticalTools<double>> tempMap;

        mapRunwayDelayItem::const_iterator itor2 = itor->second.begin();
        for(; itor2!=itor->second.end(); ++itor2)
        {
            // !runway information is ignored!
            std::vector<AirsideFlightRunwayDelayData::RunwayDelayItem*>::const_iterator itor3 = itor2->second.begin();
            for(; itor3!=itor2->second.end(); ++itor3)
            {
                TimeInterval* pTimeInterval = FindTimeInterval((*itor3)->m_tStartTime);
                if(pTimeInterval)
                {
                    tempMap[*pTimeInterval].AddNewData((*itor3)->m_lTotalDelay);
                }
            }
        }

        std::map<TimeInterval, CStatisticalTools<double>>::iterator itor3 = tempMap.begin();
        for(; itor3!=tempMap.end(); ++itor3)
        {
            itor3->second.SortData();
            mapToBuild[strSimResult][itor3->first].m_nDataCount = itor3->second.GetDataCount();
            mapToBuild[strSimResult][itor3->first].m_estTotal = (long)itor3->second.GetSum();
            mapToBuild[strSimResult][itor3->first].m_estMin = (long)itor3->second.GetMin();
            mapToBuild[strSimResult][itor3->first].m_estAverage = (long)itor3->second.GetAvarage();
            mapToBuild[strSimResult][itor3->first].m_estMax = (long)itor3->second.GetMax();
            mapToBuild[strSimResult][itor3->first].m_estQ1 = (long)itor3->second.GetPercentile(25);
            mapToBuild[strSimResult][itor3->first].m_estQ2 = (long)itor3->second.GetPercentile(50);
            mapToBuild[strSimResult][itor3->first].m_estQ3 = (long)itor3->second.GetPercentile(75);
            mapToBuild[strSimResult][itor3->first].m_estP1 = (long)itor3->second.GetPercentile(1);
            mapToBuild[strSimResult][itor3->first].m_estP5 = (long)itor3->second.GetPercentile(5);
            mapToBuild[strSimResult][itor3->first].m_estP10 = (long)itor3->second.GetPercentile(10);
            mapToBuild[strSimResult][itor3->first].m_estP90 = (long)itor3->second.GetPercentile(90);
            mapToBuild[strSimResult][itor3->first].m_estP95 = (long)itor3->second.GetPercentile(95);
            mapToBuild[strSimResult][itor3->first].m_estP99 = (long)itor3->second.GetPercentile(99);
            mapToBuild[strSimResult][itor3->first].m_estSigma = (long)itor3->second.GetSigma();
        }
    }
}

// return NULL if not found.
TimeInterval* CAirsideRunwayDelayMultiRunResult::FindTimeInterval(ElapsedTime time)
{
    size_t nCount = m_vIntervalList.size();
    for(size_t i=0; i<nCount; i++)
    {
        if(m_vIntervalList.at(i)->IsTimeInMe(time))
            return m_vIntervalList.at(i);
    }
    return NULL;
}

void CAirsideRunwayDelayMultiRunResult::InitSummaryListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC)
{
    int nCurCol = 0;
    DWORD dwStyle = LVCFMT_CENTER;
    dwStyle &= ~HDF_OWNERDRAW;
    cxListCtrl.InsertColumn(nCurCol,"",dwStyle,20);
    nCurCol++;

    dwStyle = LVCFMT_LEFT;
    dwStyle &= ~HDF_OWNERDRAW;
    cxListCtrl.InsertColumn(nCurCol, _T("SimResult"), dwStyle, 60);
    if(piSHC)
    {
        piSHC->SetDataType(nCurCol, dtSTRING);
    }
    nCurCol++;

    cxListCtrl.InsertColumn(nCurCol, "Interval", dwStyle, 100);
    if(piSHC)
    {
        piSHC->SetDataType(nCurCol, dtINT);
    }
    nCurCol++;

    cxListCtrl.InsertColumn(nCurCol, "Position", dwStyle, 60);
    if(piSHC)
    {
        piSHC->SetDataType(nCurCol, dtSTRING);
    }
    nCurCol++;

    cxListCtrl.InsertColumn(nCurCol, "Number of delay", dwStyle, 80);
    if(piSHC)
    {
        piSHC->SetDataType(nCurCol, dtINT);
    }
    nCurCol++;

    cxListCtrl.InsertColumn(nCurCol, "Total delay(secs)", dwStyle, 80);
    if(piSHC)
    {
        piSHC->SetDataType(nCurCol, dtINT);
    }
    nCurCol++;

    for(int i=0; i<sizeof(strSummaryListTitle)/sizeof(strSummaryListTitle[0]); i++)
    {
        cxListCtrl.InsertColumn(nCurCol, strSummaryListTitle[i], dwStyle, 80);
        if(piSHC)
        {
            piSHC->SetDataType(nCurCol, dtINT);
        }
        nCurCol++;
    }
}

void CAirsideRunwayDelayMultiRunResult::FillSummaryListContent(CXListCtrl& cxListCtrl, 
    const mapRunwaySummaryDelay& mapSummary, CString strPosition)
{
    mapRunwaySummaryDelay::const_iterator itor = mapSummary.begin();
    int idx = 0;
    for(; itor!=mapSummary.end(); ++itor)
    {
        CString strSimResult = itor->first;
        mapSummaryDataOfInterval::const_iterator intItor = itor->second.begin();
        bool hasInsertData = false;
        for(; intItor!=itor->second.end(); ++intItor)
        {
            if(intItor->second.m_nDataCount)
            {
                FillListItemData(intItor, idx, cxListCtrl, strSimResult, strPosition);

                hasInsertData = true;
                idx++;
            }
        }
        if(!hasInsertData) // no data inserted,
        {
            FillListItemData(itor->second.begin(), idx, cxListCtrl, strSimResult, strPosition);
            idx++;
        }
    }
}

void CAirsideRunwayDelayMultiRunResult::FillListItemData(mapSummaryDataOfInterval::const_iterator intItor, 
    int idx, CXListCtrl &cxListCtrl, CString strSimResult, CString strPosition)
{
    const RunwayDelaySummaryData& summaryData = intItor->second;
    TimeInterval intervalMark = intItor->first;
    CString strTemp;
    strTemp.Format(_T("%d"),idx+1);
    cxListCtrl.InsertItem(idx,strTemp);

    int nCurCol = 1;
    int nCurSimResult = atoi(strSimResult.Mid(9,strSimResult.GetLength()));
    CString strRun = _T("");
    strRun.Format(_T("Run%d"),nCurSimResult+1);
    cxListCtrl.SetItemText(idx,nCurCol,strRun);
    nCurCol++;
    cxListCtrl.SetItemData(idx,idx);

    strTemp.Format("%s - %s", intervalMark.m_tStart.printTime(), intervalMark.m_tEnd.printTime());
    cxListCtrl.SetItemText(idx,nCurCol,strTemp);
    nCurCol++;

    cxListCtrl.SetItemText(idx,nCurCol,strPosition);
    nCurCol++;

    strTemp.Format("%d", summaryData.m_nDataCount);
    cxListCtrl.SetItemText(idx,nCurCol,strTemp);
    nCurCol++;

    strTemp.Format("%d", summaryData.m_estTotal.asSeconds());
    cxListCtrl.SetItemText(idx,nCurCol,strTemp);
    nCurCol++;

    strTemp.Format("%d", summaryData.m_estMin.asSeconds());
    cxListCtrl.SetItemText(idx,nCurCol,strTemp);
    nCurCol++;

    strTemp.Format("%d", summaryData.m_estAverage.asSeconds());
    cxListCtrl.SetItemText(idx,nCurCol,strTemp);
    nCurCol++;

    strTemp.Format("%d", summaryData.m_estMax.asSeconds());
    cxListCtrl.SetItemText(idx,nCurCol,strTemp);
    nCurCol++;

    strTemp.Format("%d", summaryData.m_estQ1.asSeconds());
    cxListCtrl.SetItemText(idx,nCurCol,strTemp);
    nCurCol++;

    strTemp.Format("%d", summaryData.m_estQ2.asSeconds());
    cxListCtrl.SetItemText(idx,nCurCol,strTemp);
    nCurCol++;

    strTemp.Format("%d", summaryData.m_estQ3.asSeconds());
    cxListCtrl.SetItemText(idx,nCurCol,strTemp);
    nCurCol++;

    strTemp.Format("%d", summaryData.m_estP1.asSeconds());
    cxListCtrl.SetItemText(idx,nCurCol,strTemp);
    nCurCol++;

    strTemp.Format("%d", summaryData.m_estP5.asSeconds());
    cxListCtrl.SetItemText(idx,nCurCol,strTemp);
    nCurCol++;

    strTemp.Format("%d", summaryData.m_estP10.asSeconds());
    cxListCtrl.SetItemText(idx,nCurCol,strTemp);
    nCurCol++;

    strTemp.Format("%d", summaryData.m_estP90.asSeconds());
    cxListCtrl.SetItemText(idx,nCurCol,strTemp);
    nCurCol++;

    strTemp.Format("%d", summaryData.m_estP95.asSeconds());
    cxListCtrl.SetItemText(idx,nCurCol,strTemp);
    nCurCol++;

    strTemp.Format("%d", summaryData.m_estP99.asSeconds());
    cxListCtrl.SetItemText(idx,nCurCol,strTemp);
    nCurCol++;

    strTemp.Format("%d", summaryData.m_estSigma.asSeconds());
    cxListCtrl.SetItemText(idx,nCurCol,strTemp);
    nCurCol++;
}

void CAirsideRunwayDelayMultiRunResult::DrawSummary3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType)
{
    TimeInterval* pTimeInterval = (TimeInterval*)iType;
    ASSERT(pTimeInterval);
    C2DChartData c2dGraphData;

    AirsideFlightRunwayDelayReportPara* pPara = (AirsideFlightRunwayDelayReportPara*)pParameter;
    int nSubType = pPara->getSubType();
    switch (nSubType)
    {
    case AirsideFlightRunwayDelayReport::ChartType_Summary_Total:
        SetSummaryTotalDelay3DChartString(c2dGraphData, pTimeInterval);
        GenerateSummary2DChartData(c2dGraphData, m_summaryTotal, iType);
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Summary_ByLandingRoll:
        SetSummaryLandingRoll3DChartString(c2dGraphData, pTimeInterval);
        GenerateSummary2DChartData(c2dGraphData, m_summaryLandingRoll, iType);
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Summary_ByExiting:
        SetSummaryExisting3DChartString(c2dGraphData, pTimeInterval);
        GenerateSummary2DChartData(c2dGraphData, m_summaryExiting, iType);
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Summary_ByInTakeoffQueue:
        SetSummaryInTakeoffQ3DChartString(c2dGraphData, pTimeInterval);
        GenerateSummary2DChartData(c2dGraphData, m_summaryInTakeoffQ, iType);
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Summary_ByHoldShort:
        SetSummaryHoldShortLine3DChartString(c2dGraphData, pTimeInterval);
        GenerateSummary2DChartData(c2dGraphData, m_summaryHoldShortL, iType);
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Summary_ByInPosition:
        SetSummaryInPosition3DChartString(c2dGraphData, pTimeInterval);
        GenerateSummary2DChartData(c2dGraphData, m_summaryInPosition, iType);
        break;
    default:
        break;
    }

    chartWnd.DrawChart(c2dGraphData);
}

void CAirsideRunwayDelayMultiRunResult::GenerateSummary2DChartData(C2DChartData& c2dGraphData, 
    const mapRunwaySummaryDelay& mapSummaryData, int iType)
{
    int nCount = sizeof(strSummaryListTitle) / sizeof(strSummaryListTitle[0]);
    for(int i=0; i<nCount; i++)
        c2dGraphData.m_vrLegend.push_back(strSummaryListTitle[i]);

    TimeInterval* pTimeInterval = (TimeInterval*)iType;
    ASSERT(pTimeInterval);
    c2dGraphData.m_vr2DChartData.resize(13);
    mapRunwaySummaryDelay::const_iterator itor = mapSummaryData.begin();
    for(; itor!=mapSummaryData.end(); ++itor)
    {
        CString strSimName = itor->first;
        int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
        CString strXTickTitle;
        strXTickTitle.Format(_T("Run%d"), nCurSimResult+1);
        c2dGraphData.m_vrXTickTitle.push_back(strXTickTitle);

        c2dGraphData.m_vr2DChartData[0].push_back((double)itor->second.at(*pTimeInterval).m_estMin.asSeconds());
        c2dGraphData.m_vr2DChartData[1].push_back((double)itor->second.at(*pTimeInterval).m_estAverage.asSeconds());
        c2dGraphData.m_vr2DChartData[2].push_back((double)itor->second.at(*pTimeInterval).m_estMax.asSeconds());
        c2dGraphData.m_vr2DChartData[3].push_back((double)itor->second.at(*pTimeInterval).m_estQ1.asSeconds());
        c2dGraphData.m_vr2DChartData[4].push_back((double)itor->second.at(*pTimeInterval).m_estQ2.asSeconds());
        c2dGraphData.m_vr2DChartData[5].push_back((double)itor->second.at(*pTimeInterval).m_estQ3.asSeconds());
        c2dGraphData.m_vr2DChartData[6].push_back((double)itor->second.at(*pTimeInterval).m_estP1.asSeconds());
        c2dGraphData.m_vr2DChartData[7].push_back((double)itor->second.at(*pTimeInterval).m_estP5.asSeconds());
        c2dGraphData.m_vr2DChartData[8].push_back((double)itor->second.at(*pTimeInterval).m_estP10.asSeconds());
        c2dGraphData.m_vr2DChartData[9].push_back((double)itor->second.at(*pTimeInterval).m_estP90.asSeconds());
        c2dGraphData.m_vr2DChartData[10].push_back((double)itor->second.at(*pTimeInterval).m_estP95.asSeconds());
        c2dGraphData.m_vr2DChartData[11].push_back((double)itor->second.at(*pTimeInterval).m_estP99.asSeconds());
        c2dGraphData.m_vr2DChartData[12].push_back((double)itor->second.at(*pTimeInterval).m_estSigma.asSeconds());
    }
}

void CAirsideRunwayDelayMultiRunResult::SetSummaryTotalDelay3DChartString(C2DChartData& c2dGraphData, TimeInterval* pTimeInterval)
{
    c2dGraphData.m_strChartTitle = _T(" Total Runway Delay Summary ");
    c2dGraphData.m_strYtitle = _T("Delay time(secs)");
    c2dGraphData.m_strXtitle = _T("Runs");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Total Runway Summary Delay %s;%s "), pTimeInterval->m_tStart.printTime(), pTimeInterval->m_tEnd.printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetSummaryLandingRoll3DChartString( C2DChartData& c2dGraphData, TimeInterval* pTimeInterval)
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay Summary At Landing Roll ");
    c2dGraphData.m_strYtitle = _T("Delay time(secs)");
    c2dGraphData.m_strXtitle = _T("Runs");

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Runway Delay Summary Landing Roll %s;%s "), pTimeInterval->m_tStart.printTime(), pTimeInterval->m_tEnd.printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetSummaryExisting3DChartString( C2DChartData& c2dGraphData, TimeInterval* pTimeInterval)
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay Summary At Exiting ");
    c2dGraphData.m_strYtitle = _T("Delay time(secs)");
    c2dGraphData.m_strXtitle = _T("Runs");

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Runway Delay Summary Exiting %s;%s "), pTimeInterval->m_tStart.printTime(), pTimeInterval->m_tEnd.printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetSummaryInTakeoffQ3DChartString( C2DChartData& c2dGraphData, TimeInterval* pTimeInterval)
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay Summary In Takeoff Queue ");
    c2dGraphData.m_strYtitle = _T("Delay time(secs)");
    c2dGraphData.m_strXtitle = _T("Runs");

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Runway Delay Summary In Takeoff Queue %s;%s "), pTimeInterval->m_tStart.printTime(), pTimeInterval->m_tEnd.printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetSummaryHoldShortLine3DChartString( C2DChartData& c2dGraphData, TimeInterval* pTimeInterval)
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay Summary At Hold Short ");
    c2dGraphData.m_strYtitle = _T("Delay time(secs)");
    c2dGraphData.m_strXtitle = _T("Runs");

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Runway Delay Summary At Hold Short %s;%s "), pTimeInterval->m_tStart.printTime(), pTimeInterval->m_tEnd.printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetSummaryInPosition3DChartString( C2DChartData& c2dGraphData, TimeInterval* pTimeInterval)
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay Summary In Position ");
    c2dGraphData.m_strYtitle = _T("Delay time(secs)");
    c2dGraphData.m_strXtitle = _T("Runs");

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Runway Delay Summary In Position %s;%s "), pTimeInterval->m_tStart.printTime(), pTimeInterval->m_tEnd.printTime());
    c2dGraphData.m_strFooter = strFooter;
}

// get delay occurred intervals according to the selected sub type.
std::vector<TimeInterval*> CAirsideRunwayDelayMultiRunResult::GetDelayedIntervalList(CParameters *parameter) const
{
    ASSERT(parameter->getReportType() == ASReportType_Summary);
    std::vector<TimeInterval*> vResult;
    if(m_vIntervalList.empty())
        return vResult;
    const mapRunwaySummaryDelay* pSelSumData = NULL;
    AirsideFlightRunwayDelayReportPara* pPara = (AirsideFlightRunwayDelayReportPara*)parameter;
    switch (pPara->getSubType())
    {
    case AirsideFlightRunwayDelayReport::ChartType_Summary_Total:
        pSelSumData = &m_summaryTotal;
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Summary_ByLandingRoll:
        pSelSumData = &m_summaryLandingRoll;
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Summary_ByExiting:
        pSelSumData = &m_summaryExiting;
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Summary_ByInTakeoffQueue:
        pSelSumData = &m_summaryInTakeoffQ;
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Summary_ByHoldShort:
        pSelSumData = &m_summaryHoldShortL;
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Summary_ByInPosition:
        pSelSumData = &m_summaryInPosition;
        break;
    default:
        break;
    }
    ASSERT(pSelSumData);
    mapRunwaySummaryDelay::const_iterator itor = pSelSumData->begin();
    for(; itor!=pSelSumData->end(); ++itor)
    {
        std::vector<TimeInterval*>::const_iterator itor2 = m_vIntervalList.begin();
        for(; itor2!=m_vIntervalList.end(); ++itor2)
        {
            if(itor->second.at(*(*itor2)).m_nDataCount != 0)
                vResult.push_back(*itor2);
        }
    }
    if(vResult.empty())
    {
        vResult.push_back(*m_vIntervalList.begin());
        return vResult; // do not need to remove duplicated items.
    }

    sort(vResult.begin(), vResult.end(), TimeInterval::CompareTimeInterval);
    vResult.erase(unique(vResult.begin(), vResult.end() ), vResult.end());
    return vResult;
}

bool CAirsideRunwayDelayMultiRunResult::IsAvailableRunwayMark( CString strRunwayMark )
{
    std::vector<
}
