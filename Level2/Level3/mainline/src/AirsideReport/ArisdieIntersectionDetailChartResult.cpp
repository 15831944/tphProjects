#include "StdAfx.h"
#include ".\arisdieintersectiondetailchartresult.h"
#include "Parameters.h"
#include "AirsideIntersectionReportParam.h"

//////////////////////////////////////////////////////////////////////////
//CArisideIntersectionDetailCrossingChartResult
CArisideIntersectionDetailCrossingChartResult::CArisideIntersectionDetailCrossingChartResult(void)
{
}

CArisideIntersectionDetailCrossingChartResult::~CArisideIntersectionDetailCrossingChartResult(void)
{
}

void CArisideIntersectionDetailCrossingChartResult::GenerateResult( vector<CAirsideIntersectionDetailResult::ResultItem>& vResult,CParameters *pParameter )
{
	int nCount = static_cast<int>(vResult.size());
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		 CAirsideIntersectionDetailResult::ResultItem& resultItem = vResult[nItem];
		
		if(m_mapNodeCorssing.find(resultItem.m_nIndex) == m_mapNodeCorssing.end())
		{
			m_mapNodeCorssing[resultItem.m_nIndex].nNodeID = resultItem.m_nIndex;
			m_mapNodeCorssing[resultItem.m_nIndex].strNodeName = resultItem.m_strNodeName;
		}

		 if(resultItem.m_vConflicts.size() > 0)
			m_mapNodeCorssing[resultItem.m_nIndex].nDelayCount += 1;
		 else
			m_mapNodeCorssing[resultItem.m_nIndex].nNonDelayCount += 1;
	}
}

void CArisideIntersectionDetailCrossingChartResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Intersection Utilization(Crossings)");
	c2dGraphData.m_strYtitle = _T("Number of crossings");
	c2dGraphData.m_strXtitle = _T("Intersections(Index)");


	c2dGraphData.m_vrLegend.push_back(_T("Total"));
	c2dGraphData.m_vrLegend.push_back(_T("No Delay"));
	c2dGraphData.m_vrLegend.push_back(_T("Delayed"));
	
	std::vector<CString> vNodeName;
	std::vector<double> vTotal;
	std::vector<double> vDelay;
	std::vector<double> vNoDelay;

	CAirsideIntersectionReportParam *pParam =(CAirsideIntersectionReportParam *)pParameter;
	if(pParam == NULL)
		return;

	int nAllNodeCount = static_cast<int>(pParam->getAllNodes().size());

	for (int nNode = 1; nNode <= nAllNodeCount; ++ nNode)
	{
		std::map<int, CrossingData>::iterator iterFind = m_mapNodeCorssing.find(nNode);
		if(iterFind != m_mapNodeCorssing.end())
		{
			//node id
			CString strNodeID;
			strNodeID.Format(_T("%d"),(*iterFind).second.nNodeID);
			vNodeName.push_back(strNodeID);
			vTotal.push_back((*iterFind).second.nDelayCount + (*iterFind).second.nNonDelayCount);
			vDelay.push_back((*iterFind).second.nDelayCount);
			vNoDelay.push_back((*iterFind).second.nNonDelayCount);
		}
		else
		{
			CString strNodeID;
			strNodeID.Format(_T("%d"),nNode);
			vNodeName.push_back(strNodeID);
			vTotal.push_back(0);
			vDelay.push_back(0);
			vNoDelay.push_back(0);
		}
	}




	c2dGraphData.m_vrXTickTitle = vNodeName;
	c2dGraphData.m_vr2DChartData.push_back(vTotal);
	c2dGraphData.m_vr2DChartData.push_back(vNoDelay);
	c2dGraphData.m_vr2DChartData.push_back(vDelay);

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Intersection Utilization(Crossings) %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	//std::vector<RunwayCrossingDataItem>::iterator iter = m_vData.begin();
	//for (; iter!=m_vData.end(); ++iter)
	//{
	//	CString strLegend = _T("");
	//	strLegend = (*iter).m_runwayID.GetIDString();
	//	c2dGraphData.m_vrLegend.push_back(strLegend);
	//	c2dGraphData.m_vr2DChartData.push_back((*iter).m_vCrossingsData);
	//}
	chartWnd.DrawChart(c2dGraphData);
}


//////////////////////////////////////////////////////////////////////////
//

CArisideIntersectionDetailBusyChartResult::CArisideIntersectionDetailBusyChartResult()
{

}

CArisideIntersectionDetailBusyChartResult::~CArisideIntersectionDetailBusyChartResult()
{

}

