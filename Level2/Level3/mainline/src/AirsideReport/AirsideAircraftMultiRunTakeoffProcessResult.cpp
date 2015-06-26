#include "stdafx.h"
#include "AirsideAircraftMultiRunTakeoffProcessResult.h"
#include "Common/TERMFILE.H"
#include "Parameters.h"
#include "MFCExControl/XListCtrl.h"
#include "AirsideTakeoffProcessParameter.h"
#include "AirsideTakeoffProcessDetailResult.h"
#include "AirsideTakeoffProcessSummaryResult.h"
#include "AirsideTakeoffProcessReport.h"

static const char* summaryTitle[] = 
{
    "Min(mm:ss)",
    "Avg(mm:ss)",
    "Max(mm:ss)",
    "Q1(mm:ss)",
    "Q2(mm:ss)",
    "Q3(mm:ss)",
    "P1(mm:ss)",
    "P5(mm:ss)",
    "P10(mm:ss)",
    "P90(mm:ss)",
    "P95(mm:ss)",
    "P99(mm:ss)",
    "Std Dev(mm:ss)"
};

CAirsideAircraftMultiRunTakeoffProcessResult::CAirsideAircraftMultiRunTakeoffProcessResult(void)
{
}


CAirsideAircraftMultiRunTakeoffProcessResult::~CAirsideAircraftMultiRunTakeoffProcessResult(void)
{
	ClearData();
}


void CAirsideAircraftMultiRunTakeoffProcessResult::LoadMultipleRunReport( CParameters* pParameter )
{
	ClearData();
	mapLoadResult mapTakeoffQDelay;
	mapLoadResult mapTimeToPosition;
	mapLoadResult mapTakeoffDelay;
    mapSummaryLoadResult mapSumResult;
	ArctermFile file;
	MapMultiRunTakeoffProcessData mapTakeoffLoadData;
	mapSimReport::iterator iter = m_mapSimReport.begin();
    for (; iter != m_mapSimReport.end(); ++iter)
    {
        CAirsideTakeoffProcessReport* pTakeoffReport = (CAirsideTakeoffProcessReport*)iter->second;
        CString strSimResult = iter->first;
        if (pParameter->getReportType() == ASReportType_Detail)
        {
            TakeoffDetailDataList dataList;
            if(pTakeoffReport->GeDetailtResult(pParameter,dataList))
            {
                for (unsigned i = 0; i < dataList.size(); i++)
                {
                    CTakeoffProcessDetailData detailData = dataList.at(i);
                    mapTakeoffQDelay[strSimResult].push_back(detailData.m_lTakeoffQueueDelay);
                    mapTimeToPosition[strSimResult].push_back(detailData.m_lToPostionTime);
                    mapTakeoffDelay[strSimResult].push_back(detailData.m_lTakeoffDelay);
                }
            }
            else
            {
            }
        }
        else if(pParameter->getReportType() == ASReportType_Summary)
        {
            TakeoffSummaryDataList dataList;
            if (pTakeoffReport->GetSummaryResult(pParameter,dataList))
            {
                for(unsigned i=0; i<dataList.size(); i++)
                {
                    TakeoffDetailDataList& detailDataList = dataList.at(i).detailDataList;
                    for(unsigned j=0; j<detailDataList.size(); j++)
                    {
                        mapSumResult[strSimResult].push_back(detailDataList.at(j));
                    }
                }
            }
        }
    }

	BuillDetailMutipleTakeoffProcess(m_mapTakeoffQDelay,mapTakeoffQDelay,pParameter->getInterval());
	//generate time to position
	BuillDetailMutipleTakeoffProcess(m_mapTimeToPosition,mapTimeToPosition,pParameter->getInterval());
	//generate takeoff delay
	BuillDetailMutipleTakeoffProcess(m_mapTakeoffDelay,mapTakeoffDelay,pParameter->getInterval());

    //generate summary data.
    BuillSummaryMutipleTakeoffProcess(mapSumResult);

}

