#include "StdAfx.h"
#include ".\summarydelaychartresult.h"
#include "../Reports/StatisticalTools.h"
#include "../Common/elaptime.h"
#include "AirsideFlightRunwayDelayReport.h"
#include "AirsideFlightRunwayDelayData.h"
#include "AirsideFlightRunwayDelayReportPara.h"
#include "AirsideFlightRunwayDelaySummaryData.h"

SummaryDelayChartResult::SummaryDelayChartResult()
:CAirsideReportBaseResult()
{
	m_vSummaryData.clear();
}

SummaryDelayChartResult::~SummaryDelayChartResult(void)
{
}

void SummaryDelayChartResult::GenerateResult(std::vector<AirsideFlightRunwayDelaySummaryData*>& vSummaryData)
{
	m_vSummaryData.assign(vSummaryData.begin(),vSummaryData.end());
}

bool CompareDataTimeInterval(AirsideFlightRunwayDelaySummaryData* pData1, AirsideFlightRunwayDelaySummaryData* pData2)
{
	if (pData1->m_tStart < pData2->m_tStart)
		return true;

	return false;
}

void SummaryDelayChartResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if (pParameter == NULL)
		return;

	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	AirsideFlightRunwayDelayReportPara* pReportPara = (AirsideFlightRunwayDelayReportPara*)pParameter;
	int nSubType = pReportPara->getSubType();
	if (nSubType == AirsideFlightRunwayDelayReport::ChartType_Summary_Total)
	{
		c2dGraphData.m_strChartTitle = _T("Total Runway Delay Summary");
	}
	else
	{
		CString strPos = FlightRunwayDelay::POSITIONTYPE[nSubType-102];
		c2dGraphData.m_strChartTitle = _T("Runway Delay Summary At ") +strPos;
	}

	c2dGraphData.m_strXtitle = _T("Time of day");
	c2dGraphData.m_strYtitle = _T("Delay time(secs)");

	std::vector<CString> vLegend;
	vLegend.push_back("Min");
	vLegend.push_back("Avg");
	vLegend.push_back("Max");
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

	ElapsedTime tTimeInterval = ElapsedTime(pReportPara->getInterval());
	ElapsedTime tStartTime = pReportPara->getStartTime();
	ElapsedTime tEndTime = pReportPara->getEndTime();

	//calculate interval count
	int nIntCount = (tEndTime.asSeconds() - tStartTime.asSeconds())/pReportPara->getInterval();
	if ((tEndTime.asSeconds() - tStartTime.asSeconds())%pReportPara->getInterval() > 0)
		nIntCount++;

	ElapsedTime tRangeStart = tStartTime;
	for(int i = 0; i < nIntCount; i++)
	{		
		CString strTimeRange;
		ElapsedTime tRangeEnd = tRangeStart + ElapsedTime(pReportPara->getInterval()) - 1L;
		if (tRangeEnd > pReportPara->getEndTime())
			tRangeEnd = pReportPara->getEndTime();

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
	std::sort(m_vSummaryData.begin(), m_vSummaryData.end(),CompareDataTimeInterval);		//sort by time interval

	long lMin = -1L;
	long lMax = -1L;
	int nCount =0;
	long lTotal = 0L;
	CStatisticalTools<double> statisticalTool;

	long lTimeStart = -1L;
	int nItemCount = m_vSummaryData.size();
	for (int i =0; i < nItemCount; i++)
	{
		AirsideFlightRunwayDelaySummaryData* pData = m_vSummaryData.at(i);
		if (pReportPara->IsSummaryDataFitChartType(pData) == false)
			continue;
		if (pData->m_tStart.asSeconds() > lTimeStart)		//new interval
		{
			if (lTimeStart >=0L)
			{
				int nIdx = (lTimeStart - pReportPara->getStartTime().asSeconds())/pReportPara->getInterval();

				//legend data
				c2dGraphData.m_vr2DChartData[0][nIdx] = statisticalTool.GetMin();
				c2dGraphData.m_vr2DChartData[1][nIdx] = statisticalTool.GetAvarage();
				c2dGraphData.m_vr2DChartData[2][nIdx] = statisticalTool.GetMax();
				c2dGraphData.m_vr2DChartData[3][nIdx] = statisticalTool.GetPercentile(25);
				c2dGraphData.m_vr2DChartData[4][nIdx] = statisticalTool.GetPercentile(50);
				c2dGraphData.m_vr2DChartData[5][nIdx] = statisticalTool.GetPercentile(75);
				c2dGraphData.m_vr2DChartData[6][nIdx] = statisticalTool.GetPercentile(1);
				c2dGraphData.m_vr2DChartData[7][nIdx] = statisticalTool.GetPercentile(5);
				c2dGraphData.m_vr2DChartData[8][nIdx] = statisticalTool.GetPercentile(10);
				c2dGraphData.m_vr2DChartData[9][nIdx] = statisticalTool.GetPercentile(90);
				c2dGraphData.m_vr2DChartData[10][nIdx] = statisticalTool.GetPercentile(95);
				c2dGraphData.m_vr2DChartData[11][nIdx] = statisticalTool.GetPercentile(99);
				c2dGraphData.m_vr2DChartData[12][nIdx] = statisticalTool.GetSigma();

				lMin = -1L;
				lMax = -1L;
				lTotal = 0L;
				nCount = 0;
				CStatisticalTools<double> statisticalTool2;
				statisticalTool = statisticalTool2;
			}


			if (lMin > -1L && lMin > pData->m_lMin)
				lMin = pData->m_lMin;
			else
				lMin = pData->m_lMin;

			if (lMax < pData->m_lMax)
				lMax = pData->m_lMax;

			lTotal += pData->m_lTotal;
			nCount+= pData->m_nDelayCount;
			statisticalTool.CopyToolData(pData->m_statisticalTool);
			lTimeStart = pData->m_tStart.asSeconds();
		}
		else		//same interval
		{	
			if (lMin > -1L && lMin > pData->m_lMin)
				lMin = pData->m_lMin;
			else
				lMin = pData->m_lMin;

			if (lMax < pData->m_lMax)
				lMax = pData->m_lMax;
			statisticalTool.CopyToolData(pData->m_statisticalTool);
			nCount+= pData->m_nDelayCount;
		}
	}

	//add last interval  this seems no need 
	if ( lTimeStart < pReportPara->getEndTime().asSeconds() )
	{
		int nIdx = (lTimeStart - pReportPara->getStartTime().asSeconds())/pReportPara->getInterval();
		//legend data
		c2dGraphData.m_vr2DChartData[0][nIdx] = statisticalTool.GetMin();
		c2dGraphData.m_vr2DChartData[1][nIdx] = statisticalTool.GetAvarage();
		c2dGraphData.m_vr2DChartData[2][nIdx] = statisticalTool.GetMax();
		c2dGraphData.m_vr2DChartData[3][nIdx] = statisticalTool.GetPercentile(25);
		c2dGraphData.m_vr2DChartData[4][nIdx] = statisticalTool.GetPercentile(50);
		c2dGraphData.m_vr2DChartData[5][nIdx] = statisticalTool.GetPercentile(75);
		c2dGraphData.m_vr2DChartData[6][nIdx] = statisticalTool.GetPercentile(1);
		c2dGraphData.m_vr2DChartData[7][nIdx] = statisticalTool.GetPercentile(5);
		c2dGraphData.m_vr2DChartData[8][nIdx] = statisticalTool.GetPercentile(10);
		c2dGraphData.m_vr2DChartData[9][nIdx] = statisticalTool.GetPercentile(90);
		c2dGraphData.m_vr2DChartData[10][nIdx] = statisticalTool.GetPercentile(95);
		c2dGraphData.m_vr2DChartData[11][nIdx] = statisticalTool.GetPercentile(99);
		c2dGraphData.m_vr2DChartData[12][nIdx] = statisticalTool.GetSigma();
	}

	chartWnd.DrawChart(c2dGraphData);

}