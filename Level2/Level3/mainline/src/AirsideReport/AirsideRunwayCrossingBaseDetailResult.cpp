#include "StdAfx.h"
#include "AirsideRunwayCrossingBaseDetailResult.h"
#include "Parameters.h"
#include <algorithm>

CRunwayCrossingBaseDetailResult::CRunwayCrossingBaseDetailResult()
{

}

CRunwayCrossingBaseDetailResult::~CRunwayCrossingBaseDetailResult()
{

}

std::vector<ALTObjectID> CRunwayCrossingBaseDetailResult::GetRunwayList(vector<CAirsideRunwayCrossingBaseDetailResult::DetailRunwayCrossingsItem>& vResult)
{
	std::vector<ALTObjectID> vRunwayList;
	vRunwayList.clear();
	for (int i = 0; i < (int)vResult.size(); i++)
	{
		ALTObjectID runwayID(vResult[i].m_sRunway);
		if (std::find(vRunwayList.begin(),vRunwayList.end(),runwayID) == vRunwayList.end())
		{
			vRunwayList.push_back(runwayID);
		}
	}
	return vRunwayList;
}

std::vector<CString> CRunwayCrossingBaseDetailResult::GetNodeNameList(vector<CAirsideRunwayCrossingBaseDetailResult::DetailRunwayCrossingsItem>& vResult)
{
	std::vector<CString> vNodeList;
	vNodeList.clear();
	for (int i = 0; i < (int)vResult.size(); i++)
	{
		if (std::find(vNodeList.begin(),vNodeList.end(),vResult[i].m_sNodeName) == vNodeList.end())
		{
			vNodeList.push_back(vResult[i].m_sNodeName);
		}
	}
	return vNodeList;
}
///////////////////////////////////////////////////////////////////////////////////////////////
CRunwayCrossingDetailCountResult::CRunwayCrossingDetailCountResult()
{

}

CRunwayCrossingDetailCountResult::~CRunwayCrossingDetailCountResult()
{

}

void CRunwayCrossingDetailCountResult::GenerateResult(vector<CAirsideRunwayCrossingBaseDetailResult::DetailRunwayCrossingsItem>& vResult,CParameters *pParameter)
{
	ASSERT(pParameter);

	int intervalSize = 0 ;
	ElapsedTime IntervalTime ;
	IntervalTime = (pParameter->getEndTime() - pParameter->getStartTime()) ;
	intervalSize = IntervalTime.asSeconds() / pParameter->getInterval() ;

	long lUserIntervalTime = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(lUserIntervalTime);

	CString timeInterval ;
	ElapsedTime startTime = pParameter->getStartTime();
	ElapsedTime endtime ;
	for (int i = 0 ; i < intervalSize ;i++)
	{
		endtime = startTime + ElapsedTime(pParameter->getInterval()) ;
		timeInterval.Format(_T("%s-%s"),startTime.printTime(),endtime.printTime()) ;
		startTime = endtime ;
		m_vTimeRange.push_back(timeInterval) ;
	}
	std::vector<ALTObjectID>vRunwayList;
	vRunwayList = GetRunwayList(vResult);

	ElapsedTime estMinDelayTime = pParameter->getStartTime();
	for (int i = 0; i < (int)vRunwayList.size(); i++)
	{
		RunwayCrossingDataItem item;
		item.m_runwayID= vRunwayList[i];
		for (long j=0; j<intervalSize; j++)
		{
			int nCount = 0;
			ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*j);
			ElapsedTime estTempMaxDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*(j + 1));
			for (int n = 0; n < (int)vResult.size(); n++)
			{
				CAirsideRunwayCrossingBaseDetailResult::DetailRunwayCrossingsItem& crossingsItem = vResult[n];
				if (!crossingsItem.m_sRunway.CompareNoCase(item.m_runwayID.GetIDString())\
					&& crossingsItem.m_lEnterRunwayTime >= estTempMinDelayTime.asSeconds()\
					&& crossingsItem.m_lEnterRunwayTime < estTempMaxDelayTime.asSeconds())
				{
					nCount++;
				}
			}
			item.m_vCrossingsData.push_back(nCount);
		}
		m_vData.push_back(item);
	}
}

void CRunwayCrossingDetailCountResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Detailed Runway Crossings");
	c2dGraphData.m_strYtitle = _T("Number of crossings");
	c2dGraphData.m_strXtitle = _T("Data and Time of Day");	
	c2dGraphData.m_vrXTickTitle = m_vTimeRange;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Detailed Runway Crossings %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	std::vector<RunwayCrossingDataItem>::iterator iter = m_vData.begin();
	for (; iter!=m_vData.end(); ++iter)
	{
		CString strLegend = _T("");
		strLegend = (*iter).m_runwayID.GetIDString();
		c2dGraphData.m_vrLegend.push_back(strLegend);
		c2dGraphData.m_vr2DChartData.push_back((*iter).m_vCrossingsData);
	}
	chartWnd.DrawChart(c2dGraphData);
}
///////////////////////////////////////////////////////////////////////////////////////////////
CRunwayCrossingDetailWaitTimeResult::CRunwayCrossingDetailWaitTimeResult()
{

}

CRunwayCrossingDetailWaitTimeResult::~CRunwayCrossingDetailWaitTimeResult()
{

}

void CRunwayCrossingDetailWaitTimeResult::GenerateResult(vector<CAirsideRunwayCrossingBaseDetailResult::DetailRunwayCrossingsItem>& vResult,CParameters *pParameter)
{	
	ASSERT(pParameter);

	int intervalSize = 0 ;
	ElapsedTime IntervalTime ;
	IntervalTime = (pParameter->getEndTime() - pParameter->getStartTime()) ;
	intervalSize = IntervalTime.asSeconds() / pParameter->getInterval() ;

	long lUserIntervalTime = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(lUserIntervalTime);

	CString timeInterval ;
	ElapsedTime startTime = pParameter->getStartTime();
	ElapsedTime endtime ;
	for (int i = 0 ; i < intervalSize ;i++)
	{
		endtime = startTime + ElapsedTime(pParameter->getInterval()) ;
		timeInterval.Format(_T("%s-%s"),startTime.printTime(),endtime.printTime()) ;
		startTime = endtime ;
		m_vTimeRange.push_back(timeInterval) ;
	}
	std::vector<ALTObjectID>vRunwayList;
	vRunwayList = GetRunwayList(vResult);

	ElapsedTime estMinDelayTime = pParameter->getStartTime();
	for (int i = 0; i < (int)vRunwayList.size(); i++)
	{
		RunwayCrossingDataItem item;
		item.m_runwayID = vRunwayList[i];
		for (long j=0; j<intervalSize; j++)
		{
			long waitTime = 0;
			ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*j);
			ElapsedTime estTempMaxDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*(j + 1));
			for (int n = 0; n < (int)vResult.size(); n++)
			{
				CAirsideRunwayCrossingBaseDetailResult::DetailRunwayCrossingsItem& crossingsItem = vResult[n];
				if (!crossingsItem.m_sRunway.CompareNoCase(item.m_runwayID.GetIDString())\
					&& crossingsItem.m_lEnterRunwayTime >= estTempMinDelayTime.asSeconds()\
					&& crossingsItem.m_lEnterRunwayTime < estTempMaxDelayTime.asSeconds())
				{
					waitTime+= crossingsItem.m_lWaitTimeFroRunwayCrossings;
				}
			}
			item.m_vCrossingsData.push_back(waitTime);
		}
		m_vData.push_back(item);
	}
}

void CRunwayCrossingDetailWaitTimeResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Runway Crossing Wait Times");
	c2dGraphData.m_strYtitle = _T("Wait time for crossing (secs)");
	c2dGraphData.m_strXtitle = _T("Data and Time of Day");	
	c2dGraphData.m_vrXTickTitle = m_vTimeRange;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Runway Crossing Wait Times %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	std::vector<RunwayCrossingDataItem>::iterator iter = m_vData.begin();
	for (; iter!=m_vData.end(); ++iter)
	{
		CString strLegend = _T("");
		strLegend = (*iter).m_runwayID.GetIDString();
		c2dGraphData.m_vrLegend.push_back(strLegend);
		c2dGraphData.m_vr2DChartData.push_back((*iter).m_vCrossingsData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

///////////////////////////////////////////////////////////////////////////////////////////////
CRunwayCrossingDetailRunwayBayTaxiwayResult::CRunwayCrossingDetailRunwayBayTaxiwayResult()
{

}

CRunwayCrossingDetailRunwayBayTaxiwayResult::~CRunwayCrossingDetailRunwayBayTaxiwayResult()
{

}

void CRunwayCrossingDetailRunwayBayTaxiwayResult::GenerateResult(vector<CAirsideRunwayCrossingBaseDetailResult::DetailRunwayCrossingsItem>& vResult,CParameters *pParameter)
{
	ASSERT(pParameter);

	m_vCountData.clear();
	int intervalSize = 0 ;
	ElapsedTime IntervalTime ;
	IntervalTime = (pParameter->getEndTime() - pParameter->getStartTime()) ;
	intervalSize = IntervalTime.asSeconds() / pParameter->getInterval() ;

	long lUserIntervalTime = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(lUserIntervalTime);

	CString timeInterval ;
	ElapsedTime startTime = pParameter->getStartTime();
	ElapsedTime endtime ;
	for (int i = 0 ; i < intervalSize ;i++)
	{
		endtime = startTime + ElapsedTime(pParameter->getInterval()) ;
		timeInterval.Format(_T("%s-%s"),startTime.printTime(),endtime.printTime()) ;
		startTime = endtime ;
		m_vTimeRange.push_back(timeInterval) ;
	}
	std::vector<CString>vNodeList;
	vNodeList = GetNodeNameList(vResult);

	ElapsedTime estMinDelayTime = pParameter->getStartTime();
	for (int i = 0; i < (int)vNodeList.size(); i++)
	{
		RunwayCrossingNodeData item;
		item.m_sNodeName = vNodeList[i];
		for (long j=0; j<intervalSize; j++)
		{
			int nCount = 0;
			ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*j);
			ElapsedTime estTempMaxDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*(j + 1));
			for (int n = 0; n < (int)vResult.size(); n++)
			{
				CAirsideRunwayCrossingBaseDetailResult::DetailRunwayCrossingsItem& crossingsItem = vResult[n];
				if (!crossingsItem.m_sNodeName.CompareNoCase(item.m_sNodeName)\
					&& crossingsItem.m_lEnterRunwayTime >= estTempMinDelayTime.asSeconds()\
					&& crossingsItem.m_lEnterRunwayTime < estTempMaxDelayTime.asSeconds())
				{
					nCount++;
				}
			}
			item.m_vCrossingCount.push_back(nCount);
		}
		m_vCountData.push_back(item);
	}
}

void CRunwayCrossingDetailRunwayBayTaxiwayResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Runway crossings by taxiway");
	c2dGraphData.m_strYtitle = _T("Number of crossings");
	c2dGraphData.m_strXtitle = _T("Data and Time of Day");	
	c2dGraphData.m_vrXTickTitle = m_vTimeRange;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Runway crossings by taxiway %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	std::vector<RunwayCrossingNodeData>::iterator iter = m_vCountData.begin();
	for (; iter!=m_vCountData.end(); ++iter)
	{
		CString strLegend = _T("");
		strLegend = (*iter).m_sNodeName;
		c2dGraphData.m_vrLegend.push_back(strLegend);
		c2dGraphData.m_vr2DChartData.push_back((*iter).m_vCrossingCount);
	}
	chartWnd.DrawChart(c2dGraphData);
}