void CAirsideAircraftMultiRunTakeoffProcessResult::InitListHead( CXListCtrl& cxListCtrl,CParameters * parameter, int iType /*= 0*/,CSortableHeaderCtrl* piSHC/*=NULL*/ )
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

		CAirsideTakeoffProcessParameter* pTakeoffProcessPara = (CAirsideTakeoffProcessParameter*)parameter;
		switch (pTakeoffProcessPara->getSubType())
		{
		case CAirsideTakeoffProcessDetailResult::TakeoffQueueDelay:
			InitDetailListHead(cxListCtrl,m_mapTakeoffQDelay,piSHC);
			break;
		case CAirsideTakeoffProcessDetailResult::TimeToPosition:
			InitDetailListHead(cxListCtrl,m_mapTimeToPosition,piSHC);
			break;
		case CAirsideTakeoffProcessDetailResult::TakeOffDelay:
			InitDetailListHead(cxListCtrl,m_mapTakeoffDelay,piSHC);
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

void CAirsideAircraftMultiRunTakeoffProcessResult::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter, int iType /*= 0*/ )
{
	ASSERT(parameter);
	if (parameter->getReportType() == ASReportType_Detail)
	{
		CAirsideTakeoffProcessParameter* pTakeoffProcessPara = (CAirsideTakeoffProcessParameter*)parameter;
		switch (pTakeoffProcessPara->getSubType())
		{
		case CAirsideTakeoffProcessDetailResult::TakeoffQueueDelay:
			FillDetailListContent(cxListCtrl,m_mapTakeoffQDelay);
			break;
		case CAirsideTakeoffProcessDetailResult::TimeToPosition:
			FillDetailListContent(cxListCtrl,m_mapTimeToPosition);
			break;
		case CAirsideTakeoffProcessDetailResult::TakeOffDelay:
			FillDetailListContent(cxListCtrl,m_mapTakeoffDelay);
			break;
		default:
			break;
		}
	}
    else if(parameter->getReportType() == ASReportType_Summary)
    {
        CAirsideTakeoffProcessParameter* pTakeoffProcessPara = (CAirsideTakeoffProcessParameter*)parameter;
        switch (pTakeoffProcessPara->getSubType())
        {
        case CAirsideTakeoffProcessSummaryResult::TakeOffQueueTime:
            FillSummaryListContent(cxListCtrl, m_mapSumTakeoffQueue);
            break;
        case CAirsideTakeoffProcessSummaryResult::TimeAtHoldShortLine:
            FillSummaryListContent(cxListCtrl, m_mapSumHoldShortLine);
            break;
        case CAirsideTakeoffProcessSummaryResult::TaxiToPosition:
            FillSummaryListContent(cxListCtrl, m_mapSumTaxiToPosition);
            break;
        case CAirsideTakeoffProcessSummaryResult::TimeInPosition:
            FillSummaryListContent(cxListCtrl, m_mapSumTimeToPosition);
            break;
        default:
            break;
        }
    }
}

