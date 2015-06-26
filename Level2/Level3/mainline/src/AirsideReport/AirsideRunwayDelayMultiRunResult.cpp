#include "stdafx.h"
#include "AirsideRunwayDelayMultiRunResult.h"
#include "Common\TERMFILE.H"
#include "AirsideFlightRunwayDelayReport.h"
#include <vector>
#include "AirsideFlightRunwayDelayData.h"
#include "AirsideFlightRunwayDelayBaseResult.h"
#include "AirsideFlightRunwayDelayReportPara.h"

CAirsideRunwayDelayMultiRunResult::CAirsideRunwayDelayMultiRunResult( void )
{
}

CAirsideRunwayDelayMultiRunResult::~CAirsideRunwayDelayMultiRunResult( void )
{
}

void CAirsideRunwayDelayMultiRunResult::LoadMultipleRunReport(CParameters* pParameter)
{
    ClearData();
    mapRunwayDelayResult mapResultTotal,mapResultLandingRoll,mapResultExisting,mapResultTakeOffQ,mapResultHoldShort,mapResultInPosition;
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
            mapResultExisting[strSimResult][strRunwayMark].clear();
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
            case AirsideFlightRunwayDelayReport::ChartType_Detail_ByExisting:
                mapResultExisting[strSimResult][strRunwayMark].push_back(pItem);
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
    BuildDetailRunwayDelayData(m_detailExisting, mapResultExisting, pParameter);
    BuildDetailRunwayDelayData(m_detailInTakeoffQ, mapResultTakeOffQ, pParameter);
    BuildDetailRunwayDelayData(m_detailHoldShortL, mapResultHoldShort, pParameter);
    BuildDetailRunwayDelayData(m_detailInPosition, mapResultInPosition, pParameter);
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
        case AirsideFlightRunwayDelayReport::ChartType_Detail_ByExisting:
            InitDetailListHead(cxListCtrl, m_detailExisting, piSHC);
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
    }
}

void CAirsideRunwayDelayMultiRunResult::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter, int iType)
{
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
        case AirsideFlightRunwayDelayReport::ChartType_Detail_ByExisting:
            FillDetailListContent(cxListCtrl, m_detailExisting);
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
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_ByLandingRoll:
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_ByExisting:
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_ByInTakeoffQueue:
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_ByHoldShort:
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_ByInPosition:
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_Count:
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
        AirsideFlightRunwayDelayReportPara* pPara = (AirsideFlightRunwayDelayReportPara*)pParameter;
        switch (pPara->getSubType())
        {
        case AirsideFlightRunwayDelayReport::ChartType_Summary_Total:
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_ByLandingRoll:
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_ByExisting:
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_ByInTakeoffQueue:
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_ByHoldShort:
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_ByInPosition:
            break;
        case AirsideFlightRunwayDelayReport::ChartType_Summary_Count:
            break;
        default:
            break;
        }
    }
}

void CAirsideRunwayDelayMultiRunResult::ClearData()
{
    m_detailTotal.clear();
    m_detailLandingRoll.clear();
    m_detailExisting.clear();
    m_detailInTakeoffQ.clear();
    m_detailHoldShortL.clear();
    m_detailInPosition.clear();
    m_vRunwayMarkList.clear();
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
    case AirsideFlightRunwayDelayReport::ChartType_Detail_ByExisting:
        SetDetailExisting3DChartString(c2dGraphData, pParameter);
        GenerateDetail2DChartData(c2dGraphData, m_detailExisting, iType);
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
