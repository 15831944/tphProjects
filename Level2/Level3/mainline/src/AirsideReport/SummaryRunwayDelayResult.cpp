#include "StdAfx.h"
#include ".\summaryrunwaydelayresult.h"
#include "../Reports/StatisticalTools.h"
#include "../Engine/Airside/CommonInSim.h"
#include "AirsideFlightRunwayDelayData.h"
#include "AirsideFlightRunwayDelayReportPara.h"
#include "AirsideFlightRunwayDelaySummaryData.h"
#include "SummaryDelayChartResult.h"

SummaryRunwayDelayResult::SummaryRunwayDelayResult(CParameters* parameter)
:AirsideFlightRunwayDelayBaseResult(parameter)
{
}

SummaryRunwayDelayResult::~SummaryRunwayDelayResult(void)
{
	int nCount = m_vSummaryData.size();
	for (int i = 0; i < nCount; i++)
	{
		AirsideFlightRunwayDelaySummaryData* pData = m_vSummaryData.at(i);

		delete pData;
		pData = NULL;
	}
	m_vSummaryData.clear();
}

bool DelayDataCompare(AirsideFlightRunwayDelayData::RunwayDelayItem* fItem,AirsideFlightRunwayDelayData::RunwayDelayItem* sItem)
{
	if (fItem->m_PositionType < sItem->m_PositionType)
		return true;

	if (fItem->m_PositionType == sItem->m_PositionType && fItem->m_tStartTime < sItem->m_tStartTime )
		return true;

	return false;
}

void SummaryRunwayDelayResult::StatisticSummaryDataFromReportData()
{
	if (m_pReportData->m_vDataItemList.empty() ||m_pParameter == NULL)
		return;

	std::sort(m_pReportData->m_vDataItemList.begin(),m_pReportData->m_vDataItemList.end(),DelayDataCompare);

	AirsideFlightRunwayDelayData::RunwayDelayItem* pItem = m_pReportData->m_vDataItemList.at(0);
	int nTimIdx = pItem->GetTimeInIntervalIndex((CParameters*)m_pParameter);
	long lTimeStart = m_pParameter->getStartTime().asSeconds() + m_pParameter->getInterval()*nTimIdx;
	long lTimeEnd = m_pParameter->getStartTime().asSeconds() + m_pParameter->getInterval()*(nTimIdx+1);

	int nDelayCount = 1;
	long lTotal = pItem->m_lTotalDelay;
	long lMax = pItem->m_lTotalDelay;
	long lMin = lMax;
	CStatisticalTools<double> statisticalTool;
	statisticalTool.AddNewData(pItem->m_lTotalDelay);
	int nPostion = pItem->m_PositionType;

	int nCount = m_pReportData->m_vDataItemList.size();
	for ( int i = 1; i < nCount ; i++)
	{
		pItem = m_pReportData->m_vDataItemList.at(i);
		if (pItem->m_lTotalDelay ==0)
			continue;
        if(pItem->m_tStartTime >= m_pParameter->getEndTime() || pItem->m_tStartTime < m_pParameter->getStartTime())
            continue;
		if (pItem->m_PositionType == nPostion 
			&& (pItem->m_tStartTime.asSeconds() >= lTimeStart && pItem->m_tStartTime.asSeconds() <= lTimeEnd))		//same interval
		{
			statisticalTool.AddNewData(pItem->m_lTotalDelay);
			lTotal += pItem->m_lTotalDelay;
			lMax = lMax > pItem->m_lTotalDelay ? lMax : pItem->m_lTotalDelay;
			lMin = lMin < pItem->m_lTotalDelay ? lMin : pItem->m_lTotalDelay;
			nDelayCount ++;
		}
		else
		{
			AirsideFlightRunwayDelaySummaryData* pSummaryData = new AirsideFlightRunwayDelaySummaryData;
			pSummaryData->m_tStart = ElapsedTime(lTimeStart);
			pSummaryData->m_tEnd = ElapsedTime(lTimeEnd);
			pSummaryData->m_lTotal = lTotal;
			pSummaryData->m_lMax = lMax;
			pSummaryData->m_lMin = lMin;
			pSummaryData->m_lAvg = lTotal/nDelayCount;
			pSummaryData->m_nPosition = nPostion;
			pSummaryData->m_nDelayCount = nDelayCount;
			pSummaryData->m_statisticalTool = statisticalTool;
			pSummaryData->m_statisticalTool.SortData();
			m_vSummaryData.push_back(pSummaryData);

			nTimIdx = pItem->GetTimeInIntervalIndex((CParameters*)m_pParameter);
			lTimeStart = m_pParameter->getStartTime().asSeconds() + m_pParameter->getInterval()*nTimIdx;
			lTimeEnd = m_pParameter->getStartTime().asSeconds() + m_pParameter->getInterval()*(nTimIdx+1);

			nDelayCount = 1;
			lTotal = pItem->m_lTotalDelay;
			lMax = pItem->m_lTotalDelay;
			lMin = lMax;
			nPostion = pItem->m_PositionType;

			CStatisticalTools<double> statisticalTool2;
			statisticalTool2.AddNewData(pItem->m_lTotalDelay);
			statisticalTool = statisticalTool2;


		}
	}

	if(pItem != NULL)		//Add last data
	{
		AirsideFlightRunwayDelaySummaryData* pSummaryData = new AirsideFlightRunwayDelaySummaryData;
		pSummaryData->m_tStart = ElapsedTime(lTimeStart);
		pSummaryData->m_tEnd = ElapsedTime(lTimeEnd);
		pSummaryData->m_lTotal = lTotal;
		pSummaryData->m_lMax = lMax;
		pSummaryData->m_lMin = lMin;
		pSummaryData->m_lAvg = lTotal/nDelayCount;
		pSummaryData->m_nPosition = nPostion;
		pSummaryData->m_nDelayCount = nDelayCount;
		pSummaryData->m_statisticalTool = statisticalTool;
		m_vSummaryData.push_back(pSummaryData);
	}
}

