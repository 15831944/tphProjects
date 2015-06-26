#include "StdAfx.h"
#include ".\lsvehicledelaysummaryresult.h"
#include <algorithm>
#include "../Reports/StatisticalTools.h"
#include <math.h>

#include "../MFCExControl/SortableHeaderCtrl.h"
#include "../MFCExControl/XListCtrl.h"

#include "AirsideReport/CARC3DChart.h"

#include "LSVehicleDelayParam.h"

LSVehicleDelaySummaryResult::LSVehicleDelaySummaryResult(void)
{
}

LSVehicleDelaySummaryResult::~LSVehicleDelaySummaryResult(void)
{
}

void LSVehicleDelaySummaryResult::Draw3DChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" Vehicle Delay Report(Summary) ");
	c2dGraphData.m_strYtitle = _T("Delay (mins)");
	c2dGraphData.m_strXtitle = _T("Component Type");

	c2dGraphData.m_vrLegend.push_back("Min");
	c2dGraphData.m_vrLegend.push_back("Average");
	c2dGraphData.m_vrLegend.push_back("Max");
	c2dGraphData.m_vrLegend.push_back("Q1");
	c2dGraphData.m_vrLegend.push_back("Q2");
	c2dGraphData.m_vrLegend.push_back("Q3");
	c2dGraphData.m_vrLegend.push_back("P1");
	c2dGraphData.m_vrLegend.push_back("P5");
	c2dGraphData.m_vrLegend.push_back("P10");
	c2dGraphData.m_vrLegend.push_back("P90");
	c2dGraphData.m_vrLegend.push_back("P95");
	c2dGraphData.m_vrLegend.push_back("P99");
	c2dGraphData.m_vrLegend.push_back("Sigma");


	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Landside Vehicle Delay Report %s "), pParameter->GetParameterString());

	c2dGraphData.m_strFooter = strFooter;


	std::vector<CString > vXTickTitle;
	c2dGraphData.m_vr2DChartData.resize(13);
	std::map<CString,LandsideSummaryTime>::iterator iter = m_vTimeSummary.begin();
	for(; iter != m_vTimeSummary.end(); iter++)
	{
		vXTickTitle.push_back((*iter).first);
		c2dGraphData.m_vr2DChartData[0].push_back((*iter).second.m_estMin.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[1].push_back((*iter).second.m_estAverage.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[2].push_back((*iter).second.m_estMax.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[3].push_back((*iter).second.m_estQ1.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[4].push_back((*iter).second.m_estQ2.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[5].push_back((*iter).second.m_estQ3.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[6].push_back((*iter).second.m_estP1.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[7].push_back((*iter).second.m_estP5.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[8].push_back((*iter).second.m_estP10.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[9].push_back((*iter).second.m_estP90.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[10].push_back((*iter).second.m_estP95.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[11].push_back((*iter).second.m_estP99.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[12].push_back((*iter).second.m_estSigma.asSeconds()/60.0);

	}
	c2dGraphData.m_vrXTickTitle = vXTickTitle;
	chartWnd.DrawChart(c2dGraphData);
}

void LSVehicleDelaySummaryResult::GenerateResult( CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde )
{
	LSVehicleDelayResult::GenerateResult(pFunc, pParameter, pLandisde);

	//generate summary result
	std::map<int, std::vector<DelayItem> >::iterator iter =  m_mapDelayResult.begin();
	for (; iter != m_mapDelayResult.end(); ++iter)
	{
		std::vector<DelayItem> &vItems = (*iter).second;
		int nCount = static_cast<int>(vItems.size());
		std::map<CString,ElapsedTime> mapTotalDelay;
		for (int nItem = 0; nItem < nCount; ++ nItem)
		{
			DelayItem& dItem = vItems[nItem];
			mapTotalDelay[dItem.m_strTypeName] += dItem.m_eDelayTime; 
			m_vTimeSummary[dItem.m_strTypeName].m_estTotal += dItem.m_eDelayTime;
		}
		
		std::map<CString,ElapsedTime>::iterator mapIter = mapTotalDelay.begin();
		for(;mapIter != mapTotalDelay.end(); ++mapIter)
		{
			std::map<CString,LandsideSummaryTime>::iterator Summaryiter = m_vTimeSummary.find((*mapIter).first);
			if (Summaryiter != m_vTimeSummary.end())
			{
				(*Summaryiter).second.statisticalTool.AddNewData((*mapIter).second.getPrecisely());
			}
			
		}
	}

	//sort the result, 
	std::map<CString,LandsideSummaryTime>::iterator resultIter = m_vTimeSummary.begin();
	for(; resultIter != m_vTimeSummary.end(); ++resultIter)
	{
		(*resultIter).second.statisticalTool.SortData();
		//set data

		//min delay
		ElapsedTime estMin(long((*resultIter).second.statisticalTool.GetMin()/100.0+0.5));
		(*resultIter).second.m_estMin = estMin;

		//max delay
		ElapsedTime estMax(long((*resultIter).second.statisticalTool.GetMax()/100.0+0.5));
		(*resultIter).second.m_estMax = estMax;

		//average delay
		ElapsedTime estAverage(long((*resultIter).second.statisticalTool.GetAvarage()/100.0+0.5));
		(*resultIter).second.m_estAverage = estAverage;

		//Q1
		double Q1 = (*resultIter).second.statisticalTool.GetPercentile(25);
		ElapsedTime estQ1(long(Q1/100.0+0.5));
		(*resultIter).second.m_estQ1 = estQ1;

		//Q2
		double Q2 = (*resultIter).second.statisticalTool.GetPercentile(50);
		ElapsedTime estQ2(long(Q2/100.0+0.5));
		(*resultIter).second.m_estQ2 = estQ2;

		//Q3
		double Q3 = (*resultIter).second.statisticalTool.GetPercentile(75);
		ElapsedTime estQ3(long(Q3/100.0+0.5));
		(*resultIter).second.m_estQ3 = estQ3;

		//p1
		double P1 = (*resultIter).second.statisticalTool.GetPercentile(1);
		ElapsedTime estP1(long(P1/100.0+0.5));
		(*resultIter).second.m_estP1 = estP1;

		//p5
		double P5 = (*resultIter).second.statisticalTool.GetPercentile(5);
		ElapsedTime estP5(long(P5/100.0+0.5));
		(*resultIter).second.m_estP5 = estP5;

		//p10
		double P10 = (*resultIter).second.statisticalTool.GetPercentile(10);
		ElapsedTime estP10(long(P10/100.0+0.5));
		(*resultIter).second.m_estP10 = estP10;

		//p90
		double P90 = (*resultIter).second.statisticalTool.GetPercentile(90);
		ElapsedTime estP90(long(P90/100.0+0.5));
		(*resultIter).second.m_estP90 = estP90;

		//p95
		double P95 = (*resultIter).second.statisticalTool.GetPercentile(95);
		ElapsedTime estP95(long(P95/100.0+0.5));
		(*resultIter).second.m_estP95 = estP95;

		//p99
		double P99 = (*resultIter).second.statisticalTool.GetPercentile(99);
		ElapsedTime estP99(long(P99/100.0+0.5));
		(*resultIter).second.m_estP99 = estP99;

		//sigma
		double dSigma = (*resultIter).second.statisticalTool.GetSigma();
		ElapsedTime estSigma(long(dSigma/100.0+0.5));
		(*resultIter).second.m_estSigma = estSigma;
	}
	//std::vector<ElapsedTime> vVehicleDelay;
	//CStatisticalTools<double> statisticalTool;
	//
	////total delay
	//ElapsedTime lTotalDelayTime = ElapsedTime(0L);

	//std::map<int, std::vector<DelayItem> >::iterator iter =  m_mapDelayResult.begin();
	//for (; iter != m_mapDelayResult.end(); ++iter)
	//{
	//	std::vector<DelayItem> &vItems = (*iter).second;

	//	ElapsedTime eDelayTime;
	//	int nCount = static_cast<int>(vItems.size());
	//	for (int nItem = 0; nItem < nCount; ++ nItem)
	//	{
	//		DelayItem& dItem = vItems[nItem];

	//		eDelayTime += dItem.m_eDelayTime;

	//	}

	//	vVehicleDelay.push_back(eDelayTime);
	//	statisticalTool.AddNewData(eDelayTime.getPrecisely());

	//	lTotalDelayTime += eDelayTime;
	//}
	
	//ASSERT(vVehicleDelay.size() > 0);
	//if(vVehicleDelay.size() == 0)
	//	return;

	////sort the result, 
	//std::sort(vVehicleDelay.begin(), vVehicleDelay.end());



	////sort data
	//statisticalTool.SortData();
	//std::sort(vVehicleDelay.begin(), vVehicleDelay.end());

	////set data
	//m_timeSummary.m_estTotal = lTotalDelayTime;

	////min delay
	//m_timeSummary.m_estMin = vVehicleDelay[0];

	////max delay
	//m_timeSummary.m_estMax = vVehicleDelay[vVehicleDelay.size()-1];

	////average delay
	//ElapsedTime estAverageDelayTime(lTotalDelayTime.getPrecisely()/(vVehicleDelay.size()*100.0));
	//m_timeSummary.m_estAverage = estAverageDelayTime;

	////Q1
	//double Q1 = statisticalTool.GetPercentile(25);
	//ElapsedTime estQ1(Q1/100.0);
	//m_timeSummary.m_estQ1 = estQ1;

	////Q2
	//double Q2 = statisticalTool.GetPercentile(50);
	//ElapsedTime estQ2(Q2/100.0);
	//m_timeSummary.m_estQ2 = estQ2;

	////Q3
	//double Q3 = statisticalTool.GetPercentile(75);
	//ElapsedTime estQ3(Q3/100.0);
	//m_timeSummary.m_estQ3 = estQ3;

	////p1
	//double P1 = statisticalTool.GetPercentile(1);
	//ElapsedTime estP1(P1/100.0);
	//m_timeSummary.m_estP1 = estP1;

	////p5
	//double P5 = statisticalTool.GetPercentile(5);
	//ElapsedTime estP5(P5/100.0);
	//m_timeSummary.m_estP5 = estP5;

	////p10
	//double P10 = statisticalTool.GetPercentile(10);
	//ElapsedTime estP10(P10/100.0);
	//m_timeSummary.m_estP10 = estP10;

	////p90
	//double P90 = statisticalTool.GetPercentile(90);
	//ElapsedTime estP90(P90/100.0);
	//m_timeSummary.m_estP90 = estP90;

	////p95
	//double P95 = statisticalTool.GetPercentile(95);
	//ElapsedTime estP95(P95/100.0);
	//m_timeSummary.m_estP95 = estP95;

	////p99
	//double P99 = statisticalTool.GetPercentile(99);
	//ElapsedTime estP99(P99/100.0);
	//m_timeSummary.m_estP99 = estP99;

	////sigma
	//double dSigma = statisticalTool.GetSigma();
	//ElapsedTime estSigma(dSigma/100.0);
	//m_timeSummary.m_estSigma = estSigma;



}

void LSVehicleDelaySummaryResult::RefreshReport( LandsideBaseParam * parameter )
{

}

void LSVehicleDelaySummaryResult::InitListHead( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ )
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	ASSERT(piSHC != NULL);
	if (piSHC == NULL)
		return;


	int nCol = 0;
	cxListCtrl.InsertColumn(nCol, _T("Vehicle Type"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtSTRING);

	//total delay
	nCol += 1;
	cxListCtrl.InsertColumn(nCol, _T("Total Delay(hh:mm:ss)"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);

	nCol += 1;
	cxListCtrl.InsertColumn(nCol, _T("Min Delay"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);

	nCol += 1;
	cxListCtrl.InsertColumn(nCol, _T("Max Delay"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);

	nCol += 1;
	cxListCtrl.InsertColumn(nCol, _T("Mean Delay"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);

	nCol += 1;
	cxListCtrl.InsertColumn(nCol, _T("Q1"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);

	nCol += 1;
	cxListCtrl.InsertColumn(nCol, _T("Q2"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);

	nCol += 1;
	cxListCtrl.InsertColumn(nCol, _T("Q3"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);

	nCol += 1;
	cxListCtrl.InsertColumn(nCol, _T("P1"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);

	nCol += 1;
	cxListCtrl.InsertColumn(nCol, _T("P5"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);

	nCol += 1;
	cxListCtrl.InsertColumn(nCol, _T("P10"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);

	nCol += 1;
	cxListCtrl.InsertColumn(nCol, _T("P90"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);

	nCol += 1;
	cxListCtrl.InsertColumn(nCol, _T("P95"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);

	nCol += 1;
	cxListCtrl.InsertColumn(nCol, _T("P99"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);

	nCol += 1;
	cxListCtrl.InsertColumn(nCol, _T("Std dev"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);


}

void LSVehicleDelaySummaryResult::FillListContent( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter )
{	

	int nRowIndex = 0;
	int nColIndex = 1;
	std::map<CString,LandsideSummaryTime>::iterator iter = m_vTimeSummary.begin();
	for(; iter != m_vTimeSummary.end(); iter++)
	{
		////Vehicle type
		cxListCtrl.InsertItem(nRowIndex, (*iter).first);

		//total delay
		nColIndex = 1;
		cxListCtrl.SetItemText(nRowIndex, nColIndex,(*iter).second.m_estTotal.printTime());

		//Min Delay(mins)
		nColIndex += 1;
		cxListCtrl.SetItemText(nRowIndex, nColIndex,(*iter).second.m_estMin.printTime());

		//Max Delay(mins)
		nColIndex += 1;
		cxListCtrl.SetItemText(nRowIndex, nColIndex,(*iter).second.m_estMax.printTime());

		//Mean Delay(mins)
		nColIndex += 1;
		cxListCtrl.SetItemText(nRowIndex, nColIndex,(*iter).second.m_estAverage.printTime());

		//Q1(mins)
		nColIndex += 1;
		cxListCtrl.SetItemText(nRowIndex, nColIndex,(*iter).second.m_estQ1.printTime());

		//Q2(mins)
		nColIndex += 1;
		cxListCtrl.SetItemText(nRowIndex, nColIndex,(*iter).second.m_estQ2.printTime());

		//Q3(mins)
		nColIndex += 1;
		cxListCtrl.SetItemText(nRowIndex, nColIndex,(*iter).second.m_estQ3.printTime());

		//P1(mins)
		nColIndex += 1;
		cxListCtrl.SetItemText(nRowIndex, nColIndex,(*iter).second.m_estP1.printTime());

		//P5(mins)
		nColIndex += 1;
		cxListCtrl.SetItemText(nRowIndex, nColIndex,(*iter).second.m_estP5.printTime());

		//P10(mins)
		nColIndex += 1;
		cxListCtrl.SetItemText(nRowIndex, nColIndex,(*iter).second.m_estP10.printTime());

		//P90(mins)
		nColIndex += 1;
		cxListCtrl.SetItemText(nRowIndex, nColIndex,(*iter).second.m_estP90.printTime());

		//P95(mins)
		nColIndex += 1;
		cxListCtrl.SetItemText(nRowIndex, nColIndex,(*iter).second.m_estP95.printTime());

		//P99(mins)
		nColIndex += 1;
		cxListCtrl.SetItemText(nRowIndex, nColIndex,(*iter).second.m_estP99.printTime());

		//Std dev(mins)
		nColIndex += 1;
		cxListCtrl.SetItemText(nRowIndex, nColIndex,(*iter).second.m_estSigma.printTime());

		nRowIndex++;
	}
	
}

LSGraphChartTypeList LSVehicleDelaySummaryResult::GetChartList() const
{	
	LSGraphChartTypeList chartList;

	chartList.AddItem(CT_S_DelayTime,"Delay Time");

	return  chartList;
}

BOOL LSVehicleDelaySummaryResult::ReadReportData( ArctermFile& _file )
{
	int nDataCount = 0;
	_file.getInteger(nDataCount);
	_file.getLine();

	for (int i = 0; i < nDataCount; i++)
	{
		LandsideSummaryTime summaryTime;
		CString strVehicleType;
		_file.getField(strVehicleType.GetBuffer(1024),1024);
		strVehicleType.ReleaseBuffer();

		_file.getTime(summaryTime.m_estTotal);
		_file.getTime(summaryTime.m_estMin);
		_file.getTime(summaryTime.m_estAverage);
		_file.getTime(summaryTime.m_estMax);
		_file.getTime(summaryTime.m_estQ1);
		_file.getTime(summaryTime.m_estQ2);
		_file.getTime(summaryTime.m_estQ3);
		_file.getTime(summaryTime.m_estP1);
		_file.getTime(summaryTime.m_estP5);
		_file.getTime(summaryTime.m_estP10);
		_file.getTime(summaryTime.m_estP90);
		_file.getTime(summaryTime.m_estP95);
		_file.getTime(summaryTime.m_estP99);
		_file.getTime(summaryTime.m_estSigma);
		_file.getLine();
	}
	return TRUE;
}

BOOL LSVehicleDelaySummaryResult::WriteReportData( ArctermFile& _file )
{
	int nDataCount = (int)m_vTimeSummary.size();
	_file.writeInt(nDataCount);
	_file.writeLine();

	std::map<CString,LandsideSummaryTime>::iterator iter = m_vTimeSummary.begin();
	for(; iter != m_vTimeSummary.end(); iter++)
	{
		CString strVehicleType = (*iter).first;
		_file.writeField(strVehicleType);
		_file.writeTime((*iter).second.m_estTotal);
		_file.writeTime((*iter).second.m_estMin);
		_file.writeTime((*iter).second.m_estAverage);
		_file.writeTime((*iter).second.m_estMax);
		_file.writeTime((*iter).second.m_estQ1);
		_file.writeTime((*iter).second.m_estQ2);
		_file.writeTime((*iter).second.m_estQ3);
		_file.writeTime((*iter).second.m_estP1);
		_file.writeTime((*iter).second.m_estP5);
		_file.writeTime((*iter).second.m_estP10);
		_file.writeTime((*iter).second.m_estP90);
		_file.writeTime((*iter).second.m_estP95);
		_file.writeTime((*iter).second.m_estP99);
		_file.writeTime((*iter).second.m_estSigma);
		_file.writeLine();
	}
	return TRUE;
}
