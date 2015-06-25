#include "StdAfx.h"
#include ".\summarysegmentdelayresult.h"
#include "Parameters.h"
#include "../Reports/StatisticalTools.h"
#include <algorithm>
#include "CARC3DChart.h"
#include "Parameters.h"
#include "AirsideFlightDelayParam.h"


CSummarySegmentDelayResult::CSummarySegmentDelayResult(void)
{
}

CSummarySegmentDelayResult::~CSummarySegmentDelayResult(void)
{
	m_vSummarySegDelayData.clear();
}

void CSummarySegmentDelayResult::GenerateResult(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData,CParameters *pParameter)
{
	ASSERT(pParameter != NULL);

	//air segment
	CSummarySegDelayData airSegDelayData;
	airSegDelayData.m_DelaySegmentType = CAirsideFlightDelayReport::FltDelaySegment_Air;
	m_vSummarySegDelayData.push_back(airSegDelayData);

	//taxi segment
	CSummarySegDelayData taxiSegDelayData;
	taxiSegDelayData.m_DelaySegmentType = CAirsideFlightDelayReport::FltDelaySegment_Taxi;
	m_vSummarySegDelayData.push_back(taxiSegDelayData);

	//stand segment
	CSummarySegDelayData standSegDelayData;
	standSegDelayData.m_DelaySegmentType = CAirsideFlightDelayReport::FltDelaySegment_Stand;
	m_vSummarySegDelayData.push_back(standSegDelayData);

	//Service segment
	CSummarySegDelayData serviceSegDelayData;
	serviceSegDelayData.m_DelaySegmentType = CAirsideFlightDelayReport::FltDelaySegment_Service;
	m_vSummarySegDelayData.push_back(serviceSegDelayData);

	//Takeoff segment
	CSummarySegDelayData takeoffSegDelayData;
	takeoffSegDelayData.m_DelaySegmentType = CAirsideFlightDelayReport::FltDelaySegment_TakeOff;
	m_vSummarySegDelayData.push_back(takeoffSegDelayData);

	for (int j=0; j<(int)m_vSummarySegDelayData.size(); j++)
	{
		PrepareData(fltTypeDelayData, pParameter, m_vSummarySegDelayData[j]);
	}
}

void CSummarySegmentDelayResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" Segment delay Report(Summary) ");
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

	//Init3DChartLegend(c2dGraphData);

	CString strName[] = {"unknown","Air","Taxi","Stand","Service","Take Off"
	};




	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("SEGMENT DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
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

	c2dGraphData.m_vr2DChartData.resize(13);
	vector<CSummarySegDelayData>::iterator iter = m_vSummarySegDelayData.begin();
	for (; iter!=m_vSummarySegDelayData.end(); iter++)
	{
		CString strXTickTitile =strName[(*iter).m_DelaySegmentType] ;
		
		vXTickTitle.push_back(strXTickTitile);

		CSummarySegDelayData summaryData = *iter;
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

void CSummarySegmentDelayResult::PrepareData(std::vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltDelayData, CParameters *pParameter, CSummarySegDelayData& summarySegDelayData)
{
	ASSERT(pParameter);

	vector<long> vDelayTime;
	CStatisticalTools<double> statisticalTool;

	switch(summarySegDelayData.m_DelaySegmentType)
	{
	case CAirsideFlightDelayReport::FltDelaySegment_Air:
		{
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
							vDelayTime.push_back((*iter).airDelayTime);
							statisticalTool.AddNewData(((*iter).airDelayTime));
						}
					}
				}
			}
		}
		break;

	case CAirsideFlightDelayReport::FltDelaySegment_Taxi:
		{
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
							vDelayTime.push_back((*iter).taxiDelayTime);
							statisticalTool.AddNewData(((*iter).taxiDelayTime));
						}
					}
				}
			}
		}
		break;

	case CAirsideFlightDelayReport::FltDelaySegment_Stand:
		{
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
							vDelayTime.push_back((*iter).standDelayTime);
							statisticalTool.AddNewData(((*iter).standDelayTime));
						}
					}
				}
			}
		}
		break;

	case CAirsideFlightDelayReport::FltDelaySegment_Service:
		{
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
							vDelayTime.push_back((*iter).serviceDelayTime);
							statisticalTool.AddNewData(((*iter).serviceDelayTime));
						}
					}
				}
			}
		}
		break;

	case CAirsideFlightDelayReport::FltDelaySegment_TakeOff:
		{
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
							vDelayTime.push_back((*iter).takeoffDelayTime);
							statisticalTool.AddNewData(((*iter).takeoffDelayTime));
						}
					}
				}
			}
		}
		break;

	default:
		ASSERT(false);
		break;
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
	summarySegDelayData.m_SummaryDelayData.m_estTotal = estTotalDelay;

	//min delay
	long lMinDelayTime = vDelayTime[0];
	ElapsedTime estMinDelayTime(long(lMinDelayTime/100.0+0.5));
	summarySegDelayData.m_SummaryDelayData.m_estMin = estMinDelayTime;

	//max delay
	long lMaxDelayTime = vDelayTime[vDelayTime.size()-1];
	ElapsedTime estMaxTime(long(lMaxDelayTime/100.0+0.5));
	summarySegDelayData.m_SummaryDelayData.m_estMax = estMaxTime;

	//average delay
	ElapsedTime estAverageDelayTime(long(lTotalDelayTime/(vDelayTime.size()*100.0)+0.5));
	summarySegDelayData.m_SummaryDelayData.m_estAverage = estAverageDelayTime;

	//Q1
	double Q1 = statisticalTool.GetPercentile(25);
	ElapsedTime estQ1(long(Q1/100.0+0.5));
	summarySegDelayData.m_SummaryDelayData.m_estQ1 = estQ1;

	//Q2
	double Q2 = statisticalTool.GetPercentile(50);
	ElapsedTime estQ2(long(Q2/100.0+0.5));
	summarySegDelayData.m_SummaryDelayData.m_estQ2 = estQ2;

	//Q3
	double Q3 = statisticalTool.GetPercentile(75);
	ElapsedTime estQ3(long(Q3/100.0+0.5));
	summarySegDelayData.m_SummaryDelayData.m_estQ3 = estQ3;

	//p1
	double P1 = statisticalTool.GetPercentile(1);
	ElapsedTime estP1(long(P1/100.0+0.5));
	summarySegDelayData.m_SummaryDelayData.m_estP1 = estP1;

	//p5
	double P5 = statisticalTool.GetPercentile(5);
	ElapsedTime estP5(long(P5/100.0+0.5));
	summarySegDelayData.m_SummaryDelayData.m_estP5 = estP5;

	//p10
	double P10 = statisticalTool.GetPercentile(10);
	ElapsedTime estP10(long(P10/100.0+0.5));
	summarySegDelayData.m_SummaryDelayData.m_estP10 = estP10;

	//p90
	double P90 = statisticalTool.GetPercentile(90);
	ElapsedTime estP90(long(P90/100.0+0.5));
	summarySegDelayData.m_SummaryDelayData.m_estP90 = estP90;

	//p95
	double P95 = statisticalTool.GetPercentile(95);
	ElapsedTime estP95(long(P95/100.0+0.5));
	summarySegDelayData.m_SummaryDelayData.m_estP95 = estP95;

	//p99
	double P99 = statisticalTool.GetPercentile(99);
	ElapsedTime estP99(long(P99/100.0+0.5));
	summarySegDelayData.m_SummaryDelayData.m_estP99 = estP99;

	//sigma
	double dSigma = statisticalTool.GetSigma();
	ElapsedTime estSigma(long(dSigma/100.0+0.5));
	summarySegDelayData.m_SummaryDelayData.m_estSigma = estSigma;
}