#include "StdAfx.h"
#include ".\taxiwaydelayreportresult.h"

CTaxiwayDelayReportResult::CTaxiwayDelayReportResult(CTaxiwayDelayParameters* _parmeter):CAirsideReportBaseResult(),m_parmeter(_parmeter) 
{
}

CTaxiwayDelayReportResult::~CTaxiwayDelayReportResult(void)
{
}


CTaxiwayDelayDetailReportResult::CTaxiDelayReportData* CTaxiwayDelayDetailReportResult::FindData(CTaxiwayDelayReportDetailDataItem* _delayData)
{
	for (int i= 0 ; i < (int)m_GraphData.size() ;i++)
	{
		if( _delayData->m_FromIntersection.CompareNoCase(m_GraphData[i]->m_FromNodeName)==0 &&
			_delayData->m_ToIntersection.CompareNoCase(m_GraphData[i]->m_ToNodeName)==0 && 
			_delayData->m_TaxiwayName.CompareNoCase(m_GraphData[i]->m_TaxiwayName) == 0)
			return m_GraphData[i] ;
	}
	CTaxiDelayReportData* delayreportData = new CTaxiDelayReportData(m_parmeter->getStartTime(),m_Intervals,ElapsedTime(m_parmeter->getInterval())) ;
	delayreportData->m_FromNodeName = _delayData->m_FromIntersection ;
	delayreportData->m_ToNodeName = _delayData->m_ToIntersection ;
	delayreportData->m_TaxiwayName = _delayData->m_TaxiwayName ;
	m_GraphData.push_back(delayreportData) ;
	return delayreportData ;
}
void CTaxiwayDelayDetailReportResult::AddDelayData(CTaxiwayDelayReportDetailDataItem* _delayData)
{
	if(_delayData == NULL || _delayData->m_DelayTime.asSeconds() == 0)
		return;

	FindData(_delayData)->AddDelayDataToDate(_delayData) ;
}
void CTaxiwayDelayDetailReportResult::GenerateResult()
{
	m_Intervals = (	m_parmeter->getEndTime().asSeconds() - m_parmeter->getStartTime().asSeconds() ) / m_parmeter->getInterval() ;
	CTaxiwayDelayReportDetailDataItem* _delayDataItem = NULL ;
	for (int i = 0 ; i < (int)m_DetailReport->size() ;i++)
	{
		_delayDataItem = m_DetailReport->at(i) ;
		AddDelayData(_delayDataItem) ;
	}
}
void CTaxiwayDelayDetailReportResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if(pParameter == NULL)
		return ;
	C2DChartData c2dGraphData;
	CString strLabel = _T("");
	CTaxiwayDelayParameters* taxiwayDelayPar = (CTaxiwayDelayParameters*)pParameter ;
	if(taxiwayDelayPar->getSubType() == 0)
		c2dGraphData.m_strChartTitle.Format(_T("Detail Taxiway Delay(%s)"),_T("Arrival"))  ;
	else
		c2dGraphData.m_strChartTitle.Format(_T("Detail Taxiway Delay(%s)"),_T("Departure"))  ;
	c2dGraphData.m_strYtitle = _T("Delay(Second)");
	c2dGraphData.m_strXtitle = _T("Date and time delay") ;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "),_T("Detail Taxiway Delay"), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;

	CTaxiDelayReportData* delayIntervalItem = NULL ;
	CString timeInterval ;
	ElapsedTime startTime = m_parmeter->getStartTime();
	ElapsedTime endtime ;
	for (int i = 0 ; i < m_Intervals ;i++)
	{
		endtime = startTime + ElapsedTime(m_parmeter->getInterval()) ;
		timeInterval.Format(_T("%s-%s"),startTime.printTime(),endtime.printTime()) ;
		startTime = endtime ;
		c2dGraphData.m_vrXTickTitle.push_back(timeInterval) ;
	}

	CTaxiDelayReportData* TaxiDelayRep = NULL ;
	CString taxiwayName ;
	for (int i = 0 ; i < (int)m_GraphData.size() ;i++)
	{
		TaxiDelayRep = m_GraphData[i] ;
        taxiwayName.Format(_T("%s %s-%s"),TaxiDelayRep->m_TaxiwayName,TaxiDelayRep->m_FromNodeName,TaxiDelayRep->m_ToNodeName) ;

		c2dGraphData.m_vrLegend.push_back(taxiwayName);
		
		vector<double> vSegmentData;
		double m_val ;
		CSegmentDelay* PSegment = NULL ;
		for (int j = 0 ; j < (int)TaxiDelayRep->m_Data.size() ;j++)
		{
			PSegment = TaxiDelayRep->m_Data[j] ;
			if(m_parmeter->getSubType() == CTaxiwayDelayParameters::ARRIVAL)
				m_val = PSegment->m_ArrivalDelay.asSeconds() ;
			if(m_parmeter->getSubType() == CTaxiwayDelayParameters::DEPARTURE)
				m_val = PSegment->m_DepDelay.asSeconds() ;
			if(m_parmeter->getSubType() == CTaxiwayDelayParameters::TOTAL)
				m_val = PSegment->m_DepDelay.asSeconds() + PSegment->m_ArrivalDelay.asSeconds();
			vSegmentData.push_back(m_val) ;
		}
		c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
	}
	chartWnd.DrawChart(c2dGraphData);
}
void CTaxiwayDelayDetailReportResult::ClearGraphData() 
{
	for (int i = 0 ; i < (int)m_GraphData.size() ;i++)
	{
		delete m_GraphData[i] ;
	}
	m_GraphData.clear() ;
}
//////////////////////////////////////////////////////////////////////////
//summary report result 
//////////////////////////////////////////////////////////////////////////
CTaxiwayDelaySummaryReportResult::CTaxiwayDelaySummaryReportResult(std::vector<CTaxiwayDelayReportSummaryDataItem*>* _dataset ,CTaxiwayDelayParameters* _parmeter)
:m_SummaryData(_dataset),CTaxiwayDelayReportResult(_parmeter)
{

}