void SummaryRunwayDelayResult::InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC )
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);


	GetListColumns(m_lstColumns);


	if (piSHC)
		piSHC->ResetAll();


	if (piSHC)
		piSHC->ResetAll();

	for (int nCol = 0 ; nCol < (int)m_lstColumns.size(); ++nCol)
	{
		cxListCtrl.InsertColumn(nCol,m_lstColumns[nCol],LVCFMT_LEFT, 100);
		if (piSHC)
		{
			switch (nCol)
			{
			case 0: case 1: 
				piSHC->SetDataType(nCol, dtSTRING);
				break;
			default:
				piSHC->SetDataType(nCol, dtINT);
				break;
			}
		}
	}
}

void SummaryRunwayDelayResult::FillListContent( CXListCtrl& cxListCtrl)
{
	StatisticSummaryDataFromReportData();
	//add data to list ctrl
	int nCount = m_vSummaryData.size();
	for (int i = 0; i < nCount; i++)
	{
		AirsideFlightRunwayDelaySummaryData* pItem = m_vSummaryData.at(i);
		ElapsedTime tTime = pItem->m_tStart;
		ElapsedTime tTime2 = pItem->m_tEnd -1L;
		CString strData;
		strData.Format("%s-%s",tTime.printTime(),tTime2.printTime());
		int nIdx = cxListCtrl.InsertItem(i, strData);

		cxListCtrl.SetItemText(nIdx,1, FlightRunwayDelay::POSITIONTYPE[pItem->m_nPosition]);
		strData.Format("%d",pItem->m_nDelayCount);
		cxListCtrl.SetItemText(nIdx,2, strData);
		strData.Format("%d",pItem->m_lTotal);
		cxListCtrl.SetItemText(nIdx,3, strData);
		strData.Format("%d",pItem->m_lMin);
		cxListCtrl.SetItemText(nIdx,4, strData);
		strData.Format("%d",pItem->m_lAvg);
		cxListCtrl.SetItemText(nIdx,5,  strData);
		strData.Format("%d",pItem->m_lMax);
		cxListCtrl.SetItemText(nIdx,6, strData);
		strData.Format("%d",(int)pItem->m_statisticalTool.GetPercentile(25));
		cxListCtrl.SetItemText(nIdx,7, strData);
		strData.Format("%d",(int)pItem->m_statisticalTool.GetPercentile(50));
		cxListCtrl.SetItemText(nIdx,8, strData);
		strData.Format("%d",(int)pItem->m_statisticalTool.GetPercentile(75));
		cxListCtrl.SetItemText(nIdx,9, strData);
		strData.Format("%d",(int)pItem->m_statisticalTool.GetPercentile(1));
		cxListCtrl.SetItemText(nIdx,10, strData);
		strData.Format("%d",(int)pItem->m_statisticalTool.GetPercentile(5));
		cxListCtrl.SetItemText(nIdx,11, strData);
		strData.Format("%d",(int)pItem->m_statisticalTool.GetPercentile(10));
		cxListCtrl.SetItemText(nIdx,12, strData);
		strData.Format("%d",(int)pItem->m_statisticalTool.GetPercentile(90));
		cxListCtrl.SetItemText(nIdx,13, strData);
		strData.Format("%d",(int)pItem->m_statisticalTool.GetPercentile(95));
		cxListCtrl.SetItemText(nIdx,14, strData);
		strData.Format("%d",(int)pItem->m_statisticalTool.GetPercentile(99));
		cxListCtrl.SetItemText(nIdx,15, strData);
		strData.Format("%d",(int)pItem->m_statisticalTool.GetSigma());
		cxListCtrl.SetItemText(nIdx,16, strData);
	}

}

