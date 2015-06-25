#include "StdAfx.h"
#include "AirsideRunwayCrossingsBaseSummaryResult.h"
#include "Parameters.h"
#include <algorithm>
CRunwayCrossingBaseSummaryResult::CRunwayCrossingBaseSummaryResult()
{

}

CRunwayCrossingBaseSummaryResult::~CRunwayCrossingBaseSummaryResult()
{

}

std::vector<std::pair<CString,ALTObjectID> > CRunwayCrossingBaseSummaryResult::GetRunwayList(std::vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayCrossingsItem>& vResult)
{
	std::vector<std::pair<CString,ALTObjectID> > vRunwayList;
	vRunwayList.clear();
	for (int i = 0; i < (int)vResult.size(); i++)
	{
		ALTObjectID runwayID(vResult[i].m_sRunway);
		std::pair<CString,ALTObjectID> itemPair;
		itemPair.first = vResult[i].m_sNodeName;
		itemPair.second = runwayID;
		if (std::find(vRunwayList.begin(),vRunwayList.end(),itemPair) == vRunwayList.end())
		{
			vRunwayList.push_back(std::make_pair(vResult[i].m_sNodeName,runwayID));
		}
	}
	return vRunwayList;
}
//////////////////////////////////////////////////////////////////////////////////////////////
CRunwayCrossingSummaryCountResult::CRunwayCrossingSummaryCountResult()
{

}

CRunwayCrossingSummaryCountResult::~CRunwayCrossingSummaryCountResult()
{

}

void CRunwayCrossingSummaryCountResult::GenerateResult(vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayCrossingsItem>& vResult,CParameters *pParameter)
{
	m_vResult = vResult;
}

void CRunwayCrossingSummaryCountResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T("Summary runway crossings");
	c2dGraphData.m_strYtitle = _T("Number of Operations");
	c2dGraphData.m_strXtitle = _T("Runway entry point");

	std::vector<std::pair<CString,ALTObjectID> > vNodeNameList = GetRunwayList(m_vResult);
	int nNodeCount = (int)vNodeNameList.size();
	if (nNodeCount == 0)
		return;

	for (int nNode = 0; nNode < nNodeCount; ++nNode)
	{
		c2dGraphData.m_vrXTickTitle.push_back(vNodeNameList[nNode].first);
	}



	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());

	c2dGraphData.m_strFooter = strFooter;


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


	int nResultCount = (int)m_vResult.size();
	for (int nIndex = 0; nIndex < nResultCount; ++ nIndex)
	{
		CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayCrossingsItem* statisticItem = NULL;
		statisticItem = &(m_vResult[nIndex]);

		if(statisticItem == NULL)
			continue;

		//min
		vLegendData[0].push_back((*statisticItem).m_lMinCrossings);
		vLegendData[1].push_back((*statisticItem).m_dAverageCrosings);
		vLegendData[2].push_back((*statisticItem).m_lMaxCrossings);
		vLegendData[3].push_back((*statisticItem).m_staWaitTime.m_nQ1);
		vLegendData[4].push_back((*statisticItem).m_staWaitTime.m_nQ2);
		vLegendData[5].push_back((*statisticItem).m_staWaitTime.m_nQ3);
		vLegendData[6].push_back((*statisticItem).m_staWaitTime.m_nP1);
		vLegendData[7].push_back((*statisticItem).m_staWaitTime.m_nP5);
		vLegendData[8].push_back((*statisticItem).m_staWaitTime.m_nP10);
		vLegendData[9].push_back((*statisticItem).m_staWaitTime.m_nP90);
		vLegendData[10].push_back((*statisticItem).m_staWaitTime.m_nP95);
		vLegendData[11].push_back((*statisticItem).m_staWaitTime.m_nP99);
		vLegendData[12].push_back((*statisticItem).m_staWaitTime.m_nStdDev);
	}

	int nLegendCount = vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	chartWnd.DrawChart(c2dGraphData);
}
//////////////////////////////////////////////////////////////////////////////////////////////
CRunwayCrossingSummaryWaitTimeResult::CRunwayCrossingSummaryWaitTimeResult()
{

}

CRunwayCrossingSummaryWaitTimeResult::~CRunwayCrossingSummaryWaitTimeResult()
{

}

void CRunwayCrossingSummaryWaitTimeResult::GenerateResult(vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayCrossingsItem>& vResult,CParameters *pParameter)
{
	m_vResult = vResult;
}

void CRunwayCrossingSummaryWaitTimeResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Summary Runway wait times");
	c2dGraphData.m_strYtitle = _T("Number of crossings");
	c2dGraphData.m_strXtitle = _T("Runway entry point");	

	std::vector<std::pair<CString,ALTObjectID> > vNodeNameList = GetRunwayList(m_vResult);
	int nNodeCount = (int)vNodeNameList.size();
	if (nNodeCount == 0)
		return;

	for (int nNode = 0; nNode < nNodeCount; ++nNode)
	{
		c2dGraphData.m_vrXTickTitle.push_back(vNodeNameList[nNode].first);
	}



	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());

	c2dGraphData.m_strFooter = strFooter;


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


	int nResultCount = (int)m_vResult.size();
	for (int nIndex = 0; nIndex < nResultCount; ++ nIndex)
	{
		CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayCrossingsItem* statisticItem = NULL;
		statisticItem = &(m_vResult[nIndex]);

		if(statisticItem == NULL)
			continue;

		//min
		vLegendData[0].push_back((*statisticItem).m_lMinWaitTime);
		vLegendData[1].push_back((*statisticItem).m_lAverageWaitTime);
		vLegendData[2].push_back((*statisticItem).m_lMaxWaitTime);
		vLegendData[3].push_back((*statisticItem).m_staCrossingCount.m_nQ1);
		vLegendData[4].push_back((*statisticItem).m_staCrossingCount.m_nQ2);
		vLegendData[5].push_back((*statisticItem).m_staCrossingCount.m_nQ3);
		vLegendData[6].push_back((*statisticItem).m_staCrossingCount.m_nP1);
		vLegendData[7].push_back((*statisticItem).m_staCrossingCount.m_nP5);
		vLegendData[8].push_back((*statisticItem).m_staCrossingCount.m_nP10);
		vLegendData[9].push_back((*statisticItem).m_staCrossingCount.m_nP90);
		vLegendData[10].push_back((*statisticItem).m_staCrossingCount.m_nP95);
		vLegendData[11].push_back((*statisticItem).m_staCrossingCount.m_nP99);
		vLegendData[12].push_back((*statisticItem).m_staCrossingCount.m_nStdDev);
	}

	int nLegendCount = vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	chartWnd.DrawChart(c2dGraphData);
}