void CAirsideAircraftMultiRunTakeoffProcessResult::SetDetail3DChartString( C2DChartData& c2dGraphData,CParameters *pParameter )
{
	ASSERT(pParameter);
	if (pParameter->getReportType() == ASReportType_Detail)
	{
		CAirsideTakeoffProcessParameter* pTakeoffProcessPara = (CAirsideTakeoffProcessParameter*)pParameter;
		switch (pTakeoffProcessPara->getSubType())
		{
		case CAirsideTakeoffProcessDetailResult::TakeoffQueueDelay:
			SetDetailTakeoffQDelay3DChartString(c2dGraphData,pParameter);
			break;
		case CAirsideTakeoffProcessDetailResult::TimeToPosition:
			SetDetailTimeToPosition3DChartString(c2dGraphData,pParameter);
			break;
		case CAirsideTakeoffProcessDetailResult::TakeOffDelay:
			SetDetailTakeoffDelay3DChartString(c2dGraphData,pParameter);
			break;
		default:
			break;
		}
	}
}
void CAirsideAircraftMultiRunTakeoffProcessResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType /*= 0*/)
{
	if (pParameter->getReportType() == ASReportType_Detail)
	{
		CAirsideTakeoffProcessParameter* pTakeoffProcessPara = (CAirsideTakeoffProcessParameter*)pParameter;
		switch (pTakeoffProcessPara->getSubType())
		{
		case CAirsideTakeoffProcessDetailResult::TakeoffQueueDelay:
			Generate3DChartData(m_mapTakeoffQDelay,chartWnd,pParameter);
			break;
		case CAirsideTakeoffProcessDetailResult::TimeToPosition:
			Generate3DChartData(m_mapTimeToPosition,chartWnd,pParameter);
			break;
		case CAirsideTakeoffProcessDetailResult::TakeOffDelay:
			Generate3DChartData(m_mapTakeoffDelay,chartWnd,pParameter);
			break;
		default:
			break;
		}
	}
    else if(pParameter->getReportType() == ASReportType_Summary)
    {
        DrawSummary3DChart(chartWnd, pParameter);
    }
}


void CAirsideAircraftMultiRunTakeoffProcessResult::ClearData()
{
	m_mapTakeoffQDelay.clear();
	m_mapTimeToPosition.clear();
	m_mapTakeoffDelay.clear();

    m_mapSumTakeoffQueue.clear();
    m_mapSumHoldShortLine.clear();
    m_mapSumTaxiToPosition.clear();
    m_mapSumTimeToPosition.clear();
}

void CAirsideAircraftMultiRunTakeoffProcessResult::BuillDetailMutipleTakeoffProcess( MultiRunDetailMap& mapDetailData,mapLoadResult mapData,long iInterval )
{
	if (mapData.empty())
		return;
	
	long lMinDelayTime = GetMapMinValue(mapData);
	long lMaxDelayTime = GetMapMaxValue(mapData);

	if (lMinDelayTime > lMaxDelayTime)
		return;

	ElapsedTime estMinDelayTime = ElapsedTime(long(lMinDelayTime/100.0+0.5));
	ElapsedTime estMaxDelayTime = ElapsedTime(long(lMaxDelayTime/100.0+0.5));
	ElapsedTime estUserIntervalTime = ElapsedTime(iInterval);

	long lDelayTimeSegmentCount = 0;             //the count of the delayTime segment
	if (0 < iInterval)
	{
		lDelayTimeSegmentCount = (lMaxDelayTime - lMinDelayTime) / (iInterval * 100);
		estMinDelayTime = ElapsedTime((lMinDelayTime - lMinDelayTime%(iInterval*100)) /100);

		lDelayTimeSegmentCount++;
	}
	else
	{
		lDelayTimeSegmentCount= ClacTimeRange(estMaxDelayTime, estMinDelayTime, estUserIntervalTime);
	}

	mapLoadResult::iterator iter = mapData.begin();
	for (; iter != mapData.end(); ++iter)
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
			mapDetailData[iter->first].push_back(delayData);
		}
	}
}

void CAirsideAircraftMultiRunTakeoffProcessResult::InitDetailListHead( CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData,CSortableHeaderCtrl* piSHC/*=NULL*/ )
{
	if (mapDetailData.empty())
		return;

	MultiRunDetailMap::iterator iter = mapDetailData.begin();
	size_t rangeCount = iter->second.size();
	for (size_t i = 0; i < rangeCount; ++i)
	{
		MultipleRunReportData delayData = iter->second.at(i);
		CString strRange;
		ElapsedTime eStartTime;
		ElapsedTime eEndTime;
		eStartTime.setPrecisely(delayData.m_iStart);
		eEndTime.setPrecisely(delayData.m_iEnd);
		strRange.Format(_T("%s - %s"),eStartTime.printTime(), eEndTime.printTime());

		DWORD dwStyle = LVCFMT_LEFT;
		dwStyle &= ~HDF_OWNERDRAW;

		cxListCtrl.InsertColumn(2+i, strRange, /*LVCFMT_LEFT*/dwStyle, 100);
	}
}

