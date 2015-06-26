#include "stdafx.h"
#include "AirsideRunwayDelayMultiRunResult.h"
#include "Common\TERMFILE.H"
#include "AirsideFlightRunwayDelayReport.h"
#include <vector>
#include "AirsideFlightRunwayDelayData.h"
#include "AirsideFlightRunwayDelayBaseResult.h"
#include "AirsideFlightRunwayDelayReportPara.h"
#include "SummaryRunwayDelayResult.h"
#include "AirsideFlightRunwayDelaySummaryData.h"

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
    ClearDelayedIntervalList();
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
		if (pParameter->getReportType() == ASReportType_Detail)
		{
			AirsideFlightRunwayDelayData* runwayDelayData = pBaseResult->GetReportData();
			const std::vector<AirsideFlightRunwayDelayData::RunwayDelayItem*>& vRunwayDelayItem = runwayDelayData->GetDataItemList();
			int nCount = (int)vRunwayDelayItem.size();
			for (int i=0; i<nCount; ++i)
			{
				AirsideFlightRunwayDelayData::RunwayDelayItem* pItem = vRunwayDelayItem.at(i);
				CString strRunwayMark = pItem->m_strRunwayMark;
				if(IsAvailableRunwayMark(strRunwayMark, pParameter))
				{
					mapResultTotal[strSimResult][strRunwayMark].clear();
					mapResultLandingRoll[strSimResult][strRunwayMark].clear();
					mapResultExiting[strSimResult][strRunwayMark].clear();
					mapResultTakeOffQ[strSimResult][strRunwayMark].clear();
					mapResultHoldShort[strSimResult][strRunwayMark].clear();
					mapResultInPosition[strSimResult][strRunwayMark].clear();
				}
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
		else
		{
			SummaryRunwayDelayResult* pSummaryResult = (SummaryRunwayDelayResult*)pReport->GetBaseResult();
			pSummaryResult->StatisticSummaryDataFromReportData();
			std::vector<AirsideFlightRunwayDelaySummaryData*> summaryDataList = pSummaryResult->GetSummaryData();
			for (unsigned nSummary = 0; nSummary < summaryDataList.size(); nSummary++)
			{
				AirsideFlightRunwayDelaySummaryData* pSummaryData = summaryDataList[nSummary];
				TimeInterval timeInterval;
				timeInterval.m_tStart = pSummaryData->m_tStart;
				timeInterval.m_tEnd = pSummaryData->m_tEnd;
				
				CopyRunwayDelaySummaryData(m_summaryTotal[strSimResult],pSummaryData);
				switch (pSummaryData->m_nPosition + 102)
				{
				case AirsideFlightRunwayDelayReport::ChartType_Summary_ByLandingRoll:
					CopyRunwayDelaySummaryData(m_summaryLandingRoll[strSimResult],pSummaryData);
					break;
				case AirsideFlightRunwayDelayReport::ChartType_Summary_ByExiting:
					CopyRunwayDelaySummaryData(m_summaryExiting[strSimResult],pSummaryData);
					break;
				case AirsideFlightRunwayDelayReport::ChartType_Summary_ByInTakeoffQueue:
					CopyRunwayDelaySummaryData(m_summaryInTakeoffQ[strSimResult],pSummaryData);
					break;
				case AirsideFlightRunwayDelayReport::ChartType_Summary_ByHoldShort:
					CopyRunwayDelaySummaryData(m_summaryHoldShortL[strSimResult],pSummaryData);
					break;
				case AirsideFlightRunwayDelayReport::ChartType_Summary_ByInPosition:
					CopyRunwayDelaySummaryData(m_summaryInPosition[strSimResult],pSummaryData);
					break;
				default:
					break;
				}
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
    //std::vector<TimeInterval> vTimeInterval;
    //ElapsedTime tempTime = pParameter->getStartTime();
    //for(; tempTime<pParameter->getEndTime(); tempTime+=pParameter->getInterval())
    //{
    //    TimeInterval timeInterval;
    //    timeInterval.m_tStart = tempTime;
    //    timeInterval.m_tEnd = tempTime + (pParameter->getInterval() - 1l);
    //    if(timeInterval.m_tEnd >= pParameter->getEndTime())
    //        timeInterval.m_tEnd = pParameter->getEndTime() -1l;
    //    vTimeInterval.push_back(timeInterval);
    //}

    //BuildSummaryRunwayDelayData(m_summaryTotal, mapResultTotal, vTimeInterval, pParameter);
    //BuildSummaryRunwayDelayData(m_summaryLandingRoll, mapResultLandingRoll, vTimeInterval, pParameter);
    //BuildSummaryRunwayDelayData(m_summaryExiting, mapResultExiting, vTimeInterval, pParameter);
    //BuildSummaryRunwayDelayData(m_summaryInTakeoffQ, mapResultTakeOffQ, vTimeInterval, pParameter);
    //BuildSummaryRunwayDelayData(m_summaryHoldShortL, mapResultHoldShort, vTimeInterval, pParameter);
    //BuildSummaryRunwayDelayData(m_summaryInPosition, mapResultInPosition, vTimeInterval, pParameter);
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

        if(itor->second.find(strRunwayMark) == itor->second.end())
            continue;
        const std::vector<MultipleRunReportData>& vData2 = itor->second.at(strRunwayMark);
        size_t nCount2 = vData2.size();
        for(size_t i=0; i<nCount2; i++)
        {
            const MultipleRunReportData& data = vData2.at(i);
            c2dGraphData.m_vr2DChartData[simIdx].push_back((double)data.m_iData);
        }
    }

    // no data for all runways.
    if(mapDetailData.empty())
        return;
    // no data for selected runway.
    if(mapDetailData.begin()->second.find(strRunwayMark) == mapDetailData.begin()->second.end())
        return;
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
    strFooter.Format(_T("Multiple Run Report Total Runway Delay %s, %s "), GetMultipleRunReportString(),pParameter->GetParameterString());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetDetailLandingRoll3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay Landing Roll ");
    c2dGraphData.m_strYtitle = _T("Number of aircraft");
    c2dGraphData.m_strXtitle = _T("Delay time(secs)");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Multiple Run Report Runway Delay Landing Roll %s;%s "), GetMultipleRunReportString(),pParameter->GetParameterString());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetDetailExisting3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay Exiting ");
    c2dGraphData.m_strYtitle = _T("Number of aircraft");
    c2dGraphData.m_strXtitle = _T("Delay time(secs)");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Multiple Run Report Runway Delay Exiting %s;%s "), GetMultipleRunReportString(),pParameter->GetParameterString());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetDetailInTakeoffQ3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay In Takeoff Queue ");
    c2dGraphData.m_strYtitle = _T("Number of aircraft");
    c2dGraphData.m_strXtitle = _T("Delay time(secs)");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Multiple Run Report Runway Delay In Takeoff Queue %s;%s "), GetMultipleRunReportString(),pParameter->GetParameterString());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetDetailHoldShortLine3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay At Hold Short ");
    c2dGraphData.m_strYtitle = _T("Number of aircraft");
    c2dGraphData.m_strXtitle = _T("Delay time(secs)");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Multiple Run Report Runway Delay At Hold Short %s;%s "), GetMultipleRunReportString(),pParameter->GetParameterString());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetDetailInPosition3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay In Position ");
    c2dGraphData.m_strYtitle = _T("Number of aircraft");
    c2dGraphData.m_strXtitle = _T("Delay time(secs)");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Multiple Run Report Runway Delay In Position %s;%s "), GetMultipleRunReportString(),pParameter->GetParameterString());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::BuildSummaryRunwayDelayData(mapRunwaySummaryDelay& mapToBuild, 
    const mapRunwayDelayResult& oriData,  const std::vector<TimeInterval>& vTimeInterval, CParameters* parameter)
{
    // initialize map.
    mapRunwayDelayResult::const_iterator itor = oriData.begin();
    for(; itor!=oriData.end(); ++itor)
    {
        CString strSimResult = itor->first;
        std::vector<TimeInterval>::const_iterator intItor = vTimeInterval.begin();
        for(; intItor!=vTimeInterval.end(); ++intItor)
        {
            mapToBuild[strSimResult][*intItor].Clear();
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
				if ((*itor3)->m_lTotalDelay == 0)
					continue;
                TimeInterval timeInterval;
                if(FindTimeInterval(vTimeInterval, (*itor3)->m_tStartTime, timeInterval))
                {
                    tempMap[timeInterval].AddNewData((*itor3)->m_lTotalDelay);
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

// find TimeInterval from vTimeInterval which startTime in it.
bool CAirsideRunwayDelayMultiRunResult::FindTimeInterval(const std::vector<TimeInterval>& vTimeInterval, ElapsedTime startTime, TimeInterval& result)
{
    size_t nCount = vTimeInterval.size();
    for(size_t i=0; i<nCount; i++)
    {
        if(vTimeInterval.at(i).IsTimeInMe(startTime))
        {
            result = vTimeInterval.at(i);
            return true;
        }
    }
    return false;
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
	intervalMark.m_tEnd = intervalMark.m_tEnd - 1L;
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
        SetSummaryTotalDelay3DChartString(c2dGraphData, pPara,pTimeInterval);
        GenerateSummary2DChartData(c2dGraphData, m_summaryTotal, iType);
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Summary_ByLandingRoll:
        SetSummaryLandingRoll3DChartString(c2dGraphData, pPara,pTimeInterval);
        GenerateSummary2DChartData(c2dGraphData, m_summaryLandingRoll, iType);
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Summary_ByExiting:
        SetSummaryExisting3DChartString(c2dGraphData, pPara,pTimeInterval);
        GenerateSummary2DChartData(c2dGraphData, m_summaryExiting, iType);
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Summary_ByInTakeoffQueue:
        SetSummaryInTakeoffQ3DChartString(c2dGraphData, pPara,pTimeInterval);
        GenerateSummary2DChartData(c2dGraphData, m_summaryInTakeoffQ, iType);
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Summary_ByHoldShort:
        SetSummaryHoldShortLine3DChartString(c2dGraphData, pPara,pTimeInterval);
        GenerateSummary2DChartData(c2dGraphData, m_summaryHoldShortL, iType);
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Summary_ByInPosition:
        SetSummaryInPosition3DChartString(c2dGraphData, pPara,pTimeInterval);
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
	TimeInterval timeInterval = *pTimeInterval;
	timeInterval.m_tEnd = timeInterval.m_tEnd + 1L;
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

        if(itor->second.find(timeInterval) == itor->second.end())
        {
            c2dGraphData.m_vr2DChartData[0].push_back((double)0);
            c2dGraphData.m_vr2DChartData[1].push_back((double)0);
            c2dGraphData.m_vr2DChartData[2].push_back((double)0);
            c2dGraphData.m_vr2DChartData[3].push_back((double)0);
            c2dGraphData.m_vr2DChartData[4].push_back((double)0);
            c2dGraphData.m_vr2DChartData[5].push_back((double)0);
            c2dGraphData.m_vr2DChartData[6].push_back((double)0);
            c2dGraphData.m_vr2DChartData[7].push_back((double)0);
            c2dGraphData.m_vr2DChartData[8].push_back((double)0);
            c2dGraphData.m_vr2DChartData[9].push_back((double)0);
            c2dGraphData.m_vr2DChartData[10].push_back((double)0);
            c2dGraphData.m_vr2DChartData[11].push_back((double)0);
            c2dGraphData.m_vr2DChartData[12].push_back((double)0);
            continue;
        }

        c2dGraphData.m_vr2DChartData[0].push_back((double)itor->second.at(timeInterval).m_estMin.asSeconds());
        c2dGraphData.m_vr2DChartData[1].push_back((double)itor->second.at(timeInterval).m_estAverage.asSeconds());
        c2dGraphData.m_vr2DChartData[2].push_back((double)itor->second.at(timeInterval).m_estMax.asSeconds());
        c2dGraphData.m_vr2DChartData[3].push_back((double)itor->second.at(timeInterval).m_estQ1.asSeconds());
        c2dGraphData.m_vr2DChartData[4].push_back((double)itor->second.at(timeInterval).m_estQ2.asSeconds());
        c2dGraphData.m_vr2DChartData[5].push_back((double)itor->second.at(timeInterval).m_estQ3.asSeconds());
        c2dGraphData.m_vr2DChartData[6].push_back((double)itor->second.at(timeInterval).m_estP1.asSeconds());
        c2dGraphData.m_vr2DChartData[7].push_back((double)itor->second.at(timeInterval).m_estP5.asSeconds());
        c2dGraphData.m_vr2DChartData[8].push_back((double)itor->second.at(timeInterval).m_estP10.asSeconds());
        c2dGraphData.m_vr2DChartData[9].push_back((double)itor->second.at(timeInterval).m_estP90.asSeconds());
        c2dGraphData.m_vr2DChartData[10].push_back((double)itor->second.at(timeInterval).m_estP95.asSeconds());
        c2dGraphData.m_vr2DChartData[11].push_back((double)itor->second.at(timeInterval).m_estP99.asSeconds());
        c2dGraphData.m_vr2DChartData[12].push_back((double)itor->second.at(timeInterval).m_estSigma.asSeconds());
    }
}

void CAirsideRunwayDelayMultiRunResult::SetSummaryTotalDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter,TimeInterval* pTimeInterval)
{
    c2dGraphData.m_strChartTitle = _T(" Total Runway Delay Summary ");
    c2dGraphData.m_strYtitle = _T("Delay time(secs)");
    c2dGraphData.m_strXtitle = _T("Runs");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Multiple Run Report Total Runway Delay Summary  %s,%s %s;%s "),GetMultipleRunReportString(), pParameter->GetParameterString(),pTimeInterval->m_tStart.printTime(), pTimeInterval->m_tEnd.printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetSummaryLandingRoll3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter,TimeInterval* pTimeInterval)
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay Summary At Landing Roll ");
    c2dGraphData.m_strYtitle = _T("Delay time(secs)");
    c2dGraphData.m_strXtitle = _T("Runs");

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Multiple Run Report Runway Delay Summary At Landing Roll %s,%s %s;%s "), GetMultipleRunReportString(),pParameter->GetParameterString(),pTimeInterval->m_tStart.printTime(), pTimeInterval->m_tEnd.printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetSummaryExisting3DChartString( C2DChartData& c2dGraphData,CParameters *pParameter, TimeInterval* pTimeInterval)
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay Summary At Exiting ");
    c2dGraphData.m_strYtitle = _T("Delay time(secs)");
    c2dGraphData.m_strXtitle = _T("Runs");

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Multiple Run Report Runway Delay Summary At Exiting %s,%s %s;%s "), GetMultipleRunReportString(),pParameter->GetParameterString(),pTimeInterval->m_tStart.printTime(), pTimeInterval->m_tEnd.printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetSummaryInTakeoffQ3DChartString( C2DChartData& c2dGraphData,CParameters *pParameter, TimeInterval* pTimeInterval)
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay Summary In Takeoff Queue ");
    c2dGraphData.m_strYtitle = _T("Delay time(secs)");
    c2dGraphData.m_strXtitle = _T("Runs");

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Multiple Run Report Runway Delay Summary In Takeoff Queue %s,%s %s;%s "), GetMultipleRunReportString(),pParameter->GetParameterString(),pTimeInterval->m_tStart.printTime(), pTimeInterval->m_tEnd.printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetSummaryHoldShortLine3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter,TimeInterval* pTimeInterval)
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay Summary At Hold Short Line");
    c2dGraphData.m_strYtitle = _T("Delay time(secs)");
    c2dGraphData.m_strXtitle = _T("Runs");

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Multiple Run Report Runway Delay Summary At Hold Short Line %s,%s %s;%s "),GetMultipleRunReportString(), pParameter->GetParameterString(),pTimeInterval->m_tStart.printTime(), pTimeInterval->m_tEnd.printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideRunwayDelayMultiRunResult::SetSummaryInPosition3DChartString( C2DChartData& c2dGraphData,CParameters *pParameter, TimeInterval* pTimeInterval)
{
    c2dGraphData.m_strChartTitle = _T(" Runway Delay Summary In Position ");
    c2dGraphData.m_strYtitle = _T("Delay time(secs)");
    c2dGraphData.m_strXtitle = _T("Runs");

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Multiple Run Report Runway Delay Summary In Position %s,%s %s;%s "),GetMultipleRunReportString(), pParameter->GetParameterString(),pTimeInterval->m_tStart.printTime(), pTimeInterval->m_tEnd.printTime());
    c2dGraphData.m_strFooter = strFooter;
}



std::vector<TimeInterval*> CAirsideRunwayDelayMultiRunResult::GetDelayedIntervalList(CParameters *pParameter)
{
    ClearDelayedIntervalList();
    ASSERT(pParameter->getReportType() == ASReportType_Summary);
    AirsideFlightRunwayDelayReportPara* pPara = (AirsideFlightRunwayDelayReportPara*)pParameter;

    ElapsedTime intervalTime = pPara->getInterval();
    ElapsedTime endTime = pPara->getEndTime();
    for(ElapsedTime curTime = pPara->getStartTime(); curTime<endTime; curTime+=intervalTime)
    {
        TimeInterval* pTimeInterval = new TimeInterval;
        pTimeInterval->m_tStart = curTime;
        pTimeInterval->m_tEnd = curTime + intervalTime - 1L;
        if(pTimeInterval->m_tEnd > endTime)
            pTimeInterval->m_tEnd = endTime - 1L;
        m_vIntervalList.push_back(pTimeInterval);
    }

    return m_vIntervalList;
}

// filter the runway mark using settings from GUI.
bool CAirsideRunwayDelayMultiRunResult::IsAvailableRunwayMark(CString strRunwayMark, CParameters* pParameter)
{
     AirsideFlightRunwayDelayReportPara* pPara = (AirsideFlightRunwayDelayReportPara*)pParameter;
     std::vector<CAirsideRunwayUtilizationReportParam::CRunwayMarkParam> vRMParam = pPara->m_vReportMarkParam;
     std::vector<CAirsideRunwayUtilizationReportParam::CRunwayMarkParam>::const_iterator itor = vRMParam.begin();
     for(; itor!=vRMParam.end(); ++itor)
     {
         std::vector<CAirsideReportRunwayMark> vRunwayMark = itor->m_vRunwayMark;
         std::vector<CAirsideReportRunwayMark>::const_iterator itor2 = vRunwayMark.begin();
         for(; itor2!=vRunwayMark.end(); ++itor2)
         {
             if(itor2->m_strMarkName.CompareNoCase("All") == 0 ||
                 itor2->m_strMarkName.CompareNoCase(strRunwayMark) == 0)
                 return true;
         }
     }
     return false;
}

// get the runway list that delay occurred.
std::vector<CString> CAirsideRunwayDelayMultiRunResult::GetDelayedRunwayMarkList(CParameters *pParameter) const
{
    ASSERT(pParameter->getReportType() == ASReportType_Detail);
    std::vector<CString> vResult;

    const mapRunwayDetailDelay* pSelDetailData = NULL;
    AirsideFlightRunwayDelayReportPara* pPara = (AirsideFlightRunwayDelayReportPara*)pParameter;
    switch (pPara->getSubType())
    {
    case AirsideFlightRunwayDelayReport::ChartType_Detail_Total:
        pSelDetailData = &m_detailTotal;
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Detail_ByLandingRoll:
        pSelDetailData = &m_detailLandingRoll;
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Detail_ByExiting:
        pSelDetailData = &m_detailExiting;
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Detail_ByInTakeoffQueue:
        pSelDetailData = &m_detailInTakeoffQ;
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Detail_ByHoldShort:
        pSelDetailData = &m_detailHoldShortL;
        break;
    case AirsideFlightRunwayDelayReport::ChartType_Detail_ByInPosition:
        pSelDetailData = &m_detailInPosition;
        break;
    default:
        break;
    }
    ASSERT(pSelDetailData);
    mapRunwayDetailDelay::const_iterator itor = pSelDetailData->begin();
    for(; itor!=pSelDetailData->end(); ++itor)
    {
        MultiRunDetailMap::const_iterator itor2 = itor->second.begin();
        for(; itor2!=itor->second.end(); ++itor2)
        {
            std::vector<CString>::const_iterator runwayItor = vResult.begin();
            bool isRunwayRecorded = false;
            for(; runwayItor!=vResult.end(); ++runwayItor)
            {
                if((*runwayItor).CompareNoCase(itor2->first) == 0) // this runway name is recorded, do not add to the result list.
                {
                    isRunwayRecorded = true;
                    break;
                }
            }
            if(!isRunwayRecorded)
                vResult.push_back(itor2->first);
        }
    }
    return vResult;
}
BOOL CAirsideRunwayDelayMultiRunResult::WriteReportData( ArctermFile& _file )
{
	_file.writeField("Airside Multiple Run Runway Delay Report");//write report string
	_file.writeLine();

	_file.writeField("Detail Result");//write detail type
	_file.writeLine();
	WriteDetailMap(m_detailTotal,_file);
	WriteDetailMap(m_detailLandingRoll,_file);
	WriteDetailMap(m_detailExiting,_file);
	WriteDetailMap(m_detailInTakeoffQ,_file);
	WriteDetailMap(m_detailHoldShortL,_file);
	WriteDetailMap(m_detailInPosition,_file);

	_file.writeField("Summary Result");//write summary type
	_file.writeLine();
	WriteSummaryMap(m_summaryTotal,_file);
	WriteSummaryMap(m_summaryLandingRoll,_file);
	WriteSummaryMap(m_summaryExiting,_file);
	WriteSummaryMap(m_summaryInTakeoffQ,_file);
	WriteSummaryMap(m_summaryHoldShortL,_file);
	WriteSummaryMap(m_summaryInPosition,_file);
	return TRUE;
}

BOOL CAirsideRunwayDelayMultiRunResult::ReadReportData( ArctermFile& _file )
{
	ClearData();
	_file.getLine();
	//read detail
	ReadDetailMap(m_detailTotal,_file);
	ReadDetailMap(m_detailLandingRoll,_file);
	ReadDetailMap(m_detailExiting,_file);
	ReadDetailMap(m_detailInTakeoffQ,_file);
	ReadDetailMap(m_detailHoldShortL,_file);
	ReadDetailMap(m_detailInPosition,_file);

	_file.getLine();
	//read summary
	ReadSummayMap(m_summaryTotal,_file);
	ReadSummayMap(m_summaryLandingRoll,_file);
	ReadSummayMap(m_summaryExiting,_file);
	ReadSummayMap(m_summaryInTakeoffQ,_file);
	ReadSummayMap(m_summaryHoldShortL,_file);
	ReadSummayMap(m_summaryInPosition,_file);
	return TRUE;
}

CString CAirsideRunwayDelayMultiRunResult::GetReportFileName() const
{
	return _T("RunwayDelays\\RunwayDelays.rep");
}

BOOL CAirsideRunwayDelayMultiRunResult::WriteDetailMap( mapRunwayDetailDelay mapDetailData, ArctermFile& _file )
{
	long iSize = (long)mapDetailData.size();
	_file.writeInt(iSize);

	mapRunwayDetailDelay::iterator iter = mapDetailData.begin();
	for (; iter != mapDetailData.end(); ++iter)
	{
		CString strSimResult = iter->first;
		_file.writeField(strSimResult.GetBuffer(1024));
		strSimResult.ReleaseBuffer();

		int iCount = (int)iter->second.size();
		_file.writeInt(iCount);

		_file.writeLine();
		MultiRunDetailMap::iterator detailIter = iter->second.begin();
		for (; detailIter != iter->second.end(); ++detailIter)
		{
			CString strRunway = detailIter->first;
			_file.writeField(strRunway.GetBuffer(1024));
			strRunway.ReleaseBuffer();

			int iCapacity = (int)detailIter->second.size();
			_file.writeInt(iCapacity);
			for (int i = 0; i < iCapacity; i++)
			{
				const MultipleRunReportData& reportData = detailIter->second.at(i);
				_file.writeInt(reportData.m_iStart);
				_file.writeInt(reportData.m_iEnd);
				_file.writeInt(reportData.m_iData);

				_file.writeLine();
			}

			_file.writeLine();
		}

		_file.writeLine();
	}

	return TRUE;
}

BOOL CAirsideRunwayDelayMultiRunResult::ReadDetailMap( mapRunwayDetailDelay& mapDetailData,ArctermFile& _file )
{
	int iSize = 0; 
	_file.getInteger(iSize);

	for (int i = 0; i < iSize; i++)
	{
		CString strSimResult;
		_file.getField(strSimResult.GetBuffer(1024),1024);
		strSimResult.ReleaseBuffer();

		int iCount = 0;
		_file.getInteger(iCount);
		_file.getLine();

		for (int j = 0; j < iCount; j++)
		{
			CString strRunway;
			_file.getField(strRunway.GetBuffer(1024),1024);
			strRunway.ReleaseBuffer();

			int iCapacity = 0;
			_file.getInteger(iCapacity);
			for (int n = 0; n < iCapacity; n++)
			{
				MultipleRunReportData reportData;
				_file.getInteger(reportData.m_iStart);
				_file.getInteger(reportData.m_iEnd);
				_file.getInteger(reportData.m_iData);

				mapDetailData[strSimResult][strRunway].push_back(reportData);
				_file.getLine();
			}

			_file.getLine();
		}
		_file.getLine();
	}

	return TRUE;
}

BOOL CAirsideRunwayDelayMultiRunResult::WriteSummaryMap( mapRunwaySummaryDelay mapSummaryResult,ArctermFile& _file )
{
	long iSize = (long)mapSummaryResult.size();
	_file.writeInt(iSize);

	mapRunwaySummaryDelay::iterator iter = mapSummaryResult.begin();
	for (; iter != mapSummaryResult.end(); ++iter)
	{
		CString strSimResult = iter->first;
		_file.writeField(strSimResult.GetBuffer(1024));
		strSimResult.ReleaseBuffer();

		long iCount = (long)iter->second.size();
		_file.writeInt(iCount);
		_file.writeLine();
		mapSummaryDataOfInterval::iterator sumIter = iter->second.begin();
		for(; sumIter != iter->second.end(); ++sumIter)
		{
			TimeInterval intervalData = sumIter->first;
			_file.writeInt(intervalData.m_tStart.getPrecisely());
			_file.writeInt(intervalData.m_tEnd.getPrecisely());

			RunwayDelaySummaryData summaryData = sumIter->second;
			_file.writeInt(summaryData.m_nDataCount);
			_file.writeInt(summaryData.m_estTotal.getPrecisely());
			_file.writeInt(summaryData.m_estMin.getPrecisely());
			_file.writeInt(summaryData.m_estAverage.getPrecisely());
			_file.writeInt(summaryData.m_estMax.getPrecisely());
			_file.writeInt(summaryData.m_estQ1.getPrecisely());
			_file.writeInt(summaryData.m_estQ2.getPrecisely());
			_file.writeInt(summaryData.m_estQ3.getPrecisely());
			_file.writeInt(summaryData.m_estP1.getPrecisely());
			_file.writeInt(summaryData.m_estP5.getPrecisely());
			_file.writeInt(summaryData.m_estP10.getPrecisely());
			_file.writeInt(summaryData.m_estP90.getPrecisely());
			_file.writeInt(summaryData.m_estP95.getPrecisely());
			_file.writeInt(summaryData.m_estP99.getPrecisely());
			_file.writeInt(summaryData.m_estSigma.getPrecisely());

			_file.writeLine();
		}
		_file.writeLine();
	}
	return TRUE;
}

BOOL CAirsideRunwayDelayMultiRunResult::ReadSummayMap( mapRunwaySummaryDelay& mapSummaryResult,ArctermFile& _file )
{
	int iSize = 0; 
	_file.getInteger(iSize);

	for (int i = 0; i < iSize; i++)
	{
		CString strSimResult;
		_file.getField(strSimResult.GetBuffer(1024),1024);
		strSimResult.ReleaseBuffer();

		int iCount = 0;
		_file.getInteger(iCount);
		_file.getLine();

		for (int j = 0; j < iCount; j++)
		{
			TimeInterval intervalData;
			long lTime = 0;
			_file.getInteger(lTime);
			intervalData.m_tStart.setPrecisely(lTime);
			_file.getInteger(lTime);
			intervalData.m_tEnd.setPrecisely(lTime);

			RunwayDelaySummaryData summaryData;
			_file.getInteger(summaryData.m_nDataCount);
			_file.getInteger(lTime);
			summaryData.m_estTotal.setPrecisely(lTime);
			_file.getInteger(lTime);
			summaryData.m_estMin.setPrecisely(lTime);
			_file.getInteger(lTime);
			summaryData.m_estAverage.setPrecisely(lTime);
			_file.getInteger(lTime);
			summaryData.m_estMax.setPrecisely(lTime);
			_file.getInteger(lTime);
			summaryData.m_estQ1.setPrecisely(lTime);
			_file.getInteger(lTime);
			summaryData.m_estQ2.setPrecisely(lTime);
			_file.getInteger(lTime);
			summaryData.m_estQ3.setPrecisely(lTime);
			_file.getInteger(lTime);
			summaryData.m_estP1.setPrecisely(lTime);
			_file.getInteger(lTime);
			summaryData.m_estP5.setPrecisely(lTime);
			_file.getInteger(lTime);
			summaryData.m_estP10.setPrecisely(lTime);
			_file.getInteger(lTime);
			summaryData.m_estP90.setPrecisely(lTime);
			_file.getInteger(lTime);
			summaryData.m_estP95.setPrecisely(lTime);
			_file.getInteger(lTime);
			summaryData.m_estP99.setPrecisely(lTime);
			_file.getInteger(lTime);
			summaryData.m_estSigma.setPrecisely(lTime);

			mapSummaryResult[strSimResult][intervalData] = summaryData; 
			_file.getLine();
		}
		_file.getLine();
	}
	return TRUE;
}

void CAirsideRunwayDelayMultiRunResult::ClearDelayedIntervalList()
{
    size_t nCount = m_vIntervalList.size();
    for(size_t i=0; i<nCount; i++)
    {
        delete m_vIntervalList.at(i);
    }
    m_vIntervalList.clear();
}

void CAirsideRunwayDelayMultiRunResult::CopyRunwayDelaySummaryData( mapSummaryDataOfInterval& summaryInterval,AirsideFlightRunwayDelaySummaryData* pSummary )
{
	TimeInterval interval;
	interval.m_tStart = pSummary->m_tStart;
	interval.m_tEnd = pSummary->m_tEnd;
	mapSummaryDataOfInterval::iterator iter = summaryInterval.find(interval);
	if (iter != summaryInterval.end())
	{
		iter->second.m_nDataCount += pSummary->m_nDelayCount;
		iter->second.m_statisticalTool.CopyToolData(pSummary->m_statisticalTool);

		iter->second.m_estTotal += pSummary->m_lTotal;
		iter->second.m_estMin = (long)iter->second.m_statisticalTool.GetMin();;
		iter->second.m_estMax = (long)iter->second.m_statisticalTool.GetMax();
		iter->second.m_estAverage = (long)iter->second.m_statisticalTool.GetAvarage();
		iter->second.m_estQ1 = (long)iter->second.m_statisticalTool.GetPercentile(25);
		iter->second.m_estQ2 = (long)iter->second.m_statisticalTool.GetPercentile(50);
		iter->second.m_estQ3 = (long)iter->second.m_statisticalTool.GetPercentile(75);
		iter->second.m_estP1 = (long)iter->second.m_statisticalTool.GetPercentile(1);
		iter->second.m_estP5 = (long)iter->second.m_statisticalTool.GetPercentile(5);
		iter->second.m_estP10 = (long)iter->second.m_statisticalTool.GetPercentile(10);
		iter->second.m_estP90 = (long)iter->second.m_statisticalTool.GetPercentile(90);
		iter->second.m_estP95 = (long)iter->second.m_statisticalTool.GetPercentile(95);
		iter->second.m_estP99 = (long)iter->second.m_statisticalTool.GetPercentile(99);
		iter->second.m_estSigma = (long)iter->second.m_statisticalTool.GetSigma();
	}
	else
	{
		summaryInterval[interval].m_nDataCount = pSummary->m_nDelayCount;
		summaryInterval[interval].m_estTotal = pSummary->m_lTotal;
		summaryInterval[interval].m_estMin = pSummary->m_lMin;
		summaryInterval[interval].m_estAverage = pSummary->m_lAvg;
		summaryInterval[interval].m_estMax = pSummary->m_lMax;
		summaryInterval[interval].m_estQ1 = (long)pSummary->m_statisticalTool.GetPercentile(25);
		summaryInterval[interval].m_estQ2 = (long)pSummary->m_statisticalTool.GetPercentile(50);
		summaryInterval[interval].m_estQ3 = (long)pSummary->m_statisticalTool.GetPercentile(75);
		summaryInterval[interval].m_estP1 = (long)pSummary->m_statisticalTool.GetPercentile(1);
		summaryInterval[interval].m_estP5 = (long)pSummary->m_statisticalTool.GetPercentile(5);
		summaryInterval[interval].m_estP10 = (long)pSummary->m_statisticalTool.GetPercentile(10);
		summaryInterval[interval].m_estP90 = (long)pSummary->m_statisticalTool.GetPercentile(90);
		summaryInterval[interval].m_estP95 = (long)pSummary->m_statisticalTool.GetPercentile(95);
		summaryInterval[interval].m_estP99 = (long)pSummary->m_statisticalTool.GetPercentile(99);
		summaryInterval[interval].m_estSigma = (long)pSummary->m_statisticalTool.GetSigma();

		summaryInterval[interval].m_statisticalTool.CopyToolData(pSummary->m_statisticalTool);
	}
}

