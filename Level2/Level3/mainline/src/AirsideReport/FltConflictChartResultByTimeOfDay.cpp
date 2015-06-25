#include "StdAfx.h"
#include ".\fltconflictchartresultbytimeofday.h"
#include "FlightConflictReportData.h"
#include "AirsideFlightConflictPara.h"
#include "AirsideFlightConflictReport.h"

FltConflictChartResultByTimeOfDay::FltConflictChartResultByTimeOfDay(void)
{
	m_pData = NULL;
}

FltConflictChartResultByTimeOfDay::~FltConflictChartResultByTimeOfDay(void)
{
}

void FltConflictChartResultByTimeOfDay::GenerateResult(FlightConflictReportData* pData)
{
	m_pData = pData;
}

void FltConflictChartResultByTimeOfDay::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if (m_pData == NULL || pParameter == NULL)
		return;

	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T(" Conflicts by time of day ");
	c2dGraphData.m_strYtitle = _T("Number of conflicts");
	c2dGraphData.m_strXtitle = _T("Time of day");

	AirsideFlightConflictPara* pConflictPara = (AirsideFlightConflictPara*)pParameter;

	ElapsedTime tTimeInterval = ElapsedTime(pConflictPara->getInterval());
	ElapsedTime tStartTime = pConflictPara->getStartTime();
	ElapsedTime tEndTime = pConflictPara->getEndTime();

	int nIntCount = (tEndTime.asSeconds() - tStartTime.asSeconds())/pConflictPara->getInterval();
	if ((tEndTime.asSeconds() - tStartTime.asSeconds())%pConflictPara->getInterval() > 0)
		nIntCount++;

	std::vector<double> vACConflictCount;
	std::vector<double> vGSEConflictCount;

	//x tick, time range
	ElapsedTime tRangeStart = tStartTime;
	for(int i = 0; i < nIntCount; i++)
	{
		CString strTimeRange;
		ElapsedTime tRangeEnd = tRangeStart + ElapsedTime(pConflictPara->getInterval()) - 1L;
		if (tRangeEnd > pConflictPara->getEndTime())
			tRangeEnd = pConflictPara->getEndTime();

		strTimeRange.Format("%s -%s", tRangeStart.printTime(),tRangeEnd.printTime());
		c2dGraphData.m_vrXTickTitle.push_back(strTimeRange);

		vACConflictCount.push_back(0);
		vGSEConflictCount.push_back(0);

		tRangeStart = tRangeEnd +1L;
	}

	std::vector<CString> vLegend;
	vLegend.push_back("AC to AC Conflicts");
	vLegend.push_back("AC to Vehicle Conflicts");
	c2dGraphData.m_vrLegend = vLegend;

	//statistic count
	int nItemCount = m_pData->m_vDataItemList.size();
	for (int i = 0; i < nItemCount; i++)
	{
		FlightConflictReportData::ConflictDataItem* pItem = m_pData->m_vDataItemList.at(i);

		int nConfType = pItem->m_nConflictType;
		int nIntervalIdx = pItem->GetInIntervalIndex(pParameter);

		vector<double>::pointer ptr;
		if (nConfType ==0)
		{
			ptr = &vACConflictCount[nIntervalIdx];
			(*ptr)+=1;
		}
		else
		{
			ptr = &vGSEConflictCount[nIntervalIdx];
			(*ptr)+=1;
		}
	}

	c2dGraphData.m_vr2DChartData.push_back(vACConflictCount);
	c2dGraphData.m_vr2DChartData.push_back(vGSEConflictCount);

	//std::vector<double> vLegendData;
	//for(int i = 0; i < nIntCount; i++)
	//{	
	//	vLegendData.clear();
	//	vLegendData.push_back(vACConflictCount[i]);
	//	vLegendData.push_back(vGSEConflictCount[i]);

	//	c2dGraphData.m_vr2DChartData.push_back(vLegendData);
	//}

	chartWnd.DrawChart(c2dGraphData);

}