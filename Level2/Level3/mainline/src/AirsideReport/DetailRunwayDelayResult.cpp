#include "StdAfx.h"
#include ".\detailrunwaydelayresult.h"
#include "../Engine/Airside/CommonInSim.h"
#include "AirsideFlightRunwayDelayData.h"
#include "AirsideFlightRunwayDelayReportPara.h"
#include "DetailDelayChartResult.h"

DetailRunwayDelayResult::DetailRunwayDelayResult(CParameters* parameter)
:AirsideFlightRunwayDelayBaseResult(parameter)
{
}

DetailRunwayDelayResult::~DetailRunwayDelayResult(void)
{
}
void DetailRunwayDelayResult::InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC )
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
			case 5: 
				piSHC->SetDataType(nCol, dtTIME);
				break;
			case 6: case 7: case 8: case 10: case 12: case 14:
				piSHC->SetDataType(nCol, dtINT);
				break;
			default:
				piSHC->SetDataType(nCol, dtSTRING);
				break;
			}
		}
	}
}

void DetailRunwayDelayResult::FillListContent( CXListCtrl& cxListCtrl)
{
	if (m_pReportData == NULL)
		return;
	//add data to list ctrl
	int nCount = m_pReportData->m_vDataItemList.size();
	for (int i = 0; i < nCount; i++)
	{
		AirsideFlightRunwayDelayData::RunwayDelayItem* pLogItem = m_pReportData->m_vDataItemList.at(i);
		if (pLogItem->m_lTotalDelay == 0l)
			continue;

		int nIdx = cxListCtrl.InsertItem(i, pLogItem->m_strRunwayMark);
		cxListCtrl.SetItemText(nIdx,1,pLogItem->m_strACName);
		cxListCtrl.SetItemText(nIdx,2,pLogItem->m_sACType);

		CString strOp = _T("Landing");
		if (pLogItem->m_OperationType >0)
			strOp = _T("Takeoff");
		cxListCtrl.SetItemText(nIdx,3,strOp);
		cxListCtrl.SetItemText(nIdx,4,FlightRunwayDelay::POSITIONTYPE[pLogItem->m_PositionType]);
		cxListCtrl.SetItemText(nIdx,5,pLogItem->m_tStartTime.printTime());
		CString strTime;
		strTime.Format("%d", pLogItem->m_lExpectOpTime);
		cxListCtrl.SetItemText(nIdx,6, strTime);
		strTime.Format("%d", pLogItem->m_lRealityOpTime);
		cxListCtrl.SetItemText(nIdx,7, strTime);
		strTime.Format("%d", pLogItem->m_lTotalDelay);
		cxListCtrl.SetItemText(nIdx,8, strTime);
		int nReason = pLogItem->m_vReasonDetail.size() < 3 ? pLogItem->m_vReasonDetail.size() :3;
		int nCol = 8;
		for (int j = 0; j < nReason; j++)
		{
			AirsideFlightRunwayDelayData::DETAILINFO info = pLogItem->m_vReasonDetail.at(j);
			cxListCtrl.SetItemText(nIdx, ++nCol, info.second);
			strTime.Format("%d", info.first);
			cxListCtrl.SetItemText(nIdx, ++nCol, strTime);
		}
	}

}

void DetailRunwayDelayResult::RefreshReport(CParameters * parameter )
{
	m_pParameter = (AirsideFlightRunwayDelayReportPara*)parameter;
	if(m_pParameter  == NULL)
		return;

	if(m_pChartResult != NULL)
		delete m_pChartResult;

	m_pChartResult = NULL;
	if(m_pParameter->getSubType() == 0 )
		m_pParameter->setSubType(1);

	m_pChartResult = new DetailDelayChartResult;

	((DetailDelayChartResult*)m_pChartResult)->GenerateResult(m_pReportData);

}

void DetailRunwayDelayResult::GetListColumns( std::vector<CString>& lstColumns )
{
	lstColumns.clear();

	lstColumns.push_back(_T("Runway"));
	lstColumns.push_back(_T("AC ID"));
	lstColumns.push_back(_T("Flight type"));
	lstColumns.push_back(_T("Operation"));
	lstColumns.push_back(_T("Position"));
	lstColumns.push_back(_T("Start time(hh:mm:ss)"));
	lstColumns.push_back(_T("Expected op time(secs)"));
	lstColumns.push_back(_T("Real op time(secs)"));
	lstColumns.push_back(_T("Total delay time(secs)"));
	lstColumns.push_back(_T("Reason1"));
	lstColumns.push_back(_T("Delay1(secs)"));
	lstColumns.push_back(_T("Reason2"));
	lstColumns.push_back(_T("Delay2(secs)"));
	lstColumns.push_back(_T("Reason3"));
	lstColumns.push_back(_T("Delay3(secs)"));
}