void CArisideIntersectionDetailBusyChartResult::GenerateResult( vector<CAirsideIntersectionDetailResult::ResultItem>& vResult,CParameters *pParameter )
{
	int nCount = static_cast<int>(vResult.size());
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		CAirsideIntersectionDetailResult::ResultItem& resultItem = vResult[nItem];

		if(m_mapNodeBusyFactor.find(resultItem.m_nIndex) == m_mapNodeBusyFactor.end())
		{
			m_mapNodeBusyFactor[resultItem.m_nIndex].nNodeID = resultItem.m_nIndex;
			m_mapNodeBusyFactor[resultItem.m_nIndex].strNodeName = resultItem.m_strNodeName;
		}


		m_mapNodeBusyFactor[resultItem.m_nIndex].eTime += resultItem.getDurationBusy();
	}
}

void CArisideIntersectionDetailBusyChartResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	if(pParameter == NULL)
		return;

	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Intersection Utilization(Busy Factor)");
	c2dGraphData.m_strYtitle = _T("Duration(mins)");
	c2dGraphData.m_strXtitle = _T("Intersections(Index)");


	c2dGraphData.m_vrLegend.push_back(_T("Free"));
	c2dGraphData.m_vrLegend.push_back(_T("Occupied"));

	std::vector<CString> vNodeName;
	std::vector<double> vFree;
	std::vector<double> vOccupied;

	ElapsedTime eReportTime = pParameter->getEndTime() - pParameter->getStartTime();


	CAirsideIntersectionReportParam *pParam =(CAirsideIntersectionReportParam *)pParameter;
	if(pParam == NULL)
		return;

	int nAllNodeCount = static_cast<int>(pParam->getAllNodes().size());

	for (int nNode = 1; nNode <= nAllNodeCount; ++ nNode)
	{
		std::map<int, BusyFactorData> ::iterator iterFind = m_mapNodeBusyFactor.find(nNode);
		if(iterFind != m_mapNodeBusyFactor.end())
		{
			//node id
			CString strNodeID;
			strNodeID.Format(_T("%d"),(*iterFind).second.nNodeID);
			vNodeName.push_back(strNodeID);
			//vNodeName.push_back((*iterNode).second.strNodeName);
			vFree.push_back(eReportTime.asMinutes() - (*iterFind).second.eTime.asMinutes());
			vOccupied.push_back((*iterFind).second.eTime.asMinutes());
		}
		else
		{
			CString strNodeID;
			strNodeID.Format(_T("%d"),nNode);
			vNodeName.push_back(strNodeID);
			vFree.push_back(eReportTime.asMinutes());
			vOccupied.push_back(0);
		}
	}



	//std::map<int, BusyFactorData> ::iterator iterNode = m_mapNodeBusyFactor.begin();
	//for (;iterNode != m_mapNodeBusyFactor.end(); ++iterNode)
	//{
	//	//node id
	//	CString strNodeID;
	//	strNodeID.Format(_T("%d"),(*iterNode).second.nNodeID);
	//	vNodeName.push_back(strNodeID);
	//	//vNodeName.push_back((*iterNode).second.strNodeName);
	//	vFree.push_back(eReportTime.asMinutes() - (*iterNode).second.eTime.asMinutes());
	//	vOccupied.push_back((*iterNode).second.eTime.asMinutes());

	//}
	c2dGraphData.m_vrXTickTitle = vNodeName;
	c2dGraphData.m_vr2DChartData.push_back(vFree);
	c2dGraphData.m_vr2DChartData.push_back(vOccupied);

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Intersection Utilization(Busy Factor) %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);
}

//////////////////////////////////////////////////////////////////////////
//CArisideIntersectionDetailDelayChartResult
CArisideIntersectionDetailDelayChartResult::CArisideIntersectionDetailDelayChartResult()
{

}

CArisideIntersectionDetailDelayChartResult::~CArisideIntersectionDetailDelayChartResult()
{

}

void CArisideIntersectionDetailDelayChartResult::GenerateResult( vector<CAirsideIntersectionDetailResult::ResultItem>& vResult,CParameters *pParameter )
{
	int nCount = static_cast<int>(vResult.size());
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		CAirsideIntersectionDetailResult::ResultItem& resultItem = vResult[nItem];

		if(m_mapNodeDelayTime.find(resultItem.m_nIndex) == m_mapNodeDelayTime.end())
		{
			m_mapNodeDelayTime[resultItem.m_nIndex].nNodeID = resultItem.m_nIndex;
			m_mapNodeDelayTime[resultItem.m_nIndex].strNodeName = resultItem.m_strNodeName;
		}
		m_mapNodeDelayTime[resultItem.m_nIndex].eTime += resultItem.getTotalDelayTime();
	}
}

void CArisideIntersectionDetailDelayChartResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Intersection Delay(mins)");
	c2dGraphData.m_strYtitle = _T("Duration(mins)");
	c2dGraphData.m_strXtitle = _T("Intersections(Index)");


	c2dGraphData.m_vrLegend.push_back(_T("Delay Time"));

	std::vector<CString> vNodeName;
	std::vector<double> vDelayTime;

	CAirsideIntersectionReportParam *pParam =(CAirsideIntersectionReportParam *)pParameter;
	if(pParam == NULL)
		return;

	int nAllNodeCount = static_cast<int>(pParam->getAllNodes().size());

	for (int nNode = 1; nNode <= nAllNodeCount; ++ nNode)
	{
		std::map<int, DelayTimeData> ::iterator iterFind = m_mapNodeDelayTime.find(nNode);
		if(iterFind != m_mapNodeDelayTime.end())
		{
			//node id
			CString strNodeID;
			strNodeID.Format(_T("%d"),(*iterFind).second.nNodeID);
			vNodeName.push_back(strNodeID);

			//vNodeName.push_back((*iterNode).second.strNodeName);
			vDelayTime.push_back((*iterFind).second.eTime.asMinutes());
		}
		else
		{
			CString strNodeID;
			strNodeID.Format(_T("%d"),nNode);
			vNodeName.push_back(strNodeID);
			vDelayTime.push_back(0);
		}
	}

	//std::map<int, DelayTimeData>::iterator iterNode = m_mapNodeDelayTime.begin();
	//for (;iterNode != m_mapNodeDelayTime.end(); ++iterNode)
	//{
	//	//node id
	//	CString strNodeID;
	//	strNodeID.Format(_T("%d"),(*iterNode).second.nNodeID);
	//	vNodeName.push_back(strNodeID);

	//	//vNodeName.push_back((*iterNode).second.strNodeName);
	//	vDelayTime.push_back((*iterNode).second.eTime.asMinutes());

	//}
	c2dGraphData.m_vrXTickTitle = vNodeName;
	c2dGraphData.m_vr2DChartData.push_back(vDelayTime);

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Intersection Delay(mins) %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);
}






//////////////////////////////////////////////////////////////////////////
//CArisideIntersectionDetailConflictChartResult
CArisideIntersectionDetailConflictChartResult::CArisideIntersectionDetailConflictChartResult()
{

}

CArisideIntersectionDetailConflictChartResult::~CArisideIntersectionDetailConflictChartResult()
{

}

void CArisideIntersectionDetailConflictChartResult::GenerateResult( vector<CAirsideIntersectionDetailResult::ResultItem>& vResult,CParameters *pParameter )
{
	int nCount = static_cast<int>(vResult.size());
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		CAirsideIntersectionDetailResult::ResultItem& resultItem = vResult[nItem];

		if(m_mapNodeConflict.find(resultItem.m_nIndex) == m_mapNodeConflict.end())
		{
			m_mapNodeConflict[resultItem.m_nIndex].nNodeID = resultItem.m_nIndex;
			m_mapNodeConflict[resultItem.m_nIndex].strNodeName = resultItem.m_strNodeName;
		}


		m_mapNodeConflict[resultItem.m_nIndex].nCount += resultItem.GetDelayCount();
	}
}

void CArisideIntersectionDetailConflictChartResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Intersection Conflicts");
	c2dGraphData.m_strYtitle = _T("Number of Conflicts");
	c2dGraphData.m_strXtitle = _T("Intersections(Index)");


	c2dGraphData.m_vrLegend.push_back(_T("Conflict Count"));

	std::vector<CString> vNodeName;
	std::vector<double> vConflictCount;


	CAirsideIntersectionReportParam *pParam =(CAirsideIntersectionReportParam *)pParameter;
	if(pParam == NULL)
		return;

	int nAllNodeCount = static_cast<int>(pParam->getAllNodes().size());

	for (int nNode = 1; nNode <= nAllNodeCount; ++ nNode)
	{
		std::map<int, ConflictCountData> ::iterator iterFind = m_mapNodeConflict.find(nNode);
		if(iterFind != m_mapNodeConflict.end())
		{
			//node id
			CString strNodeID;
			strNodeID.Format(_T("%d"),(*iterFind).second.nNodeID);
			vNodeName.push_back(strNodeID);
			//vNodeName.push_back((*iterNode).second.strNodeName);
			vConflictCount.push_back((*iterFind).second.nCount);
		}
		else
		{
			CString strNodeID;
			strNodeID.Format(_T("%d"),nNode);
			vNodeName.push_back(strNodeID);
			vConflictCount.push_back(0);

		}
	}

	//std::map<int, ConflictCountData >::iterator iterNode = m_mapNodeConflict.begin();
	//for (;iterNode != m_mapNodeConflict.end(); ++iterNode)
	//{
	//	//node id
	//	CString strNodeID;
	//	strNodeID.Format(_T("%d"),(*iterNode).second.nNodeID);
	//	vNodeName.push_back(strNodeID);
	//	//vNodeName.push_back((*iterNode).second.strNodeName);
	//	vConflictCount.push_back((*iterNode).second.nCount);

	//}
	c2dGraphData.m_vrXTickTitle = vNodeName;
	c2dGraphData.m_vr2DChartData.push_back(vConflictCount);

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Intersection Conflicts %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);
}

 






