void CAirsideAircraftMultiRunTakeoffProcessResult::FillDetailListContent( CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData )
{
	MultiRunDetailMap::iterator iter = mapDetailData.begin();
	int idx = 0;
	for (; iter != mapDetailData.end(); ++iter)
	{
		CString strIndex;
		strIndex.Format(_T("%d"),idx+1);
		cxListCtrl.InsertItem(idx,strIndex);

		CString strSimName = iter->first;
		int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
		CString strRun = _T("");
		strRun.Format(_T("Run%d"),nCurSimResult+1);

		//	wndListCtrl.InsertItem(i, strRun);
		cxListCtrl.SetItemText(idx,1,strRun);
		cxListCtrl.SetItemData(idx,idx);
		for (size_t n = 0; n < iter->second.size(); ++n)
		{
			MultipleRunReportData delayData = iter->second.at(n);
			CString strCount;
			if(n <  iter->second.size())
				strCount.Format(_T("%d"), delayData.m_iData);
			else
				strCount.Format(_T("%d"),0) ;

			cxListCtrl.SetItemText(idx, n + 2, strCount);
		}
		idx++;
	}
}

void CAirsideAircraftMultiRunTakeoffProcessResult::Generate3DChartData( MultiRunDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter )
{
	C2DChartData c2dGraphData;

	SetDetail3DChartString(c2dGraphData,pParameter);

	if (mapDetailData.empty() == true)
		return;

	MultiRunDetailMap::iterator iter = mapDetailData.begin();
	for (unsigned iTitle = 0; iTitle < iter->second.size(); iTitle++)
	{
		MultipleRunReportData delayData = iter->second.at(iTitle);
		ElapsedTime eStartTime;
		ElapsedTime eEndTime;
		eStartTime.setPrecisely(delayData.m_iStart);
		eEndTime.setPrecisely(delayData.m_iEnd);

		CString strTimeRange;
		strTimeRange.Format(_T("%02d:%02d:%02d-%02d:%02d:%02d"), eStartTime.GetHour(), eStartTime.GetMinute(),eStartTime.GetSecond(), eEndTime.GetHour(), eEndTime.GetSecond(),eEndTime.GetSecond());
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
		for (unsigned i = 0; i < iter->second.size(); i++)
		{
			MultipleRunReportData delayData = iter->second.at(i);
			vData.push_back(delayData.m_iData);
		}
		c2dGraphData.m_vr2DChartData.push_back(vData);
	}
	chartWnd.DrawChart(c2dGraphData);
}


void CAirsideAircraftMultiRunTakeoffProcessResult::SetDetailTakeoffQDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T("Take off delay(Detailed)");
	c2dGraphData.m_strYtitle = _T("Number of flights ");
	c2dGraphData.m_strXtitle = _T("Time(HH:MM:SS)");	

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Airside Take off delay Report(Detailed) %s "),pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;
}

void CAirsideAircraftMultiRunTakeoffProcessResult::SetDetailTakeoffDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T("Take off delay(Detailed)");
	c2dGraphData.m_strYtitle = _T("Number of flights ");
	c2dGraphData.m_strXtitle = _T("Time(HH:MM:SS)");	

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Airside Take off delay Report(Detailed) %s "),pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;
}

void CAirsideAircraftMultiRunTakeoffProcessResult::SetDetailTimeToPosition3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T("Time to position(Detailed)");
	c2dGraphData.m_strYtitle = _T("Number of flights ");
	c2dGraphData.m_strXtitle = _T("Time(HH:MM:SS)");	

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Airside Time to position Report(Detailed) %s "),pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;
}

