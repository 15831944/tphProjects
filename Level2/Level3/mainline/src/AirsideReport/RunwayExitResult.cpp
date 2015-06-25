#include "StdAfx.h"
#include ".\runwayexitresult.h"
#include "RunwayExitParameter.h"
#include "RunwayExitReport.h"
CRunwayExitResult::CRunwayExitResult(void):CAirsideReportBaseResult()
{
}

CRunwayExitResult::~CRunwayExitResult(void)
{
}
void CRunwayDetailExitResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if(pParameter == NULL)
		return ;
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle.Format(_T("Runway Exit Detail Delay"))  ;

	c2dGraphData.m_strYtitle = _T("Number of aircrafts");
	c2dGraphData.m_strXtitle = _T("Date and time") ;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "),_T("Runway Exit Detail Delay"), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;

	int intervalSize = 0 ;
	ElapsedTime IntervalTime ;
	IntervalTime = (pParameter->getEndTime() - pParameter->getStartTime()) ;
	intervalSize = IntervalTime.asSeconds() / pParameter->getInterval() ;

	CTimeIntervalFlightData* PTimeIntervalData = NULL ;
	IntervalTime = pParameter->getStartTime() ;
	CString timeInterval ;
	ElapsedTime startTime = pParameter->getStartTime();
	ElapsedTime endtime ;
	for (int i = 0 ; i < intervalSize ;i++)
	{
		endtime = startTime + ElapsedTime(pParameter->getInterval()) ;
		timeInterval.Format(_T("%s-%s"),startTime.printTime(),endtime.printTime()) ;
		startTime = endtime ;
		c2dGraphData.m_vrXTickTitle.push_back(timeInterval) ;
	}

	CRunwayExitRepDataItem* RunwayExitRepDataItem = NULL ;
	CString Strval ;
	int size = (int)( m_ReportBaseData->GetData()->size() );
	for (int i = 0 ; i < size; i++)
	{
		RunwayExitRepDataItem = m_ReportBaseData->GetData()->at(i) ;
		Strval.Format(_T("%s %s"),RunwayExitRepDataItem->m_RunwayName,RunwayExitRepDataItem->m_RunwayExitName) ;
		c2dGraphData.m_vrLegend.push_back(Strval);

		std::vector<double> segmentData ;

		CTimeIntervalFlightData* TimerIntervalData = NULL ;
		for (int j = 0 ; j < (int)RunwayExitRepDataItem->GetData()->size() ;j++)
		{
			TimerIntervalData = RunwayExitRepDataItem->GetData()->at(j) ;
			segmentData.push_back((long)TimerIntervalData->m_FlightData.size()) ;
		}
		c2dGraphData.m_vr2DChartData.push_back(segmentData);
	}
	chartWnd.DrawChart(c2dGraphData);
}


void CRunwaySummaryExitResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if(pParameter == NULL)
		return ;
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle.Format(_T("Runway Exit Summary Delay"))  ;

	c2dGraphData.m_strYtitle = _T("Number of aircrafts");
	c2dGraphData.m_strXtitle = _T("Date and time") ;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "),_T("Runway Exit Summary Delay"), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;

	c2dGraphData.m_vrLegend.push_back(_T("Min")) ;
	c2dGraphData.m_vrLegend.push_back(_T("Mean")) ;
	c2dGraphData.m_vrLegend.push_back(_T("Max")) ;
	c2dGraphData.m_vrLegend.push_back(_T("Q1")) ;
	c2dGraphData.m_vrLegend.push_back(_T("Q2")) ;
	c2dGraphData.m_vrLegend.push_back(_T("Q3")) ;

	c2dGraphData.m_vrLegend.push_back(_T("P1")) ;
	c2dGraphData.m_vrLegend.push_back(_T("P5")) ;
	c2dGraphData.m_vrLegend.push_back(_T("P10")) ;
	c2dGraphData.m_vrLegend.push_back(_T("P90")) ;
	c2dGraphData.m_vrLegend.push_back(_T("P95")) ;
	c2dGraphData.m_vrLegend.push_back(_T("P99")) ;

	std::vector<double> m_Mins ;
	std::vector<double> m_Means ;
	std::vector<double> m_Maxs ;
	std::vector<double> m_Q1 ;
	std::vector<double> m_Q2 ;
	std::vector<double> m_Q3 ;
	std::vector<double> m_P1 ;
	std::vector<double> m_p5 ;
	std::vector<double> m_P10 ;
	std::vector<double> m_P90 ;
	std::vector<double> m_P95 ;
	std::vector<double> m_P99 ;
	for (int i = 0 ; i < (int)m_SummaryData->size() ;i++)
	{
		CRunwayExitSummaryDataItem dataItem ;
		dataItem = m_SummaryData->at(i) ;
		CString strval ;
		strval.Format(_T("%s %s"),dataItem.m_Runway,dataItem.m_ExitName) ;
		c2dGraphData.m_vrXTickTitle.push_back(strval) ;

		m_Mins.push_back(dataItem.m_MinUsager) ;
		m_Means.push_back(dataItem.m_MeanUsager);
		m_Maxs.push_back(dataItem.m_MaxUsager) ;
		m_Q1.push_back(dataItem.m_Q1) ;
		m_Q2.push_back(dataItem.m_Q2) ;
		m_Q3.push_back(dataItem.m_Q3) ;
		m_P1.push_back(dataItem.m_P1) ;
		m_p5.push_back(dataItem.m_p5) ;
		m_P10.push_back(dataItem.m_p10) ;
		m_P90.push_back(dataItem.m_p90) ;
		m_P95.push_back(dataItem.m_P95) ;
		m_P99.push_back(dataItem.m_p99) ;
	}

	c2dGraphData.m_vr2DChartData.push_back(m_Mins) ;
	c2dGraphData.m_vr2DChartData.push_back(m_Means) ;
	c2dGraphData.m_vr2DChartData.push_back(m_Maxs) ;
	c2dGraphData.m_vr2DChartData.push_back(m_Q1) ;
	c2dGraphData.m_vr2DChartData.push_back(m_Q2) ;
	c2dGraphData.m_vr2DChartData.push_back(m_Q3) ;
	c2dGraphData.m_vr2DChartData.push_back(m_P1) ;
	c2dGraphData.m_vr2DChartData.push_back(m_p5) ;
	c2dGraphData.m_vr2DChartData.push_back(m_P10) ;
	c2dGraphData.m_vr2DChartData.push_back(m_P90) ;
	c2dGraphData.m_vr2DChartData.push_back(m_P95) ;
	c2dGraphData.m_vr2DChartData.push_back(m_P95) ;

	chartWnd.DrawChart(c2dGraphData);
}