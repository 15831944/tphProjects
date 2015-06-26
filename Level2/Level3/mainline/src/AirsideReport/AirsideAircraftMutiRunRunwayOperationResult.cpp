#include "stdafx.h"
#include "AirsideAircraftMutiRunRunwayOperationResult.h"
#include "Common\TERMFILE.H"
#include "AirsideRunwayOperationReportParam.h"
#include "AirsideRunwayOperationsReport.h"
#include "CARC3DChart.h"
#include "AirsideRunwayOperationReportDetailResult.h"

static const CString str[] = 
{
    _T("Min"),
    _T("Min Interval"),
    _T("Average"),
    _T("Max"),
    _T("Max Interval"),
    _T("Q1"),
    _T("Q2"),
    _T("Q3"),
    _T("P1"),
    _T("P5"),
    _T("P10"),
    _T("P90"),
    _T("P95"),
    _T("P99"),
    _T("Std Dev")
};

CAirsideAircraftMutiRunRunwayOperationResult::CAirsideAircraftMutiRunRunwayOperationResult(void)
{
}


CAirsideAircraftMutiRunRunwayOperationResult::~CAirsideAircraftMutiRunRunwayOperationResult(void)
{
	ClearData();
}
 
void CAirsideAircraftMutiRunRunwayOperationResult::LoadMultipleRunReport(CParameters* pParameter)
{
    ClearData();

	maRunwayLoadOperation mapLandingOperation;
	maRunwayLoadOperation mapTakeoffOperation;
	mapRunwayTimeOperation mapMovementInterval; 
    ArctermFile file;
	mapSimReport::iterator iter = m_mapSimReport.begin();
	for (; iter != m_mapSimReport.end(); ++iter)
    {
        AirsideRunwayOperationsReport* pRunwayOpReport = (AirsideRunwayOperationsReport*)iter->second;
        CString strSimResult = iter->first;

		CAirsideRunwayOperationReportBaseResult* pResult = pRunwayOpReport->GetRunwayOperationResult();
		if (pParameter->getReportType() == ASReportType_Detail)
		{
			CAirsideRunwayOperationReportDetailResult* pDetailResult = (CAirsideRunwayOperationReportDetailResult*)pResult;
			for (unsigned i = 0; i < pDetailResult->m_vFlightOperateValue.size(); i++)
			{
				CAirsideRunwayOperationReportDetailResult::FlightOperationData operationData = pDetailResult->m_vFlightOperateValue.at(i);
				if (operationData.m_sOperation.CompareNoCase("Landing") == 0)
				{
					mapLandingOperation[strSimResult][operationData.m_sRunway].push_back(operationData.m_lTime);
				}
				else if (operationData.m_sOperation.CompareNoCase("Takeoff") == 0)
				{
					mapTakeoffOperation[strSimResult][operationData.m_sRunway].push_back(operationData.m_lTime);
				}
			}

			for (unsigned nRunway = 0; nRunway < pDetailResult->m_vRunwayOperationDetail.size(); ++ nRunway)
			{
				CAirsideRunwayOperationReportDetailResult::RunwayOperationDetail* pRunwayData = pDetailResult->m_vRunwayOperationDetail.at(nRunway);

				for (unsigned nLeadtrail = 0; nLeadtrail < pRunwayData->m_vWakeVortexDetailValue.size(); ++ nLeadtrail)
				{
					CAirsideRunwayOperationReportDetailResult::WakeVortexDetailValue wakeVotex = pRunwayData->m_vWakeVortexDetailValue.at(nLeadtrail);
					CString strClassification;
					strClassification.Format(_T("%s-%s"),wakeVotex.m_strClassLeadName,wakeVotex.m_strClassTrailName);

					ClassificationValue landTrail;
					landTrail.m_strLandTrail = strClassification;
					landTrail.m_lFlightCount = wakeVotex.m_nFlightCount;
					m_mapLandTrailOperation[strSimResult][pRunwayData->m_runWaymark.m_strMarkName].push_back(landTrail);

					std::vector<ClassificationValue>::iterator findIter = std::find(m_mapLandTrailOperation[strSimResult]["ALL"].begin(),m_mapLandTrailOperation[strSimResult]["ALL"].end(),landTrail);
					if (findIter != m_mapLandTrailOperation[strSimResult]["ALL"].end())
					{
						findIter->m_lFlightCount += landTrail.m_lFlightCount;
					}
					else
					{
						m_mapLandTrailOperation[strSimResult]["ALL"].push_back(landTrail);
					}
				}
				
				for (unsigned nTime = 0; nTime < pRunwayData->m_vRunwayTimeValue.size(); ++ nTime)
				{
					CAirsideRunwayOperationReportDetailResult::RunwayOperationTimeValue timeValue = pRunwayData->m_vRunwayTimeValue.at(nTime);
					//make runway mark and landing/takeoff format
					CString strLandingMark;
					strLandingMark.Format(_T("%s Landings"),pRunwayData->m_runWaymark.m_strMarkName);
					RunwayTimeValue landingTime;
					landingTime.m_eTime = timeValue.m_eTime;
					landingTime.m_iOperation = timeValue.m_nLandingCount;
					CString strTakeoffMark;
					strTakeoffMark.Format(_T("%s TakeOff"),pRunwayData->m_runWaymark.m_strMarkName);
					RunwayTimeValue takeoffTime;
					takeoffTime.m_eTime = timeValue.m_eTime;
					takeoffTime.m_iOperation = timeValue.m_nTakeOffCount;

					mapMovementInterval[strSimResult][strLandingMark].push_back(landingTime);
					mapMovementInterval[strSimResult][strTakeoffMark].push_back(takeoffTime);
				}
			}
		}
		else
		{
			CAirsideRunwayOperationReportSummaryResult* pSummaryResult = (CAirsideRunwayOperationReportSummaryResult*)pResult;
			for (unsigned i = 0; i < pSummaryResult->m_vOperationStatistic.size(); i++)
			{
				CAirsideRunwayOperationReportSummaryResult::RunwayOperationStatisticItem* pOpItem = pSummaryResult->m_vOperationStatistic.at(i);

				//landing
				{
					CAirsideRunwayOperationReportSummaryResult::StatisticsSummaryItem landingItem = pOpItem->m_landingStastic;
					 SummaryRunwayOperationReportItem staSumItem;
					 staSumItem.m_runWaymark = pOpItem->m_runWaymark;
					 InitSummaryRunwayOperationData(staSumItem,landingItem);
					  m_summaryDataLanding[strSimResult].push_back(staSumItem);
				}

				//takeoff
				{
					CAirsideRunwayOperationReportSummaryResult::StatisticsSummaryItem takeoffItem = pOpItem->m_TakeOffStastic;
					SummaryRunwayOperationReportItem staSumItem;
					staSumItem.m_runWaymark = pOpItem->m_runWaymark;
					InitSummaryRunwayOperationData(staSumItem,takeoffItem);
					m_summaryDataTakeOff[strSimResult].push_back(staSumItem);

				}
				//movements
				{
					CAirsideRunwayOperationReportSummaryResult::StatisticsSummaryItem movementItem = pOpItem->m_MovementStastic;
					SummaryRunwayOperationReportItem staSumItem;
					staSumItem.m_runWaymark = pOpItem->m_runWaymark;
					InitSummaryRunwayOperationData(staSumItem,movementItem);
					m_summaryDataMovement[strSimResult].push_back(staSumItem);
				}
			}
		}
    }

	//generate runway operation multiple run landing 
	BuildDetailMultipleRunwayOperation(m_mapLandingOperation,mapLandingOperation,pParameter);
	//generate runway operation multiple run takeoff
	BuildDetailMultipleRunwayOperation(m_mapTakeoffOperation,mapTakeoffOperation,pParameter);
	//generate runway operation multiple run movement interval
	BuildDetailMultipleRunwayTimeValue(m_mapMovementInterval,mapMovementInterval,pParameter);
}

