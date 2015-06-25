#include "StdAfx.h"
#include ".\airsideflightconflictsummaryresult.h"
#include "FlightConflictReportData.h"
#include "AirsideFlightConflictPara.h"
#include "AirsideFlightConflictReport.h"
#include "FlightConflictSummaryData.h"
#include "SummaryParaFilter.h"
#include "../Reports/StatisticalTools.h"
#include "../Engine/Airside/CommonInSim.h"
#include "FlightConflictSummaryChartResult.h"

AirsideFlightConflictSummaryResult::AirsideFlightConflictSummaryResult(CParameters * parameter)
:AirsideFlightConflictBaseResult(parameter)
{

}

AirsideFlightConflictSummaryResult::~AirsideFlightConflictSummaryResult(void)
{
	int nCount = m_vSummaryData.size();
	for (int i = 0; i < nCount; i++)
	{
		FlightConflictSummaryData* pData = m_vSummaryData.at(i);

		delete pData;
		pData = NULL;
	}
	m_vSummaryData.clear();
}

bool ConflictDataCompare(FlightConflictReportData::ConflictDataItem* fItem,FlightConflictReportData::ConflictDataItem* sItem)
{

	if (fItem->m_nConflictType < sItem->m_nConflictType)
		return true;

	if (fItem->m_nConflictType == sItem->m_nConflictType 
		&&  fItem->m_nLocationType < sItem->m_nLocationType)
		return true;

	if (fItem->m_nConflictType == sItem->m_nConflictType 
		&& fItem->m_nLocationType == sItem->m_nLocationType 
		&& fItem->m_nOperationType < sItem->m_nOperationType )
		return true;

	if (fItem->m_nConflictType == sItem->m_nConflictType 
		&& fItem->m_nLocationType == sItem->m_nLocationType 
		&& fItem->m_nOperationType == sItem->m_nOperationType 
		&& fItem->m_nActionType < sItem->m_nActionType)	
		return true;

	if (fItem->m_nConflictType == sItem->m_nConflictType 
		&& fItem->m_nLocationType == sItem->m_nLocationType
		&& fItem->m_nOperationType == sItem->m_nOperationType
		&& fItem->m_nActionType == sItem->m_nActionType 
		&& fItem->m_nAreaID < sItem->m_nAreaID )
		return true;

	if (fItem->m_nConflictType == sItem->m_nConflictType 
		&& fItem->m_nLocationType == sItem->m_nLocationType 
		&& fItem->m_nOperationType == sItem->m_nOperationType
		&& fItem->m_nActionType == sItem->m_nActionType 
		&& fItem->m_nAreaID == sItem->m_nAreaID  
		&& fItem->m_tTime < sItem->m_tTime)
		return true;

	return false;
}