void CTaxiwayDelaySummaryReportResult::GenerateResult()
{

}
void CTaxiwayDelaySummaryReportResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if(pParameter == NULL)
		return ;
	C2DChartData c2dGraphData;
	CString strLabel = _T("");
	CTaxiwayDelayParameters* taxiwayDelayPar = (CTaxiwayDelayParameters*)pParameter ;
	
	c2dGraphData.m_strChartTitle.Format(_T("%s"),_T("Summary Taxiway Delays"))  ;
	
	c2dGraphData.m_strYtitle = _T("Delay(Second)");
	c2dGraphData.m_strXtitle = _T("Taxiway intersection") ;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "),_T("Summary Taxiway Delay"), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;
	
	CTaxiwayDelayReportSummaryDataItem* summaryDataItem = NULL ;
	CString XEntryName ;
	for (int i = 0 ;i < (int)m_SummaryData->size() ;i++)
	{
		summaryDataItem = m_SummaryData->at(i) ;
		XEntryName.Format(_T("indx-%d"),i+1) ;
		c2dGraphData.m_vrXTickTitle.push_back(XEntryName) ;

	}

	c2dGraphData.m_vrLegend.push_back(_T("Max")) ;
	c2dGraphData.m_vrLegend.push_back(_T("Mean")) ;
	c2dGraphData.m_vrLegend.push_back(_T("Min")) ;
	c2dGraphData.m_vrLegend.push_back(_T("Q1")) ;
	c2dGraphData.m_vrLegend.push_back(_T("Q2")) ;
	c2dGraphData.m_vrLegend.push_back(_T("Q3")) ;
	c2dGraphData.m_vrLegend.push_back(_T("P1")) ;
	c2dGraphData.m_vrLegend.push_back(_T("P5")) ;
	c2dGraphData.m_vrLegend.push_back(_T("P90")) ;
	c2dGraphData.m_vrLegend.push_back(_T("P95")) ;
	c2dGraphData.m_vrLegend.push_back(_T("P99")) ;

	std::vector<double> m_Max;
	std::vector<double> m_Mean;
	std::vector<double> m_Min ;
	std::vector<double> m_Q1 ;
	std::vector<double> m_Q2 ;
	std::vector<double> m_Q3 ;
	std::vector<double> m_P1 ;
	std::vector<double> m_P5 ;
	std::vector<double> m_P90 ;
	std::vector<double> m_P95 ;
	std::vector<double> m_P99 ;

	for (int i = 0 ; i < (int)m_SummaryData->size() ;i++)
	{
		summaryDataItem = m_SummaryData->at(i) ;
		m_Max.push_back(summaryDataItem->m_MaxDelay.asSeconds()) ;
		m_Mean.push_back(summaryDataItem->m_MeanDelay.asSeconds()) ;
		m_Min.push_back(summaryDataItem->m_MinDelay.asSeconds()) ;
		m_Q1.push_back(summaryDataItem->m_Q1.asSeconds()) ;
		m_Q2.push_back(summaryDataItem->m_Q2.asSeconds()) ;
		m_Q3.push_back(summaryDataItem->m_Q3.asSeconds()) ;
		m_P1.push_back(summaryDataItem->m_P1.asSeconds()) ;
		m_P5.push_back(summaryDataItem->m_P5.asSeconds()) ;
		m_P90.push_back(summaryDataItem->m_P90.asSeconds());
		m_P95.push_back(summaryDataItem->m_P95.asSeconds());
		m_P99.push_back(summaryDataItem->m_P99.asSeconds()) ;
	}
	c2dGraphData.m_vr2DChartData.push_back(m_Max) ;
	c2dGraphData.m_vr2DChartData.push_back(m_Mean) ;
	c2dGraphData.m_vr2DChartData.push_back(m_Min) ;
	c2dGraphData.m_vr2DChartData.push_back(m_Q1) ;
	c2dGraphData.m_vr2DChartData.push_back(m_Q2) ;
	c2dGraphData.m_vr2DChartData.push_back(m_Q3) ;
	c2dGraphData.m_vr2DChartData.push_back(m_P1) ;
	c2dGraphData.m_vr2DChartData.push_back(m_P5) ;
	c2dGraphData.m_vr2DChartData.push_back(m_P90) ;
	c2dGraphData.m_vr2DChartData.push_back(m_P95) ;
	c2dGraphData.m_vr2DChartData.push_back(m_P99) ;
	chartWnd.DrawChart(c2dGraphData);
}