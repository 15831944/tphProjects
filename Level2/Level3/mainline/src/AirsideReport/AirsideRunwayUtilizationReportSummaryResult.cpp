#include "StdAfx.h"
#include ".\airsiderunwayutilizationreportsummaryresult.h"
#include "../Reports/StatisticalTools.h"
#include "AirsideRunwayUtilizationReportBaseResult.h"
#include "AirsideRunwayUtilizationReport.h"

using namespace AirsideRunwayUtilizationReport;
CSummaryResult::CSummaryResult(void)
{

}

CSummaryResult::~CSummaryResult(void)
{
	Clear();
}

void CSummaryResult::GenerateResult( CParameters *pParameter )
{
	CAirsideRunwayUtilizationReportBaseResult::GenerateResult(pParameter);
	
	Clear();
	//init result

	m_eTimeInterval = ElapsedTime(pParameter->getInterval());//interval is 1 hours
	if(m_eTimeInterval <= ElapsedTime(0L))
		m_eTimeInterval = ElapsedTime(60 * 60L);



	//min, max occupancy time
	ElapsedTime eStartTime = pParameter->getStartTime();
	ElapsedTime eEndTime = pParameter->getEndTime();

	for (;eStartTime < eEndTime; eStartTime += m_eTimeInterval)
	{

		CSummaryIntervalValue *pIntervalValue  = new CSummaryIntervalValue();
		pIntervalValue->m_eTimeStart = eStartTime;
		pIntervalValue->m_eTimeEnd = eStartTime + m_eTimeInterval;

		//for each interval , calculate the statistic value
		int nItemCount = (int) m_vDetailResult.size();

		for (int nItem = 0; nItem < nItemCount; ++nItem)
		{
			DetailFlightRunwayResult *pResult = m_vDetailResult[nItem];
			if(pResult == NULL)
				return;

			if(pResult->m_eTimeStart >= eStartTime && pResult->m_eTimeStart < eStartTime + m_eTimeInterval)
			{
				pIntervalValue->m_vOccupancyInfo.push_back(pResult);
			}
		}

		m_vIntervalResult.push_back(pIntervalValue);
	}
	StatisticIntervalResult();
}

void AirsideRunwayUtilizationReport::CSummaryResult::Clear()
{
	std::vector<CSummaryIntervalValue *>::iterator iter = m_vIntervalResult.begin();
	
	for (;iter != m_vIntervalResult.end(); ++iter)
	{
		delete (*iter);
	}

	m_vIntervalResult.clear();
}

void AirsideRunwayUtilizationReport::CSummaryResult::StatisticIntervalResult()
{
	int nCount = (int)m_vIntervalResult.size();
	for (int nitem = 0; nitem < nCount; ++nitem)
	{
		if(m_vIntervalResult[nitem])
			m_vIntervalResult[nitem]->StatisticResult(m_eTimeInterval);
	}
}

void AirsideRunwayUtilizationReport::CSummaryIntervalValue::StatisticResult( ElapsedTime eTimeInterval)
{
	int nItemCount = (int)m_vOccupancyInfo.size();
	if(nItemCount <= 0)
		return;
		
	//set the min max default value, for get the min max value
	m_eMin = ElapsedTime(24*60*60L);
	m_eMax = ElapsedTime(0L);

	m_eTotal = ElapsedTime(0L);
		
	CStatisticalTools<double> statisticalTool;

	std::vector<DetailFlightRunwayResult *>::iterator iter = m_vOccupancyInfo.begin();
	for (; iter != m_vOccupancyInfo.end(); ++iter)
	{
		DetailFlightRunwayResult * pResult = (*iter);
		if(pResult == NULL)
			continue;

		if(m_eMax < pResult->m_eTimeOccupancy)//max
			m_eMax = pResult->m_eTimeOccupancy;

		if(m_eMin > pResult->m_eTimeOccupancy)//min
			m_eMin = pResult->m_eTimeOccupancy;

		statisticalTool.AddNewData(pResult->m_eTimeOccupancy.asSeconds());
		
		m_eTotal += pResult->m_eTimeOccupancy;//total
	}

	statisticalTool.SortData();
	//statistical value

	m_eAverage = ElapsedTime(1L *(m_eTotal.asSeconds()/nItemCount));

	m_eQ1 = ElapsedTime(static_cast<long>(statisticalTool.GetPercentile(25)));
	m_eQ2 = ElapsedTime(static_cast<long>(statisticalTool.GetPercentile(50)));
	m_eQ3 = ElapsedTime(static_cast<long>(statisticalTool.GetPercentile(75)));
	m_eP1 = ElapsedTime(static_cast<long>(statisticalTool.GetPercentile(1)));
	m_eP5 = ElapsedTime(static_cast<long>(statisticalTool.GetPercentile(5)));
	m_eP10 = ElapsedTime(static_cast<long>(statisticalTool.GetPercentile(10)));
	m_eP90 = ElapsedTime(static_cast<long>(statisticalTool.GetPercentile(90)));
	m_eP95 = ElapsedTime(static_cast<long>(statisticalTool.GetPercentile(95)));
	m_eP99 = ElapsedTime(static_cast<long>(statisticalTool.GetPercentile(99)));
	m_eStdDev = ElapsedTime(static_cast<long>(statisticalTool.GetSigma()));

	m_dUtilziation = (m_eTotal.asSeconds() * 100.0)/eTimeInterval.asSeconds();
}

