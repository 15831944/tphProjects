#include "StdAfx.h"
#include ".\detailsegmentdelayresult.h"
#include "AirsideFlightDelayParam.h"
#include "DetailFlightDelayData.h"
#include "AirsideFlightDelayReport.h"
#include "CARC3DChart.h"


CDetailSegmentDelayResult::CDetailSegmentDelayResult(void)
: m_estStartTime(0l)
, m_estEndTime(0l)
{
}

CDetailSegmentDelayResult::~CDetailSegmentDelayResult(void)
{
	ClearAllData();
}

void CDetailSegmentDelayResult::GenerateResult(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData,CParameters *pParameter)
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

		//lDelayTimeSegmentCount++;
	}
	else
	{
		lDelayTimeSegmentCount= ClacTimeRange(estMaxDelayTime, estMinDelayTime, estUserIntervalTime);
	}

	bool bFirstFlag = false;
	for (long i=0; i<lDelayTimeSegmentCount; i++)
	{
		ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*i);
		ElapsedTime estTempMaxDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*(i + 1));

		CString strTimeRange = _T("");
		//strTimeRange.Format(_T("%s-%s"), estTempMinDelayTime.printTime(), estTempMaxDelayTime.printTime());
		strTimeRange.Format(_T("%02d:%02d-%02d:%02d"), estTempMinDelayTime.asHours(), estTempMinDelayTime.GetMinute(), estTempMaxDelayTime.asHours(), estTempMaxDelayTime.GetMinute());
		m_vTimeRange.push_back(strTimeRange);

		//the first time
		if (!bFirstFlag)
		{
			bFirstFlag = true;

			//air
			CSegmentDelayData *pSegmentDelayData = new CSegmentDelayData;
			pSegmentDelayData->m_DelaySegmentType = CAirsideFlightDelayReport::FltDelaySegment_Air;
			m_vSegmentData.push_back(pSegmentDelayData);

			//taxi
			pSegmentDelayData = new CSegmentDelayData;
			pSegmentDelayData->m_DelaySegmentType = CAirsideFlightDelayReport::FltDelaySegment_Taxi;
			m_vSegmentData.push_back(pSegmentDelayData);

			//stand
			pSegmentDelayData = new CSegmentDelayData;
			pSegmentDelayData->m_DelaySegmentType = CAirsideFlightDelayReport::FltDelaySegment_Stand;
			m_vSegmentData.push_back(pSegmentDelayData);

			//service
			pSegmentDelayData = new CSegmentDelayData;
			pSegmentDelayData->m_DelaySegmentType = CAirsideFlightDelayReport::FltDelaySegment_Service;
			m_vSegmentData.push_back(pSegmentDelayData);

			//take off
			pSegmentDelayData = new CSegmentDelayData;
			pSegmentDelayData->m_DelaySegmentType = CAirsideFlightDelayReport::FltDelaySegment_TakeOff;
			m_vSegmentData.push_back(pSegmentDelayData);
		}

		vector<CSegmentDelayData*>::iterator iter = m_vSegmentData.begin();
		
		for (; iter!=m_vSegmentData.end(); iter++)
		{
			(*iter)->m_vData.push_back(GetFlightCountInIntervalTime((*iter)->m_DelaySegmentType, pParameter, estTempMinDelayTime, estTempMaxDelayTime, fltTypeDelayData)/100);
		}
	}
}

void CDetailSegmentDelayResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" Delay vs. time of day ");
	c2dGraphData.m_strYtitle = _T("Delays (mins)");
	c2dGraphData.m_strXtitle.Format(_T("Time of day"));
	c2dGraphData.m_vrXTickTitle = m_vTimeRange;

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

	vector<CSegmentDelayData*>::iterator iter = m_vSegmentData.begin();
	for (; iter!=m_vSegmentData.end(); iter++)
	{
		switch((*iter)->m_DelaySegmentType)
		{
		case CAirsideFlightDelayReport::FltDelaySegment_Air:
			c2dGraphData.m_vrLegend.push_back(_T("Air"));
			break;

		case CAirsideFlightDelayReport::FltDelaySegment_Taxi:
			c2dGraphData.m_vrLegend.push_back(_T("Taxi"));
			break;

		case CAirsideFlightDelayReport::FltDelaySegment_Stand:
			c2dGraphData.m_vrLegend.push_back(_T("Stand"));
			break;

		case CAirsideFlightDelayReport::FltDelaySegment_Service:
			c2dGraphData.m_vrLegend.push_back(_T("Service"));
			break;

		case CAirsideFlightDelayReport::FltDelaySegment_TakeOff:
			c2dGraphData.m_vrLegend.push_back(_T("Takeoff"));
			break;

		default:
			ASSERT(false);
			break;
		}

		vector<double> vSegmentData;
		for (int i=0; i<(int)(*iter)->m_vData.size(); i++)
		{
			double dData = (*iter)->m_vData[i];
			ElapsedTime estTime(dData);
			vSegmentData.push_back(estTime.asSeconds()/60.0);
		}
		c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
	}

	chartWnd.DrawChart(c2dGraphData);
}

