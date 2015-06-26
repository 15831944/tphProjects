#include "StdAfx.h"
#include ".\detailtotaldelayresult.h"
#include "AirsideFlightDelayParam.h"
#include "DetailFlightDelayData.h"
#include "AirsideFlightDelayReport.h"
#include "CARC3DChart.h"

CDetailTotalDelayResult::CDetailTotalDelayResult(void)
{
}

CDetailTotalDelayResult::~CDetailTotalDelayResult(void)
{
	ClearAllData();
}

int CDetailTotalDelayResult::GetFlightCountInIntervalTime(FlightConstraint& fltConstraint, ElapsedTime& estMinDelayTime, ElapsedTime& estMaxDelayTime, 
								                          vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData)
{
	int nDelayCount = 0;

	for (int i=0; i<(int)fltTypeDelayData.size(); i++)
	{
		if (fltConstraint.fits(fltTypeDelayData[i].m_fltCons))
		{
			vector<CAirsideFlightDelayReport::FltDelayItem>::iterator iter = fltTypeDelayData[i].m_vDelayData.begin();
			for (; iter!=fltTypeDelayData[i].m_vDelayData.end(); iter++)
			{
				if (estMinDelayTime.asSeconds()*100 <= (*iter).totalDelayTime
					&& (*iter).totalDelayTime < estMaxDelayTime.asSeconds()*100)
				{
					nDelayCount++;
				}
			}
		}
	}

	return nDelayCount;
}

void CDetailTotalDelayResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" Number of flights vs. total delay ");
	c2dGraphData.m_strYtitle = _T("Number of flights");
	c2dGraphData.m_strXtitle = _T("Delay (hh:mm )");	

	c2dGraphData.m_vrXTickTitle = m_vTimeRange;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("TOTAL DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
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
			vSegmentData.push_back(dData);
		}
		
		c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
	}

	chartWnd.DrawChart(c2dGraphData);
}

void CDetailTotalDelayResult::GenerateResult(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData,CParameters *pParameter)
{
	ASSERT(pParameter != NULL);

	ClearAllData();

	long lMinDelayTime = GetMinDelayTime(fltTypeDelayData);
	long lMaxDelayTime = GetMaxDelayTime(fltTypeDelayData);

	//delay time error
	if (lMaxDelayTime < lMinDelayTime)
	{
		return;
	}

	ElapsedTime estMinDelayTime = ElapsedTime(long(lMinDelayTime/100.0+0.5));
	ElapsedTime estMaxDelayTime = ElapsedTime(long(lMaxDelayTime/100.0+0.5));

	long lUserIntervalTime = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(lUserIntervalTime);

	long lDelayTimeSegmentCount = 0;             //the count of the delayTime segment
	if (0 < lUserIntervalTime)
	{
        long lActMinDelayTime = lMinDelayTime - lMinDelayTime%(lUserIntervalTime*100);
        estMinDelayTime = ElapsedTime(lActMinDelayTime/100);

        lDelayTimeSegmentCount = (lMaxDelayTime-lActMinDelayTime) / (lUserIntervalTime*100);
        if((lMaxDelayTime-lActMinDelayTime)%(lUserIntervalTime*100) != 0)
            lDelayTimeSegmentCount += 1;
	}
	else
	{
		lDelayTimeSegmentCount= ClacTimeRange(estMaxDelayTime, estMinDelayTime, estUserIntervalTime);
	}

    m_estStartTime = estMinDelayTime;
    m_estEndTime   = estMaxDelayTime;

	bool bSetTimeRange = false;
	for (int j=0; j<(int)pParameter->getFlightConstraintCount(); j++)
	{
		FlightConstraint flightConstraint = pParameter->getFlightConstraint(j);

		CDetailFlightDelayData *pDetailFlightDelayData = new CDetailFlightDelayData;
		pDetailFlightDelayData->m_fltCons = flightConstraint;

		for (long i=0; i<lDelayTimeSegmentCount; i++)
		{
			ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*i);
			ElapsedTime estTempMaxDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*(i + 1));

			if (!bSetTimeRange)
			{
				CString strTimeRange = _T("");
				//strTimeRange.Format(_T("%s-%s"), estTempMinDelayTime.printTime(), estTempMaxDelayTime.printTime());
				strTimeRange.Format(_T("%02d:%02d-%02d:%02d"), estTempMinDelayTime.asHours(), estTempMinDelayTime.GetMinute(), estTempMaxDelayTime.asHours(), estTempMaxDelayTime.GetMinute());
				m_vTimeRange.push_back(strTimeRange);
			}			

			pDetailFlightDelayData->m_vData.push_back(GetFlightCountInIntervalTime(flightConstraint, estTempMinDelayTime, estTempMaxDelayTime, fltTypeDelayData));
		}

		bSetTimeRange = true;

		m_vFlightData.push_back(pDetailFlightDelayData);
	}
}

long CDetailTotalDelayResult::GetMinDelayTime(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData)
{
	vector<CAirsideFlightDelayReport::FltTypeDelayItem>::iterator iter = fltTypeDelayData.begin();

	vector<CAirsideFlightDelayReport::FltDelayItem>::iterator iterItem = (*iter).m_vDelayData.begin();

	long lMinDelayTime = 0;

	bool bInitFlag = true;
	for (; iter!=fltTypeDelayData.end(); iter++)
	{
		iterItem = (*iter).m_vDelayData.begin();

		for (; iterItem!=(*iter).m_vDelayData.end(); iterItem++)
		{
			if (bInitFlag)
			{
				bInitFlag = false;
				lMinDelayTime = (*iterItem).totalDelayTime;
			}
			else
			{
				if ((*iterItem).totalDelayTime < lMinDelayTime)
				{
					lMinDelayTime = (*iterItem).totalDelayTime;
				}
			}
		}
	}

	return lMinDelayTime;
}

long CDetailTotalDelayResult::GetMaxDelayTime(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData)
{
	vector<CAirsideFlightDelayReport::FltTypeDelayItem>::iterator iter = fltTypeDelayData.begin();

	vector<CAirsideFlightDelayReport::FltDelayItem>::iterator iterItem = (*iter).m_vDelayData.begin();

	long lMaxDelayTime = 0;

	bool bInitFlag = true;
	for (; iter!=fltTypeDelayData.end(); iter++)
	{
		iterItem = (*iter).m_vDelayData.begin();

		for (; iterItem!=(*iter).m_vDelayData.end(); iterItem++)
		{
			if (bInitFlag)
			{
				bInitFlag = false;
				lMaxDelayTime = (*iterItem).totalDelayTime;
			}
			else
			{
				if ((*iterItem).totalDelayTime > lMaxDelayTime)
				{
					lMaxDelayTime = (*iterItem).totalDelayTime;
				}
			}
		}
	}

	return lMaxDelayTime;
}