void CAirsideAircraftMutiRunRunwayOperationResult::InitListHead(CXListCtrl& cxListCtrl, CParameters* parameter, int iType, CSortableHeaderCtrl* piSHC)
{
    cxListCtrl.DeleteAllItems();

    while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
        cxListCtrl.DeleteColumn(0);

    if (parameter->getReportType() == ASReportType_Detail)
    {
		DWORD headStyle = LVCFMT_CENTER;
		headStyle &= ~HDF_OWNERDRAW;
		cxListCtrl.InsertColumn(0,"",headStyle,20);

		headStyle = LVCFMT_LEFT;
		headStyle &= ~HDF_OWNERDRAW;
		cxListCtrl.InsertColumn(1, _T("SimResult"), headStyle, 80);
		

		AirsideRunwayOperationReportParam* pOpPara = (AirsideRunwayOperationReportParam*)parameter;
		switch (pOpPara->getSubType())
		{
		case AirsideRunwayOperationsReport::ChartType_Detail_LandingsByRunway:
			{
				cxListCtrl.InsertColumn(2, _T("Runway"), headStyle, 80);
				InitDetailListHead(cxListCtrl,m_mapLandingOperation,piSHC);
			}
			break;
		case AirsideRunwayOperationsReport::ChartType_Detail_TakeOffByRunway:
		{
			cxListCtrl.InsertColumn(2, _T("Runway"), headStyle, 80);
			InitDetailListHead(cxListCtrl,m_mapTakeoffOperation,piSHC);
		}
			break;
		case AirsideRunwayOperationsReport::ChartType_Detail_LeadTrailByRunway:
		{
			cxListCtrl.InsertColumn(2, _T("Runway"), headStyle, 80);
			InitDetailClassificationHead(cxListCtrl,m_mapLandTrailOperation,piSHC);
		}
			break;
		case AirsideRunwayOperationsReport::ChartType_Detail_MovementsPerRunway:
			{
				cxListCtrl.InsertColumn(2, _T("Runway & L/T/M"), headStyle, 80);
				InitDetailListHead(cxListCtrl,m_mapMovementInterval,piSHC);
			}
			break;
		default:
			break;
		}
    }
    else if(parameter->getReportType() == ASReportType_Summary)
    {
        InitSummaryListHead(cxListCtrl, parameter, piSHC);
    }
}

void CAirsideAircraftMutiRunRunwayOperationResult::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter, int iType /*= 0*/ )
{
    ASSERT(parameter);
    if (parameter->getReportType() == ASReportType_Detail)
    {
		AirsideRunwayOperationReportParam* pOpPara = (AirsideRunwayOperationReportParam*)parameter;
		switch (pOpPara->getSubType())
		{
		case AirsideRunwayOperationsReport::ChartType_Detail_LandingsByRunway:
			FillDetailListOperationContent(cxListCtrl,m_mapLandingOperation);
			break;
		case AirsideRunwayOperationsReport::ChartType_Detail_TakeOffByRunway:
			FillDetailListOperationContent(cxListCtrl,m_mapTakeoffOperation);
			break;
		case AirsideRunwayOperationsReport::ChartType_Detail_LeadTrailByRunway:
			FillDetailListLandTrailContent(cxListCtrl,m_mapLandTrailOperation);
			break;
		case AirsideRunwayOperationsReport::ChartType_Detail_MovementsPerRunway:
			FillDetailListOperationContent(cxListCtrl,m_mapMovementInterval);
			break;
		default:
			break;
		}
    }
    else if(parameter->getReportType() == ASReportType_Summary)
    {
        AirsideRunwayOperationReportParam* pRunWayOpParam = (AirsideRunwayOperationReportParam*)parameter;
        int nSubType = pRunWayOpParam->getSubType();
        switch(nSubType)
        {
        case AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Operations:
            FillSummaryListContent(cxListCtrl, m_summaryDataMovement);
            break;
        case AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Landing:
            FillSummaryListContent(cxListCtrl, m_summaryDataLanding);
            break;
        case AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_TakeOff:
            FillSummaryListContent(cxListCtrl, m_summaryDataLanding);
            break;
        default:
            break;
        }
    }
}

