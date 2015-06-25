#include "StdAfx.h"
#include ".\detailservicedelayresult.h"
#include "AirsideFlightDelayParam.h"
#include "DetailFlightDelayData.h"
#include "AirsideFlightDelayReport.h"
#include "CARC3DChart.h"

CDetailServiceDelayResult::CDetailServiceDelayResult(void)
{
}

CDetailServiceDelayResult::~CDetailServiceDelayResult(void)
{
	ClearAllData();
}

int CDetailServiceDelayResult::GetFlightCountInIntervalTime(FlightConstraint& fltConstraint, ElapsedTime& estMinDelayTime, ElapsedTime& estMaxDelayTime, 
														  vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData)
{
	int nDelayTime = 0;

	for (int i=0; i<(int)fltTypeDelayData.size(); i++)
	{
		if (fltConstraint.fits(fltTypeDelayData[i].m_fltCons))
		{
			vector<CAirsideFlightDelayReport::FltDelayItem>::iterator iter = fltTypeDelayData[i].m_vDelayData.begin();
			for (; iter!=fltTypeDelayData[i].m_vDelayData.end(); iter++)
			{
				std::vector<CAirsideFlightDelayReport::FltNodeDelayItem>::iterator nodeDelayIter = (*iter).vNodeDelay.begin();
				for (; nodeDelayIter!=(*iter).vNodeDelay.end(); nodeDelayIter++)
				{
					if (estMinDelayTime.asSeconds()*100 <= (*nodeDelayIter).eActArriveTime
						&& (*nodeDelayIter).eActArriveTime < estMaxDelayTime.asSeconds()*100
						&& (*nodeDelayIter).nSegment == CAirsideFlightDelayReport::FltDelaySegment_Service)
					{
						nDelayTime += (*nodeDelayIter).delayTime;
					}
				}
			}
		}
	}

	return nDelayTime;
}

void CDetailServiceDelayResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" GSE Induced Delay vs. time of day ");
	c2dGraphData.m_strYtitle = _T("Delays(mins)");
	c2dGraphData.m_strXtitle.Format(_T("Time of day"));	
	c2dGraphData.m_vrXTickTitle = m_vTimeRange;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("GSE INDUCED DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
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

	vector<CDetailFlightDelayData*>::iterator iter = m_vFlightData.begin();
	for (; iter!=m_vFlightData.end(); iter++)
	{
		CString strLegend = _T("");
		(*iter)->m_fltCons.screenPrint(strLegend);
		c2dGraphData.m_vrLegend.push_back(strLegend);

		int nSegmentCount = (int)(*iter)->m_vData.size();

		vector<double> vSegmentData;

		for (int i=0; i<nSegmentCount; i++)
		{
			double dData = (*iter)->m_vData[i];
			ElapsedTime estTime(dData);
			vSegmentData.push_back(estTime.asSeconds()/60.0);
		}

		c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
	}

	chartWnd.DrawChart(c2dGraphData);
}