#include "StdAfx.h"
#include ".\airsidecontrollerworkloadresult.h"
#include "AirsideControllerWorkloadParam.h"
#include "../Results/AirsideFlightLog.h"
#include "../Results/AirsideReportLog.h"
#include "CARC3DChart.h"


using namespace AirsideControllerWorkload;

//////////////////////////////////////////////////////////////////////////
//Result Item
AirsideControllerWorkload::CResultItem::CResultItem()
{
	m_nToatalIndex = 0;
	m_nPushBacks = 0;
	m_nTaxiStart = 0;
	m_nTakeOff_NoConflict = 0;
	m_nTakeOff_Conflict = 0;
	m_nLanding_NoConflict = 0;
	m_nLanding_Conflict = 0;
	m_nGoArround = 0;
	m_nVacateRunway = 0;
	m_nCrossActive = 0;
	m_nHandOffGround = 0;
	m_nHandOffAir = 0;
	m_nAltitudeChange = 0;
	m_nVectorAir = 0;
	m_nHolding = 0;
	m_nDirectRoute_Air = 0;
	m_nRerouteGround = 0;
	m_nContribution = 0;
	m_nExtendedDownwind = 0;
}

AirsideControllerWorkload::CResultItem::~CResultItem()
{

}


//////////////////////////////////////////////////////////////////////////

CReportResult::CReportResult(void)
{

}

CReportResult::~CReportResult(void)
{


}

void AirsideControllerWorkload::CReportResult::GenerateResult( CBGetLogFilePath pCBGetLogFilePath, CAirsideControllerWorkloadParam *pParam )
{
	CString strDescFilepath = (*pCBGetLogFilePath)(AirsideDescFile);
	CString strEventFilePath = (*pCBGetLogFilePath)(AirsideEventFile);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	EventLog<AirsideFlightEventStruct> airsideFlightEventLog;
	AirsideFlightLog fltLog;
	fltLog.SetEventLog(&airsideFlightEventLog);
	fltLog.LoadDataIfNecessary(strDescFilepath);
	fltLog.LoadEventDataIfNecessary(strEventFilePath);


	//read new log
	strDescFilepath = (*pCBGetLogFilePath)(AirsideFlightDescFileReport);
	strEventFilePath = (*pCBGetLogFilePath)(AirsideFlightEventFileReport);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	ElapsedTime eStartTime = pParam->getStartTime();
	ElapsedTime eEndTime = pParam->getEndTime();

	GenerateIntervalTime(pParam);

	AirsideReportLog::AirsideFlightLogData mFlightLogData;
	mFlightLogData.OpenInput(strDescFilepath.GetString() ,strEventFilePath.GetString());
	int nCount = mFlightLogData.getItemCount();


	
}
void AirsideControllerWorkload::CReportResult::GenerateIntervalTime(CAirsideControllerWorkloadParam *pParam)
{
	if (pParam == NULL)
		return;

	m_vResult.clear();
	int intervalSize = 0 ;
	ElapsedTime IntervalTime ;
	IntervalTime = (pParam->getEndTime() - pParam->getStartTime());
	intervalSize = IntervalTime.asSeconds() / pParam->getInterval();

	long lUserIntervalTime = pParam->getInterval();
	long lStartTime = pParam->getStartTime().asSeconds();

	for (long nIneterVer = 0 ; nIneterVer < intervalSize ;nIneterVer++)
	{
		CResultItem resultItem;
		resultItem.m_eStartTime = lStartTime + nIneterVer*lUserIntervalTime;
		resultItem.m_eEndTime = resultItem.m_eStartTime + lUserIntervalTime;
		m_vResult.push_back(resultItem);
	}
}

