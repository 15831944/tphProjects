#include "StdAfx.h"
#include ".\airsideintersectionsummarychartresult.h"
#include "Parameters.h"
#include "AirsideIntersectionReportParam.h"
CAirsideIntersectionSummaryBaseChartResult::CAirsideIntersectionSummaryBaseChartResult(void)
{
	m_pResultList = NULL;
}

CAirsideIntersectionSummaryBaseChartResult::~CAirsideIntersectionSummaryBaseChartResult(void)
{

}

void CAirsideIntersectionSummaryBaseChartResult::GenerateResult( std::vector<CAirsideIntersectionSummarylResult::NodeSummaryResultItem>& vResult,CParameters *pParameter )
{
	m_pResultList = &vResult;
}






//////////////////////////////////////////////////////////////////////////
//CAirsideIntersectionSummaryCrossingTimeChartResult
CAirsideIntersectionSummaryCrossingTimeChartResult::CAirsideIntersectionSummaryCrossingTimeChartResult( void )
{

}

CAirsideIntersectionSummaryCrossingTimeChartResult::~CAirsideIntersectionSummaryCrossingTimeChartResult( void )
{

}

void CAirsideIntersectionSummaryCrossingTimeChartResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	if(m_pResultList == NULL)
		return;
	std::vector<CAirsideIntersectionSummarylResult::NodeSummaryResultItem>& vResult = *m_pResultList;


	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T("Intersection Crossings(Summary)");
	c2dGraphData.m_strYtitle = _T("Crossing Time(Buffer) (secs)");
	c2dGraphData.m_strXtitle = _T("Date Time");


	//get node data
	CAirsideIntersectionReportParam *pParam = (CAirsideIntersectionReportParam *)pParameter;
	if(pParam == NULL)
		return;

	int nNodeID = pParam->getSummaryNodeID();
	int nNodeCount = static_cast<int>(vResult.size());
	CAirsideIntersectionSummarylResult::NodeSummaryResultItem *pNodeSummaryResult = NULL;
	for (int nNode = 0; nNode < nNodeCount; ++nNode)
	{
		if(vResult[nNode].m_nNodeID == nNodeID)
		{
			pNodeSummaryResult = &vResult[nNode];
			break;
		}
	}

	if(pNodeSummaryResult == NULL)
		return;



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
	vLegends.push_back(_T("Sigma"));


	std::vector< std::vector<double> > vLegendData;
	vLegendData.resize(vLegends.size());


	int nResultCount = (int)pNodeSummaryResult->m_vIntervalItem.size();
	for (int nIndex = 0; nIndex < nResultCount; ++ nIndex)
	{
		CAirsideIntersectionSummarylResult::SummaryResultItem& statisticItem = pNodeSummaryResult->m_vIntervalItem[nIndex];


		c2dGraphData.m_vrXTickTitle.push_back(statisticItem.GetInterval());
		//min
		vLegendData[0].push_back(statisticItem.summaryCrossing.m_minTime.asSeconds());
		vLegendData[1].push_back(statisticItem.summaryCrossing.m_avgTime.asSeconds());
		vLegendData[2].push_back(statisticItem.summaryCrossing.m_maxTime.asSeconds());
		vLegendData[3].push_back(statisticItem.summaryCrossing.m_nQ1.asSeconds());
		vLegendData[4].push_back(statisticItem.summaryCrossing.m_nQ2.asSeconds());
		vLegendData[5].push_back(statisticItem.summaryCrossing.m_nQ3.asSeconds());
		vLegendData[6].push_back(statisticItem.summaryCrossing.m_nP1.asSeconds());
		vLegendData[7].push_back(statisticItem.summaryCrossing.m_nP5.asSeconds());
		vLegendData[8].push_back(statisticItem.summaryCrossing.m_nP10.asSeconds());
		vLegendData[9].push_back(statisticItem.summaryCrossing.m_nP90.asSeconds());
		vLegendData[10].push_back(statisticItem.summaryCrossing.m_nP95.asSeconds());
		vLegendData[11].push_back(statisticItem.summaryCrossing.m_nP99.asSeconds());
		vLegendData[12].push_back(statisticItem.summaryCrossing.m_nStdDev.asSeconds());
	}

	int nLegendCount = vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());

	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);
}






//////////////////////////////////////////////////////////////////////////
//CAirsideIntersectionSummaryCrossingDelayChartResult
CAirsideIntersectionSummaryCrossingDelayChartResult::CAirsideIntersectionSummaryCrossingDelayChartResult( void )
{

}

CAirsideIntersectionSummaryCrossingDelayChartResult::~CAirsideIntersectionSummaryCrossingDelayChartResult( void )
{

}

void CAirsideIntersectionSummaryCrossingDelayChartResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	if(m_pResultList == NULL)
		return;
	std::vector<CAirsideIntersectionSummarylResult::NodeSummaryResultItem>& vResult = *m_pResultList;


	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T("Intersection Delay(Summary)");
	c2dGraphData.m_strYtitle = _T("Delay Time (secs)");
	c2dGraphData.m_strXtitle = _T("Date Time");


	//get node data
	CAirsideIntersectionReportParam *pParam = (CAirsideIntersectionReportParam *)pParameter;
	if(pParam == NULL)
		return;

	int nNodeID = pParam->getSummaryNodeID();
	int nNodeCount = static_cast<int>(vResult.size());
	CAirsideIntersectionSummarylResult::NodeSummaryResultItem *pNodeSummaryResult = NULL;
	for (int nNode = 0; nNode < nNodeCount; ++nNode)
	{
		if(vResult[nNode].m_nNodeID == nNodeID)
		{
			pNodeSummaryResult = &vResult[nNode];
			break;
		}
	}

	if(pNodeSummaryResult == NULL)
		return;



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
	vLegends.push_back(_T("Sigma"));


	std::vector< std::vector<double> > vLegendData;
	vLegendData.resize(vLegends.size());


	int nResultCount = (int)pNodeSummaryResult->m_vIntervalItem.size();
	for (int nIndex = 0; nIndex < nResultCount; ++ nIndex)
	{
		CAirsideIntersectionSummarylResult::SummaryResultItem& statisticItem = pNodeSummaryResult->m_vIntervalItem[nIndex];


		c2dGraphData.m_vrXTickTitle.push_back(statisticItem.GetInterval());
		//min
		vLegendData[0].push_back(statisticItem.summaryDelay.m_minTime.asSeconds());
		vLegendData[1].push_back(statisticItem.summaryDelay.m_avgTime.asSeconds());
		vLegendData[2].push_back(statisticItem.summaryDelay.m_maxTime.asSeconds());
		vLegendData[3].push_back(statisticItem.summaryDelay.m_nQ1.asSeconds());
		vLegendData[4].push_back(statisticItem.summaryDelay.m_nQ2.asSeconds());
		vLegendData[5].push_back(statisticItem.summaryDelay.m_nQ3.asSeconds());
		vLegendData[6].push_back(statisticItem.summaryDelay.m_nP1.asSeconds());
		vLegendData[7].push_back(statisticItem.summaryDelay.m_nP5.asSeconds());
		vLegendData[8].push_back(statisticItem.summaryDelay.m_nP10.asSeconds());
		vLegendData[9].push_back(statisticItem.summaryDelay.m_nP90.asSeconds());
		vLegendData[10].push_back(statisticItem.summaryDelay.m_nP95.asSeconds());
		vLegendData[11].push_back(statisticItem.summaryDelay.m_nP99.asSeconds());
		vLegendData[12].push_back(statisticItem.summaryDelay.m_nStdDev.asSeconds());
	}

	int nLegendCount = vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());

	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);
}