void CAirsideAircraftMutiRunRunwayOperationResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter, int iType /*= 0*/ )
{
	if (pParameter->getReportType() == ASReportType_Detail)
	{
		AirsideRunwayOperationReportParam* pOpPara = (AirsideRunwayOperationReportParam*)pParameter;
		switch (pOpPara->getSubType())
		{
		case AirsideRunwayOperationsReport::ChartType_Detail_LandingsByRunway:
			Generate3DChartCountData(m_mapLandingOperation,chartWnd,pParameter,iType);
			break;
		case AirsideRunwayOperationsReport::ChartType_Detail_TakeOffByRunway:
			Generate3DChartCountData(m_mapTakeoffOperation,chartWnd,pParameter,iType);
			break;
		case AirsideRunwayOperationsReport::ChartType_Detail_LeadTrailByRunway:
			Generate3DChartLeadTrailData(m_mapLandTrailOperation,chartWnd,pParameter,iType);
			break;
		case AirsideRunwayOperationsReport::ChartType_Detail_MovementsPerRunway:
			Generate3DChartCountData(m_mapMovementInterval,chartWnd,pParameter,iType);
			break;
		default:
			break;
		}
	}    
	else if(pParameter->getReportType() == ASReportType_Summary)
    {
        DrawSummary3DChart(chartWnd, pParameter, iType);
    }
}

void CAirsideAircraftMutiRunRunwayOperationResult::ClearData()
{
	m_mapLandingOperation.clear();
	m_mapTakeoffOperation.clear();
	m_mapLandTrailOperation.clear();
	m_mapMovementInterval.clear();

    m_summaryDataLanding.clear();
    m_summaryDataTakeOff.clear();
    m_summaryDataMovement.clear();
}

void CAirsideAircraftMutiRunRunwayOperationResult::InitSummaryListHead(CXListCtrl& cxListCtrl, CParameters* pParam, CSortableHeaderCtrl* piSHC)
{
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

    AirsideRunwayOperationReportParam* pRunWayOpParam = (AirsideRunwayOperationReportParam*)pParam;
    int nSubType = pRunWayOpParam->getSubType();
    switch(nSubType)
    {
    case AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_TakeOff:
        for(int i=0; i<15; i++)
        {
            cxListCtrl.InsertColumn(nCurCol, str[i]+_T("(TO)"), LVCFMT_LEFT, 100);
            if(piSHC)
            {
                piSHC->SetDataType(nCurCol,dtINT);
            }
            nCurCol++;
        }
        break;
    case AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Landing:
        for(int i=0; i<15; i++)
        {
            cxListCtrl.InsertColumn(nCurCol, str[i]+_T("(Landing)"), LVCFMT_LEFT, 100);
            if(piSHC)
            {
                piSHC->SetDataType(nCurCol,dtINT);
            }
            nCurCol++;
        }
        break;
    case AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Operations:
        for(int i=0; i<15; i++)
        {
            cxListCtrl.InsertColumn(nCurCol, str[i]+_T("(Movements)"), LVCFMT_LEFT, 100);
            if(piSHC)
            {
                piSHC->SetDataType(nCurCol,dtINT);
            }
            nCurCol++;
        }
        break;
    default:
        break;
    }
}

void CAirsideAircraftMutiRunRunwayOperationResult::FillSummaryListContent(CXListCtrl& cxListCtrl, const mapSummaryData& mapSum)
{
    mapSummaryData::const_iterator itor = mapSum.begin();
    int idx = 0;
    for(; itor!=mapSum.end(); itor++)
    {
        const vectorSummaryRunwayOpRepItem& vStaSumItems = itor->second;
        size_t nCount = vStaSumItems.size();
        for(size_t i=0; i<nCount; i++)
        {
            CString strIndex;
            strIndex.Format(_T("%d"),idx+1);
            cxListCtrl.InsertItem(idx,strIndex);

            CString strSimName = itor->first;
            int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
            CString strRun = _T("");
            strRun.Format(_T("Run%d"),nCurSimResult+1);

            int nCurCol = 1;
            cxListCtrl.SetItemText(idx, nCurCol, strRun);
            cxListCtrl.SetItemData(idx, idx);
            nCurCol++;

            const SummaryRunwayOperationReportItem& sumItem = vStaSumItems.at(i);

            CString strTemp;
            strTemp.Format("%s", sumItem.m_runWaymark.m_strMarkName);
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;

            strTemp.Format("%d", sumItem.m_minCount);
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;

            strTemp.Format("%s", sumItem.m_strMinInterval);
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;

            strTemp.Format("%d", sumItem.m_nAverageCount);
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;

            strTemp.Format("%d", sumItem.m_maxCount);
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;

            strTemp.Format("%s", sumItem.m_strMaxInterval);
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;

            strTemp.Format("%d", sumItem.m_nQ1);
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;

            strTemp.Format("%d", sumItem.m_nQ2);
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;

            strTemp.Format("%d", sumItem.m_nQ3);
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;

            strTemp.Format("%d", sumItem.m_nP1);
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;

            strTemp.Format("%d", sumItem.m_nP5);
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;

            strTemp.Format("%d", sumItem.m_nP10);
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;

            strTemp.Format("%d", sumItem.m_nP90);
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;

            strTemp.Format("%d", sumItem.m_nP95);
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;

            strTemp.Format("%d", sumItem.m_nP99);
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;

            strTemp.Format("%d", sumItem.m_nStdDev);
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;

            idx++;
        }
    }
}

void CAirsideAircraftMutiRunRunwayOperationResult::DrawSummary3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType)
{
    C2DChartData c2dGraphData;
    AirsideRunwayOperationReportParam* pRunWayOpParam = (AirsideRunwayOperationReportParam*)pParameter;
    int nSubType = pRunWayOpParam->getSubType();
    switch(nSubType)
    {
    case AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Operations:
        SetSummaryMovement3DChartString(c2dGraphData, pParameter);
        GenerateSummary2DChartData(c2dGraphData, m_summaryDataMovement, iType);
        break;
    case AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Landing:
        SetSummaryLanding3DChartString(c2dGraphData, pParameter);
        GenerateSummary2DChartData(c2dGraphData, m_summaryDataLanding, iType);
        break;
    case AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_TakeOff:
        SetSummaryTakeOff3DChartString(c2dGraphData, pParameter);
        GenerateSummary2DChartData(c2dGraphData, m_summaryDataTakeOff, iType);
        break;
    default:
        break;
    }

    chartWnd.DrawChart(c2dGraphData);
}

