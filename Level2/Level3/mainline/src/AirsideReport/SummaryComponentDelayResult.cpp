#include "StdAfx.h"
#include ".\summarycomponentdelayresult.h"
#include "../Reports/StatisticalTools.h"
#include "Parameters.h"
#include <algorithm>
#include "CARC3DChart.h"
#include "Parameters.h"
#include "AirsideFlightDelayParam.h"


CSummaryComponentDelayResult::CSummaryComponentDelayResult(void)
{
}

CSummaryComponentDelayResult::~CSummaryComponentDelayResult(void)
{
}

void CSummaryComponentDelayResult::GenerateResult(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData,CParameters *pParameter)
{
	ASSERT(pParameter != NULL);

	//slowed 
	CSummaryComponentData slowedComponentData;
	slowedComponentData.m_DelayReason = FltDelayReason_Slowed;
	m_vSummaryComponentData.push_back(slowedComponentData);

	//vectored 
	CSummaryComponentData vectoredComponentData;
	vectoredComponentData.m_DelayReason = FltDelayReason_Vectored;
	m_vSummaryComponentData.push_back(vectoredComponentData);

	//air hold 
	CSummaryComponentData airHoldComponentData;
	airHoldComponentData.m_DelayReason = FltDelayReason_AirHold;
	m_vSummaryComponentData.push_back(airHoldComponentData);

	//altitude changed
	CSummaryComponentData altitudeChangedComponentData;
	altitudeChangedComponentData.m_DelayReason = FltDelayReason_AltitudeChanged;
	m_vSummaryComponentData.push_back(altitudeChangedComponentData);

	//side step
	CSummaryComponentData sideSetpComponentData;
	sideSetpComponentData.m_DelayReason = FltDelayReason_SideStep;
	m_vSummaryComponentData.push_back(sideSetpComponentData);

	//stop
	CSummaryComponentData stopComponentData;
	stopComponentData.m_DelayReason = FltDelayReason_Stop;
	m_vSummaryComponentData.push_back(stopComponentData);

	//service
	CSummaryComponentData serviceComponentData;
	serviceComponentData.m_DelayReason = FltDelayReason_Service;
	m_vSummaryComponentData.push_back(serviceComponentData);

	for (int j=0; j<(int)m_vSummaryComponentData.size(); j++)
	{
		PrepareData(fltTypeDelayData, pParameter, m_vSummaryComponentData[j]);
	}
}

void CSummaryComponentDelayResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{

	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" Component delay Report(Summary) ");
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

	CString strName[] = {"unknown","Slowed","Vectored","Air Hold","RunwayHold" ,"Alt Changed","Side Step","PushbackClearance","Stop","Service"
	};

	c2dGraphData.m_vr2DChartData.resize(13);

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("COMPONENT DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	CAirsideFlightDelayParam* pairsideFlightDelayParam = (CAirsideFlightDelayParam*)pParameter;
	for (int i=0; i<(int)pairsideFlightDelayParam->getFlightConstraintCount(); i++)
	{
		FlightConstraint fltCons = pairsideFlightDelayParam->getFlightConstraint(i);

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

	vector<CSummaryComponentData>::iterator iter = m_vSummaryComponentData.begin();
	for (; iter!=m_vSummaryComponentData.end(); iter++)
	{
		CString strXTickTitile = strName[(*iter).m_DelayReason];
		
		vXTickTitle.push_back(strXTickTitile);

		CSummaryComponentData summaryData = *iter;
		c2dGraphData.m_vr2DChartData[0].push_back(summaryData.m_SummaryDelayData.m_estMin.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[1].push_back(summaryData.m_SummaryDelayData.m_estAverage.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[2].push_back(summaryData.m_SummaryDelayData.m_estMax.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[3].push_back(summaryData.m_SummaryDelayData.m_estQ1.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[4].push_back(summaryData.m_SummaryDelayData.m_estQ2.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[5].push_back(summaryData.m_SummaryDelayData.m_estQ3.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[6].push_back(summaryData.m_SummaryDelayData.m_estP1.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[7].push_back(summaryData.m_SummaryDelayData.m_estP5.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[8].push_back(summaryData.m_SummaryDelayData.m_estP10.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[9].push_back(summaryData.m_SummaryDelayData.m_estP90.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[10].push_back(summaryData.m_SummaryDelayData.m_estP95.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[11].push_back(summaryData.m_SummaryDelayData.m_estP99.asSeconds()/60.0);
		c2dGraphData.m_vr2DChartData[12].push_back(summaryData.m_SummaryDelayData.m_estSigma.asSeconds()/60.0);

	}
	c2dGraphData.m_vrXTickTitle = vXTickTitle;
	chartWnd.DrawChart(c2dGraphData);


}

void CSummaryComponentDelayResult::PrepareData(std::vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltDelayData, 
											   CParameters *pParameter, CSummaryComponentData& summaryComponentDelayData)
{
	ASSERT(pParameter);

	vector<long> vDelayTime;
	CStatisticalTools<double> statisticalTool;

	for (int i=0; i<(int)pParameter->getFlightConstraintCount(); i++)
	{
		FlightConstraint flightConstraint = pParameter->getFlightConstraint(i);	

		for (int i=0; i<(int)fltDelayData.size(); i++)
		{
			if (flightConstraint.fits(fltDelayData[i].m_fltCons))
			{
				vector<CAirsideFlightDelayReport::FltDelayItem>::iterator iter = fltDelayData[i].m_vDelayData.begin();
				for (; iter!=fltDelayData[i].m_vDelayData.end(); iter++)
				{
					vector<CAirsideFlightDelayReport::FltNodeDelayItem>::iterator iterNodeDelayItem = (*iter).vNodeDelay.begin();

					for (; iterNodeDelayItem!=(*iter).vNodeDelay.end(); iterNodeDelayItem++)
					{
						if (summaryComponentDelayData.m_DelayReason == (*iterNodeDelayItem).nReason)
						{
							vDelayTime.push_back((*iterNodeDelayItem).delayTime);
							statisticalTool.AddNewData((*iterNodeDelayItem).delayTime);
						}
					}					
				}
			}
		}
	}

	//if there is no delay
	if ((int)vDelayTime.size() < 1)
	{
		return;
	}

	//sort data
	statisticalTool.SortData();
	std::sort(vDelayTime.begin(), vDelayTime.end());

	//set data
	//total delay
	long lTotalDelayTime = 0;
	for (int i=0; i<(int)vDelayTime.size(); i++)
	{
		lTotalDelayTime += vDelayTime[i];
	}
	ElapsedTime estTotalDelay(long(lTotalDelayTime/100.0+0.5));
	summaryComponentDelayData.m_SummaryDelayData.m_estTotal = estTotalDelay;

	//min delay
	long lMinDelayTime = vDelayTime[0];
	ElapsedTime estMinDelayTime(long(lMinDelayTime/100.0+0.5));
	summaryComponentDelayData.m_SummaryDelayData.m_estMin = estMinDelayTime;

	//max delay
	long lMaxDelayTime = vDelayTime[vDelayTime.size()-1];
	ElapsedTime estMaxTime(long(lMaxDelayTime/100.0+0.5));
	summaryComponentDelayData.m_SummaryDelayData.m_estMax = estMaxTime;

	//average delay
	ElapsedTime estAverageDelayTime(long(lTotalDelayTime/(vDelayTime.size()*100.0)+0.5));
	summaryComponentDelayData.m_SummaryDelayData.m_estAverage = estAverageDelayTime;

	//Q1
	double Q1 = statisticalTool.GetPercentile(25);
	ElapsedTime estQ1(long(Q1/100.0+0.5));
	summaryComponentDelayData.m_SummaryDelayData.m_estQ1 = estQ1;

	//Q2
	double Q2 = statisticalTool.GetPercentile(50);
	ElapsedTime estQ2(long(Q2/100.0+0.5));
	summaryComponentDelayData.m_SummaryDelayData.m_estQ2 = estQ2;

	//Q3
	double Q3 = statisticalTool.GetPercentile(75);
	ElapsedTime estQ3(long(Q3/100.0+0.5));
	summaryComponentDelayData.m_SummaryDelayData.m_estQ3 = estQ3;

	//p1
	double P1 = statisticalTool.GetPercentile(1);
	ElapsedTime estP1(long(P1/100.0+0.5));
	summaryComponentDelayData.m_SummaryDelayData.m_estP1 = estP1;

	//p5
	double P5 = statisticalTool.GetPercentile(5);
	ElapsedTime estP5(long(P5/100.0+0.5));
	summaryComponentDelayData.m_SummaryDelayData.m_estP5 = estP5;

	//p10
	double P10 = statisticalTool.GetPercentile(10);
	ElapsedTime estP10(long(P10/100.0+0.5));
	summaryComponentDelayData.m_SummaryDelayData.m_estP10 = estP10;

	//p90
	double P90 = statisticalTool.GetPercentile(90);
	ElapsedTime estP90(long(P90/100.0+0.5));
	summaryComponentDelayData.m_SummaryDelayData.m_estP90 = estP90;

	//p95
	double P95 = statisticalTool.GetPercentile(95);
	ElapsedTime estP95(long(P95/100.0+0.5));
	summaryComponentDelayData.m_SummaryDelayData.m_estP95 = estP95;

	//p99
	double P99 = statisticalTool.GetPercentile(99);
	ElapsedTime estP99(long(P99/100.0+0.5));
	summaryComponentDelayData.m_SummaryDelayData.m_estP99 = estP99;

	//sigma
	double dSigma = statisticalTool.GetSigma();
	ElapsedTime estSigma(long(dSigma/100.0+0.5));
	summaryComponentDelayData.m_SummaryDelayData.m_estSigma = estSigma;
}