BOOL AirsideRunwayUtilizationReport::CSummaryIntervalValue::WriteReportData( ArctermFile& _file )
{	
	_file.writeTime(m_eTimeStart,TRUE);
	_file.writeTime(m_eTimeEnd,TRUE);

	_file.writeTime(m_eMin,TRUE);
	_file.writeTime(m_eAverage,TRUE);
	_file.writeTime(m_eMax,TRUE);

	_file.writeTime(m_eQ1,TRUE);
	_file.writeTime(m_eQ2,TRUE);
	_file.writeTime(m_eQ3,TRUE);
	_file.writeTime(m_eP1,TRUE);
	_file.writeTime(m_eP5,TRUE);
	_file.writeTime(m_eP10,TRUE);
	_file.writeTime(m_eP90,TRUE);
	_file.writeTime(m_eP95,TRUE);
	_file.writeTime(m_eP99,TRUE);
	_file.writeTime(m_eStdDev,TRUE);
	_file.writeDouble(m_dUtilziation);

	return TRUE;
}

BOOL AirsideRunwayUtilizationReport::CSummaryIntervalValue::ReadReportData( ArctermFile& _file )
{
	_file.getTime(m_eTimeStart,TRUE);
	_file.getTime(m_eTimeEnd,TRUE);

	_file.getTime(m_eMin,TRUE);
	_file.getTime(m_eAverage,TRUE);
	_file.getTime(m_eMax,TRUE);

	_file.getTime(m_eQ1,TRUE);
	_file.getTime(m_eQ2,TRUE);
	_file.getTime(m_eQ3,TRUE);
	_file.getTime(m_eP1,TRUE);
	_file.getTime(m_eP5,TRUE);
	_file.getTime(m_eP10,TRUE);
	_file.getTime(m_eP90,TRUE);
	_file.getTime(m_eP95,TRUE);
	_file.getTime(m_eP99,TRUE);
	_file.getTime(m_eStdDev,TRUE);
	_file.getFloat(m_dUtilziation);

	return TRUE;
}
void AirsideRunwayUtilizationReport::CSummaryResult::InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC )
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);


	int nColCount = 15;
	CString strColHeader[] = 
	{
		_T("Interval"),
		_T("Min(sec)"),
		_T("Average(sec)"),
		_T("Max(sec)"),
		_T("Q1(sec)"),
		_T("Q2(sec)"),
		_T("Q3(sec)"),
		_T("P1(sec)"),
		_T("P5(sec)"),
		_T("P10(sec)"),
		_T("P90(sec)"),
		_T("P95(sec)"),
		_T("P99(sec)"),
		_T("Std dev(sec)"),
		_T("Utilization(%)")
	};

	int nHeaderLen[] = 
	{
		120,
		60,
		60,
		60,
		60,
		60,
		60,
		60,
		60,
		60,
		60,
		60,
		60,
		100,
		100
	};

	if (piSHC)
		piSHC->ResetAll();

	for (int nCol = 0 ; nCol < nColCount; ++nCol)
	{
		cxListCtrl.InsertColumn(nCol,strColHeader[nCol],LVCFMT_LEFT, nHeaderLen[nCol]);
		if (piSHC)
		{
			switch (nCol)
			{
			case 0:
				piSHC->SetDataType(nCol,dtSTRING);
				break;
			case 14:
				piSHC->SetDataType(nCol,dtDEC);
				break;
			default:
				piSHC->SetDataType(nCol,dtINT);
				break;
			}
		}
	}
}

