#include "StdAfx.h"
#include ".\flightstandoperationdetailchartreport.h"
#include "StandOperationData.h"
#include "StandOperationDataCalculation.h"
#pragma warning (disable:4244)
///////////////////////detail occupancy utilization///////////////////////////////////////////////////

CDetailStandOccupancyUtilizationChartResult::CDetailStandOccupancyUtilizationChartResult()
{

}

CDetailStandOccupancyUtilizationChartResult::~CDetailStandOccupancyUtilizationChartResult()
{

}

void CDetailStandOccupancyUtilizationChartResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Detailed Stand Utilization Report(Occupancy)");
	c2dGraphData.m_strYtitle = _T("Number of Stands");
	c2dGraphData.m_strXtitle = _T("Occupancy (mins at stand)");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Detailed Stand Utilization %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	std::vector<StandOccupancyData>::iterator iter = m_vOccupancyData.begin();
	for (; iter!=m_vOccupancyData.end(); ++iter)
	{
		c2dGraphData.m_vrLegend.push_back(iter->m_sName);
		c2dGraphData.m_vr2DChartData.push_back((*iter).m_vStandData);
	}
	chartWnd.DrawChart(c2dGraphData);
}


void CDetailStandOccupancyUtilizationChartResult::GenerateResult(std::vector<CStandOperationReportData*>& vResult,CParameters *pParameter)
{
	ASSERT(pParameter);

	std::vector<long> vSchedStandTimeOccupany;
	std::vector<long> vActualStandTimeOccupany;

	CalculateStandOccupancyTime(true, vResult, vSchedStandTimeOccupany);
	CalculateStandOccupancyTime(false, vResult, vActualStandTimeOccupany);

	std::sort(vSchedStandTimeOccupany.begin(), vSchedStandTimeOccupany.end());
	std::sort(vActualStandTimeOccupany.begin(), vActualStandTimeOccupany.end());

	std::reverse(vSchedStandTimeOccupany.begin(), vSchedStandTimeOccupany.end());
	std::reverse(vActualStandTimeOccupany.begin(), vActualStandTimeOccupany.end());

	long lSchedStandTimeOccupany = 0l;
	if (!vSchedStandTimeOccupany.empty())
	{
		lSchedStandTimeOccupany = vSchedStandTimeOccupany.front();
	}

	long lActualStandTimeOccupany = 0l;
	if (!vActualStandTimeOccupany.empty())
	{
		lActualStandTimeOccupany = vActualStandTimeOccupany.front();
	}

	int nIntervalSize = 0;
	long lMaxOccupancyTime  = max(lSchedStandTimeOccupany, lActualStandTimeOccupany);
	nIntervalSize = lMaxOccupancyTime / pParameter->getInterval() ;

    if(lMaxOccupancyTime%pParameter->getInterval() != 0)
        nIntervalSize += 1;

	if (nIntervalSize ==0)
		nIntervalSize = 1;

	CString strTimeInt(_T(""));
	for (int i = 0 ; i < nIntervalSize ;i++)
	{
		ElapsedTime estTempTime(pParameter->getInterval()*(i+1));
		strTimeInt.Format(_T("%d"),estTempTime.asMinutes());
		m_vXAxisTitle.push_back(strTimeInt);
	}

	StandOccupancyData SchedItem;
	SchedItem.m_sName = _T("Schedule");
	StandOccupancyData ActualItem;
	ActualItem.m_sName = _T("Actual");	

	size_t nSchedSize = vSchedStandTimeOccupany.size();
	size_t nActualSize = vActualStandTimeOccupany.size();

	long lStart = 0;
	long lEnd = 0;

	for (int i=0; i< nIntervalSize; i++)
	{
		lStart = pParameter->getInterval()*i;
		lEnd = pParameter->getInterval()*(i+1);

		int nSchedCount = 0;
		for (size_t j = 0; j < nSchedSize; j++)
		{
			long lTime = vSchedStandTimeOccupany.at(j);
		
			if( lStart <= lTime && lTime < lEnd)
				nSchedCount++;
		}
		SchedItem.m_vStandData.push_back(nSchedCount);

		int nActualCount = 0;
		for (size_t j = 0; j < nActualSize; j++)
		{
			long lTime = vActualStandTimeOccupany.at(j);

			if( lStart <= lTime && lTime < lEnd)
				nActualCount++;						
		}
		ActualItem.m_vStandData.push_back(nActualCount);
	}
	m_vOccupancyData.push_back(SchedItem);
	m_vOccupancyData.push_back(ActualItem);

}
////////////////detail percentage////////////////////////////////////////////////////////////////////////
CDetailStandUtilizationPercentageChartResult::CDetailStandUtilizationPercentageChartResult()
{

}