void CAirsideAircraftMutiRunRunwayOperationResult::GenerateSummary2DChartData(C2DChartData& c2dGraphData, const mapSummaryData& mapSumData, int iType)
{
    int nCount = sizeof(str) / sizeof(str[0]);
    for(int i=0; i<nCount; i++)
    {
        if(i==1 || i==4)
            continue;
        CString strLegend = str[i];
        c2dGraphData.m_vrLegend.push_back(strLegend);
    }

    RumwayMarkWithLandingTakeOff* pMarkToShow = (RumwayMarkWithLandingTakeOff*)iType;
    if(pMarkToShow == NULL)
        return;
    c2dGraphData.m_vr2DChartData.resize(13);
    mapSummaryData::const_iterator iter = mapSumData.begin();
    for(; iter != mapSumData.end(); ++iter)
    {
        CString strSimName = iter->first;
        int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
        CString strXTickTitle;
        strXTickTitle.Format(_T("Run%d"), nCurSimResult+1);
        c2dGraphData.m_vrXTickTitle.push_back(strXTickTitle);

        const vectorSummaryRunwayOpRepItem& vItems = iter->second;
        size_t nCount = vItems.size();
        for(size_t i=0; i<nCount; i++)
        {
            const SummaryRunwayOperationReportItem& item = vItems.at(i);
            if(item.m_runWaymark == pMarkToShow->m_mark)
            {
                c2dGraphData.m_vr2DChartData[0].push_back((double)item.m_minCount);
                c2dGraphData.m_vr2DChartData[1].push_back((double)item.m_nAverageCount);
                c2dGraphData.m_vr2DChartData[2].push_back((double)item.m_maxCount);
                c2dGraphData.m_vr2DChartData[3].push_back((double)item.m_nQ1);
                c2dGraphData.m_vr2DChartData[4].push_back((double)item.m_nQ2);
                c2dGraphData.m_vr2DChartData[5].push_back((double)item.m_nQ3);
                c2dGraphData.m_vr2DChartData[6].push_back((double)item.m_nP1);
                c2dGraphData.m_vr2DChartData[7].push_back((double)item.m_nP5);
                c2dGraphData.m_vr2DChartData[8].push_back((double)item.m_nP10);
                c2dGraphData.m_vr2DChartData[9].push_back((double)item.m_nP90);
                c2dGraphData.m_vr2DChartData[10].push_back((double)item.m_nP95);
                c2dGraphData.m_vr2DChartData[11].push_back((double)item.m_nP99);
                c2dGraphData.m_vr2DChartData[12].push_back((double)item.m_nStdDev);
            }
        }
    }
}

void CAirsideAircraftMutiRunRunwayOperationResult::SetSummaryMovement3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter)
{
    c2dGraphData.m_strChartTitle = _T(" Runway Operational Statistic - Movements ");
    c2dGraphData.m_strYtitle = _T("Number of Operations");
    c2dGraphData.m_strXtitle = _T("Runs");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Runway Operational Statistic - Movements %s "), pParameter->GetParameterString());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideAircraftMutiRunRunwayOperationResult::SetSummaryLanding3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T(" Runway Operational Statistic - Landing ");
    c2dGraphData.m_strYtitle = _T("Number of Operations");
    c2dGraphData.m_strXtitle = _T("Runs");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Runway Operational Statistic - Landing %s "), pParameter->GetParameterString());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideAircraftMutiRunRunwayOperationResult::SetSummaryTakeOff3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T(" Runway Operational Statistic - Take Off ");
    c2dGraphData.m_strYtitle = _T("Number of Operations");
    c2dGraphData.m_strXtitle = _T("Runs");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Runway Operational Statistic - Take Off %s "), pParameter->GetParameterString());
    c2dGraphData.m_strFooter = strFooter;
}
void CAirsideAircraftMutiRunRunwayOperationResult::BuildDetailMultipleRunwayOperation( mapRunwayDetailOperation& mapDetailData,maRunwayLoadOperation mapData,CParameters* pParameter )
{
	if (mapData.empty())
		return;

	ElapsedTime estMinDelayTime = pParameter->getStartTime();
	ElapsedTime estMaxDelayTime = pParameter->getEndTime();

	long lMinDelayTime = estMinDelayTime.asSeconds();
	long lMaxDelayTime = estMaxDelayTime.asSeconds();

	if (lMinDelayTime > lMaxDelayTime)
		return;

	long iInterval = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(iInterval);

	long lDelayTimeSegmentCount = 0;             //the count of the delayTime segment
	if (0 < pParameter->getInterval())
	{
        long lActMinDelayTime = (lMinDelayTime - lMinDelayTime%(iInterval)) /100;
        estMinDelayTime = ElapsedTime(lActMinDelayTime);
        lDelayTimeSegmentCount = (lMaxDelayTime - lActMinDelayTime) / (iInterval);

        if((lMaxDelayTime - lActMinDelayTime) % (iInterval) != 0)
            lDelayTimeSegmentCount += 1;
        if(lDelayTimeSegmentCount == 0)
            lDelayTimeSegmentCount = 1;
	}
	else
	{
		lDelayTimeSegmentCount= ClacTimeRange(estMaxDelayTime, estMinDelayTime, estUserIntervalTime);
	}

	maRunwayLoadOperation::iterator opIter = mapData.begin();
	for (; opIter != mapData.end(); ++opIter)
	{
		mapDetailData[opIter->first].clear();

		mapLoadResult::iterator iter = opIter->second.begin();
		for (; iter != opIter->second.end(); ++iter)
		{
			for (long i = 0; i < lDelayTimeSegmentCount; i++)
			{
				ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*i);
				ElapsedTime estTempMaxDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*(i + 1));

				MultipleRunReportData delayData;
				delayData.m_iStart = estTempMinDelayTime.getPrecisely();
				delayData.m_iEnd = estTempMaxDelayTime.getPrecisely();

				long iCount = GetIntervalCount(delayData.m_iStart,delayData.m_iEnd,iter->second);
				delayData.m_iData = iCount;
				mapDetailData[opIter->first][iter->first].push_back(delayData);

				//generate runway all data
				long iSize = (long)mapDetailData[opIter->first]["All"].size();
				if (iSize > i )//exsit this item
				{
					mapDetailData[opIter->first]["All"][i].m_iData += iCount;
				}
				else
				{
					mapDetailData[opIter->first]["All"].push_back(delayData);
				}
			}
		}
	}


}

