#include "StdAfx.h"
#include ".\summarydelayresult.h"
#include "Parameters.h"
#include "CARC3DChart.h"



CSummaryDelayResult::CSummaryDelayResult(void)
{
}

CSummaryDelayResult::~CSummaryDelayResult(void)
{
}

void CSummaryDelayResult::GenerateResult(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData,CParameters *pParameter)
{
	ASSERT(pParameter != NULL);

	ClearAllData();

	for (int j=0; j<(int)pParameter->getFlightConstraintCount(); j++)
	{
		FlightConstraint flightConstraint = pParameter->getFlightConstraint(j);

		CSummaryFlightTypeData summaryFlightTypeData;
		summaryFlightTypeData.m_fltConstraint = flightConstraint;

		vector<long> vDelayTime;
		CStatisticalTools<double> statisticalTool;
		//if no data ,continue
		if (!PrepareData(fltTypeDelayData, flightConstraint, vDelayTime, statisticalTool))
		{
			continue;
		}

		//set data
		//total delay
		long lTotalDelayTime = 0;
		for (int i=0; i<(int)vDelayTime.size(); i++)
		{
			lTotalDelayTime += vDelayTime[i];
		}

		ElapsedTime estTotalDelay(long(lTotalDelayTime/100.0+0.5));
		summaryFlightTypeData.m_summaryData.m_estTotal = estTotalDelay;

		//min delay
		long lMinDelayTime = vDelayTime[0];
		ElapsedTime estMinDelayTime(long(lMinDelayTime/100.0+0.5));
		summaryFlightTypeData.m_summaryData.m_estMin = estMinDelayTime;

		//max delay
		long lMaxDelayTime = vDelayTime[vDelayTime.size()-1];
		ElapsedTime estMaxTime(long(lMaxDelayTime/100.0+0.5));
		summaryFlightTypeData.m_summaryData.m_estMax = estMaxTime;

		//average delay
		ElapsedTime estAverageDelayTime(long(lTotalDelayTime/(vDelayTime.size()*100.0)+0.5));
		summaryFlightTypeData.m_summaryData.m_estAverage = estAverageDelayTime;

		//Q1
		double Q1 = statisticalTool.GetPercentile(25);
		ElapsedTime estQ1(long(Q1/100.0+0.5));
		summaryFlightTypeData.m_summaryData.m_estQ1 = estQ1;

		//Q2
		double Q2 = statisticalTool.GetPercentile(50);
		ElapsedTime estQ2(long(Q2/100.0+0.5));
		summaryFlightTypeData.m_summaryData.m_estQ2 = estQ2;

		//Q3
		double Q3 = statisticalTool.GetPercentile(75);
		ElapsedTime estQ3(long(Q3/100.0+0.5));
		summaryFlightTypeData.m_summaryData.m_estQ3 = estQ3;

		//p1
		double P1 = statisticalTool.GetPercentile(1);
		ElapsedTime estP1(long(P1/100.0+0.5));
		summaryFlightTypeData.m_summaryData.m_estP1 = estP1;

		//p5
		double P5 = statisticalTool.GetPercentile(5);
		ElapsedTime estP5(long(P5/100.0+0.5));
		summaryFlightTypeData.m_summaryData.m_estP5 = estP5;

		//p10
		double P10 = statisticalTool.GetPercentile(10);
		ElapsedTime estP10(long(P10/100.0+0.5));
		summaryFlightTypeData.m_summaryData.m_estP10 = estP10;

		//p90
		double P90 = statisticalTool.GetPercentile(90);
		ElapsedTime estP90(long(P90/100.0+0.5));
		summaryFlightTypeData.m_summaryData.m_estP90 = estP90;

		//p95
		double P95 = statisticalTool.GetPercentile(95);
		ElapsedTime estP95(long(P95/100.0+0.5));
		summaryFlightTypeData.m_summaryData.m_estP95 = estP95;

		//p99
		double P99 = statisticalTool.GetPercentile(99);
		ElapsedTime estP99(long(P99/100.0+0.5));
		summaryFlightTypeData.m_summaryData.m_estP99 = estP99;

		//sigma
		double dSigma = statisticalTool.GetSigma();
		ElapsedTime estSigma(long(dSigma/100.0+0.5));
		summaryFlightTypeData.m_summaryData.m_estSigma = estSigma;

		m_vData.push_back(summaryFlightTypeData);
	}
}

void CSummaryDelayResult::ClearAllData()
{
	m_vData.clear();
}

void CSummaryDelayResult::Init3DChartLegend(C2DChartData& c2dGraphData)
{
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
}
void CSummaryDelayResult::Fill3DChartData(CSummaryData& summaryData,C2DChartData& c2dGraphData)
{
	c2dGraphData.m_vr2DChartData[0].push_back(summaryData.m_estMin.asSeconds()/60.0);
	c2dGraphData.m_vr2DChartData[1].push_back(summaryData.m_estAverage.asSeconds()/60.0);
	c2dGraphData.m_vr2DChartData[2].push_back(summaryData.m_estMax.asSeconds()/60.0);
	c2dGraphData.m_vr2DChartData[3].push_back(summaryData.m_estQ1.asSeconds()/60.0);
	c2dGraphData.m_vr2DChartData[4].push_back(summaryData.m_estQ2.asSeconds()/60.0);
	c2dGraphData.m_vr2DChartData[5].push_back(summaryData.m_estQ3.asSeconds()/60.0);
	c2dGraphData.m_vr2DChartData[6].push_back(summaryData.m_estP1.asSeconds()/60.0);
	c2dGraphData.m_vr2DChartData[7].push_back(summaryData.m_estP5.asSeconds()/60.0);
	c2dGraphData.m_vr2DChartData[8].push_back(summaryData.m_estP10.asSeconds()/60.0);
	c2dGraphData.m_vr2DChartData[9].push_back(summaryData.m_estP90.asSeconds()/60.0);
	c2dGraphData.m_vr2DChartData[10].push_back(summaryData.m_estP95.asSeconds()/60.0);
	c2dGraphData.m_vr2DChartData[11].push_back(summaryData.m_estP99.asSeconds()/60.0);
	c2dGraphData.m_vr2DChartData[12].push_back(summaryData.m_estSigma.asSeconds()/60.0);
}

void CSummaryDelayResult::GetSummaryResult(vector<CSummaryFlightTypeData>& vSummaryResult)
{
	vSummaryResult = m_vData;
}