CDetailStandUtilizationPercentageChartResult::~CDetailStandUtilizationPercentageChartResult()
{

}

void CDetailStandUtilizationPercentageChartResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Detailed Stand Utilization Report(Occupancy ratio)");
	c2dGraphData.m_strYtitle = _T("Number of Stands");
	c2dGraphData.m_strXtitle = _T("Occupancy (% of report time)");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Detailed Stand Utilization %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	std::vector<StandOccupancyData>::iterator iter = m_vOccupancyData.begin();
	for (; iter!=m_vOccupancyData.end(); ++iter)
	{
		c2dGraphData.m_vrLegend.push_back(iter->m_sName);
		c2dGraphData.m_vr2DChartData.push_back((*iter).m_vStandData);
	}

	chartWnd.DrawChart(c2dGraphData);
}

void CDetailStandUtilizationPercentageChartResult::GenerateResult(std::vector<CStandOperationReportData*>& vResult,CParameters *pParameter)
{
	ASSERT(pParameter);

	std::vector<long> vSchedStandTimeOccupany;
	std::vector<long> vActualStandTimeOccupany;

	CalculateStandOccupancyTime(true, vResult, vSchedStandTimeOccupany);
	CalculateStandOccupancyTime(false, vResult, vActualStandTimeOccupany);

	std::sort(vSchedStandTimeOccupany.begin(), vSchedStandTimeOccupany.end());
	std::sort(vActualStandTimeOccupany.begin(), vActualStandTimeOccupany.end());

	std::reverse(vSchedStandTimeOccupany.begin(), vSchedStandTimeOccupany.end());
	std::reverse(vActualStandTimeOccupany.begin(), vActualStandTimeOccupany.end());

	long lSchedStandTimeOccupany = 0l;
	if (!vSchedStandTimeOccupany.empty())
	{
		lSchedStandTimeOccupany = vSchedStandTimeOccupany.front();
	}

	long lActualStandTimeOccupany = 0l;
	if (!vActualStandTimeOccupany.empty())
	{
		lActualStandTimeOccupany = vActualStandTimeOccupany.front();
	}

	int nIntervalSize = 0;
	ElapsedTime lMaxOccupancyTime  = ElapsedTime(max(lSchedStandTimeOccupany, lActualStandTimeOccupany));
	int nDuration = pParameter->getEndTime().asSeconds() - pParameter->getStartTime().asSeconds();
	nIntervalSize = int((double)lMaxOccupancyTime.asSeconds() / (double)(nDuration)*100)/10 ;

	if (nIntervalSize ==0)
		nIntervalSize = 1;

	CString strTimeInt(_T(""));
	for (int i = 0 ; i < 10 ;i++)
	{
		strTimeInt.Format(_T("%d"),(i+1)*10);
		m_vXAxisTitle.push_back(strTimeInt);
	}

	StandOccupancyData SchedItem;
	SchedItem.m_sName = _T("Schedule");
	StandOccupancyData ActualItem;
	ActualItem.m_sName = _T("Actual");	

	size_t nSchedSize = vSchedStandTimeOccupany.size();
	size_t nActualSize = vActualStandTimeOccupany.size();

	int nStart = 0;
	int nEnd = 0;

	for (int i =0; i< nIntervalSize; i++)
	{
		nStart = i *10;
		nEnd = (i+1) *10;

		int nSchedCount = 0;
		for (size_t j = 0; j < nSchedSize; j++)
		{
			int nPercent = ((double)vSchedStandTimeOccupany.at(j))/nDuration*100;

			if( nStart <= nPercent && nPercent < nEnd)
				nSchedCount++;
		}
		SchedItem.m_vStandData.push_back(nSchedCount);

		int nActualCount = 0;
		for (size_t j = 0; j < nActualSize; j++)
		{
			int nPercent = ((double)vActualStandTimeOccupany.at(j))/nDuration*100;

			if( nStart <= nPercent && nPercent < nEnd)
				nActualCount++;						
		}
		ActualItem.m_vStandData.push_back(nActualCount);
	}

	for(int i = nIntervalSize; i < 10; i++)
	{
		SchedItem.m_vStandData.push_back(0);	
		ActualItem.m_vStandData.push_back(0);
	}

	m_vOccupancyData.push_back(SchedItem);
	m_vOccupancyData.push_back(ActualItem);
}