void AirsideRunwayUtilizationReport::CSummaryResult::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{

	int nCount = (int)m_vIntervalResult.size();
	for (int nitem = 0; nitem < nCount; ++nitem)
	{
		if(m_vIntervalResult[nitem] == NULL)
			continue;

		CSummaryIntervalValue * pIntervalValue = m_vIntervalResult[nitem];
		
		//time range
		CString strTimeRange;
		strTimeRange.Format(_T("%s"),pIntervalValue->m_eTimeStart.PrintDateTime());
		cxListCtrl.InsertItem(nitem,strTimeRange);
		
		//min
		CString strLabel = _T("");
		strLabel.Format(_T("%d"),pIntervalValue->m_eMin.asSeconds());
		cxListCtrl.SetItemText(nitem,1,strLabel);

		strLabel.Empty();
		strLabel.Format(_T("%d"),pIntervalValue->m_eAverage.asSeconds());
		cxListCtrl.SetItemText(nitem,2,strLabel);

		strLabel.Empty();
		strLabel.Format(_T("%d"),pIntervalValue->m_eMax.asSeconds());
		cxListCtrl.SetItemText(nitem,3,strLabel);

		strLabel.Empty();
		strLabel.Format(_T("%d"),pIntervalValue->m_eQ1.asSeconds());
		cxListCtrl.SetItemText(nitem,4,strLabel);

		strLabel.Empty();
		strLabel.Format(_T("%d"),pIntervalValue->m_eQ2.asSeconds());
		cxListCtrl.SetItemText(nitem,5,strLabel);

		strLabel.Empty();
		strLabel.Format(_T("%d"),pIntervalValue->m_eQ3.asSeconds());
		cxListCtrl.SetItemText(nitem,6,strLabel);

		strLabel.Empty();
		strLabel.Format(_T("%d"),pIntervalValue->m_eP1.asSeconds());
		cxListCtrl.SetItemText(nitem,7,strLabel);

		strLabel.Empty();
		strLabel.Format(_T("%d"),pIntervalValue->m_eP5.asSeconds());
		cxListCtrl.SetItemText(nitem,8,strLabel);

		strLabel.Empty();
		strLabel.Format(_T("%d"),pIntervalValue->m_eP10.asSeconds());
		cxListCtrl.SetItemText(nitem,9,strLabel);

		strLabel.Empty();
		strLabel.Format(_T("%d"),pIntervalValue->m_eP90.asSeconds());
		cxListCtrl.SetItemText(nitem,10,strLabel);

		strLabel.Empty();
		strLabel.Format(_T("%d"),pIntervalValue->m_eP95.asSeconds());
		cxListCtrl.SetItemText(nitem,11,strLabel);

		strLabel.Empty();
		strLabel.Format(_T("%d"),pIntervalValue->m_eP99.asSeconds());
		cxListCtrl.SetItemText(nitem,12,strLabel);

		strLabel.Empty();
		strLabel.Format(_T("%d"),pIntervalValue->m_eStdDev.asSeconds());
		cxListCtrl.SetItemText(nitem,13,strLabel);

		strLabel.Empty();
		strLabel.Format(_T("%.1f"),pIntervalValue->m_dUtilziation);
		cxListCtrl.SetItemText(nitem,14,strLabel);

	}
}

