#include "StdAfx.h"
#include ".\fltconflictchartresultbyoperationandtimeofday.h"
#include "FlightConflictReportData.h"
#include "AirsideFlightConflictPara.h"
#include "AirsideFlightConflictReport.h"
#include "../Engine/Airside/CommonInSim.h"

FltConflictChartResultByOperationAndTimeOfDay::FltConflictChartResultByOperationAndTimeOfDay(void)
{
	m_pData = NULL;
}

FltConflictChartResultByOperationAndTimeOfDay::~FltConflictChartResultByOperationAndTimeOfDay(void)
{
}

void FltConflictChartResultByOperationAndTimeOfDay::GenerateResult(FlightConflictReportData* pData)
{
	m_pData = pData;
}

void FltConflictChartResultByOperationAndTimeOfDay::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if (m_pData == NULL || pParameter == NULL)
		return;

	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T(" Conflicts by operation and time of day ");
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

	std::map< int,std::vector<double> > vACOpConflicts;	
	std::map< int,std::vector<double> > vGSEOpConflicts;
	m_pData->SortItemsByOperationType();

	std::vector<double> vACConflictCount; 
	std::vector<double> vGSEConflictCount;
	int nACOperation = -1;
	int nGSEOperation = -1;

	vACConflictCount.clear();
	vACConflictCount.resize(nIntCount,0);
	vGSEConflictCount.clear();
	vGSEConflictCount.resize(nIntCount,0);

	int nItemCount = m_pData->m_vDataItemList.size();
	for (int i = 0; i < nItemCount; i++)
	{
		FlightConflictReportData::ConflictDataItem* pItem = m_pData->m_vDataItemList.at(i);
		int nConfType = pItem->m_nConflictType;
		if (nConfType == 0 &&(pItem->m_nOperationType > nACOperation || pItem->m_nOperationType < nACOperation))	//a new ac-ac operation type
		{
			//record conflict count
			if (nACOperation >=0)
				vACOpConflicts.insert(std::map< int,std::vector<double> > ::value_type(nACOperation,vACConflictCount));

			//clear for new statistic
			vACConflictCount.clear();
			vACConflictCount.resize(nIntCount,0);
			nACOperation = pItem->m_nOperationType;
		}

		if (nConfType == 1 &&(pItem->m_nOperationType > nGSEOperation || pItem->m_nOperationType < nGSEOperation))	//a new ac-gse operation type
		{
			if (nGSEOperation >=0 )
				vGSEOpConflicts.insert(std::map< int,std::vector<double> > ::value_type(nGSEOperation,vGSEConflictCount));

			vGSEConflictCount.clear();
			vGSEConflictCount.resize(nIntCount,0);
			nGSEOperation = pItem->m_nOperationType;	
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

	if (nACOperation >=0)			//need add the last statistic data
		vACOpConflicts.insert(std::map< int,std::vector<double> > ::value_type(nACOperation,vACConflictCount));
	if (nGSEOperation >=0)
		vGSEOpConflicts.insert(std::map< int,std::vector<double> > ::value_type(nGSEOperation,vGSEConflictCount));
	


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
	for (int i = 0; i <  FlightConflict::OPERATION_FIELD; i++)
	{
		CString strName =  FlightConflict::OPERATIONTYPE[i];
		CString strLegend;
		std::vector<double> vConflicts;

		vConflicts = vACOpConflicts[i];
		if (!vConflicts.empty())
		{
			strLegend.Format("%s AC-AC Conflicts",strName);
			vLegend.push_back(strLegend);
			c2dGraphData.m_vr2DChartData.push_back(vConflicts);
		}

		vConflicts = vGSEOpConflicts[i];
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