void SummaryRunwayDelayResult::RefreshReport(CParameters * parameter )
{
	m_pParameter = (AirsideFlightRunwayDelayReportPara*)parameter;
	if(m_pParameter  == NULL)
		return;

	if(m_pChartResult != NULL)
		delete m_pChartResult;

	m_pChartResult = NULL;

	m_pChartResult = new SummaryDelayChartResult;

	((SummaryDelayChartResult*)m_pChartResult)->GenerateResult(m_vSummaryData);
}

BOOL SummaryRunwayDelayResult::ReadReportData( ArctermFile& _file )
{
	AirsideFlightRunwayDelayBaseResult::ReadReportData(_file);
	//StatisticSummaryDataFromReportData();
	return TRUE;
}

void SummaryRunwayDelayResult::GetListColumns( std::vector<CString>& lstColumns )
{
	lstColumns.clear();

	lstColumns.push_back(_T("Interval"));
	lstColumns.push_back(_T("Position"));
	lstColumns.push_back(_T("Number of delay"));
	lstColumns.push_back(_T("Total delay(secs)"));
	lstColumns.push_back(_T("Min delay(secs)"));
	lstColumns.push_back(_T("Average delay(secs)"));
	lstColumns.push_back(_T("Max delay(secs)"));

	lstColumns.push_back(_T("Q1"));
	lstColumns.push_back(_T("Q2"));
	lstColumns.push_back(_T("Q3"));
	lstColumns.push_back(_T("P1"));
	lstColumns.push_back(_T("P5"));
	lstColumns.push_back(_T("P10"));
	lstColumns.push_back(_T("P90"));
	lstColumns.push_back(_T("P95"));
	lstColumns.push_back(_T("P99"));
	lstColumns.push_back(_T("Std Dev"));


}

std::vector<AirsideFlightRunwayDelaySummaryData*> SummaryRunwayDelayResult::GetSummaryData()
{
	return m_vSummaryData;
}