void AirsideFlightConflictSummaryResult::StatisticSummaryDataFromReportData()
{
	if (m_pReportData->m_vDataItemList.empty() ||m_pParameter == NULL)
		return;

	std::sort(m_pReportData->m_vDataItemList.begin(),m_pReportData->m_vDataItemList.end(),ConflictDataCompare);

	FlightConflictReportData::ConflictDataItem* pItem = m_pReportData->m_vDataItemList.at(0);
	int nTimIdx = pItem->GetInIntervalIndex((CParameters*)m_pParameter);
	long lTimeStart = m_pParameter->getStartTime().asSeconds() + m_pParameter->getInterval()*nTimIdx;
	long lTimeEnd = m_pParameter->getStartTime().asSeconds() + m_pParameter->getInterval()*(nTimIdx+1);

	ReportDataFilter* pParaFilter = 
		new ReportDataFilter(lTimeStart,lTimeEnd, pItem->m_nConflictType,pItem->m_nAreaID,pItem->m_nLocationType,pItem->m_nOperationType,pItem->m_nActionType);
	CString strName = pItem->m_strAreaName;
	int nConflictCount = 1;
	ElapsedTime tTotal = pItem->m_tDelay;
	ElapsedTime tMax = pItem->m_tDelay;
	ElapsedTime tMin = tMax;
	CStatisticalTools<double> statisticalTool;
	statisticalTool.AddNewData(pItem->m_tDelay.asSeconds());

	int nCount = m_pReportData->m_vDataItemList.size();
	for ( int i = 1; i < nCount ; i++)
	{
		pItem = m_pReportData->m_vDataItemList.at(i);
		if (pParaFilter->IsFit(pItem))
		{
			statisticalTool.AddNewData(pItem->m_tDelay.asSeconds());
			tTotal += pItem->m_tDelay;
			tMax = tMax > pItem->m_tDelay ? tMax : pItem->m_tDelay;
			tMin = tMin < pItem->m_tDelay ? tMin : pItem->m_tDelay;
			nConflictCount ++;
		}
		else
		{
			FlightConflictSummaryData* pSummaryData = new FlightConflictSummaryData;
			pSummaryData->m_pParaFilter = pParaFilter;
			pSummaryData->m_strAreaName = strName;
			pSummaryData->m_nConflictCount = nConflictCount;
			pSummaryData->m_tTotal = tTotal;
			pSummaryData->m_tMax = tMax;
			pSummaryData->m_tMin = tMin;
			pSummaryData->m_statisticalTool = statisticalTool;
			m_vSummaryData.push_back(pSummaryData);

			nTimIdx = pItem->GetInIntervalIndex((CParameters*)m_pParameter);
			lTimeStart = m_pParameter->getStartTime().asSeconds() + m_pParameter->getInterval()*nTimIdx;
			lTimeEnd = m_pParameter->getStartTime().asSeconds() + m_pParameter->getInterval()*(nTimIdx+1);

			pParaFilter = new ReportDataFilter(lTimeStart,lTimeEnd, pItem->m_nConflictType,pItem->m_nAreaID,pItem->m_nLocationType,pItem->m_nOperationType,pItem->m_nActionType);
			strName = pItem->m_strAreaName;
			nConflictCount = 1;
			tTotal = pItem->m_tDelay;
			tMax = pItem->m_tDelay;
			tMin = tMax;
			
			CStatisticalTools<double> statisticalTool2;
			statisticalTool2.AddNewData(pItem->m_tDelay.asSeconds());
			statisticalTool = statisticalTool2;
			

		}
	}

	if(pItem != NULL)		//Add last data
	{
		FlightConflictSummaryData* pSummaryData = new FlightConflictSummaryData;
		pSummaryData->m_pParaFilter = pParaFilter;
		pSummaryData->m_strAreaName = strName;
		pSummaryData->m_nConflictCount = nConflictCount;
		pSummaryData->m_tTotal = tTotal;
		pSummaryData->m_tMax = tMax;
		pSummaryData->m_tMin = tMin;
		pSummaryData->m_statisticalTool = statisticalTool;
		m_vSummaryData.push_back(pSummaryData);

	}
}

void AirsideFlightConflictSummaryResult::InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC )
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);


	GetListColumns(m_lstColumns);
	

	if (piSHC)
		piSHC->ResetAll();

	for (int nCol = 0 ; nCol < (int)m_lstColumns.size(); ++nCol)
	{
		cxListCtrl.InsertColumn(nCol,m_lstColumns[nCol],LVCFMT_LEFT, 100);
		if (piSHC)
		{
			switch (nCol)
			{
			case 0: case 1: case 2: 
			case 3: case 4: case 5:
				piSHC->SetDataType(nCol, dtSTRING);
				break;
			case 6:
				piSHC->SetDataType(nCol, dtINT);
				break;
			
			default:
				piSHC->SetDataType(nCol, dtTIME);
				break;
			}
		}
	}
}