void CAirsideAircraftMutiRunRunwayOperationResult::BuildDetailMultipleRunwayTimeValue( mapRunwayDetailOperation& mapDetailData,mapRunwayTimeOperation mapData,CParameters* pParameter )
{
	if (mapData.empty())
		return;

	ElapsedTime estMinDelayTime = pParameter->getStartTime();
	ElapsedTime estMaxDelayTime = pParameter->getEndTime();

	long lMinDelayTime = estMinDelayTime.asSeconds();
	long lMaxDelayTime = estMaxDelayTime.asSeconds();

	if (lMinDelayTime > lMaxDelayTime)
		return;

	long iInterval = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(iInterval);

	long lDelayTimeSegmentCount = 0;             //the count of the delayTime segment
	if (0 < pParameter->getInterval())
	{
        long lActMinDelayTime = (lMinDelayTime - lMinDelayTime%(iInterval)) /100;
        estMinDelayTime = ElapsedTime(lActMinDelayTime);
        lDelayTimeSegmentCount = (lMaxDelayTime - lActMinDelayTime) / (iInterval);

        if((lMaxDelayTime - lActMinDelayTime) % (iInterval) != 0)
            lDelayTimeSegmentCount += 1;
        if(lDelayTimeSegmentCount == 0)
            lDelayTimeSegmentCount = 1;
	}
	else
	{
		lDelayTimeSegmentCount= ClacTimeRange(estMaxDelayTime, estMinDelayTime, estUserIntervalTime);
	}
	mapRunwayTimeOperation::iterator timeIter = mapData.begin();
	for (; timeIter != mapData.end(); ++timeIter)
	{
		mapDetailData[timeIter->first].clear();
		mapRunwayTimeValue::iterator iter = timeIter->second.begin();
		for (; iter != timeIter->second.end(); ++iter)
		{
			mapDetailData[timeIter->first][iter->first].clear();
			for (long i = 0; i < lDelayTimeSegmentCount; i++)
			{
				ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*i);
				ElapsedTime estTempMaxDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*(i + 1));

				MultipleRunReportData delayData;
				delayData.m_iStart = estTempMinDelayTime.getPrecisely();
				delayData.m_iEnd = estTempMaxDelayTime.getPrecisely();

				long iCount = GetIntervalTimeValue(delayData.m_iStart,delayData.m_iEnd,iter->second);
				delayData.m_iData = iCount;
				mapDetailData[timeIter->first][iter->first].push_back(delayData);

				//generate runway all data
				long iSize = (long)mapDetailData[timeIter->first]["All"].size();
				if (iSize > i )//exsit this item
				{
					mapDetailData[timeIter->first]["All"][i].m_iData += iCount;
				}
				else
				{
					mapDetailData[timeIter->first]["All"].push_back(delayData);
				}
			}
		}
	}

}

long CAirsideAircraftMutiRunRunwayOperationResult::GetIntervalTimeValue( long iStart,long iEnd,std::vector<RunwayTimeValue> vData ) const
{
	long lOperation = 0;
	for (unsigned i = 0; i < vData.size(); i++)
	{
		RunwayTimeValue runwayTime = vData.at(i);
		if (runwayTime.m_eTime >= iStart && runwayTime.m_eTime < iEnd)
		{
			lOperation += runwayTime.m_iOperation;
		}
	}
	return lOperation;
}

void CAirsideAircraftMutiRunRunwayOperationResult::InitDetailListHead( CXListCtrl& cxListCtrl,mapRunwayDetailOperation mapDetailData,CSortableHeaderCtrl* piSHC/*=NULL*/ )
{
	if (mapDetailData.empty())
		return;

	mapRunwayDetailOperation::iterator iter = mapDetailData.begin();

	if (iter->second.empty())
		return;
	
	MultiRunDetailMap::iterator mapIter = iter->second.begin();

	size_t rangeCount = mapIter->second.size();
	for (size_t i = 0; i < rangeCount; ++i)
	{
		MultipleRunReportData delayData = mapIter->second.at(i);
		CString strRange;
		ElapsedTime eStartTime;
		ElapsedTime eEndTime;
		eStartTime.setPrecisely(delayData.m_iStart);
		eEndTime.setPrecisely(delayData.m_iEnd);
		strRange.Format(_T("%s - %s"),eStartTime.printTime(), eEndTime.printTime());

		DWORD dwStyle = LVCFMT_LEFT;
		dwStyle &= ~HDF_OWNERDRAW;

		cxListCtrl.InsertColumn(3+i, strRange, /*LVCFMT_LEFT*/dwStyle, 100);
	}
}

void CAirsideAircraftMutiRunRunwayOperationResult::FillDetailListLandTrailContent( CXListCtrl& cxListCtrl,mapLandTrailOperation mapDetailData )
{
	mapLandTrailOperation::iterator iter = mapDetailData.begin();
	int idx = 0;
	for (; iter != mapDetailData.end(); ++iter)
	{
		mapRunwayMark::iterator markIter = iter->second.begin();
		for (; markIter != iter->second.end(); ++markIter)
		{
			CString strIndex;
			strIndex.Format(_T("%d"),idx+1);
			cxListCtrl.InsertItem(idx,strIndex);

			CString strSimName = iter->first;
			int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
			CString strRun = _T("");
			strRun.Format(_T("Run%d"),nCurSimResult+1);

			cxListCtrl.SetItemText(idx,1,strRun);
			cxListCtrl.SetItemText(idx,2,markIter->first);
			cxListCtrl.SetItemData(idx,idx);

			for (size_t n = 0; n < markIter->second.size(); ++n)
			{
				ClassificationValue typeData = markIter->second.at(n);
				CString strCount;
				if(n <  markIter->second.size())
					strCount.Format(_T("%d"), typeData.m_lFlightCount);
				else
					strCount.Format(_T("%d"),0) ;

				cxListCtrl.SetItemText(idx, n + 3, strCount);
			}
			idx++;
		}
	}
}

