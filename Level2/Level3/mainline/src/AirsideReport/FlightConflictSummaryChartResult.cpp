#include "StdAfx.h"
#include ".\flightconflictsummarychartresult.h"
#include "../Reports/StatisticalTools.h"
#include "AirsideFlightConflictPara.h"
#include "AirsideFlightConflictReport.h"
#include "FlightConflictSummaryData.h"
#include "../Common/elaptime.h"
#include "SummaryParaFilter.h"

FlightConflictSummaryChartResult::FlightConflictSummaryChartResult()
:CAirsideReportBaseResult()
{
	m_vSummaryData.clear();
}

FlightConflictSummaryChartResult::~FlightConflictSummaryChartResult(void)
{
}

void FlightConflictSummaryChartResult::GenerateResult(std::vector<FlightConflictSummaryData*>& vSummaryData)
{
	m_vSummaryData.assign(vSummaryData.begin(),vSummaryData.end());
}

bool CompareTimeInterval(FlightConflictSummaryData* pData1, FlightConflictSummaryData* pData2)
{
	if (pData1->m_pParaFilter->m_lTimeStart <= pData2->m_pParaFilter->m_lTimeStart)
		return true;

	return false;
}

void FlightConflictSummaryChartResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if (pParameter == NULL)
		return;

	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T("Conflict report(summary) ");
	c2dGraphData.m_strXtitle = _T("Time of day");
	c2dGraphData.m_strYtitle = _T("Delay time(min)");

	std::vector<CString> vLegend;
	vLegend.push_back("Min");
	vLegend.push_back("Max");
	vLegend.push_back("Total");
	vLegend.push_back("Q1");
	vLegend.push_back("Q2");
	vLegend.push_back("Q3");
	vLegend.push_back("P1");
	vLegend.push_back("P5");
	vLegend.push_back("P10");
	vLegend.push_back("P90");
	vLegend.push_back("P95");
	vLegend.push_back("P99");
	vLegend.push_back("Sig dev");
	c2dGraphData.m_vrLegend = vLegend;




	AirsideFlightConflictPara* pConflictPara = (AirsideFlightConflictPara*)pParameter;

	ElapsedTime tTimeInterval = ElapsedTime(pConflictPara->getInterval());
	ElapsedTime tStartTime = pConflictPara->getStartTime();
	ElapsedTime tEndTime = pConflictPara->getEndTime();

	//calculate interval count
	int nIntCount = (tEndTime.asSeconds() - tStartTime.asSeconds())/pConflictPara->getInterval();
	if ((tEndTime.asSeconds() - tStartTime.asSeconds())%pConflictPara->getInterval() > 0)
		nIntCount++;

	ElapsedTime tRangeStart = tStartTime;
	for(int i = 0; i < nIntCount; i++)
	{		
		CString strTimeRange;
		ElapsedTime tRangeEnd = tRangeStart + ElapsedTime(pConflictPara->getInterval()) - 1L;
		if (tRangeEnd > pConflictPara->getEndTime())
			tRangeEnd = pConflictPara->getEndTime();

		strTimeRange.Format("%s -%s", tRangeStart.printTime(),tRangeEnd.printTime());
		c2dGraphData.m_vrXTickTitle.push_back(strTimeRange);
		tRangeStart = tRangeEnd + 1L;
	}
	int nLegend = vLegend.size();
	c2dGraphData.m_vr2DChartData.resize(nLegend);
	for (int i = 0; i <nLegend; i++)
	{
		c2dGraphData.m_vr2DChartData[i].resize(nIntCount,0);
	}

	//statistic count
	std::sort(m_vSummaryData.begin(), m_vSummaryData.end(),CompareTimeInterval);		//sort by time interval
	
	ElapsedTime tMax = -1L;
	ElapsedTime tMin = -1L;
	ElapsedTime tTotal = 0L;
	CStatisticalTools<double> statisticalTool;

	long lTimeStart = -1L;
	int nItemCount = m_vSummaryData.size();
	for (int i =0; i < nItemCount; i++)
	{
		FlightConflictSummaryData* pData = m_vSummaryData.at(i);
		if (pConflictPara->getSummaryDataFilter()->IsFit(pData) == false)
			continue;

		if (pData->m_pParaFilter->m_lTimeStart > lTimeStart)		//new interval
		{
			if (lTimeStart >=0L)
			{
				int nIdx = (lTimeStart - pConflictPara->getStartTime().asSeconds())/pConflictPara->getInterval();
				double m_eQ1 = statisticalTool.GetPercentile(25);
				double m_eQ2 = statisticalTool.GetPercentile(50);
				double m_eQ3 = statisticalTool.GetPercentile(75);
				double m_eP1 =statisticalTool.GetPercentile(1);
				double m_eP5 = statisticalTool.GetPercentile(5);
				double m_eP10 = statisticalTool.GetPercentile(10);
				double m_eP90 = statisticalTool.GetPercentile(90);
				double m_eP95 = statisticalTool.GetPercentile(95);
				double m_eP99 = statisticalTool.GetPercentile(99);
				double m_eStdDev =statisticalTool.GetSigma();

				//legend data
				c2dGraphData.m_vr2DChartData[0][nIdx] = ((double)tMin.asSeconds()/60);
				c2dGraphData.m_vr2DChartData[1][nIdx] = ((double)tMax.asSeconds()/60);
				c2dGraphData.m_vr2DChartData[2][nIdx] = ((double)tTotal.asSeconds()/60);
				c2dGraphData.m_vr2DChartData[3][nIdx] = (m_eQ1/60);
				c2dGraphData.m_vr2DChartData[4][nIdx] = (m_eQ2/60);
				c2dGraphData.m_vr2DChartData[5][nIdx] = (m_eQ3/60);
				c2dGraphData.m_vr2DChartData[6][nIdx] = (m_eP1/60);
				c2dGraphData.m_vr2DChartData[7][nIdx] = (m_eP5/60);
				c2dGraphData.m_vr2DChartData[8][nIdx] = (m_eP10/60);
				c2dGraphData.m_vr2DChartData[9][nIdx] = (m_eP90/60);
				c2dGraphData.m_vr2DChartData[10][nIdx] = (m_eP95/60);
				c2dGraphData.m_vr2DChartData[11][nIdx] = (m_eP99/60);
				c2dGraphData.m_vr2DChartData[12][nIdx] = (m_eStdDev/60);

				tMax = -1L;
				tMin = -1L;
				tTotal = 0L;
				CStatisticalTools<double> statisticalTool2;
				statisticalTool = statisticalTool2;
			}


			if (tMin > -1L && tMin > pData->m_tMin)
				tMin = pData->m_tMin;
			else
				tMin = pData->m_tMin;

			if (tMax < pData->m_tMax)
				tMax = pData->m_tMax;

			tTotal += pData->m_tTotal;
			statisticalTool.CopyToolData(pData->m_statisticalTool);
			lTimeStart = pData->m_pParaFilter->m_lTimeStart;
		}
		else		//same interval
		{	
			if (tMin > -1L && tMin > pData->m_tMin)
				tMin = pData->m_tMin;
			else
				tMin = pData->m_tMin;

			if (tMax < pData->m_tMax)
				tMax = pData->m_tMax;

			tTotal += pData->m_tTotal;
			statisticalTool.CopyToolData(pData->m_statisticalTool);
		}
	}

	//add last interval
	if (lTimeStart >= 0L)
	{
		int nIdx = (lTimeStart - pConflictPara->getStartTime().asSeconds())/pConflictPara->getInterval();
		double m_eQ1 = statisticalTool.GetPercentile(25);
		double m_eQ2 = statisticalTool.GetPercentile(50);
		double m_eQ3 = statisticalTool.GetPercentile(75);
		double m_eP1 =statisticalTool.GetPercentile(1);
		double m_eP5 = statisticalTool.GetPercentile(5);
		double m_eP10 = statisticalTool.GetPercentile(10);
		double m_eP90 = statisticalTool.GetPercentile(90);
		double m_eP95 = statisticalTool.GetPercentile(95);
		double m_eP99 = statisticalTool.GetPercentile(99);
		double m_eStdDev =statisticalTool.GetSigma();

		//legend data
		c2dGraphData.m_vr2DChartData[0][nIdx] = ((double)tMin.asSeconds()/60);
		c2dGraphData.m_vr2DChartData[1][nIdx] = ((double)tMax.asSeconds()/60);
		c2dGraphData.m_vr2DChartData[2][nIdx] = ((double)tTotal.asSeconds()/60);
		c2dGraphData.m_vr2DChartData[3][nIdx] = (m_eQ1/60);
		c2dGraphData.m_vr2DChartData[4][nIdx] = (m_eQ2/60);
		c2dGraphData.m_vr2DChartData[5][nIdx] = (m_eQ3/60);
		c2dGraphData.m_vr2DChartData[6][nIdx] = (m_eP1/60);
		c2dGraphData.m_vr2DChartData[7][nIdx] = (m_eP5/60);
		c2dGraphData.m_vr2DChartData[8][nIdx] = (m_eP10/60);
		c2dGraphData.m_vr2DChartData[9][nIdx] = (m_eP90/60);
		c2dGraphData.m_vr2DChartData[10][nIdx] = (m_eP95/60);
		c2dGraphData.m_vr2DChartData[11][nIdx] = (m_eP99/60);
		c2dGraphData.m_vr2DChartData[12][nIdx] = (m_eStdDev/60);
	}


	chartWnd.DrawChart(c2dGraphData);

}