void AirsideFlightConflictSummaryResult::FillListContent( CXListCtrl& cxListCtrl)
{
	StatisticSummaryDataFromReportData();
	//add data to list ctrl
	int nCount = m_vSummaryData.size();
	for (int i = 0; i < nCount; i++)
	{
		FlightConflictSummaryData* pConflict = m_vSummaryData.at(i);
		ElapsedTime tTime = ElapsedTime(pConflict->m_pParaFilter->m_lTimeStart);
		ElapsedTime tTime2 = ElapsedTime(pConflict->m_pParaFilter->m_lTimeEnd -1L);
		CString strData;
		strData.Format("%s-%s",tTime.printTime(),tTime2.printTime());
		int nIdx = cxListCtrl.InsertItem(i, strData);

		CString strConType = "AC-AC";
		if (pConflict->m_pParaFilter->m_nConflictType ==1)
			strConType = "AC-GSE";
		cxListCtrl.SetItemText(nIdx,1, strConType);
		cxListCtrl.SetItemText(nIdx,2, pConflict->m_strAreaName);
		cxListCtrl.SetItemText(nIdx,3,  FlightConflict::LOCATIONTYPE[pConflict->m_pParaFilter->m_nLocationType]);
		cxListCtrl.SetItemText(nIdx,4,  FlightConflict::OPERATIONTYPE[pConflict->m_pParaFilter->m_nOperationType]);
		cxListCtrl.SetItemText(nIdx,5,  FlightConflict::ACTIONTYPE[pConflict->m_pParaFilter->m_nActionType]);
		strData.Format("%d", pConflict->m_nConflictCount);
		cxListCtrl.SetItemText(nIdx,6, strData);
		cxListCtrl.SetItemText(nIdx,7, pConflict->m_tTotal.printTime());
		cxListCtrl.SetItemText(nIdx,8, pConflict->m_tMin.printTime());
		cxListCtrl.SetItemText(nIdx,9, pConflict->m_tMax.printTime());

		tTime = ElapsedTime(pConflict->m_statisticalTool.GetPercentile(25));
		cxListCtrl.SetItemText(nIdx,10, tTime.printTime());
		tTime = ElapsedTime(pConflict->m_statisticalTool.GetPercentile(50));
		cxListCtrl.SetItemText(nIdx,11, tTime.printTime());
		tTime = ElapsedTime(pConflict->m_statisticalTool.GetPercentile(75));
		cxListCtrl.SetItemText(nIdx,12, tTime.printTime());

		tTime = ElapsedTime(pConflict->m_statisticalTool.GetPercentile(1));
		cxListCtrl.SetItemText(nIdx,13, tTime.printTime());
		tTime = ElapsedTime(pConflict->m_statisticalTool.GetPercentile(5));
		cxListCtrl.SetItemText(nIdx,14, tTime.printTime());
		tTime = ElapsedTime(pConflict->m_statisticalTool.GetPercentile(10));
		cxListCtrl.SetItemText(nIdx,15, tTime.printTime());

		tTime = ElapsedTime(pConflict->m_statisticalTool.GetPercentile(90));
		cxListCtrl.SetItemText(nIdx,16, tTime.printTime());
		tTime = ElapsedTime(pConflict->m_statisticalTool.GetPercentile(95));
		cxListCtrl.SetItemText(nIdx,17, tTime.printTime());
		tTime = ElapsedTime(pConflict->m_statisticalTool.GetPercentile(99));
		cxListCtrl.SetItemText(nIdx,18, tTime.printTime());
		tTime = ElapsedTime(pConflict->m_statisticalTool.GetSigma());
		cxListCtrl.SetItemText(nIdx,19, tTime.printTime());
	}

}

void AirsideFlightConflictSummaryResult::RefreshReport(CParameters * parameter )
{
	m_pParameter = (AirsideFlightConflictPara*)parameter;
	if(m_pParameter  == NULL)
		return;

	if(m_pChartResult != NULL)
		delete m_pChartResult;

	m_pChartResult = NULL;

	m_pChartResult = new FlightConflictSummaryChartResult;

	((FlightConflictSummaryChartResult*)m_pChartResult)->GenerateResult(m_vSummaryData);
}

BOOL AirsideFlightConflictSummaryResult::ReadReportData( ArctermFile& _file )
{
	AirsideFlightConflictBaseResult::ReadReportData(_file);
	//StatisticSummaryDataFromReportData();
	return TRUE;
}

void AirsideFlightConflictSummaryResult::GetListColumns( std::vector<CString>& lstColumns )
{
	lstColumns.clear();

	lstColumns.push_back(_T("Interval"));
	lstColumns.push_back(_T("Type"));
	lstColumns.push_back(_T("Area/Sector"));
	lstColumns.push_back(_T("Location"));
	lstColumns.push_back(_T("Operation"));
	lstColumns.push_back(_T("Action"));
	lstColumns.push_back(_T("Number of conflicts"));
	lstColumns.push_back(_T("Total delay(hh:mm:ss)"));
	lstColumns.push_back(_T("Min delay(hh:mm:ss)"));
	lstColumns.push_back(_T("Max delay(hh:mm:ss)"));

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