void CAirsideAircraftMutiRunRunwayOperationResult::InitDetailClassificationHead( CXListCtrl& cxListCtrl,mapLandTrailOperation mapDetailData,CSortableHeaderCtrl* piSHC/*=NULL*/ )
{
	if (mapDetailData.empty())
		return;

	mapLandTrailOperation::iterator iter = mapDetailData.begin();

	if (iter->second.empty())
		return;

	mapRunwayMark::iterator markIter = iter->second.begin();

	size_t rangeCount = markIter->second.size();
	for (size_t i = 0; i < rangeCount; ++i)
	{
		ClassificationValue typeData = markIter->second.at(i);

		DWORD dwStyle = LVCFMT_LEFT;
		dwStyle &= ~HDF_OWNERDRAW;

		cxListCtrl.InsertColumn(3+i, typeData.m_strLandTrail, /*LVCFMT_LEFT*/dwStyle, 100);
	}
}

void CAirsideAircraftMutiRunRunwayOperationResult::FillDetailListOperationContent( CXListCtrl& cxListCtrl,mapRunwayDetailOperation mapDetailData )
{
	mapRunwayDetailOperation::iterator iter = mapDetailData.begin();
	int idx = 0;
	for (; iter != mapDetailData.end(); ++iter)
	{
		MultiRunDetailMap::iterator detaiIter = iter->second.begin();
		for (; detaiIter != iter->second.end(); ++detaiIter)
		{
			CString strIndex;
			strIndex.Format(_T("%d"),idx+1);
			cxListCtrl.InsertItem(idx,strIndex);

			CString strSimName = iter->first;
			int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
			CString strRun = _T("");
			strRun.Format(_T("Run%d"),nCurSimResult+1);

			cxListCtrl.SetItemText(idx,1,strRun);
			cxListCtrl.SetItemText(idx,2,detaiIter->first);
			cxListCtrl.SetItemData(idx,idx);

			for (size_t n = 0; n < detaiIter->second.size(); ++n)
			{
				MultipleRunReportData delayData = detaiIter->second.at(n);
				CString strCount;
				if(n <  detaiIter->second.size())
					strCount.Format(_T("%d"), delayData.m_iData);
				else
					strCount.Format(_T("%d"),0) ;

				cxListCtrl.SetItemText(idx, n + 3, strCount);
			}
			idx++;
		}
	}
}

void CAirsideAircraftMutiRunRunwayOperationResult::SetDetail3DChartString( C2DChartData& c2dGraphData,CParameters *pParameter )
{
	ASSERT(pParameter);
	if (pParameter->getReportType() == ASReportType_Detail)
	{
		AirsideRunwayOperationReportParam* pOpPara = (AirsideRunwayOperationReportParam*)pParameter;
		switch (pOpPara->getSubType())
		{
		case AirsideRunwayOperationsReport::ChartType_Detail_LandingsByRunway:
			SetDetailLanding3DChartString(c2dGraphData,pParameter);
			break;
		case AirsideRunwayOperationsReport::ChartType_Detail_TakeOffByRunway:
			SetDetailTakeoff3DChartString(c2dGraphData,pParameter);
			break;
		case AirsideRunwayOperationsReport::ChartType_Detail_LeadTrailByRunway:
			SetDetailLandTrail3DChartString(c2dGraphData,pParameter);
			break;
		case AirsideRunwayOperationsReport::ChartType_Detail_MovementsPerRunway:
			SetDetailMovement3DChartString(c2dGraphData,pParameter);
			break;
		default:
			break;
		}
	}
}

void CAirsideAircraftMutiRunRunwayOperationResult::SetDetailLanding3DChartString( C2DChartData& c2dGraphData,CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T(" Landings by Runway ");
	c2dGraphData.m_strYtitle = _T("Number of Operations");
	c2dGraphData.m_strXtitle = _T("Time");

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());

}

void CAirsideAircraftMutiRunRunwayOperationResult::SetDetailTakeoff3DChartString( C2DChartData& c2dGraphData,CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T(" Take Off by Runway ");
	c2dGraphData.m_strYtitle = _T("Number of Operations");
	c2dGraphData.m_strXtitle = _T("Time");

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());

}

void CAirsideAircraftMutiRunRunwayOperationResult::SetDetailLandTrail3DChartString( C2DChartData& c2dGraphData,CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T(" Land Trail Operations by Runway ");


	c2dGraphData.m_strYtitle = _T("Number of Operations");
	c2dGraphData.m_strXtitle = _T("Runway");

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());
}

void CAirsideAircraftMutiRunRunwayOperationResult::SetDetailMovement3DChartString( C2DChartData& c2dGraphData,CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T(" Movements per Interval ");


	c2dGraphData.m_strYtitle = _T("Number of Operations");
	c2dGraphData.m_strXtitle = _T("Time of Day");

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());
}