void AirsideRunwayUtilizationReport::CSummaryResult::RefreshReport( CParameters * parameter )
{
	CAirsideRunwayUtilizationReportParam *pParam = (CAirsideRunwayUtilizationReportParam *)parameter;
	if(pParam  == NULL)
		return;

	if(m_pChartResult != NULL)
		delete m_pChartResult;
	m_pChartResult = NULL;
	if(pParam->getSubType() == -1 )
		pParam->setSubType(CAirsideRunwayUtilizationReport::ChartType_Summary_RunwayOccupancy);
	if(pParam->getSubType() == CAirsideRunwayUtilizationReport::ChartType_Summary_RunwayOccupancy)
	{
		m_pChartResult = new CSummaryRunwayOccupancyChart;
		CSummaryRunwayOccupancyChart *pChart = (CSummaryRunwayOccupancyChart *)m_pChartResult;

		pChart->GenerateResult(m_vIntervalResult);
	}
	else if(pParam->getSubType() == CAirsideRunwayUtilizationReport::ChartType_Summary_RunwayUtilization)
	{
		m_pChartResult = new CSummaryRunwayUtilizationChart;
		CSummaryRunwayUtilizationChart *pChart = (CSummaryRunwayUtilizationChart *)m_pChartResult;
		pChart->GenerateResult(m_vIntervalResult);
	}

}

BOOL AirsideRunwayUtilizationReport::CSummaryResult::WriteReportData( ArctermFile& _file )
{
	//write summary or detail
	//title
	_file.writeField("Runway Utilization Summary Report");
	_file.writeLine();
	//columns
	_file.writeField(_T("Columns"));
	_file.writeLine();

	_file.writeInt((int)ASReportType_Summary);
	_file.writeLine();

	//write report data one by one
	int nResultCount = (int)m_vIntervalResult.size();
	_file.writeInt(nResultCount);
	for (int nResult = 0; nResult < nResultCount; ++nResult)
	{
		CSummaryIntervalValue *pResult = m_vIntervalResult[nResult];
		if (pResult == NULL)
			continue;
		pResult->WriteReportData(_file);
		_file.writeLine();
	}


	return TRUE;
}

BOOL AirsideRunwayUtilizationReport::CSummaryResult::ReadReportData( ArctermFile& _file )
{


	int nResultCount = 0;
	_file.getInteger(nResultCount);
	for (int nResult = 0; nResult < nResultCount; ++nResult)
	{
		CSummaryIntervalValue *pResult = new CSummaryIntervalValue;
		if (pResult == NULL)
			continue;
		pResult->ReadReportData(_file);
		m_vIntervalResult.push_back(pResult);
		_file.getLine();
	}

	return TRUE;
}































AirsideRunwayUtilizationReport::CSummaryRunwayOccupancyChart::CSummaryRunwayOccupancyChart()
{
	m_pvIntervalResult = NULL;
}

AirsideRunwayUtilizationReport::CSummaryRunwayOccupancyChart::~CSummaryRunwayOccupancyChart()
{

}

void AirsideRunwayUtilizationReport::CSummaryRunwayOccupancyChart::GenerateResult( std::vector<CSummaryIntervalValue *>& vIntervalResult )
{
	m_pvIntervalResult = &vIntervalResult;
}