void CAirsideAircraftMultiRunTakeoffProcessResult::BuillSummaryMutipleTakeoffProcess(const mapSummaryLoadResult& mapSummaryResult)
{
    mapSummaryLoadResult::const_iterator simIter = mapSummaryResult.begin();
    for(; simIter!=mapSummaryResult.end(); ++simIter)
    {
        CStatisticalTools<double> toolTakeoffQDelay, toolHoldShortLine, toolTaxiToPosition, toolTimeToPosition;
        size_t nCount = simIter->second.size();
        for(size_t i=0; i<nCount; i++)
        {
            toolTakeoffQDelay.AddNewData(simIter->second[i].m_lTakeoffQueueDelay);
            toolHoldShortLine.AddNewData(simIter->second[i].m_lDelayHoldShortLine);
            toolTaxiToPosition.AddNewData(simIter->second[i].m_lToPostionTime);
            toolTimeToPosition.AddNewData(simIter->second[i].m_lTimeInPosition);
        }
        toolTakeoffQDelay.SortData();
        toolHoldShortLine.SortData();
        toolTaxiToPosition.SortData();
        toolTimeToPosition.SortData();

        StatisticSummaryData(simIter->first, toolTakeoffQDelay, m_mapSumTakeoffQueue);
        StatisticSummaryData(simIter->first, toolHoldShortLine, m_mapSumHoldShortLine);
        StatisticSummaryData(simIter->first, toolTaxiToPosition, m_mapSumTaxiToPosition);
        StatisticSummaryData(simIter->first, toolTimeToPosition, m_mapSumTimeToPosition);
    }

}

void CAirsideAircraftMultiRunTakeoffProcessResult::StatisticSummaryData(CString strSim, const CStatisticalTools<double> &tool, MultiRunSummaryMap &mapSum)
{
    mapSum[strSim].m_estTotal.setPrecisely((long)tool.GetSum());
    mapSum[strSim].m_estAverage.setPrecisely((long)tool.GetAvarage());
    mapSum[strSim].m_estMin.setPrecisely((long)tool.GetMin());
    mapSum[strSim].m_estMax.setPrecisely((long)tool.GetMax());
    mapSum[strSim].m_estQ1.setPrecisely((long)tool.GetPercentile(25));
    mapSum[strSim].m_estQ2.setPrecisely((long)tool.GetPercentile(50));
    mapSum[strSim].m_estQ3.setPrecisely((long)tool.GetPercentile(75));
    mapSum[strSim].m_estP1.setPrecisely((long)tool.GetPercentile(1));
    mapSum[strSim].m_estP5.setPrecisely((long)tool.GetPercentile(5));
    mapSum[strSim].m_estP10.setPrecisely((long)tool.GetPercentile(10));
    mapSum[strSim].m_estP90.setPrecisely((long)tool.GetPercentile(90));
    mapSum[strSim].m_estP95.setPrecisely((long)tool.GetPercentile(95));
    mapSum[strSim].m_estP99.setPrecisely((long)tool.GetPercentile(99));
    mapSum[strSim].m_estSigma.setPrecisely((long)tool.GetSigma());
}

void CAirsideAircraftMultiRunTakeoffProcessResult::InitSummaryListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC)
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

    for(int i=0; i<13; i++)
    {
        cxListCtrl.InsertColumn(nCurCol, summaryTitle[i], LVCFMT_LEFT, 100);
        if(piSHC)
        {
            piSHC->SetDataType(nCurCol,dtTIME);
        }
        nCurCol++;
    }
}

