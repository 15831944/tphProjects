#include "StdAfx.h"
#include "CSummaryFlightScheduleDelay.h"
#include <algorithm>
#include "CARC3DChart.h"
#include "Parameters.h"
#include "AirsideFlightDelayParam.h"

CSummaryFlightScheduleDelayResult::CSummaryFlightScheduleDelayResult(void)
{
}

CSummaryFlightScheduleDelayResult::~CSummaryFlightScheduleDelayResult(void)
{
	ClearAllData();
}

long CSummaryFlightScheduleDelayResult::GetFlightScheduleDelayTime(CAirsideFlightDelayReport::FltDelayItem& fltDelayItem)const
{
	if (fltDelayItem.bArrOrDeplDelay)
	{
		return max(fltDelayItem.actEndTime - fltDelayItem.planSt,0l);
	}

	return max(fltDelayItem.actStartTime - fltDelayItem.planSt,0l);
}

bool CSummaryFlightScheduleDelayResult::PrepareData(std::vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltDelayData, FlightConstraint& fltConstraint, vector<long>& vDelayTime, CStatisticalTools<double>& statisticalTool)
{
	for (int i=0; i<(int)fltDelayData.size(); i++)
	{
		if (fltConstraint.fits(fltDelayData[i].m_fltCons))
		{
			vector<CAirsideFlightDelayReport::FltDelayItem>::iterator iter = fltDelayData[i].m_vDelayData.begin();
			for (; iter!=fltDelayData[i].m_vDelayData.end(); iter++)
			{
				long scheduleDealy = GetFlightScheduleDelayTime(*iter);
				vDelayTime.push_back(scheduleDealy);
				statisticalTool.AddNewData(scheduleDealy);
			}
		}
	}

	//if there is no delay
	if ((int)vDelayTime.size() < 1)
	{
		return false;
	}

	//sort data
	statisticalTool.SortData();
	std::sort(vDelayTime.begin(), vDelayTime.end());

	return true;
}

void CSummaryFlightScheduleDelayResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" Schedule delay Report(Summary) ");
	c2dGraphData.m_strYtitle = _T("Delay (mins)");
	c2dGraphData.m_strXtitle = _T("Flight Type");

	Init3DChartLegend(c2dGraphData);

	c2dGraphData.m_vr2DChartData.resize(13);

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("SCHEDULE DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
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

	vector<CSummaryFlightTypeData>::iterator iter = m_vData.begin();
	for (; iter!=m_vData.end(); iter++)
	{
		CString strXTickTitile = _T("");
		(*iter).m_fltConstraint.screenPrint(strXTickTitile);
		vXTickTitle.push_back(strXTickTitile);

		CSummaryFlightTypeData summaryData = *iter;
		Fill3DChartData(summaryData.m_summaryData,c2dGraphData);

	}
	c2dGraphData.m_vrXTickTitle = vXTickTitle;
	chartWnd.DrawChart(c2dGraphData);
}