int CDetailSegmentDelayResult::GetFlightCountInIntervalTime(CAirsideFlightDelayReport::FltDelaySegment delaySegmentType, CParameters *pParameter, ElapsedTime& estMinDelayTime, 
															ElapsedTime& estMaxDelayTime, vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData)
{
	int nDelayTime = 0;

	switch(delaySegmentType)
	{
	case CAirsideFlightDelayReport::FltDelaySegment_Air:
		{
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
							std::vector<CAirsideFlightDelayReport::FltNodeDelayItem>::iterator nodeDelayIter = (*iter).vNodeDelay.begin();
							for (; nodeDelayIter!=(*iter).vNodeDelay.end(); nodeDelayIter++)
							{
								if (estMinDelayTime.asSeconds()*100 <= (*nodeDelayIter).eActArriveTime
									&& (*nodeDelayIter).eActArriveTime < estMaxDelayTime.asSeconds()*100
									&& (*nodeDelayIter).nSegment == CAirsideFlightDelayReport::FltDelaySegment_Air)
								{
									nDelayTime += (*nodeDelayIter).delayTime;
								}
							}
						}
					}			
				}
			}
		}
		break;

	case CAirsideFlightDelayReport::FltDelaySegment_Taxi:
		{
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
							std::vector<CAirsideFlightDelayReport::FltNodeDelayItem>::iterator nodeDelayIter = (*iter).vNodeDelay.begin();
							for (; nodeDelayIter!=(*iter).vNodeDelay.end(); nodeDelayIter++)
							{
								if (estMinDelayTime.asSeconds()*100 <= (*nodeDelayIter).eActArriveTime
									&& (*nodeDelayIter).eActArriveTime < estMaxDelayTime.asSeconds()*100
									&& (*nodeDelayIter).nSegment == CAirsideFlightDelayReport::FltDelaySegment_Taxi)
								{
									nDelayTime += (*nodeDelayIter).delayTime;
								}
							}
						}
					}			
				}
			}
		}
		break;

	case CAirsideFlightDelayReport::FltDelaySegment_Stand:
		{
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
							std::vector<CAirsideFlightDelayReport::FltNodeDelayItem>::iterator nodeDelayIter = (*iter).vNodeDelay.begin();
							for (; nodeDelayIter!=(*iter).vNodeDelay.end(); nodeDelayIter++)
							{
								if (estMinDelayTime.asSeconds()*100 <= (*nodeDelayIter).eActArriveTime
									&& (*nodeDelayIter).eActArriveTime < estMaxDelayTime.asSeconds()*100
									&& (*nodeDelayIter).nSegment == CAirsideFlightDelayReport::FltDelaySegment_Stand)
								{
									nDelayTime += (*nodeDelayIter).delayTime;
								}
							}
						}
					}			
				}
			}
		}
	    break;

	case CAirsideFlightDelayReport::FltDelaySegment_Service:
		{
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
			}
		}
	    break;

	case CAirsideFlightDelayReport::FltDelaySegment_TakeOff:
		{
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
							std::vector<CAirsideFlightDelayReport::FltNodeDelayItem>::iterator nodeDelayIter = (*iter).vNodeDelay.begin();
							for (; nodeDelayIter!=(*iter).vNodeDelay.end(); nodeDelayIter++)
							{
								if (estMinDelayTime.asSeconds()*100 <= (*nodeDelayIter).eActArriveTime
									&& (*nodeDelayIter).eActArriveTime < estMaxDelayTime.asSeconds()*100
									&& (*nodeDelayIter).nSegment == CAirsideFlightDelayReport::FltDelaySegment_TakeOff)
								{
									nDelayTime += (*nodeDelayIter).delayTime;
								}
							}
						}
					}			
				}
			}
		}
		break;

	default:
	    break;
	}
	
	return nDelayTime;
}

void CDetailSegmentDelayResult::ClearAllData()
{
	for (int i=0; i<(int)m_vSegmentData.size(); i++)
	{
		delete m_vSegmentData[i];
	}

	m_vSegmentData.clear();
	m_vTimeRange.clear();
}