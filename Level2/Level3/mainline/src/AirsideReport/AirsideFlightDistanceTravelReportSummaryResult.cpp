#include "StdAfx.h"
#include ".\airsideflightdistancetravelreportsummaryresult.h"
#include "../common/UnitsManager.h"
#include "CARC3DChart.h"
#include "Parameters.h"
#include "../Reports/StatisticalTools.h"
#include <algorithm>
CAirsideFlightDistanceTravelReportSummaryResult::CAirsideFlightDistanceTravelReportSummaryResult(void)
{
}

CAirsideFlightDistanceTravelReportSummaryResult::~CAirsideFlightDistanceTravelReportSummaryResult(void)
{
}

void CAirsideFlightDistanceTravelReportSummaryResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" Distance Travel(Summary) ");
	c2dGraphData.m_strYtitle = _T("Distance Travel(") + pParameter->GetUnitManager()->GetLengthUnitString() +")" ;
	c2dGraphData.m_strXtitle = _T("Flight Type");

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

	for (size_t i =0; i < m_vResult.size();++ i)
	{
		CString strFltType;
		m_vResult[i].fltCons.screenPrint(strFltType);
		c2dGraphData.m_vrXTickTitle.push_back(strFltType);

		c2dGraphData.m_vr2DChartData[0].push_back(pParameter->GetUnitManager()->ConvertLength(UM_LEN_CENTIMETERS, pParameter->GetUnitManager()->GetLengthUnits(),m_vResult[i].summaryData.m_dMin));
		c2dGraphData.m_vr2DChartData[1].push_back(pParameter->GetUnitManager()->ConvertLength(UM_LEN_CENTIMETERS, pParameter->GetUnitManager()->GetLengthUnits(),m_vResult[i].summaryData.m_dAverage));
		c2dGraphData.m_vr2DChartData[2].push_back(pParameter->GetUnitManager()->ConvertLength(UM_LEN_CENTIMETERS, pParameter->GetUnitManager()->GetLengthUnits(),m_vResult[i].summaryData.m_dMax));
		c2dGraphData.m_vr2DChartData[3].push_back(pParameter->GetUnitManager()->ConvertLength(UM_LEN_CENTIMETERS, pParameter->GetUnitManager()->GetLengthUnits(),m_vResult[i].summaryData.m_dQ1));
		c2dGraphData.m_vr2DChartData[4].push_back(pParameter->GetUnitManager()->ConvertLength(UM_LEN_CENTIMETERS, pParameter->GetUnitManager()->GetLengthUnits(),m_vResult[i].summaryData.m_dQ2));
		c2dGraphData.m_vr2DChartData[5].push_back(pParameter->GetUnitManager()->ConvertLength(UM_LEN_CENTIMETERS, pParameter->GetUnitManager()->GetLengthUnits(),m_vResult[i].summaryData.m_dQ3));
		c2dGraphData.m_vr2DChartData[6].push_back(pParameter->GetUnitManager()->ConvertLength(UM_LEN_CENTIMETERS, pParameter->GetUnitManager()->GetLengthUnits(),m_vResult[i].summaryData.m_dP1));
		c2dGraphData.m_vr2DChartData[7].push_back(pParameter->GetUnitManager()->ConvertLength(UM_LEN_CENTIMETERS, pParameter->GetUnitManager()->GetLengthUnits(),m_vResult[i].summaryData.m_dP5));
		c2dGraphData.m_vr2DChartData[8].push_back(pParameter->GetUnitManager()->ConvertLength(UM_LEN_CENTIMETERS, pParameter->GetUnitManager()->GetLengthUnits(),m_vResult[i].summaryData.m_dP10));
		c2dGraphData.m_vr2DChartData[9].push_back(pParameter->GetUnitManager()->ConvertLength(UM_LEN_CENTIMETERS, pParameter->GetUnitManager()->GetLengthUnits(),m_vResult[i].summaryData.m_dP90));
		c2dGraphData.m_vr2DChartData[10].push_back(pParameter->GetUnitManager()->ConvertLength(UM_LEN_CENTIMETERS, pParameter->GetUnitManager()->GetLengthUnits(),m_vResult[i].summaryData.m_dP95));
		c2dGraphData.m_vr2DChartData[11].push_back(pParameter->GetUnitManager()->ConvertLength(UM_LEN_CENTIMETERS, pParameter->GetUnitManager()->GetLengthUnits(),m_vResult[i].summaryData.m_dP99));
		c2dGraphData.m_vr2DChartData[12].push_back(pParameter->GetUnitManager()->ConvertLength(UM_LEN_CENTIMETERS, pParameter->GetUnitManager()->GetLengthUnits(),m_vResult[i].summaryData.m_dSigma));
	}
	c2dGraphData.m_strFooter = _T(" Distance Travel(Summary) ;") + pParameter->GetParameterString();
	chartWnd.DrawChart(c2dGraphData);
}

void CAirsideFlightDistanceTravelReportSummaryResult::GenerateResult(std::vector<CAirsideDistanceTravelReport::DistanceTravelReportItem>& vResult,CParameters *pParameter)
{
	m_vResult.clear();
	size_t nFltConsCount =  pParameter->getFlightConstraintCount();
	for (size_t i =0; i < nFltConsCount;++i)
	{
		CFltTypeSummaryData summaryData(pParameter->getFlightConstraint(i));
		m_vResult.push_back(summaryData);
	}


	for (size_t j = 0; j < vResult.size(); ++j)
	{
		for (size_t k =0; k <m_vResult.size(); ++k)
		{
			if (m_vResult[k].fltCons.fits(vResult[j].fltDesc))
			{
				m_vResult[k].vResult.push_back(vResult[j].distance);
			}
		}
	}


	for (size_t m = 0; m< m_vResult.size();++m)
	{
		m_vResult[m].GenerateSummaryData();
	}
}
void CAirsideFlightDistanceTravelReportSummaryResult::CFltTypeSummaryData::GenerateSummaryData()
{
	if (vResult.size() == 0)
		return;

	CStatisticalTools<double> statisticalTool;

	std::sort(vResult.begin(),vResult.end());


	for (size_t i =0; i< vResult.size();++i)
	{
		statisticalTool.AddNewData(vResult[i]);
		summaryData.m_dTotal += vResult[i];
		summaryData.m_dCount += 1;
	}

	summaryData.m_dMin = *(vResult.begin());
	summaryData.m_dMax = *(vResult.rbegin());

	summaryData.m_dAverage = summaryData.m_dTotal/summaryData.m_dCount;
	
	summaryData.m_dQ1 = statisticalTool.GetPercentile(25);
	summaryData.m_dQ2 = statisticalTool.GetPercentile(50);
	summaryData.m_dQ3 = statisticalTool.GetPercentile(75);



	summaryData.m_dP1 = statisticalTool.GetPercentile(1);
	summaryData.m_dP5 = statisticalTool.GetPercentile(5);
	summaryData.m_dP10 = statisticalTool.GetPercentile(10);
	summaryData.m_dP90 = statisticalTool.GetPercentile(90);
	summaryData.m_dP95 = statisticalTool.GetPercentile(95);
	summaryData.m_dP99 = statisticalTool.GetPercentile(99);
	summaryData.m_dSigma = statisticalTool.GetSigma();



}





