#include "StdAfx.h"
#include ".\fltconflictchartresultbylocationandtimeofday.h"
#include "FlightConflictReportData.h"
#include "AirsideFlightConflictPara.h"
#include "AirsideFlightConflictReport.h"
#include "../Engine/Airside/CommonInSim.h"

FltConflictChartResultByLocationAndTimeOfDay::FltConflictChartResultByLocationAndTimeOfDay(void)
{
	m_pData = NULL;
}

FltConflictChartResultByLocationAndTimeOfDay::~FltConflictChartResultByLocationAndTimeOfDay(void)
{
}

void FltConflictChartResultByLocationAndTimeOfDay::GenerateResult(FlightConflictReportData* pData)
{
	m_pData = pData;
}

void FltConflictChartResultByLocationAndTimeOfDay::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if (m_pData == NULL || pParameter == NULL)
		return;

	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T(" Conflicts by location and time of day ");
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
	std::map< int,std::vector<double> > vACLocationConflicts;	
	std::map< int,std::vector<double> > vGSELocationConflicts;
	m_pData->SortItemsByLocationType();

	std::vector<double> vACConflictCount; 
	std::vector<double> vGSEConflictCount;
	int nACLocation = -1;
	int nGSELocation = -1;

	vACConflictCount.clear();
	vACConflictCount.resize(nIntCount,0);
	vGSEConflictCount.clear();
	vGSEConflictCount.resize(nIntCount,0);

	int nItemCount = m_pData->m_vDataItemList.size();
	for (int i = 0; i < nItemCount; i++)
	{
		FlightConflictReportData::ConflictDataItem* pItem = m_pData->m_vDataItemList.at(i);
		int nConfType = pItem->m_nConflictType;
		if (nConfType == 0 &&(pItem->m_nLocationType > nACLocation || pItem->m_nLocationType < nACLocation))	//a new ac-ac operation type
		{
			//record conflict count
			if (nACLocation >=0)
				vACLocationConflicts.insert(std::map< int,std::vector<double> > ::value_type(nACLocation,vACConflictCount));

			//clear for new statistic
			vACConflictCount.clear();
			vACConflictCount.resize(nIntCount,0);
			nACLocation = pItem->m_nLocationType;
		}

		if (nConfType == 1 &&(pItem->m_nLocationType > nGSELocation || pItem->m_nLocationType < nGSELocation))	//a new ac-gse operation type
		{
			if (nGSELocation >=0 )
				vGSELocationConflicts.insert(std::map< int,std::vector<double> > ::value_type(nGSELocation,vGSEConflictCount));

			vGSEConflictCount.clear();
			vGSEConflictCount.resize(nIntCount,0);
			nGSELocation = pItem->m_nLocationType;	
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

	if (nACLocation >=0)			//need add the last statistic data
		vACLocationConflicts.insert(std::map< int,std::vector<double> > ::value_type(nACLocation,vACConflictCount));
	if (nGSELocation >=0)
		vGSELocationConflicts.insert(std::map< int,std::vector<double> > ::value_type(nGSELocation,vGSEConflictCount));



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
	for (int i = 0; i <  FlightConflict::LOCATION_FIELD; i++)
	{
		CString strName =  FlightConflict::LOCATIONTYPE[i];
		CString strLegend;
		std::vector<double> vConflicts;

		vConflicts = vACLocationConflicts[i];
		if (!vConflicts.empty())
		{
			strLegend.Format("%s AC-AC Conflicts",strName);
			vLegend.push_back(strLegend);	
			c2dGraphData.m_vr2DChartData.push_back(vConflicts);
		}

		vConflicts = vGSELocationConflicts[i];
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