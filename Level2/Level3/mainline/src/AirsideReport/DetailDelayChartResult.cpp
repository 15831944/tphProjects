#include "StdAfx.h"
#include ".\detaildelaychartresult.h"
#include "AirsideFlightRunwayDelayReport.h"
#include "AirsideFlightRunwayDelayData.h"
#include "AirsideFlightRunwayDelayReportPara.h"
#include <algorithm>

DetailDelayChartResult::DetailDelayChartResult()
:CAirsideReportBaseResult()
{
	m_pData = NULL;
}

DetailDelayChartResult::~DetailDelayChartResult(void)
{
}

void DetailDelayChartResult::GenerateResult(AirsideFlightRunwayDelayData* pData)
{
	m_pData = pData;
}

bool CompareByLogicRunway(AirsideFlightRunwayDelayData::RunwayDelayItem* fItem,AirsideFlightRunwayDelayData::RunwayDelayItem* sItem)
{
	if (fItem->m_nRwyID < sItem->m_nRwyID)
		return true;

	if (fItem->m_nRwyID == sItem->m_nRwyID && fItem->m_nRwyMark < sItem->m_nRwyMark )
		return true;

	return false;
}

void DetailDelayChartResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if (m_pData == NULL || pParameter == NULL)
		return;

	C2DChartData c2dGraphData;

	AirsideFlightRunwayDelayReportPara* pReportPara = (AirsideFlightRunwayDelayReportPara*)pParameter;

	int nSubType = pReportPara->getSubType();
	if (nSubType == AirsideFlightRunwayDelayReport::ChartType_Detail_Total)
	{
		c2dGraphData.m_strChartTitle = _T("Total Runway Delay");
	}
	else
	{
		CString strPos = FlightRunwayDelay::POSITIONTYPE[nSubType-2];
		c2dGraphData.m_strChartTitle = _T("Runway Delay ") +strPos;
	}
	c2dGraphData.m_strXtitle = _T("Delay time(secs)");
	c2dGraphData.m_strYtitle = _T("Number of aircraft");

	long lInterval = pReportPara->getInterval();
	long lMaxDelay = 0l;

	std::vector<AirsideFlightRunwayDelayData::RunwayDelayItem*> vFitData;
	vFitData.clear();

	int nItemCount = m_pData->m_vDataItemList.size();
	for (int i = 0; i < nItemCount; i++)
	{
		AirsideFlightRunwayDelayData::RunwayDelayItem* pItem = m_pData->m_vDataItemList.at(i);
		if (!pReportPara->IsDetailDataFitChartType(pItem))
			continue;

		if (pItem->m_lTotalDelay > lMaxDelay)
			lMaxDelay = pItem->m_lTotalDelay;

		vFitData.push_back(pItem);
	}

	int nIntCount = lMaxDelay/lInterval;
	if (lMaxDelay%lInterval >0)
		nIntCount++;

	if (nIntCount ==0)
		nIntCount++;

	long lRangeStart = 1l;
	for(int i = 0; i < nIntCount; i++)
	{
		CString strTimeRange;
		long lRangeEnd = lRangeStart + pReportPara->getInterval() -1l;

		strTimeRange.Format("%d -%d", lRangeStart,lRangeEnd);
		c2dGraphData.m_vrXTickTitle.push_back(strTimeRange);

		lRangeStart = lRangeEnd +1l;
	}

	//statistic count
	std::map< CString,std::vector<double> > vRwyDelays;	
	std::vector<double> vDelayCount;
	int nRwyID = -1;
	int nRwyMark = -1;
	CString strMark = "";

	std::sort(vFitData.begin(), vFitData.end(), CompareByLogicRunway);
	int nCount = vFitData.size();
	for (int i =0; i <nCount; i++)
	{
		AirsideFlightRunwayDelayData::RunwayDelayItem* pItem =vFitData.at(i);
		if (pItem->m_lTotalDelay == 0l)
			continue;

		if ((pItem->m_nRwyID > nRwyID || pItem->m_nRwyID < nRwyID) 
			&& (pItem->m_nRwyMark > nRwyMark || pItem->m_nRwyMark < nRwyMark))	//a new operation type
		{
			//record conflict count
			if (nRwyID >=0 && nRwyMark >=0)
			{
				vRwyDelays.insert(std::map< CString,std::vector<double> > ::value_type(strMark,vDelayCount));
			}

			//clear for new statistic
			vDelayCount.clear();
			vDelayCount.resize(nIntCount,0);
			nRwyID = pItem->m_nRwyID;
			nRwyMark = pItem->m_nRwyMark;
			strMark = pItem->m_strRunwayMark;
		}

		int nIntervalIdx = pItem->GetDelayInIntervalIndex(lInterval);

		vector<double>::pointer ptr = &vDelayCount[nIntervalIdx];
		(*ptr) +=1;
		
	}

	//add last statistic data
	if (nRwyID >=0 && nRwyMark >=0)
	{
		vRwyDelays.insert(std::map< CString,std::vector<double> > ::value_type(strMark,vDelayCount));
	}

	if (nCount ==0 && nIntCount ==1)	//no data
	{
		vDelayCount.clear();
		vDelayCount.resize(nIntCount,0);
		vRwyDelays.insert(std::map< CString,std::vector<double> > ::value_type(strMark,vDelayCount));
	}

	std::vector<CString> vLegend;
	std::map< CString, std::vector<double> >::iterator iter = vRwyDelays.begin();
	for (; iter != vRwyDelays.end(); iter++)
	{
		strMark = iter->first;
		CString strLegend;
		strLegend.Format("Runway %s",strMark);
		vLegend.push_back(strLegend);

		c2dGraphData.m_vr2DChartData.push_back(iter->second);
	}

	c2dGraphData.m_vrLegend = vLegend;
	chartWnd.DrawChart(c2dGraphData);
}