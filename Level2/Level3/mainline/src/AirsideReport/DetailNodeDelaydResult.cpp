#include "StdAfx.h"
#include ".\detailnodedelaydresult.h"
#include "AirsideNodeDelayReport.h"
#include "Parameters.h"
#include "AirsideNodeDelayReportParameter.h"
#include "CommonMethod.h"

CDetailNodeDelaydResult::CDetailNodeDelaydResult(void)
{
}

CDetailNodeDelaydResult::~CDetailNodeDelaydResult(void)
{
	m_vData.clear();
}

void CDetailNodeDelaydResult::GenerateResult(std::vector<CAirsideNodeDelayReport::nodeDelay>& nodeDelayData,CParameters *pParameter)
{
	ASSERT(pParameter);
	m_vData.clear();

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

//		lDelayTimeSegmentCount++;
	}
	else
	{
		lDelayTimeSegmentCount= ClacTimeRange(estMaxDelayTime, estMinDelayTime, estUserIntervalTime);
	}

	for (long i=0; i<lDelayTimeSegmentCount; i++)
	{
		ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*i);
		ElapsedTime estTempMaxDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*(i + 1));

		CString strTimeRange = _T("");
		//strTimeRange.Format(_T("%s-%s"), estTempMinDelayTime.printTime(), estTempMaxDelayTime.printTime());
		strTimeRange.Format(_T("%02d:%02d-%02d:%02d"), estTempMinDelayTime.asHours(), estTempMinDelayTime.GetMinute(), estTempMaxDelayTime.asHours(), estTempMaxDelayTime.GetMinute());
		m_vTimeRange.push_back(strTimeRange);

		GetNodeDelayTimeInIntervalTime(estTempMinDelayTime, estTempMaxDelayTime, nodeDelayData, pParameter);
	}
}

bool CDetailNodeDelaydResult::IsNodeSelected(CAirsideNodeDelayReport::nodeDelay reportNodeDelay, CParameters *pParameter)
{
	ASSERT(pParameter);

	CAirsideNodeDelayReportParameter* airsideNodeDelayReportParameter = (CAirsideNodeDelayReportParameter*)pParameter;
	for (int i=0; i<(int)airsideNodeDelayReportParameter->getReportNodeCount(); i++)
	{
		CAirsideReportNode airsideReportNode = airsideNodeDelayReportParameter->getReportNode(i);

		if (reportNodeDelay.node.GetNodeID() == airsideReportNode.GetNodeID()
			&& reportNodeDelay.node.GetNodeType() == airsideReportNode.GetNodeType())
		{
			return true;
		}
	}

	return false;
}

void CDetailNodeDelaydResult::GetNodeDelayTimeInIntervalTime(ElapsedTime& estStartTime, ElapsedTime& estEndTime, vector<CAirsideNodeDelayReport::nodeDelay>& nodeDelayData, CParameters *pParameter)
{
	ASSERT(pParameter);

	for (int i=0; i<(int)nodeDelayData.size(); i++)
	{
		CAirsideNodeDelayReport::nodeDelay reportNodeDelay = nodeDelayData[i];

		if (!IsNodeSelected(reportNodeDelay, pParameter))
		{
			continue;
		}

		//calculate the delay time in time range
		long lDelayTime = 0;
		for (int i=0; i<(int)reportNodeDelay.vDelayItem.size(); i++)
		{
			//int the time range
			if (reportNodeDelay.vDelayItem[i].lEventTime >= estStartTime.asSeconds()*100
				&& reportNodeDelay.vDelayItem[i].lEventTime < estEndTime.asSeconds()*100)
			{
				lDelayTime += reportNodeDelay.vDelayItem[i].delayTime;
			}
		}

		int nIndex = 0;
		//exist
		if (IsNodeInResult(reportNodeDelay, nIndex))
		{
			m_vData[nIndex].m_lDelayTime.push_back(lDelayTime);
		}
		//doesn't exist
		else
		{
			CDetailNodeDelayDataItem detailNodeDelayDataItem;
			detailNodeDelayDataItem.m_nNodeID = reportNodeDelay.node.GetNodeID();
			detailNodeDelayDataItem.m_nNodeType = (int)reportNodeDelay.node.GetNodeType();
			detailNodeDelayDataItem.m_lDelayTime.push_back(lDelayTime);

			m_vData.push_back(detailNodeDelayDataItem);
		}		
	}
}

bool CDetailNodeDelaydResult::IsNodeInResult(CAirsideNodeDelayReport::nodeDelay reportNodeDelay, int& nIndex)
{
	reportNodeDelay.node.GetNodeID();
	for (int i=0; i<(int)m_vData.size(); i++)
	{
		if (m_vData[i].m_nNodeID == reportNodeDelay.node.GetNodeID()
			&& m_vData[i].m_nNodeType == (int)reportNodeDelay.node.GetNodeType())
		{
			nIndex = i;
			return true;
		}		
	}

	return false;
}

long CDetailNodeDelaydResult::GetMinDelayTime(vector<CAirsideNodeDelayReport::nodeDelay>& nodeDelayData)
{
	long lMinDelayTime = 0;

	bool bInitFlag = true;

	vector<CAirsideNodeDelayReport::nodeDelay>::iterator iter = nodeDelayData.begin();
	for (; iter!=nodeDelayData.end(); iter++)
	{
		vector<CAirsideNodeDelayReport::nodeDelayItem>::iterator nodeDelayItemIter = (*iter).vDelayItem.begin();
		for (; nodeDelayItemIter!=(*iter).vDelayItem.end(); nodeDelayItemIter++)
		{
			if (bInitFlag)
			{
				bInitFlag = false;
				lMinDelayTime = (*nodeDelayItemIter).lEventTime;
			}
			else
			{
				if ((*nodeDelayItemIter).lEventTime < lMinDelayTime)
				{
					lMinDelayTime = (*nodeDelayItemIter).lEventTime;
				}
			}
		}
	}

	return lMinDelayTime;
}

