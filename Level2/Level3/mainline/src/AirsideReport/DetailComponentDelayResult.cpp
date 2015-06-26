#include "StdAfx.h"
#include ".\detailcomponentdelayresult.h"
#include "AirsideFlightDelayParam.h"
#include "DetailFlightDelayData.h"
#include "AirsideFlightDelayReport.h"
#include "CARC3DChart.h"

CDetailComponentDelayResult::CDetailComponentDelayResult(void)
{
}

CDetailComponentDelayResult::~CDetailComponentDelayResult(void)
{
	ClearAllData();
}

void CDetailComponentDelayResult::GenerateResult(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData,CParameters *pParameter)
{
	ASSERT(pParameter != NULL);

	ClearAllData();

	ElapsedTime estMinDelayTime = pParameter->getStartTime();
	ElapsedTime estMaxDelayTime = pParameter->getEndTime();

	long lMinDelayTime = estMinDelayTime.asSeconds();
	long lMaxDelayTime = estMaxDelayTime.asSeconds();

	//delay time error
	if (lMaxDelayTime < lMinDelayTime)
	{
		return;
	}

	m_estStartTime = estMinDelayTime;
	m_estEndTime   = estMaxDelayTime;

	long lUserIntervalTime = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(lUserIntervalTime);

	long lDelayTimeSegmentCount = 0;             //the count of the delayTime segment
	if (0 < lUserIntervalTime)
	{
		lDelayTimeSegmentCount = (lMaxDelayTime - lMinDelayTime) / (lUserIntervalTime);
		estMinDelayTime = ElapsedTime((lMinDelayTime - lMinDelayTime%(lUserIntervalTime)));

	//	lDelayTimeSegmentCount++;
	}
	else
	{
		lDelayTimeSegmentCount= ClacTimeRange(estMaxDelayTime, estMinDelayTime, estUserIntervalTime);
	}

	bool bFirstFlag = true;
	for (long i=0; i<lDelayTimeSegmentCount; i++)
	{
		ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*i);
		ElapsedTime estTempMaxDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*(i + 1));

		CString strTimeRange = _T("");
		//strTimeRange.Format(_T("%s-%s"), estTempMinDelayTime.printTime(), estTempMaxDelayTime.printTime());
		strTimeRange.Format(_T("%02d:%02d-%02d:%02d"), estTempMinDelayTime.asHours(), estTempMinDelayTime.GetMinute(), estTempMaxDelayTime.asHours(), estTempMaxDelayTime.GetMinute());
		m_vTimeRange.push_back(strTimeRange);

		if (bFirstFlag)
		{
			bFirstFlag = false;

			//slowed
			CDetailComponentData* pDetailComponentData = new CDetailComponentData;
			pDetailComponentData->m_DelayReason = FltDelayReason_Slowed;
			pDetailComponentData->m_vData.push_back(GetFlightCountInIntervalTime(pDetailComponentData->m_DelayReason, pParameter, estTempMinDelayTime, estTempMaxDelayTime, fltTypeDelayData)/100);
			m_vComponentData.push_back(pDetailComponentData);

			//vectored
			pDetailComponentData = new CDetailComponentData;
			pDetailComponentData->m_DelayReason = FltDelayReason_Vectored;
			pDetailComponentData->m_vData.push_back(GetFlightCountInIntervalTime(pDetailComponentData->m_DelayReason, pParameter, estTempMinDelayTime, estTempMaxDelayTime, fltTypeDelayData)/100);
			m_vComponentData.push_back(pDetailComponentData);

			//hold
			pDetailComponentData = new CDetailComponentData;
			pDetailComponentData->m_DelayReason = FltDelayReason_AirHold;
			pDetailComponentData->m_vData.push_back(GetFlightCountInIntervalTime(pDetailComponentData->m_DelayReason, pParameter, estTempMinDelayTime, estTempMaxDelayTime, fltTypeDelayData)/100);
			m_vComponentData.push_back(pDetailComponentData);

			//alt change
			pDetailComponentData = new CDetailComponentData;
			pDetailComponentData->m_DelayReason = FltDelayReason_AltitudeChanged;
			pDetailComponentData->m_vData.push_back(GetFlightCountInIntervalTime(pDetailComponentData->m_DelayReason, pParameter, estTempMinDelayTime, estTempMaxDelayTime, fltTypeDelayData)/100);
			m_vComponentData.push_back(pDetailComponentData);

			//side step
			pDetailComponentData = new CDetailComponentData;
			pDetailComponentData->m_DelayReason = FltDelayReason_SideStep;
			pDetailComponentData->m_vData.push_back(GetFlightCountInIntervalTime(pDetailComponentData->m_DelayReason, pParameter, estTempMinDelayTime, estTempMaxDelayTime, fltTypeDelayData)/100);
			m_vComponentData.push_back(pDetailComponentData);

			//stop
			pDetailComponentData = new CDetailComponentData;
			pDetailComponentData->m_DelayReason = FltDelayReason_Stop;
			pDetailComponentData->m_vData.push_back(GetFlightCountInIntervalTime(pDetailComponentData->m_DelayReason, pParameter, estTempMinDelayTime, estTempMaxDelayTime, fltTypeDelayData)/100);
			m_vComponentData.push_back(pDetailComponentData);

			//service
			pDetailComponentData = new CDetailComponentData;
			pDetailComponentData->m_DelayReason = FltDelayReason_Service;
			pDetailComponentData->m_vData.push_back(GetFlightCountInIntervalTime(pDetailComponentData->m_DelayReason, pParameter, estTempMinDelayTime, estTempMaxDelayTime, fltTypeDelayData)/100);
			m_vComponentData.push_back(pDetailComponentData);
		}	
		else
		{
			vector<CDetailComponentData*>::iterator iter = m_vComponentData.begin();
			for (; iter!=m_vComponentData.end(); iter++)
			{
				(*iter)->m_vData.push_back(GetFlightCountInIntervalTime((*iter)->m_DelayReason, pParameter, estTempMinDelayTime, estTempMaxDelayTime, fltTypeDelayData)/100);
			}
		}
	}
}

void CDetailComponentDelayResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" Delay component vs. time of day ");
	c2dGraphData.m_strYtitle = _T("Delays (mins)");
	c2dGraphData.m_strXtitle.Format(_T("Time of day"));
	c2dGraphData.m_vrXTickTitle = m_vTimeRange;

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

	vector<CDetailComponentData*>::iterator iter = m_vComponentData.begin();
	for (; iter!=m_vComponentData.end(); iter++)
	{
		switch((*iter)->m_DelayReason)
		{
		case FltDelayReason_Slowed:
			c2dGraphData.m_vrLegend.push_back(_T("Slowed"));
			break;

		case FltDelayReason_Vectored:
			c2dGraphData.m_vrLegend.push_back(_T("Vectored"));
			break;

		case FltDelayReason_AirHold:
			c2dGraphData.m_vrLegend.push_back(_T("Hold"));
			break;

		case FltDelayReason_AltitudeChanged:
			c2dGraphData.m_vrLegend.push_back(_T("Alt change"));
			break;

		case FltDelayReason_SideStep:
			c2dGraphData.m_vrLegend.push_back(_T("Side step"));
			break;

		case FltDelayReason_Stop:
			c2dGraphData.m_vrLegend.push_back(_T("Stop"));
			break;

		case FltDelayReason_Service:
			c2dGraphData.m_vrLegend.push_back(_T("Service"));
			break;

		default:
			ASSERT(false);
			break;
		}

		vector<double> vComponentData;
		for (int i=0; i<(int)(*iter)->m_vData.size(); i++)
		{
			double dData = (*iter)->m_vData[i];
			ElapsedTime estTime(dData);
			vComponentData.push_back(estTime.asSeconds()/60.0);
		}
		c2dGraphData.m_vr2DChartData.push_back(vComponentData);
	}

	chartWnd.DrawChart(c2dGraphData);
}

int CDetailComponentDelayResult::GetFlightCountInIntervalTime(FltDelayReason delayReason, CParameters *pParameter, 
															  ElapsedTime& estMinDelayTime, ElapsedTime& estMaxDelayTime, vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData)
{
	ASSERT(pParameter);

	int nDelayTime = 0;

	for (int j=0; j<(int)pParameter->getFlightConstraintCount(); j++)
	{
		FlightConstraint flightConstraint = pParameter->getFlightConstraint(j);

		for (int i=0; i<(int)fltTypeDelayData.size(); i++)
		{
			if (flightConstraint.fits(fltTypeDelayData[i].m_fltCons))
			{
				vector<CAirsideFlightDelayReport::FltDelayItem>::iterator iter = fltTypeDelayData[i].m_vDelayData.begin();
				for (; iter!=fltTypeDelayData[i].m_vDelayData.end(); iter++)
				{
					vector<CAirsideFlightDelayReport::FltNodeDelayItem>::iterator iterNodeDelayItem = (*iter).vNodeDelay.begin();

					for (; iterNodeDelayItem!=(*iter).vNodeDelay.end(); iterNodeDelayItem++)
					{
						if (estMinDelayTime.asSeconds()*100 <= (*iterNodeDelayItem).eArriveTime
							&& (*iterNodeDelayItem).eArriveTime < estMaxDelayTime.asSeconds()*100
							&& delayReason == (*iterNodeDelayItem).nReason)
						{
							nDelayTime += (*iterNodeDelayItem).delayTime;
						}
					}						
				}
			}
		}
	}	

	return nDelayTime;
}

void CDetailComponentDelayResult::ClearAllData()
{
	for (int i=0; i<(int)m_vComponentData.size(); i++)
	{
		delete m_vComponentData[i];
	}

	m_vComponentData.clear();
	m_vTimeRange.clear();
}