void AirsideRunwayUtilizationReport::CSummaryRunwayOccupancyChart::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	if (m_pvIntervalResult == NULL)
		return;

	if(pParameter == NULL)
		return;


	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T(" Summary Occupancy per Interval ");


	c2dGraphData.m_strYtitle = _T("Time(sec)");
	c2dGraphData.m_strXtitle = _T("Time of day");

	//get all legends
	std::vector<CString> vLegends; 
	//get the legend
	vLegends.push_back(_T("Min"));
	vLegends.push_back(_T("Average"));
	vLegends.push_back(_T("Max"));
	vLegends.push_back(_T("Q1"));
	vLegends.push_back(_T("Q2"));
	vLegends.push_back(_T("Q3"));
	vLegends.push_back(_T("P1"));
	vLegends.push_back(_T("P5"));
	vLegends.push_back(_T("P10"));
	vLegends.push_back(_T("P90"));
	vLegends.push_back(_T("P95"));
	vLegends.push_back(_T("P99"));
	vLegends.push_back(_T("Std Dev"));

	c2dGraphData.m_vrLegend = vLegends;


	c2dGraphData.m_vr2DChartData.resize(vLegends.size()) ;

	//x tick, runway
	int nItemCount = (int)(*m_pvIntervalResult).size();
	for (int nItem = 0; nItem < nItemCount; ++nItem)
	{
		if((*m_pvIntervalResult)[nItem] == NULL)
			continue;

		CSummaryIntervalValue * pIntervalValue = (*m_pvIntervalResult)[nItem];

		CString strTick;
		strTick.Format(_T("%s - %s"),pIntervalValue->m_eTimeStart.PrintDateTime(),pIntervalValue->m_eTimeEnd.PrintDateTime());
		c2dGraphData.m_vrXTickTitle.push_back(strTick);

		//get the chart data
		ElapsedTime eTimeOccupied = ElapsedTime(0L);

		//legend data
		c2dGraphData.m_vr2DChartData[0].push_back((double)pIntervalValue->m_eMin.asSeconds());
		c2dGraphData.m_vr2DChartData[1].push_back((double)pIntervalValue->m_eAverage.asSeconds());
		c2dGraphData.m_vr2DChartData[2].push_back((double)pIntervalValue->m_eMax.asSeconds());
		c2dGraphData.m_vr2DChartData[3].push_back((double)pIntervalValue->m_eQ1.asSeconds());
		c2dGraphData.m_vr2DChartData[4].push_back((double)pIntervalValue->m_eQ2.asSeconds());
		c2dGraphData.m_vr2DChartData[5].push_back((double)pIntervalValue->m_eQ3.asSeconds());
		c2dGraphData.m_vr2DChartData[6].push_back((double)pIntervalValue->m_eP1.asSeconds());
		c2dGraphData.m_vr2DChartData[7].push_back((double)pIntervalValue->m_eP5.asSeconds());
		c2dGraphData.m_vr2DChartData[8].push_back((double)pIntervalValue->m_eP10.asSeconds());
		c2dGraphData.m_vr2DChartData[9].push_back((double)pIntervalValue->m_eP90.asSeconds());
		c2dGraphData.m_vr2DChartData[10].push_back((double)pIntervalValue->m_eP95.asSeconds());
		c2dGraphData.m_vr2DChartData[11].push_back((double)pIntervalValue->m_eP99.asSeconds());
		c2dGraphData.m_vr2DChartData[12].push_back((double)pIntervalValue->m_eStdDev.asSeconds());

	}

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());


	bool bCommaFlag = true;
	CAirsideRunwayUtilizationReportParam* pParam = (CAirsideRunwayUtilizationReportParam*)pParameter;
	//for (int i=0; i<(int)pParam->getFlightConstraintCount(); i++)
	//{
	//	FlightConstraint fltCons = pParam->getFlightConstraint(i);

	//	CString strFlight(_T(""));
	//	fltCons.screenPrint(strFlight.GetBuffer(1024));

	//	if (bCommaFlag)
	//	{
	//		bCommaFlag = false;
	//		strFooter.AppendFormat("%s", strFlight);
	//	}
	//	else
	//	{
	//		strFooter.AppendFormat(",%s", strFlight);
	//	}
	//}
	c2dGraphData.m_strFooter = strFooter;


	//std::vector<CString> vLegends;
	////get the legend
	//int nLegendCount = m_vRunwayOperationDetail->at(0)->m_vWakeVortexDetailValue.size();
	//for (int nLegend = 0; nLegend < nLegendCount; ++nLegend)
	//{
	//	CString strLegend;
	//	strLegend.Format(_T("%s-%s"),
	//		m_vRunwayOperationDetail->at(0)->m_vWakeVortexDetailValue[nLegend].m_strClassLeadName,
	//		m_vRunwayOperationDetail->at(0)->m_vWakeVortexDetailValue[nLegend].m_strClassTrailName);
	//	vLegends.push_back(strLegend);

	//}


	chartWnd.DrawChart(c2dGraphData);



}


















//////////////////////////////////////////////////////////////////////////
//

AirsideRunwayUtilizationReport::CSummaryRunwayUtilizationChart::CSummaryRunwayUtilizationChart()
{
	m_pvIntervalResult = NULL;
}

AirsideRunwayUtilizationReport::CSummaryRunwayUtilizationChart::~CSummaryRunwayUtilizationChart()
{

}

