#include "StdAfx.h"
#include ".\summarynodedelayresult.h"
#include "AirsideNodeDelayReportParameter.h"
#include <algorithm>
#include "CARC3DChart.h"
#include "AirsideReportNode.h"

CSummaryNodeDelayResult::CSummaryNodeDelayResult(void)
{
}

CSummaryNodeDelayResult::~CSummaryNodeDelayResult(void)
{
	m_vData.clear();
}

bool CSummaryNodeDelayResult::IsNodeExist(std::vector<CAirsideNodeDelayReport::nodeDelay>& nodeDelayData, CAirsideReportNode& airsideReportNode, int& nIndex)
{
	for (int i=0; i<(int)nodeDelayData.size(); i++)
	{
		if (airsideReportNode.GetNodeID() == nodeDelayData[i].node.GetNodeID()
			&& airsideReportNode.GetNodeType() == nodeDelayData[i].node.GetNodeType())
		{
			nIndex = i;
			return true;
		}
	}

	return false;
}

bool CSummaryNodeDelayResult::PrepareData(CAirsideNodeDelayReport::nodeDelay& reportNodeDelay, vector<long>& vDelayTime, CStatisticalTools<double>& statisticalTool)
{
	for (int i=0; i<(int)reportNodeDelay.vDelayItem.size(); i++)
	{
		vDelayTime.push_back(reportNodeDelay.vDelayItem[i].delayTime);
		statisticalTool.AddNewData(reportNodeDelay.vDelayItem[i].delayTime);
	}

	//no data
	if (vDelayTime.size() < 1)
	{
		return false;
	}

	//sort data
	statisticalTool.SortData();
	std::sort(vDelayTime.begin(), vDelayTime.end());

	return true;
}

void CSummaryNodeDelayResult::GenerateResult(std::vector<CAirsideNodeDelayReport::nodeDelay>& nodeDelayData,CParameters *pParameter)
{
	ASSERT(pParameter);
	m_vData.clear();

	CAirsideNodeDelayReportParameter* pAirsideNodeDelayReportParameter = (CAirsideNodeDelayReportParameter*)pParameter;
	for (int i=0; i<(int)pAirsideNodeDelayReportParameter->getReportNodeCount(); i++)
	{
		CAirsideReportNode airsideReportNode = pAirsideNodeDelayReportParameter->getReportNode(i);

		int nIndex = 0;
		//not exist
		if (!IsNodeExist(nodeDelayData, airsideReportNode, nIndex))
		{
			continue;
		}

		CAirsideNodeDelayReport::nodeDelay reportNodeDelay = nodeDelayData[nIndex];

		vector<long> vDelayTime;
		CStatisticalTools<double> statisticalTool;

		if (!PrepareData(reportNodeDelay, vDelayTime, statisticalTool))
		{
			continue;
		}

		CSummaryNodeDelayDataItem summaryNodeDelayDataItem;
		summaryNodeDelayDataItem.m_nNodeID = airsideReportNode.GetNodeID();
		summaryNodeDelayDataItem.m_nNodeType = (int)airsideReportNode.GetNodeType();

		//set data
		//total delay
		long lTotalDelayTime = 0;
		for (int i=0; i<(int)vDelayTime.size(); i++)
		{
			lTotalDelayTime += vDelayTime[i];
		}
		ElapsedTime estTotalDelay(long(lTotalDelayTime/100.0+0.5));
		summaryNodeDelayDataItem.m_summaryData.m_estTotal = estTotalDelay;

		//min delay
		long lMinDelayTime = vDelayTime[0];
		ElapsedTime estMinDelayTime(long(lMinDelayTime/100.0+0.5));
		summaryNodeDelayDataItem.m_summaryData.m_estMin = estMinDelayTime;

		//max delay
		long lMaxDelayTime = vDelayTime[vDelayTime.size()-1];
		ElapsedTime estMaxTime(long(lMaxDelayTime/100.0+0.5));
		summaryNodeDelayDataItem.m_summaryData.m_estMax = estMaxTime;

		//average delay
		ElapsedTime estAverageDelayTime(long(lTotalDelayTime/(vDelayTime.size()*100.0)+0.5));
		summaryNodeDelayDataItem.m_summaryData.m_estAverage = estAverageDelayTime;

		//Q1
		double Q1 = statisticalTool.GetPercentile(25);
		ElapsedTime estQ1(long(Q1/100.0+0.5));
		summaryNodeDelayDataItem.m_summaryData.m_estQ1 = estQ1;

		//Q2
		double Q2 = statisticalTool.GetPercentile(50);
		ElapsedTime estQ2(long(Q2/100.0+0.5));
		summaryNodeDelayDataItem.m_summaryData.m_estQ2 = estQ2;

		//Q3
		double Q3 = statisticalTool.GetPercentile(75);
		ElapsedTime estQ3(long(Q3/100.0+0.5));
		summaryNodeDelayDataItem.m_summaryData.m_estQ3 = estQ3;

		//p1
		double P1 = statisticalTool.GetPercentile(1);
		ElapsedTime estP1(long(P1/100.0+0.5));
		summaryNodeDelayDataItem.m_summaryData.m_estP1 = estP1;

		//p5
		double P5 = statisticalTool.GetPercentile(5);
		ElapsedTime estP5(long(P5/100.0+0.5));
		summaryNodeDelayDataItem.m_summaryData.m_estP5 = estP5;

		//p10
		double P10 = statisticalTool.GetPercentile(10);
		ElapsedTime estP10(long(P10/100.0+0.5));
		summaryNodeDelayDataItem.m_summaryData.m_estP10 = estP10;

		//p90
		double P90 = statisticalTool.GetPercentile(90);
		ElapsedTime estP90(long(P90/100.0+0.5));
		summaryNodeDelayDataItem.m_summaryData.m_estP90 = estP90;

		//p95
		double P95 = statisticalTool.GetPercentile(95);
		ElapsedTime estP95(long(P95/100.0+0.5));
		summaryNodeDelayDataItem.m_summaryData.m_estP95 = estP95;

		//p99
		double P99 = statisticalTool.GetPercentile(99);
		ElapsedTime estP99(long(P99/100.0+0.5));
		summaryNodeDelayDataItem.m_summaryData.m_estP99 = estP99;

		//sigma
		double dSigma = statisticalTool.GetSigma();
		ElapsedTime estSigma(long(dSigma/100.0+0.5));
		summaryNodeDelayDataItem.m_summaryData.m_estSigma = estSigma;

		m_vData.push_back(summaryNodeDelayDataItem);
	}
}

void CSummaryNodeDelayResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" Node delay Report(Summary) ");
	c2dGraphData.m_strYtitle = _T("Delay (mins)");
	c2dGraphData.m_strXtitle = _T("Node");

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

	c2dGraphData.m_vr2DChartData.resize(13);

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("NODE DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	CAirsideNodeDelayReportParameter* pairsideNodeDelayParam = (CAirsideNodeDelayReportParameter*)pParameter;
	for (int i=0; i<(int)pairsideNodeDelayParam->getFlightConstraintCount(); i++)
	{
		FlightConstraint fltCons = pairsideNodeDelayParam->getFlightConstraint(i);

		CString strFlight(_T(""));
		fltCons.screenPrint(strFlight.GetBuffer(1024));

		if (bCommaFlag)
		{
			bCommaFlag = false;
			strFooter.AppendFormat("%s", strFlight);
		}
		else
		{
			strFooter.AppendFormat(",%s", strFlight);
		}
	}
	c2dGraphData.m_strFooter = strFooter;

	std::vector<CString > vXTickTitle;

	vector<CSummaryNodeDelayDataItem>::iterator iter = m_vData.begin();
	for (; iter!=m_vData.end(); iter++)
	{	
		
		CSummaryNodeDelayDataItem summaryData = *iter;
		CAirsideReportNode rpnode;
		rpnode.SetNodeID(summaryData.m_nNodeID);
		rpnode.SetNodeType((CAirsideReportNode::ReportNodeType)summaryData.m_nNodeType);
		CString strXTickTitile = rpnode.GetNodeName() ;

		vXTickTitle.push_back(strXTickTitile);


		c2dGraphData.m_vr2DChartData[0].push_back(summaryData.m_summaryData.m_estMin.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[1].push_back(summaryData.m_summaryData.m_estAverage.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[2].push_back(summaryData.m_summaryData.m_estMax.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[3].push_back(summaryData.m_summaryData.m_estQ1.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[4].push_back(summaryData.m_summaryData.m_estQ2.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[5].push_back(summaryData.m_summaryData.m_estQ3.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[6].push_back(summaryData.m_summaryData.m_estP1.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[7].push_back(summaryData.m_summaryData.m_estP5.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[8].push_back(summaryData.m_summaryData.m_estP10.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[9].push_back(summaryData.m_summaryData.m_estP90.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[10].push_back(summaryData.m_summaryData.m_estP95.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[11].push_back(summaryData.m_summaryData.m_estP99.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[12].push_back(summaryData.m_summaryData.m_estSigma.asSeconds()/60.0);

	}
	c2dGraphData.m_vrXTickTitle = vXTickTitle;
	chartWnd.DrawChart(c2dGraphData);
}

void CSummaryNodeDelayResult::GetSummaryResult(vector<CSummaryNodeDelayDataItem>& vSummaryResult)
{
	vSummaryResult = m_vData;
}