void CAirsideAircraftMultiRunTakeoffProcessResult::FillSummaryListContent(CXListCtrl& cxListCtrl,const MultiRunSummaryMap& mapSum)
{
    MultiRunSummaryMap::const_iterator iter = mapSum.begin();
    int idx = 0;
    for(; iter!=mapSum.end(); ++iter)
    {
        CString strIndex;
        strIndex.Format(_T("%d"),idx+1);
        cxListCtrl.InsertItem(idx,strIndex);

        CString strSimName = iter->first;
        int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
        CString strRun = _T("");
        strRun.Format(_T("Run%d"),nCurSimResult+1);

        int nCurCol = 1;
        cxListCtrl.SetItemText(idx, nCurCol, strRun);
        cxListCtrl.SetItemData(idx, idx);
        nCurCol++;

        CSummaryData sumData = iter->second;
        CString strTemp;

        strTemp.Format("%02d:%02d", sumData.m_estMin.GetMinute(), sumData.m_estMin.GetSecond());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%02d:%02d", sumData.m_estAverage.GetMinute(), sumData.m_estAverage.GetSecond());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%02d:%02d", sumData.m_estMax.GetMinute(), sumData.m_estMax.GetSecond());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%02d:%02d", sumData.m_estQ1.GetMinute(), sumData.m_estQ1.GetSecond());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%02d:%02d", sumData.m_estQ2.GetMinute(), sumData.m_estQ2.GetSecond());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%02d:%02d", sumData.m_estQ3.GetMinute(), sumData.m_estQ3.GetSecond());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%02d:%02d", sumData.m_estP1.GetMinute(), sumData.m_estP1.GetSecond());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%02d:%02d", sumData.m_estP5.GetMinute(), sumData.m_estP5.GetSecond());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%02d:%02d", sumData.m_estP10.GetMinute(), sumData.m_estP10.GetSecond());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%02d:%02d", sumData.m_estP90.GetMinute(), sumData.m_estP90.GetSecond());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%02d:%02d", sumData.m_estP95.GetMinute(), sumData.m_estP95.GetSecond());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%02d:%02d", sumData.m_estP99.GetMinute(), sumData.m_estP99.GetSecond());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%02d:%02d", sumData.m_estSigma.GetMinute(), sumData.m_estSigma.GetSecond());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        idx++;
    }
}

void CAirsideAircraftMultiRunTakeoffProcessResult::DrawSummary3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
    C2DChartData c2dGraphData;
    CAirsideTakeoffProcessParameter* pTakeoffProcessPara = (CAirsideTakeoffProcessParameter*)pParameter;
    switch (pTakeoffProcessPara->getSubType())
    {
    case CAirsideTakeoffProcessSummaryResult::TakeOffQueueTime:
        SetSummaryTakeoffQDelay3DChartString(c2dGraphData, pParameter);
        GenerateSummary2DChartData(c2dGraphData, m_mapSumTakeoffQueue);
        break;
    case CAirsideTakeoffProcessSummaryResult::TimeAtHoldShortLine:
        SetSummaryHoldShortLind3DChartString(c2dGraphData, pParameter);
        GenerateSummary2DChartData(c2dGraphData, m_mapSumHoldShortLine);
        break;
    case CAirsideTakeoffProcessSummaryResult::TaxiToPosition:
        SetSummaryToPositionTime3DChartString(c2dGraphData, pParameter);
        GenerateSummary2DChartData(c2dGraphData, m_mapSumTaxiToPosition);
        break;
    case CAirsideTakeoffProcessSummaryResult::TimeInPosition:
        SetSummaryArrivalPosition3DChartString(c2dGraphData, pParameter);
        GenerateSummary2DChartData(c2dGraphData, m_mapSumTimeToPosition);
        break;
    default:
        break;
    }

    chartWnd.DrawChart(c2dGraphData);
}