//////////detail idle time utilization///////////////////////////////////////////////////////////////
CDetailStandIdleTimeUtiliztionChartResult::CDetailStandIdleTimeUtiliztionChartResult()
{

}

CDetailStandIdleTimeUtiliztionChartResult::~CDetailStandIdleTimeUtiliztionChartResult()
{

}

void CDetailStandIdleTimeUtiliztionChartResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Detailed Stand Utilization Report(Unused time)");
	c2dGraphData.m_strYtitle = _T("Number of Stands");
	c2dGraphData.m_strXtitle = _T("Unused time (mins at stand)");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Detailed Stand Utilization %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	std::vector<StandOccupancyData>::iterator iter = m_vOccupancyData.begin();
	for (; iter!=m_vOccupancyData.end(); ++iter)
	{
		c2dGraphData.m_vrLegend.push_back(iter->m_sName);
		c2dGraphData.m_vr2DChartData.push_back((*iter).m_vStandData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CDetailStandIdleTimeUtiliztionChartResult::GenerateResult(std::vector<CStandOperationReportData*>& vResult,CParameters *pParameter)
{
	ASSERT(pParameter);

	std::vector<long> vSchedStandTimeOccupany;
	std::vector<long> vActualStandTimeOccupany;

	CalculateStandOccupancyTime(true, vResult, vSchedStandTimeOccupany);
	CalculateStandOccupancyTime(false, vResult, vActualStandTimeOccupany);

	std::sort(vSchedStandTimeOccupany.begin(), vSchedStandTimeOccupany.end());
	std::sort(vActualStandTimeOccupany.begin(), vActualStandTimeOccupany.end());

	long lSchedStandTimeOccupany = 0l;
	if (!vSchedStandTimeOccupany.empty())
	{
		lSchedStandTimeOccupany = vSchedStandTimeOccupany.front();
	}

	long lActualStandTimeOccupany = 0l;
	if (!vActualStandTimeOccupany.empty())
	{
		lActualStandTimeOccupany = vActualStandTimeOccupany.front();
	}

	int nIntervalSize = 0;
	long lMinOccupancyTime  = min(lSchedStandTimeOccupany, lActualStandTimeOccupany);
	int nDuration = pParameter->getEndTime().asSeconds() - pParameter->getStartTime().asSeconds();
	nIntervalSize = (nDuration - lMinOccupancyTime) / pParameter->getInterval() ;

	if (nIntervalSize ==0)
		nIntervalSize = 1;

	CString strTimeInt(_T(""));
	for (int i = 0 ; i < nIntervalSize ;i++)
	{
		ElapsedTime estTempTime(pParameter->getInterval()*(i+1));
		strTimeInt.Format(_T("%d"),estTempTime.asMinutes());
		m_vXAxisTitle.push_back(strTimeInt);
	}

	StandOccupancyData SchedItem;
	SchedItem.m_sName = _T("Schedule");
	StandOccupancyData ActualItem;
	ActualItem.m_sName = _T("Actual");	

	size_t nSchedSize = vSchedStandTimeOccupany.size();
	size_t nActualSize = vActualStandTimeOccupany.size();

	long lStart = 0;
	long lEnd = 0;

	for (int i = 0; i < nIntervalSize; i++)
	{
		lStart = pParameter->getInterval()*i;
		lEnd = pParameter->getInterval()*(i+1);

		int nSchedCount = 0;
		for (size_t j = 0; j < nSchedSize; j++)
		{
			long lTime = nDuration - vSchedStandTimeOccupany.at(j);

			if( lStart <= lTime && lTime < lEnd)
				nSchedCount++;
		}
		SchedItem.m_vStandData.push_back(nSchedCount);

		int nActualCount = 0;
		for (size_t j = 0; j < nActualSize; j++)
		{
			long lTime = nDuration - vActualStandTimeOccupany.at(j);

			if( lStart <= lTime && lTime < lEnd)
				nActualCount++;						
		}
		ActualItem.m_vStandData.push_back(nActualCount);
	}
	m_vOccupancyData.push_back(SchedItem);
	m_vOccupancyData.push_back(ActualItem);

}


//////////////////detail conflict////////////////////////////////////////////////////////////////////
CDetailStandConflictChartResult::CDetailStandConflictChartResult()
{

}

CDetailStandConflictChartResult::~CDetailStandConflictChartResult()
{

}

void CDetailStandConflictChartResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Stand conflicts");
	c2dGraphData.m_strYtitle = _T("Number of Stands");
	c2dGraphData.m_strXtitle = _T("Number of conflicts");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Detailed Stand Utilization %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	std::vector<StandOccupancyData>::iterator iter = m_vOccupancyData.begin();
	for (; iter!=m_vOccupancyData.end(); ++iter)
	{
		c2dGraphData.m_vrLegend.push_back(iter->m_sName);
		c2dGraphData.m_vr2DChartData.push_back((*iter).m_vStandData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CDetailStandConflictChartResult::GenerateResult(std::vector<CStandOperationReportData*>& vResult,CParameters *pParameter)
{
	ASSERT(pParameter);

	std::vector<int> vArrStandConflicts;
	std::vector<int> vDepStandConflicts;

	CalculateStandConflict('A', vResult, vArrStandConflicts);
	CalculateStandConflict('D', vResult, vDepStandConflicts);

	std::sort(vArrStandConflicts.begin(), vArrStandConflicts.end());
	std::sort(vDepStandConflicts.begin(), vDepStandConflicts.end());

	std::reverse(vArrStandConflicts.begin(), vArrStandConflicts.end());
	std::reverse(vDepStandConflicts.begin(), vDepStandConflicts.end());

	long lArrStandConflicts = 0l;
	if (!vArrStandConflicts.empty())
	{
		lArrStandConflicts = vArrStandConflicts.front();
	}

	long lDepStandConflicts = 0l;
	if (!vDepStandConflicts.empty())
	{
		lDepStandConflicts = vDepStandConflicts.front();
	}

	int nIntervalSize = 0;
	int nMaxConflicts  =  max(lArrStandConflicts, lDepStandConflicts);
	nIntervalSize = nMaxConflicts / 5 ;

    if(nMaxConflicts%5 != 0)
        nIntervalSize += 1;

	if (nIntervalSize ==0)
		nIntervalSize = 1;

	CString strIntName(_T(""));
	for (int i = 0 ; i < nIntervalSize ;i++)
	{
		strIntName.Format(_T("%d"),(i+1)*5);
		m_vXAxisTitle.push_back(strIntName);
	}

	StandOccupancyData ArrItem;
	ArrItem.m_sName = _T("Arrival");
	StandOccupancyData DepItem;
	DepItem.m_sName = _T("Departure");

	size_t nArrSize = vArrStandConflicts.size();
	size_t nDepSize = vDepStandConflicts.size();

	int nStart = 0;
	int nEnd = 0;

	for (int i=0; i< nIntervalSize; i++)
	{
		nStart = i*5;
		nEnd = (i+1)*5;

		int nArrCount = 0;
		for (size_t j = 0; j < nArrSize; j++)
		{
			int nConflict = vArrStandConflicts.at(j);

			if( nStart <= nConflict && nConflict < nEnd)
				nArrCount++;						
		}
		ArrItem.m_vStandData.push_back(nArrCount);

		int nDepCount = 0;
		for (size_t j = 0; j < nDepSize; j++)
		{
			int nConflict = vDepStandConflicts.at(j);

			if( nStart <= nConflict && nConflict < nEnd)
				nDepCount++;						
		}
		DepItem.m_vStandData.push_back(nDepCount);
	}
	m_vOccupancyData.push_back(ArrItem);
	m_vOccupancyData.push_back(DepItem);

}

///////detail delay//////////////////////////////////////////////////////////////////////////////////
CDetailStandDelayChartResult::CDetailStandDelayChartResult()
{

}

CDetailStandDelayChartResult::~CDetailStandDelayChartResult()
{

}

void CDetailStandDelayChartResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Stand Delays");
	c2dGraphData.m_strYtitle = _T("Number of Stands");
	c2dGraphData.m_strXtitle = _T("Time (hh:mm:ss)");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Detailed Stand Utilization %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	std::vector<StandOccupancyData>::iterator iter = m_vOccupancyData.begin();
	for (; iter!=m_vOccupancyData.end(); ++iter)
	{
		c2dGraphData.m_vrLegend.push_back(iter->m_sName);
		c2dGraphData.m_vr2DChartData.push_back((*iter).m_vStandData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CDetailStandDelayChartResult::GenerateResult(std::vector<CStandOperationReportData*>& vResult,CParameters *pParameter)
{
	ASSERT(pParameter);


	std::vector<long> vArrStandDelay;
	std::vector<long> vDepStandDelay;

	CalculateStandDelay('A', vResult, vArrStandDelay);
	CalculateStandDelay('D', vResult, vDepStandDelay);

	std::sort(vArrStandDelay.begin(), vArrStandDelay.end());
	std::sort(vDepStandDelay.begin(), vDepStandDelay.end());

	std::reverse(vArrStandDelay.begin(), vArrStandDelay.end());
	std::reverse(vDepStandDelay.begin(), vDepStandDelay.end());	

	int nIntervalSize = 0;
	long lArrStandDelay = 0l;
	if (!vArrStandDelay.empty())
	{
		lArrStandDelay = vArrStandDelay.front();
	}

	long lDepStandDelay = 0l;
	if (!vDepStandDelay.empty())
	{
		lDepStandDelay = vDepStandDelay.front();
	}

	long lMaxDelay  =  max(lArrStandDelay, lDepStandDelay);
	nIntervalSize = lMaxDelay / pParameter->getInterval() ;

    if((lMaxDelay%pParameter->getInterval()) != 0)
        nIntervalSize += 1;

	if (nIntervalSize ==0)
		nIntervalSize = 1;

	CString strTimeInt(_T(""));
	for (int i = 0 ; i < nIntervalSize ;i++)
	{
		ElapsedTime estTempTime(pParameter->getInterval()*(i+1));
		strTimeInt.Format(_T("%d"),estTempTime.asMinutes());
		m_vXAxisTitle.push_back(strTimeInt);
	}

	StandOccupancyData ArrItem;
	ArrItem.m_sName = _T("Arrival");	
	StandOccupancyData DepItem;
	DepItem.m_sName = _T("Departure"); 

	size_t nArrSize = vArrStandDelay.size();
	size_t nDepSize = vDepStandDelay.size();

	long lStart = 0;
	long lEnd = 0;

	for (int i=0; i< nIntervalSize; i++)
	{
		lStart = pParameter->getInterval()*i;
		lEnd = pParameter->getInterval()*(i+1);

		int nArrCount = 0;
		for (size_t j = 0; j < nArrSize; j++)
		{
			long lDelay = vArrStandDelay.at(j);

			if( lStart <= lDelay && lDelay < lEnd)
				nArrCount++;						
		}
		ArrItem.m_vStandData.push_back(nArrCount);

		int nDepCount = 0;
		for (size_t j = 0; j < nDepSize; j++)
		{
			long lDelay = vDepStandDelay.at(j);

			if( lStart <= lDelay && lDelay < lEnd)
				nDepCount++;						
		}
		DepItem.m_vStandData.push_back(nDepCount);
	}
	m_vOccupancyData.push_back(ArrItem);
	m_vOccupancyData.push_back(DepItem);

}