void CAirsideAircraftMutiRunRunwayOperationResult::Generate3DChartCountData( mapRunwayDetailOperation mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter,int iType )
{
	C2DChartData c2dGraphData;

	SetDetail3DChartString(c2dGraphData,pParameter);

	if (mapDetailData.empty() == true)
		return;

	RumwayMarkWithLandingTakeOff* pMark = (RumwayMarkWithLandingTakeOff*)iType;
	mapRunwayDetailOperation::iterator iter = mapDetailData.begin();
	CString strMarkOperation = pMark->GetMarkOperation();
	strMarkOperation.TrimLeft();
	strMarkOperation.TrimRight();
	for (unsigned iTitle = 0; iTitle <(iter->second)[strMarkOperation].size(); iTitle++)
	{
		MultipleRunReportData delayData = iter->second[strMarkOperation].at(iTitle);
		ElapsedTime eStartTime;
		ElapsedTime eEndTime;
		eStartTime.setPrecisely(delayData.m_iStart);
		eEndTime.setPrecisely(delayData.m_iEnd);

		CString strTimeRange;
		strTimeRange.Format(_T("%s-%s"),eStartTime.printTime(),eEndTime.printTime());
		c2dGraphData.m_vrXTickTitle.push_back(strTimeRange);
	}

	for (iter = mapDetailData.begin(); iter != mapDetailData.end(); ++iter)
	{
		CString strSimName = iter->first;
		int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
		CString strLegend;
		strLegend.Format(_T("Run%d"),nCurSimResult+1);
		c2dGraphData.m_vrLegend.push_back(strLegend);

		std::vector<double>  vData;
		for (unsigned i = 0; i <(iter->second)[strMarkOperation].size(); i++)
		{
			MultipleRunReportData delayData = (iter->second)[strMarkOperation].at(i);
			vData.push_back(delayData.m_iData);
		}
		c2dGraphData.m_vr2DChartData.push_back(vData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CAirsideAircraftMutiRunRunwayOperationResult::Generate3DChartLeadTrailData( mapLandTrailOperation mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter,int iType )
{
	C2DChartData c2dGraphData;

	SetDetail3DChartString(c2dGraphData,pParameter);

	if (mapDetailData.empty() == true)
		return;

	RumwayMarkWithLandingTakeOff* pMark = (RumwayMarkWithLandingTakeOff*)iType;
	mapLandTrailOperation::iterator iter = mapDetailData.begin();
	
	CString strMarkOperation = pMark->GetMarkOperation();
	strMarkOperation.TrimLeft();
	strMarkOperation.TrimRight();
	for (unsigned iTitle = 0; iTitle <(iter->second)[strMarkOperation].size(); iTitle++)
	{
		ClassificationValue typeName = iter->second[strMarkOperation].at(iTitle);
		c2dGraphData.m_vrXTickTitle.push_back(typeName.m_strLandTrail);
	}

	for (iter = mapDetailData.begin(); iter != mapDetailData.end(); ++iter)
	{
		CString strSimName = iter->first;
		int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
		CString strLegend;
		strLegend.Format(_T("Run%d"),nCurSimResult+1);
		c2dGraphData.m_vrLegend.push_back(strLegend);

		std::vector<double>  vData;
		for (unsigned i = 0; i < (iter->second)[strMarkOperation].size(); i++)
		{
			ClassificationValue typeName =(iter->second)[strMarkOperation].at(i);
			vData.push_back(typeName.m_lFlightCount);
		}
		c2dGraphData.m_vr2DChartData.push_back(vData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CAirsideAircraftMutiRunRunwayOperationResult::InitSummaryRunwayOperationData( SummaryRunwayOperationReportItem& staSumItem,const CAirsideRunwayOperationReportSummaryResult::StatisticsSummaryItem& dataItem )
{
	staSumItem.m_minCount = dataItem.m_minCount;
	staSumItem.m_strMinInterval = dataItem.m_strMinInterval;
	staSumItem.m_nAverageCount = dataItem.m_nAverageCount;
	staSumItem.m_maxCount = dataItem.m_maxCount;
	staSumItem.m_strMaxInterval = dataItem.m_strMaxInterval;

	staSumItem.m_nQ1 = dataItem.m_nQ1;
	staSumItem.m_nQ2 = dataItem.m_nQ2;
	staSumItem.m_nQ3 = dataItem.m_nQ3;
	staSumItem.m_nP1 = dataItem.m_nP1;
	staSumItem.m_nP5 = dataItem.m_nP5;
	staSumItem.m_nP10 = dataItem.m_nP10;
	staSumItem.m_nP90 = dataItem.m_nP90;
	staSumItem.m_nP95 = dataItem.m_nP95;
	staSumItem.m_nP99 = dataItem.m_nP99;
	staSumItem.m_nStdDev = dataItem.m_nStdDev;
}

BOOL CAirsideAircraftMutiRunRunwayOperationResult::WriteReportData( ArctermFile& _file )
{
	_file.writeField("Airside Multiple Run Runway Operation Report");//write report string
	_file.writeLine();

	_file.writeField("Detail Result");//write detail type
	_file.writeLine();
	WriteDetailMap(m_mapLandingOperation,_file);
	WriteDetailMap(m_mapTakeoffOperation,_file);
	WriteLandTrailMap(m_mapLandTrailOperation,_file);
	WriteDetailMap(m_mapMovementInterval,_file);

	_file.writeField("Summary Result");//write summary type
	_file.writeLine();
	WriteSummaryMap(m_summaryDataLanding,_file);
	WriteSummaryMap(m_summaryDataTakeOff,_file);
	WriteSummaryMap(m_summaryDataMovement,_file);
	return TRUE;
}

BOOL CAirsideAircraftMutiRunRunwayOperationResult::ReadReportData( ArctermFile& _file )
{
	ClearData();
	_file.getLine();
	//read detail
	ReadDetailMap(m_mapLandingOperation,_file);
	ReadDetailMap(m_mapTakeoffOperation,_file);
	ReadLandTrailMap(m_mapLandTrailOperation,_file);
	ReadDetailMap(m_mapMovementInterval,_file);

	_file.getLine();
	//read summary
	ReadSummayMap(m_summaryDataLanding,_file);
	ReadSummayMap(m_summaryDataTakeOff,_file);
	ReadSummayMap(m_summaryDataMovement,_file);
	return TRUE;
}

CString CAirsideAircraftMutiRunRunwayOperationResult::GetReportFileName() const
{
	return CString("RunwayOperations\\RunwayOperations.rep");
}

BOOL CAirsideAircraftMutiRunRunwayOperationResult::WriteDetailMap( mapRunwayDetailOperation mapDetailData, ArctermFile& _file )
{
	long iSize = (long)mapDetailData.size();
	_file.writeInt(iSize);

	mapRunwayDetailOperation::iterator iter = mapDetailData.begin();
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

BOOL CAirsideAircraftMutiRunRunwayOperationResult::ReadDetailMap( mapRunwayDetailOperation& mapDetailData,ArctermFile& _file )
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

BOOL CAirsideAircraftMutiRunRunwayOperationResult::WriteLandTrailMap( mapLandTrailOperation mapDetailData,ArctermFile& _file )
{
	long iSize = (long)mapDetailData.size();
	_file.writeInt(iSize);

	mapLandTrailOperation::iterator iter = mapDetailData.begin();
	for (; iter != mapDetailData.end(); ++iter)
	{
		CString strSimResult = iter->first;
		_file.writeField(strSimResult.GetBuffer(1024));
		strSimResult.ReleaseBuffer();

		int iCount = (int)iter->second.size();
		_file.writeInt(iCount);

		_file.writeLine();
		mapRunwayMark::iterator detailIter = iter->second.begin();
		for (; detailIter != iter->second.end(); ++detailIter)
		{
			CString strRunway = detailIter->first;
			_file.writeField(strRunway.GetBuffer(1024));
			strRunway.ReleaseBuffer();

			int iCapacity = (int)detailIter->second.size();
			_file.writeInt(iCapacity);
			for (int i = 0; i < iCapacity; i++)
			{
				ClassificationValue reportData = detailIter->second.at(i);
				_file.writeField(reportData.m_strLandTrail.GetBuffer(1024));
				reportData.m_strLandTrail.ReleaseBuffer();
				_file.writeInt(reportData.m_lFlightCount);

				_file.writeLine();
			}

			_file.writeLine();
		}

		_file.writeLine();
	}

	return TRUE;
}

BOOL CAirsideAircraftMutiRunRunwayOperationResult::ReadLandTrailMap( mapLandTrailOperation& mapDetailData,ArctermFile& _file )
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
				ClassificationValue reportData;
				_file.getField(reportData.m_strLandTrail.GetBuffer(1024),1024);
				reportData.m_strLandTrail.ReleaseBuffer();
				_file.getInteger(reportData.m_lFlightCount);

				mapDetailData[strSimResult][strRunway].push_back(reportData);
				_file.getLine();
			}

			_file.getLine();
		}
		_file.getLine();
	}

	return TRUE;
}

BOOL CAirsideAircraftMutiRunRunwayOperationResult::WriteSummaryMap( mapSummaryData mapSummaryResult,ArctermFile& _file )
{
	long iSize = (long)mapSummaryResult.size();
	_file.writeInt(iSize);

	mapSummaryData::iterator iter = mapSummaryResult.begin();
	for (; iter != mapSummaryResult.end(); ++iter)
	{
		CString strSimResult = iter->first;
		_file.writeField(strSimResult.GetBuffer(1024));
		strSimResult.ReleaseBuffer();

		long iCount = (long)iter->second.size();
		_file.writeInt(iCount);
		_file.writeLine();
		for (int i = 0; i < iCount; i++)
		{
			SummaryRunwayOperationReportItem reportItem = iter->second.at(i);
			_file.writeInt(reportItem.m_runWaymark.m_enumRunwayMark);
			_file.writeInt(reportItem.m_runWaymark.m_nRunwayID);
			_file.writeField(reportItem.m_runWaymark.m_strMarkName.GetBuffer(1024));
			reportItem.m_runWaymark.m_strMarkName.ReleaseBuffer();

			_file.writeInt(reportItem.m_minCount);
			_file.writeField(reportItem.m_strMinInterval.GetBuffer(1024));
			reportItem.m_strMinInterval.ReleaseBuffer();
			_file.writeInt(reportItem.m_nAverageCount);
			_file.writeInt(reportItem.m_maxCount);
			_file.writeField(reportItem.m_strMaxInterval.GetBuffer(1024));
			reportItem.m_strMaxInterval.ReleaseBuffer();
			_file.writeInt(reportItem.m_nQ1);
			_file.writeInt(reportItem.m_nQ2);
			_file.writeInt(reportItem.m_nQ3);
			_file.writeInt(reportItem.m_nP1);
			_file.writeInt(reportItem.m_nP5);
			_file.writeInt(reportItem.m_nP10);
			_file.writeInt(reportItem.m_nP90);
			_file.writeInt(reportItem.m_nP95);
			_file.writeInt(reportItem.m_nP99);
			_file.writeInt(reportItem.m_nStdDev);

			_file.writeLine();
		}
		_file.writeLine();
	}
	return TRUE;
}

BOOL CAirsideAircraftMutiRunRunwayOperationResult::ReadSummayMap( mapSummaryData& mapSummaryResult,ArctermFile& _file )
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
			SummaryRunwayOperationReportItem reportItem;
			int iRunwayMark = 0;
			_file.getInteger(iRunwayMark);
			reportItem.m_runWaymark.m_enumRunwayMark = RUNWAY_MARK(iRunwayMark);
			_file.getInteger(reportItem.m_runWaymark.m_nRunwayID);
			_file.getField(reportItem.m_runWaymark.m_strMarkName.GetBuffer(1024),1024);
			reportItem.m_runWaymark.m_strMarkName.ReleaseBuffer();

			_file.getInteger(reportItem.m_minCount);
			_file.getField(reportItem.m_strMinInterval.GetBuffer(1024),1024);
			reportItem.m_strMinInterval.ReleaseBuffer();
			_file.getInteger(reportItem.m_nAverageCount);
			_file.getInteger(reportItem.m_maxCount);
			_file.getField(reportItem.m_strMaxInterval.GetBuffer(1024),1024);
			reportItem.m_strMaxInterval.ReleaseBuffer();
			_file.getInteger(reportItem.m_nQ1);
			_file.getInteger(reportItem.m_nQ2);
			_file.getInteger(reportItem.m_nQ3);
			_file.getInteger(reportItem.m_nP1);
			_file.getInteger(reportItem.m_nP5);
			_file.getInteger(reportItem.m_nP10);
			_file.getInteger(reportItem.m_nP90);
			_file.getInteger(reportItem.m_nP95);
			_file.getInteger(reportItem.m_nP99);
			_file.getInteger(reportItem.m_nStdDev);

			mapSummaryResult[strSimResult].push_back(reportItem);
			_file.getLine();
		}
		_file.getLine();
	}
	return TRUE;
}

