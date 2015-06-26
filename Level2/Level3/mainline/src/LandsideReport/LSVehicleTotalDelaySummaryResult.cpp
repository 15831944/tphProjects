#include "StdAfx.h"
#include ".\lsvehicletotaldelaysummaryresult.h"
#include "../MFCExControl/SortableHeaderCtrl.h"
#include "../MFCExControl/XListCtrl.h"
#include "LandsideBaseParam.h"

#include "AirsideReport/CARC3DChart.h"

LSVehicleTotalDelaySummaryResult::LSVehicleTotalDelaySummaryResult(void)
{
}

LSVehicleTotalDelaySummaryResult::~LSVehicleTotalDelaySummaryResult(void)
{
}

void LSVehicleTotalDelaySummaryResult::Draw3DChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" Vehicle Delay Report(Summary) ");
	c2dGraphData.m_strYtitle = _T("Delay (mins)");
	c2dGraphData.m_strXtitle = _T("Vehicle Type");

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
	strFooter.Format(_T("Landside Vehicle Total Delay Report %s "), pParameter->GetParameterString());

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

void LSVehicleTotalDelaySummaryResult::GenerateResult( CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde )
{
	LSVehicleTotalDelayResult::GenerateResult(pFunc,pParameter,pLandisde);

	std::sort(m_vResult.begin(),m_vResult.end());
	//generate summary result
	int nVehicleCountData = (int)(m_vResult.size());
	for (int i = 0; i < nVehicleCountData; i++)
	{
		TotalDelayItem& dataItem = m_vResult[i];
		m_vTimeSummary[dataItem.m_strVehicleTypeName].m_estTotal += dataItem.m_eTotalDelayTime;
		m_vTimeSummary[dataItem.m_strVehicleTypeName].statisticalTool.AddNewData(dataItem.m_eTotalDelayTime.getPrecisely());
	}

	ASSERT(m_vTimeSummary.size() > 0);
	if(m_vTimeSummary.size() == 0)
		return;

	//sort the result, 
	std::map<CString,LandsideSummaryTime>::iterator iter = m_vTimeSummary.begin();
	for(; iter != m_vTimeSummary.end(); iter++)
	{
		(*iter).second.statisticalTool.SortData();
		//set data

		//min delay
		ElapsedTime estMin(long((*iter).second.statisticalTool.GetMin()/100.0+0.5));
		(*iter).second.m_estMin = estMin;

		//max delay
		ElapsedTime estMax(long((*iter).second.statisticalTool.GetMax()/100.0+0.5));
		(*iter).second.m_estMax = estMax;

		//average delay
		ElapsedTime estAverage(long((*iter).second.statisticalTool.GetAvarage()/100.0+0.5));
		(*iter).second.m_estAverage = estAverage;

		//Q1
		double Q1 = (*iter).second.statisticalTool.GetPercentile(25);
		ElapsedTime estQ1(long(Q1/100.0+0.5));
		(*iter).second.m_estQ1 = estQ1;

		//Q2
		double Q2 = (*iter).second.statisticalTool.GetPercentile(50);
		ElapsedTime estQ2(long(Q2/100.0+0.5));
		(*iter).second.m_estQ2 = estQ2;

		//Q3
		double Q3 = (*iter).second.statisticalTool.GetPercentile(75);
		ElapsedTime estQ3(long(Q3/100.0+0.5));
		(*iter).second.m_estQ3 = estQ3;

		//p1
		double P1 = (*iter).second.statisticalTool.GetPercentile(1);
		ElapsedTime estP1(long(P1/100.0+0.5));
		(*iter).second.m_estP1 = estP1;

		//p5
		double P5 = (*iter).second.statisticalTool.GetPercentile(5);
		ElapsedTime estP5(long(P5/100.0+0.5));
		(*iter).second.m_estP5 = estP5;

		//p10
		double P10 = (*iter).second.statisticalTool.GetPercentile(10);
		ElapsedTime estP10(long(P10/100.0+0.5));
		(*iter).second.m_estP10 = estP10;

		//p90
		double P90 = (*iter).second.statisticalTool.GetPercentile(90);
		ElapsedTime estP90(long(P90/100.0+0.5));
		(*iter).second.m_estP90 = estP90;

		//p95
		double P95 = (*iter).second.statisticalTool.GetPercentile(95);
		ElapsedTime estP95(long(P95/100.0+0.5));
		(*iter).second.m_estP95 = estP95;

		//p99
		double P99 = (*iter).second.statisticalTool.GetPercentile(99);
		ElapsedTime estP99(long(P99/100.0+0.5));
		(*iter).second.m_estP99 = estP99;

		//sigma
		double dSigma = (*iter).second.statisticalTool.GetSigma();
		ElapsedTime estSigma(long(dSigma/100.0+0.5));
		(*iter).second.m_estSigma = estSigma;
	}

}

void LSVehicleTotalDelaySummaryResult::RefreshReport( LandsideBaseParam * parameter )
{
	
}

void LSVehicleTotalDelaySummaryResult::InitListHead( CListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ )
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
	cxListCtrl.InsertColumn(nCol, _T("Min"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);

	nCol += 1;
	cxListCtrl.InsertColumn(nCol, _T("Max"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);

	nCol += 1;
	cxListCtrl.InsertColumn(nCol, _T("Average"), LVCFMT_LEFT, 100);
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
	cxListCtrl.InsertColumn(nCol, _T("sigma"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(nCol,dtTIME);
}

void LSVehicleTotalDelaySummaryResult::FillListContent( CListCtrl& cxListCtrl, LandsideBaseParam * parameter )
{
	int nRowIndex = 0;
	int nColIndex = 1;
	std::map<CString,LandsideSummaryTime>::iterator iter = m_vTimeSummary.begin();
	for(; iter != m_vTimeSummary.end(); iter++)
	{
		//Vehicle type
		cxListCtrl.InsertItem(nRowIndex, (*iter).first);

		//total delay
		nColIndex = 1;
		cxListCtrl.SetItemText(nRowIndex, nColIndex,(*iter).second.m_estTotal.printTime());

		nColIndex += 1;
		//Min Delay(mins)
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

LSGraphChartTypeList LSVehicleTotalDelaySummaryResult::GetChartList() const
{
	LSGraphChartTypeList chartList;

	chartList.AddItem(CT_S_TotalDelayTime,"Delay time VS statistic");

	return  chartList;
}

BOOL LSVehicleTotalDelaySummaryResult::ReadReportData( ArctermFile& _file )
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
		m_vTimeSummary[strVehicleType] = summaryTime;
		_file.getLine();
	}
	return TRUE;
}

BOOL LSVehicleTotalDelaySummaryResult::WriteReportData( ArctermFile& _file )
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
