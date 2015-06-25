#include "StdAfx.h"
#include ".\fltconflictchartresultbyactionandtimeofday.h"
#include "FlightConflictReportData.h"
#include "AirsideFlightConflictPara.h"
#include "AirsideFlightConflictReport.h"
#include "../Engine/Airside/CommonInSim.h"

FltConflictChartResultByActionAndTimeOfDay::FltConflictChartResultByActionAndTimeOfDay(void)
:CAirsideReportBaseResult()
{
	m_pData = NULL;
}

FltConflictChartResultByActionAndTimeOfDay::~FltConflictChartResultByActionAndTimeOfDay(void)
{
}

void FltConflictChartResultByActionAndTimeOfDay::GenerateResult(FlightConflictReportData* pData)
{
	m_pData = pData;
}

void FltConflictChartResultByActionAndTimeOfDay::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if (m_pData == NULL || pParameter == NULL)
		return;

	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T(" Conflicts by action and time of day ");
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
	std::map< int,std::vector<double> > vACActionConflicts;	
	std::map< int,std::vector<double> > vGSEActionConflicts;
	m_pData->SortItemsByActionType();

	std::vector<double> vACConflictCount; 
	std::vector<double> vGSEConflictCount;
	int nACActionType = -1;
	int nGSEActionType = -1;

	vACConflictCount.clear();
	vACConflictCount.resize(nIntCount,0);
	vGSEConflictCount.clear();
	vGSEConflictCount.resize(nIntCount,0);

	int nItemCount = m_pData->m_vDataItemList.size();
	for (int i = 0; i < nItemCount; i++)
	{
		FlightConflictReportData::ConflictDataItem* pItem = m_pData->m_vDataItemList.at(i);
		int nConfType = pItem->m_nConflictType;
		if (nConfType == 0 &&(pItem->m_nActionType > nACActionType || pItem->m_nActionType < nACActionType))	//a new ac-ac operation type
		{
			//record conflict count
			if (nACActionType >=0)
				vACActionConflicts.insert(std::map< int,std::vector<double> > ::value_type(nACActionType,vACConflictCount));

			//clear for new statistic
			vACConflictCount.clear();
			vACConflictCount.resize(nIntCount,0);
			nACActionType = pItem->m_nActionType;
		}

		if (nConfType == 1 &&(pItem->m_nActionType > nGSEActionType || pItem->m_nActionType < nGSEActionType))	//a new ac-gse operation type
		{
			if (nGSEActionType >=0 )
				vGSEActionConflicts.insert(std::map< int,std::vector<double> > ::value_type(nGSEActionType,vGSEConflictCount));

			vGSEConflictCount.clear();
			vGSEConflictCount.resize(nIntCount,0);
			nGSEActionType = pItem->m_nActionType;	
		}

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

	if (nACActionType >=0)			//need add the last statistic data
		vACActionConflicts.insert(std::map< int,std::vector<double> > ::value_type(nACActionType,vACConflictCount));
	if (nGSEActionType >=0)
		vGSEActionConflicts.insert(std::map< int,std::vector<double> > ::value_type(nGSEActionType,vGSEConflictCount));
	


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
	for (int i = 0; i < FlightConflict::ACTION_FIELD; i++)
	{
		CString strName =  FlightConflict::ACTIONTYPE[i];
		CString strLegend;
		std::vector<double> vConflicts;

		vConflicts = vACActionConflicts[i];
		if (!vConflicts.empty())
		{
			strLegend.Format("%s AC-AC Conflicts",strName);
			vLegend.push_back(strLegend);

			c2dGraphData.m_vr2DChartData.push_back(vConflicts);
		}

		vConflicts = vGSEActionConflicts[i];
		if (!vConflicts.empty())
		{
			strLegend.Format("%s AC-GSE Conflicts", strName);
			vLegend.push_back(strLegend);

			c2dGraphData.m_vr2DChartData.push_back(vConflicts);		
		}
	}

	c2dGraphData.m_vrLegend = vLegend;

	chartWnd.DrawChart(c2dGraphData);

}