void AirsideRunwayUtilizationReport::CSummaryRunwayUtilizationChart::GenerateResult( std::vector<CSummaryIntervalValue *>& vIntervalResult )
{
	m_pvIntervalResult = &vIntervalResult;


}

void AirsideRunwayUtilizationReport::CSummaryRunwayUtilizationChart::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{

	if (m_pvIntervalResult == NULL)
		return;

	if(pParameter == NULL)
		return;
	
	if((*m_pvIntervalResult).size() == 0)
		return;

	double dPercentageMin = 100.0;
	double dPercentageMax = 0.0;
	double dTotalPercentage = 0.0;
	CStatisticalTools<double> statisticalTool;
	int nItemCount = (int)(*m_pvIntervalResult).size();
	for (int nItem = 0; nItem < nItemCount; ++nItem)
	{
		if((*m_pvIntervalResult)[nItem] == NULL)
			continue;
		CSummaryIntervalValue * pIntervalValue = (*m_pvIntervalResult)[nItem];

		pIntervalValue->m_dUtilziation;
		statisticalTool.AddNewData(pIntervalValue->m_dUtilziation);
		
		if(dPercentageMin > pIntervalValue->m_dUtilziation)
			dPercentageMin = pIntervalValue->m_dUtilziation;

		if(dPercentageMax < pIntervalValue->m_dUtilziation)
			dPercentageMax = pIntervalValue->m_dUtilziation;

		dTotalPercentage += pIntervalValue->m_dUtilziation;
	
	}

	double dAveagePercentage = dTotalPercentage/nItemCount;

	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T(" Summary Utilization Interval ");


	c2dGraphData.m_strYtitle = _T("Usage Percentage(%)");
	c2dGraphData.m_strXtitle = _T("Statistic");

	//get all legends
	std::vector<CString> vLegends; 
	//get the legend
	vLegends.push_back(_T("Min"));
	vLegends.push_back(_T("Average"));
	vLegends.push_back(_T("Max"));
	vLegends.push_back(_T("Q1"));
	vLegends.push_back(_T("Q2"));
	vLegends.push_back(_T("Q3"));
	vLegends.push_back(_T("P1"));
	vLegends.push_back(_T("P5"));
	vLegends.push_back(_T("P10"));
	vLegends.push_back(_T("P90"));
	vLegends.push_back(_T("P95"));
	vLegends.push_back(_T("P99"));
	vLegends.push_back(_T("Std Dev"));

	c2dGraphData.m_vrLegend = vLegends;
	
	//xtick
	CString strXTick;
	strXTick.Format(_T("%s - %s"),pParameter->getStartTime().PrintDateTime(), pParameter->getEndTime().PrintDateTime());
	c2dGraphData.m_vrXTickTitle.push_back(strXTick);

	c2dGraphData.m_vr2DChartData.resize(vLegends.size()) ;

	//min
	//average....... legend
	c2dGraphData.m_vr2DChartData[0].push_back(dPercentageMin);
	c2dGraphData.m_vr2DChartData[1].push_back(dAveagePercentage);
	c2dGraphData.m_vr2DChartData[2].push_back(dPercentageMax);
	c2dGraphData.m_vr2DChartData[3].push_back(statisticalTool.GetPercentile(25));
	c2dGraphData.m_vr2DChartData[4].push_back(statisticalTool.GetPercentile(50));
	c2dGraphData.m_vr2DChartData[5].push_back(statisticalTool.GetPercentile(75));
	c2dGraphData.m_vr2DChartData[6].push_back(statisticalTool.GetPercentile(1));
	c2dGraphData.m_vr2DChartData[7].push_back(statisticalTool.GetPercentile(5));
	c2dGraphData.m_vr2DChartData[8].push_back(statisticalTool.GetPercentile(10));
	c2dGraphData.m_vr2DChartData[9].push_back(statisticalTool.GetPercentile(90));
	c2dGraphData.m_vr2DChartData[10].push_back(statisticalTool.GetPercentile(95));
	c2dGraphData.m_vr2DChartData[11].push_back(statisticalTool.GetPercentile(99));
	c2dGraphData.m_vr2DChartData[12].push_back(statisticalTool.GetSigma());


	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());

	c2dGraphData.m_strFooter = strFooter;



	chartWnd.DrawChart(c2dGraphData);

}

