long CDetailNodeDelaydResult::GetMaxDelayTime(vector<CAirsideNodeDelayReport::nodeDelay>& nodeDelayData)
{
	long lMaxDelayTime = 0;

	bool bInitFlag = true;

	vector<CAirsideNodeDelayReport::nodeDelay>::iterator iter = nodeDelayData.begin();
	for (; iter!=nodeDelayData.end(); iter++)
	{
		vector<CAirsideNodeDelayReport::nodeDelayItem>::iterator nodeDelayItemIter = (*iter).vDelayItem.begin();
		for (; nodeDelayItemIter!=(*iter).vDelayItem.end(); nodeDelayItemIter++)
		{
			if (bInitFlag)
			{
				bInitFlag = false;
				lMaxDelayTime = (*nodeDelayItemIter).lEventTime;
			}
			else
			{
				if ((*nodeDelayItemIter).lEventTime > lMaxDelayTime)
				{
					lMaxDelayTime = (*nodeDelayItemIter).lEventTime;
				}
			}
		}
	}

	return lMaxDelayTime;
}

long CDetailNodeDelaydResult::ClacTimeRange(ElapsedTime& eMaxTime,ElapsedTime& eMinValue,ElapsedTime& eInterval)
{
	if (eMinValue == eMaxTime)
	{
		eMinValue = ElapsedTime(0L);
	}
	bool bInSecond = false;
	int _nMinNumCol = 3;
	ElapsedTime timeDiff = eMaxTime - eMinValue;

	ElapsedTime eUnitTimeOnX;

	int nTimeDiffAsHours = timeDiff.asHours();
	int nTimeDiffAsMin = timeDiff.asMinutes();
	int nTimeDiffAsSec = timeDiff.asSeconds();

	if( nTimeDiffAsHours > _nMinNumCol )
	{
		// if hh  > _nMinNumCol, display column up to 2*_nMinNumCol
		int nMinHours = nTimeDiffAsHours / (2*_nMinNumCol) + 1;
		eUnitTimeOnX.set( nMinHours, 0, 0 );
		eMinValue.SetMinute( 0 );
		eMinValue.SetSecond( 0 );
		if( nTimeDiffAsHours > (2*_nMinNumCol) )
			_nMinNumCol = (2*_nMinNumCol);
		else
			_nMinNumCol = nTimeDiffAsHours;
	}
	else if( nTimeDiffAsMin > _nMinNumCol )
	{
		// else if hhmm > nNumCol min, so the column unit should be hhmm % 2 nNumCol min
		int nMinMinutes = nTimeDiffAsMin / ( 2*_nMinNumCol ) + 1;
		eUnitTimeOnX.set( nMinMinutes / 60, nMinMinutes % 60, 0 );
		eMinValue.SetSecond( 0 );
		if( nTimeDiffAsMin > 2 * _nMinNumCol )
			_nMinNumCol = 2 * _nMinNumCol;
		else
			_nMinNumCol = nTimeDiffAsMin;
	}
	else if( nTimeDiffAsSec > _nMinNumCol )
	{
		// be hhmmss % 2 nNumCol sec.
		int mMinSeconds = nTimeDiffAsSec / ( 2*_nMinNumCol ) + 1;
		eUnitTimeOnX.set( mMinSeconds / 3600, mMinSeconds / 60, mMinSeconds % 60 );
		if( nTimeDiffAsSec > 2 * _nMinNumCol )
			_nMinNumCol = 2 * _nMinNumCol;
		else
			_nMinNumCol = nTimeDiffAsSec;
		bInSecond = true;
	}

	eInterval = eUnitTimeOnX;
	if (eInterval == ElapsedTime(0L))
	{
		eInterval = ElapsedTime(60L);
	}

	return _nMinNumCol;
}

void CDetailNodeDelaydResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" Node delay vs. total delay ");
	c2dGraphData.m_strYtitle = _T("Delays(mins)");
	c2dGraphData.m_strXtitle = _T("Delays(mins)");	
	c2dGraphData.m_vrXTickTitle = m_vTimeRange;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("NODE DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	CAirsideNodeDelayReportParameter* pairsideNodeDelayParam = (CAirsideNodeDelayReportParameter*)pParameter;
	for (int i=0; i<(int)pairsideNodeDelayParam->getFlightConstraintCount(); i++)
	{
		FlightConstraint fltCons = pairsideNodeDelayParam->getFlightConstraint(i);

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

	vector<CDetailNodeDelayDataItem>::iterator iter = m_vData.begin();
	for (; iter!=m_vData.end(); iter++)
	{
		CString strLegend = _T("");
		strLegend = CommonMethod::GetObjectName((*iter).m_nNodeType, (*iter).m_nNodeID);
		c2dGraphData.m_vrLegend.push_back(strLegend);

		int nSegmentCount = (int)(*iter).m_lDelayTime.size();

		vector<double> vSegmentData;

		for (int i=0; i<nSegmentCount; i++)
		{
			double dData = (*iter).m_lDelayTime[i];
			ElapsedTime estTime(long(dData/100.0+0.5));
			vSegmentData.push_back(estTime.asSeconds()/60.0);
		}

		c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
	}

	chartWnd.DrawChart(c2dGraphData);
}