void AirsideControllerWorkload::CReportResult::FillListContent( CXListCtrl& cxListCtrl, CAirsideControllerWorkloadParam *parameter )
{
	int nCount = m_vResult.size();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		CResultItem& result = m_vResult[nItem];



		{

			CString strInterval ;

			strInterval.Format(_T("%s - %s"),result.m_eStartTime.PrintDateTime(), result.m_eEndTime.PrintDateTime());
			cxListCtrl.InsertItem(nItem,strInterval);
		}
		int nIndex = 1;
		//total index
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nToatalIndex));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nPushBacks));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nPushBacks));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nTaxiStart));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nTaxiStart));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nTakeOff_NoConflict));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nTakeOff_NoConflict));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nTakeOff_Conflict));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nTakeOff_Conflict));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nLanding_NoConflict));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nLanding_NoConflict));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nLanding_Conflict));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nLanding_Conflict));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nGoArround));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nGoArround));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nVacateRunway));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nVacateRunway));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nCrossActive));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nCrossActive));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nHandOffGround));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nHandOffGround));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nHandOffAir));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nHandOffAir));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nAltitudeChange));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nAltitudeChange));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nVectorAir));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nVectorAir));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nHolding));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nHolding));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nDirectRoute_Air));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nDirectRoute_Air));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nRerouteGround));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nRerouteGround));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nContribution));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nContribution));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nExtendedDownwind));

		nIndex += 1;
		cxListCtrl.SetItemText(nItem, nIndex, formatInt(result.m_nExtendedDownwind));
	}
}

CString AirsideControllerWorkload::CReportResult::formatInt( int nValue ) const
{
	CString strText;
	strText.Format(_T("%d"),nValue);
	return strText;

}

void AirsideControllerWorkload::CReportResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	
	CAirsideControllerWorkloadParam *pParam = (CAirsideControllerWorkloadParam *)pParameter;
	int intervalSize = 0 ;
	ElapsedTime IntervalTime ;
	IntervalTime = (pParameter->getEndTime() - pParameter->getStartTime()) ;
	intervalSize = IntervalTime.asSeconds() / pParameter->getInterval() ;

	long lUserIntervalTime = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(lUserIntervalTime);

	CString timeInterval ;
	ElapsedTime startTime = pParameter->getStartTime();
	ElapsedTime endtime ;
	
	//time label
	std::vector<CString> vTimeRange;
	std::vector<double> vdValue;

	for (int nItem = 0;nItem < static_cast<int>(m_vResult.size()); ++nItem)
	{
		CResultItem& item = m_vResult[nItem];
		CString strText;
		strText.Format(_T("%s - %s"), item.m_eEndTime.PrintDateTime(), item.m_eStartTime.PrintDateTime());
		vTimeRange.push_back(strText);
		vdValue.push_back(0.0);
	}
	int nSubType =  pParameter->getSubType();
	CString strYTitle = _T("Total Index");
	if (nSubType >= 0 && nSubType < CAirsideControllerWorkloadParam::weight_count)
		strYTitle = pParam->getWeightName((CAirsideControllerWorkloadParam::enumWeight)nSubType);
	else
		strYTitle = _T("Total Index");



	CString strLabel = _T("");
	C2DChartData c2dGraphData;


	c2dGraphData.m_strYtitle = strYTitle;
	c2dGraphData.m_strXtitle = _T("Data and Time");	
	c2dGraphData.m_vrXTickTitle = vTimeRange;

	c2dGraphData.m_strChartTitle = strYTitle;

	//set footer
	c2dGraphData.m_strFooter = strYTitle;

	//std::vector<CResultItem >::iterator iter = m_vResult.begin();
	//for (; iter!=m_vResult.end(); ++iter)
	//{
		CString strLegend = _T("");
		strLegend = strYTitle;
		c2dGraphData.m_vrLegend.push_back(strLegend);

		c2dGraphData.m_vr2DChartData.push_back(vdValue);
	//}
	chartWnd.DrawChart(c2dGraphData);
}

CString AirsideControllerWorkload::CReportResult::FormartInt( int nInt ) const
{
	CString strText;
	strText.Format(_T("%d"),nInt);
	return strText;
}









