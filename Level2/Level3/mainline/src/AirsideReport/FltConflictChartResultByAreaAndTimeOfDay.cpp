#include "StdAfx.h"
#include ".\fltconflictchartresultbyareaandtimeofday.h"
#include "FlightConflictReportData.h"
#include "AirsideFlightConflictPara.h"
#include "AirsideFlightConflictReport.h"

FltConflictChartResultByAreaAndTimeOfDay::FltConflictChartResultByAreaAndTimeOfDay(void)
{
	m_pData = NULL;
}

FltConflictChartResultByAreaAndTimeOfDay::~FltConflictChartResultByAreaAndTimeOfDay(void)
{
}

void FltConflictChartResultByAreaAndTimeOfDay::GenerateResult(FlightConflictReportData* pData)
{
	m_pData = pData;
}

void FltConflictChartResultByAreaAndTimeOfDay::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if (m_pData == NULL || pParameter == NULL)
		return;

	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T(" Conflicts by area and time of day ");
	c2dGraphData.m_strYtitle = _T("Number of conflicts");
	c2dGraphData.m_strXtitle = _T("Time of day");

	AirsideFlightConflictPara* pConflictPara = (AirsideFlightConflictPara*)pParameter;

	ElapsedTime tTimeInterval = ElapsedTime(pConflictPara->getInterval());
	ElapsedTime tStartTime = pConflictPara->getStartTime();
	ElapsedTime tEndTime = pConflictPara->getEndTime();

	//calculate interval count
	int nIntCount = (tEndTime.asSeconds() - tStartTime.asSeconds())/pConflictPara->getInterval();
	if ((tEndTime.asSeconds() - tStartTime.asSeconds())%pConflictPara->getInterval() > 0)
		nIntCount++;

	//statistic count
	std::map< int,std::vector<double> > vACAeraConflicts;	
	std::map< int,std::vector<double> > vGSEAreaConflicts;
	m_pData->SortItemsByArea();

	std::vector<double> vACConflictCount; 
	std::vector<double> vGSEConflictCount;
	int nAreaId = -2;
	std::map<int, CString> vAreas;

	int nItemCount = m_pData->m_vDataItemList.size();
	for (int i = 0; i < nItemCount; i++)
	{
		FlightConflictReportData::ConflictDataItem* pItem = m_pData->m_vDataItemList.at(i);
		if (pItem->m_nAreaID > nAreaId || pItem->m_nAreaID < nAreaId)	//a new operation type
		{
			//record conflict count
			if (nAreaId >=0)
			{
				vACAeraConflicts.insert(std::map< int,std::vector<double> > ::value_type(nAreaId,vACConflictCount));
				vGSEAreaConflicts.insert(std::map< int,std::vector<double> > ::value_type(nAreaId,vGSEConflictCount));

			}

			//clear for new statistic
			vACConflictCount.clear();
			vGSEConflictCount.clear();
			vACConflictCount.resize(nIntCount,0);
			vGSEConflictCount.resize(nIntCount,0);
			nAreaId = pItem->m_nAreaID;	
			if (nAreaId >=0)
				vAreas.insert(std::map<int, CString>::value_type(nAreaId,pItem->m_strAreaName));
		}

		int nConfType = pItem->m_nConflictType;
		int nIntervalIdx = pItem->GetInIntervalIndex(pParameter);

		vector<double>::pointer ptr;
		if (nConfType ==0)
		{
			ptr = &vACConflictCount[nIntervalIdx];
			(*ptr) +=1;
		}
		else
		{
			ptr = &vGSEConflictCount[nIntervalIdx];
			(*ptr) +=1;
		}
	}

	if (nAreaId >=0)			//need add the last statistic data
	{
		vACAeraConflicts.insert(std::map< int,std::vector<double> > ::value_type(nAreaId,vACConflictCount));
		vGSEAreaConflicts.insert(std::map< int,std::vector<double> > ::value_type(nAreaId,vGSEConflictCount));
	}

	//draw
	ElapsedTime tRangeStart = tStartTime;

	for(int i = 0; i < nIntCount; i++)
	{		
		CString strTimeRange;
		ElapsedTime tRangeEnd = tRangeStart + ElapsedTime(pConflictPara->getInterval()) - 1L;
		if (tRangeEnd > pConflictPara->getEndTime())
			tRangeEnd = pConflictPara->getEndTime();

		strTimeRange.Format("%s -%s", tRangeStart.printTime(),tRangeEnd.printTime());
		c2dGraphData.m_vrXTickTitle.push_back(strTimeRange);
		tRangeStart = tRangeEnd + 1L;
	}

	//legend
	std::vector<CString> vLegend;
	int nAreaCount = vAreas.size();
	for (std::map< int,CString >::iterator iter = vAreas.begin(); iter != vAreas.end(); iter++)
	{
		int nIdx = iter->first;
		CString strName = iter->second;
		CString strLegend;
		strLegend.Format("%s AC-AC Conflicts",strName);
		vLegend.push_back(strLegend);
		strLegend.Format("%s AC-GSE Conflicts", strName);
		vLegend.push_back(strLegend);

		std::vector<double> vConflicts;
		vConflicts = vACAeraConflicts[nIdx];
		if (vConflicts.empty())
		{
			vConflicts.resize(nIntCount, 0);
		}
		c2dGraphData.m_vr2DChartData.push_back(vConflicts);

		vConflicts = vGSEAreaConflicts[nIdx];
		if (vConflicts.empty())
		{
			vConflicts.resize(nIntCount, 0);
		}
		c2dGraphData.m_vr2DChartData.push_back(vConflicts);		
	}

	if (nAreaCount == 0 && nAreaId == -1) //no area defined
	{
		CString strLegend = "All AC-AC Conflicts";
		vLegend.push_back(strLegend);
		strLegend = "All AC-GSE Conflicts";
		vLegend.push_back(strLegend);

		c2dGraphData.m_vr2DChartData.push_back(vACConflictCount);
		c2dGraphData.m_vr2DChartData.push_back(vGSEConflictCount);		
	}

	c2dGraphData.m_vrLegend = vLegend;

	chartWnd.DrawChart(c2dGraphData);

}