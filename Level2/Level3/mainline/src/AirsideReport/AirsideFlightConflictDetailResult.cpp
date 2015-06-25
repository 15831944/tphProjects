#include "StdAfx.h"
#include ".\airsideflightconflictdetailresult.h"
#include "FlightConflictReportData.h"
#include "AirsideFlightConflictPara.h"
#include "AirsideFlightConflictReport.h"
#include "FltConflictChartResultByDistOfConfPerAC.h"
#include "FltConflictChartResultByActionAndTimeOfDay.h"
#include "FltConflictChartResultByAreaAndTimeOfDay.h"
#include "FltConflictChartResultByLocationAndTimeOfDay.h"
#include "FltConflictChartResultByOperationAndTimeOfDay.h"
#include "FltConflictChartResultByTimeOfDay.h"
#include "../Engine/Airside/CommonInSim.h"

AirsideFlightConflictDetailResult::AirsideFlightConflictDetailResult(CParameters* parameter)
:AirsideFlightConflictBaseResult(parameter)
{
}

AirsideFlightConflictDetailResult::~AirsideFlightConflictDetailResult(void)
{
}

void AirsideFlightConflictDetailResult::InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC )
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
			case 1: case 4:
				piSHC->SetDataType(nCol, dtTIME);
				break;
			default:
				piSHC->SetDataType(nCol, dtSTRING);
				break;
			}
		}
	}
}

void AirsideFlightConflictDetailResult::FillListContent( CXListCtrl& cxListCtrl)
{
	if (m_pReportData == NULL)
		return;
	//add data to list ctrl
	int nCount = m_pReportData->m_vDataItemList.size();
	for (int i = 0; i < nCount; i++)
	{
		FlightConflictReportData::ConflictDataItem* pLogItem = m_pReportData->m_vDataItemList.at(i);
		int nIdx = cxListCtrl.InsertItem(i, pLogItem->m_strACName);
		CString strTime = pLogItem->m_tTime.printTime();
		cxListCtrl.SetItemText(nIdx,1,strTime);
		cxListCtrl.SetItemText(nIdx,2,pLogItem->m_str2ndPartyName);
		cxListCtrl.SetItemText(nIdx,3, FlightConflict::ACTIONTYPE[pLogItem->m_nActionType]);
		strTime = pLogItem->m_tDelay.printTime();
		cxListCtrl.SetItemText(nIdx,4,strTime);
		cxListCtrl.SetItemText(nIdx,5,pLogItem->m_strAreaName);
		cxListCtrl.SetItemText(nIdx,6, FlightConflict::LOCATIONTYPE[pLogItem->m_nLocationType]);
		cxListCtrl.SetItemText(nIdx,7, FlightConflict::OPERATIONTYPE[pLogItem->m_nOperationType]);
	}

}

void AirsideFlightConflictDetailResult::RefreshReport(CParameters * parameter )
{
	m_pParameter = (AirsideFlightConflictPara*)parameter;
	if(m_pParameter  == NULL)
		return;

	if(m_pChartResult != NULL)
		delete m_pChartResult;

	m_pChartResult = NULL;
	if(m_pParameter->getSubType() == 0 )
		m_pParameter->setSubType(AirsideFlightConflictReport::ChartType_Detail_DistributionOfPerAC);

	if(m_pParameter->getSubType() == AirsideFlightConflictReport::ChartType_Detail_DistributionOfPerAC)
	{
		m_pChartResult = new FltConflictChartResultByDistOfConfPerAC;

		((FltConflictChartResultByDistOfConfPerAC*)m_pChartResult)->GenerateResult(m_pReportData);

	}
	else if(m_pParameter->getSubType() == AirsideFlightConflictReport::ChartType_Detail_ByTimeOfDay)
	{
		m_pChartResult = new FltConflictChartResultByTimeOfDay;

		((FltConflictChartResultByTimeOfDay*)m_pChartResult)->GenerateResult(m_pReportData);
	}
	else if(m_pParameter->getSubType() == AirsideFlightConflictReport::ChartType_Detail_ByAreaAndTimeOfDay)
	{
		m_pChartResult = new FltConflictChartResultByAreaAndTimeOfDay;

		((FltConflictChartResultByAreaAndTimeOfDay*)m_pChartResult)->GenerateResult(m_pReportData);
	}
	else if(m_pParameter->getSubType() == AirsideFlightConflictReport::ChartType_Detail_ByActionAndTimeOfDay)
	{
		m_pChartResult = new FltConflictChartResultByActionAndTimeOfDay;

		((FltConflictChartResultByActionAndTimeOfDay*)m_pChartResult)->GenerateResult(m_pReportData);
	}
	else if(m_pParameter->getSubType() == AirsideFlightConflictReport::ChartType_Detail_ByOperationAndTimeOfDay)
	{
		m_pChartResult = new FltConflictChartResultByOperationAndTimeOfDay;

		((FltConflictChartResultByOperationAndTimeOfDay*)m_pChartResult)->GenerateResult(m_pReportData);
	}
	else if(m_pParameter->getSubType() == AirsideFlightConflictReport::ChartType_Detail_ByLocationAndTimeOfDay)
	{
		m_pChartResult = new FltConflictChartResultByLocationAndTimeOfDay;

		((FltConflictChartResultByLocationAndTimeOfDay*)m_pChartResult)->GenerateResult(m_pReportData);
	}
	else
	{

		ASSERT(0);
		//should not get here
	}

}

void AirsideFlightConflictDetailResult::GetListColumns( std::vector<CString>& lstColumns )
{
	lstColumns.clear();

	lstColumns.push_back(_T("AC1"));
	lstColumns.push_back(_T("Time(hh:mm:ss)"));
	lstColumns.push_back(_T("2nd party"));
	lstColumns.push_back(_T("Action"));
	lstColumns.push_back(_T("Delay(hh:mm:ss)"));
	lstColumns.push_back(_T("Area/Sector"));
	lstColumns.push_back(_T("Location"));
	lstColumns.push_back(_T("Operation"));

}