void CAirsideAircraftMultiRunTakeoffProcessResult::SetSummaryTakeoffQDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter)
{
    c2dGraphData.m_strChartTitle = _T(" Take off Q time Report ");
    c2dGraphData.m_strYtitle = _T("Time(mins)");
    c2dGraphData.m_strXtitle = _T("Runs");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Summary Airside Take off Q time Report %s "), pParameter->GetParameterString());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideAircraftMultiRunTakeoffProcessResult::SetSummaryHoldShortLind3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter)
{
    c2dGraphData.m_strChartTitle = _T(" Time at Hold Short Line Report ");
    c2dGraphData.m_strYtitle = _T("Time(mins)");
    c2dGraphData.m_strXtitle = _T("Runs");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Summary Airside Time at Hold Short Line Report %s "), pParameter->GetParameterString());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideAircraftMultiRunTakeoffProcessResult::SetSummaryToPositionTime3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter)
{
    c2dGraphData.m_strChartTitle = _T(" Taxi to Position Report ");
    c2dGraphData.m_strYtitle = _T("Time(mins)");
    c2dGraphData.m_strXtitle = _T("Runs");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Summary Airside Taxi to Position Report %s "), pParameter->GetParameterString());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideAircraftMultiRunTakeoffProcessResult::SetSummaryArrivalPosition3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter)
{
    c2dGraphData.m_strChartTitle = _T(" Time in Position Report ");
    c2dGraphData.m_strYtitle = _T("Time(mins)");
    c2dGraphData.m_strXtitle = _T("Runs");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("Summary Onboard Time in Position Report %s "), pParameter->GetParameterString());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideAircraftMultiRunTakeoffProcessResult::GenerateSummary2DChartData(C2DChartData& c2dGraphData, const MultiRunSummaryMap& multiRunSummaryMap)
{
    int nCount = sizeof(summaryTitle) / sizeof(summaryTitle[0]);
    for(int i=0; i<nCount; i++)
    {
        CString strLegend = summaryTitle[i];
        strLegend.TrimRight("(mm:ss)");
        c2dGraphData.m_vrLegend.push_back(strLegend);
    }

    c2dGraphData.m_vr2DChartData.resize(13);
    MultiRunSummaryMap::const_iterator iter = multiRunSummaryMap.begin();
    for(; iter != multiRunSummaryMap.end(); ++iter)
    {
        CString strSimName = iter->first;
        int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
        CString strXTickTitle;
        strXTickTitle.Format(_T("Run%d"), nCurSimResult+1);
        c2dGraphData.m_vrXTickTitle.push_back(strXTickTitle);

        c2dGraphData.m_vr2DChartData[0].push_back((double)iter->second.m_estMin.getPrecisely()/6000.0);
        c2dGraphData.m_vr2DChartData[1].push_back((double)iter->second.m_estAverage.getPrecisely()/6000.0);
        c2dGraphData.m_vr2DChartData[2].push_back((double)iter->second.m_estMax.getPrecisely()/6000.0);
        c2dGraphData.m_vr2DChartData[3].push_back((double)iter->second.m_estQ1.getPrecisely()/6000.0);
        c2dGraphData.m_vr2DChartData[4].push_back((double)iter->second.m_estQ2.getPrecisely()/6000.0);
        c2dGraphData.m_vr2DChartData[5].push_back((double)iter->second.m_estQ3.getPrecisely()/6000.0);
        c2dGraphData.m_vr2DChartData[6].push_back((double)iter->second.m_estP1.getPrecisely()/6000.0);
        c2dGraphData.m_vr2DChartData[7].push_back((double)iter->second.m_estP5.getPrecisely()/6000.0);
        c2dGraphData.m_vr2DChartData[8].push_back((double)iter->second.m_estP10.getPrecisely()/6000.0);
        c2dGraphData.m_vr2DChartData[9].push_back((double)iter->second.m_estP90.getPrecisely()/6000.0);
        c2dGraphData.m_vr2DChartData[10].push_back((double)iter->second.m_estP95.getPrecisely()/6000.0);
        c2dGraphData.m_vr2DChartData[11].push_back((double)iter->second.m_estP99.getPrecisely()/6000.0);
        c2dGraphData.m_vr2DChartData[12].push_back((double)iter->second.m_estSigma.getPrecisely()/6000.0);
    }
}
