#include "StdAfx.h"
#include ".\fltconflictchartresultbydistofconfperac.h"
#include "FlightConflictReportData.h"
#include "AirsideFlightConflictPara.h"

FltConflictChartResultByDistOfConfPerAC::FltConflictChartResultByDistOfConfPerAC()
:CAirsideReportBaseResult()
{
	m_pData = NULL;
}

FltConflictChartResultByDistOfConfPerAC::~FltConflictChartResultByDistOfConfPerAC(void)
{
}

void FltConflictChartResultByDistOfConfPerAC::GenerateResult(FlightConflictReportData* pData)
{
	m_pData = pData;
}

void FltConflictChartResultByDistOfConfPerAC::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if (m_pData == NULL || pParameter == NULL)
		return;

	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T(" Distribution of conflict per aircraft ");
	c2dGraphData.m_strXtitle = _T("Number of conflicts");
	c2dGraphData.m_strYtitle = _T("Number of aircrafts");

	//AirsideFlightConflictPara* pConflictPara = (AirsideFlightConflictPara*)pParameter;

	std::vector<CString> vLegend;
	vLegend.push_back("AC to AC Conflicts");
	vLegend.push_back("AC to Vehicle Conflicts");
	c2dGraphData.m_vrLegend = vLegend;

	//statistic count
	typedef pair<int,int> IntPair;

	std::map<int,IntPair> vACConflicts;	
	vACConflicts.clear();

	int nACCount = 0;
	int nGSECount = 0;
	int nMaxCount = 0;

	//statistic each flight's conflicts
	m_pData->SortItemsByAC();

	int nItemCount = m_pData->m_vDataItemList.size();
	if (nItemCount >0)
	{
		FlightConflictReportData::ConflictDataItem* pItem = m_pData->m_vDataItemList.at(0);
		int nACId = pItem->m_nACID;

		for (int i = 0; i < nItemCount; i++)
		{
			pItem = m_pData->m_vDataItemList.at(i);

			if (pItem->m_nACID == nACId)		//if same ac, statistic
			{
				if (pItem->m_nConflictType ==0)
					nACCount++;
				else
					nGSECount++;
			}
			else			//different ac, new statistic
			{
				//record conflict count
				IntPair value(nACCount,nGSECount);
				pair<int,IntPair> value2(nACId,value);
				vACConflicts.insert(value2);

				//get max conflict count
				if (nMaxCount < nACCount)
					nMaxCount = nACCount;

				if (nMaxCount < nGSECount)
					nMaxCount = nGSECount;

				//clear for new statistic
				nACCount =0;
				nGSECount =0;
				nACId = pItem->m_nACID;

				if (pItem->m_nConflictType ==0)
					nACCount++;
				else
					nGSECount++;

			}

		}

		if (nACId >= 0)		//add the last flight's conflict data
		{
			IntPair value(nACCount,nGSECount);
			pair<int,IntPair> value2(nACId,value);
			vACConflicts.insert(value2);

			//get max conflict count
			if (nMaxCount < nACCount)
				nMaxCount = nACCount;

			if (nMaxCount < nGSECount)
				nMaxCount = nGSECount;
		}
	}

	//assume the interval count equal 5, and calculate interval value
	int nInteralCount = 6;
	int nInterval = nMaxCount/nInteralCount;
	if (nMaxCount% nInteralCount > 0)
		nInterval +=1;

	if (nMaxCount <= 6)			//if the max count less than five, assume only two interval
	{
		nInteralCount = 2;
		nInterval = 3;
	}
	else if (nMaxCount <= 12)		//4 intervals
	{
		nInteralCount = 4;
		nInterval = 3;
	}
	else if (nMaxCount <= 20)		//5 intervals
	{
		nInteralCount = 5;
		nInterval = nMaxCount/nInteralCount;
		if (nMaxCount% nInteralCount > 0)
			nInterval +=1;
	}

	int ACConflictCount[6] = {0,0,0,0,0,0};
	int GSEConflictCount[6] = {0,0,0,0,0,0};

	std::map<int,IntPair>::iterator pItr;

	for(pItr = vACConflicts.begin(); pItr != vACConflicts.end(); pItr++)
	{	
		IntPair value = pItr->second;
		if (value.first > 0)
		{
			int nACIdx = value.first/nInterval -1;	
			if (value.first % nInterval >0 )
				nACIdx +=1;

			ACConflictCount[nACIdx]++;
		}

		if (value.second > 0)
		{
			int nGSEIdx = value.second/nInterval -1;
			if (value.second % nInterval >0 )
				nGSEIdx +=1;

			GSEConflictCount[nGSEIdx]++;
		}
	}

	std::vector<double> vLegendData;
	for(int i = 0; i < nInteralCount; i++)
	{	
		vLegendData.push_back(ACConflictCount[i]);

		CString strIntRange;
		strIntRange.Format("%d -%d", nInterval*i+1,nInterval*(i+1));
		c2dGraphData.m_vrXTickTitle.push_back(strIntRange);
	}
	c2dGraphData.m_vr2DChartData.push_back(vLegendData);

	vLegendData.clear();
	for(int i = 0; i < nInteralCount; i++)
		vLegendData.push_back(GSEConflictCount[i]);
	c2dGraphData.m_vr2DChartData.push_back(vLegendData);

	chartWnd.DrawChart(c2dGraphData);

}