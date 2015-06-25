#include "StdAfx.h"
#include ".\flightstandoperationsummarychartreport.h"
#include "StandOperationData.h"

////////////////////////////////////////////////////////////////////////////////////////////
CSummaryStandOperationChartResult::CSummaryStandOperationChartResult()
{

}

CSummaryStandOperationChartResult::~CSummaryStandOperationChartResult()
{

}

void CSummaryStandOperationChartResult::GenerateResult( std::vector<CStandOperationReportData*>& vResult,CParameters *pParameter )
{

}

void CSummaryStandOperationChartResult::GenerateResult( std::vector<CSummaryStandOperationData*>& vSummaryResult)
{
	m_vResult.assign(vSummaryResult.begin(), vSummaryResult.end());
}

/////summary conflict////////////////////////////////////////////////////////////////////////////////
CSummaryStandConflictChartResult::CSummaryStandConflictChartResult()
{

}

CSummaryStandConflictChartResult::~CSummaryStandConflictChartResult()
{

}

void CSummaryStandConflictChartResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Summary Stand Conflict");
	c2dGraphData.m_strYtitle = _T("Count");
	c2dGraphData.m_strXtitle = _T("Stand");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Summary Stand Operations %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	std::vector<CString> vLegends;
	//get the legend
	vLegends.push_back(_T("Min"));
	vLegends.push_back(_T("Average"));
	vLegends.push_back(_T("Max"));
	vLegends.push_back(_T("Q1"));
	vLegends.push_back(_T("Q2"));
	vLegends.push_back(_T("Q3"));
	vLegends.push_back(_T("P1"));
	vLegends.push_back(_T("P5"));
	vLegends.push_back(_T("P10"));
	vLegends.push_back(_T("P90"));
	vLegends.push_back(_T("P95"));
	vLegends.push_back(_T("P99"));
	vLegends.push_back(_T("Sigma"));


	std::vector< std::vector<double> > vLegendData;
	vLegendData.resize(vLegends.size());


	size_t nResultCount = m_vResult.size();
	for (size_t i = 0; i < nResultCount; i++)
	{
		CSummaryStandOperationData* pData = m_vResult.at(i);

		if(pData->m_eDataType != CSummaryStandOperationData::StandConflictSummaryData)
			continue;

		c2dGraphData.m_vrXTickTitle.push_back(pData->m_sName);

		vLegendData[0].push_back(pData->m_SummaryData.GetMin());
		vLegendData[1].push_back(pData->m_SummaryData.GetAvarage());
		vLegendData[2].push_back(pData->m_SummaryData.GetMax());
		vLegendData[3].push_back(pData->m_SummaryData.GetPercentile(25));
		vLegendData[4].push_back(pData->m_SummaryData.GetPercentile(50));
		vLegendData[5].push_back(pData->m_SummaryData.GetPercentile(75));
		vLegendData[6].push_back(pData->m_SummaryData.GetPercentile(1));
		vLegendData[7].push_back(pData->m_SummaryData.GetPercentile(5));
		vLegendData[8].push_back(pData->m_SummaryData.GetPercentile(10));
		vLegendData[9].push_back(pData->m_SummaryData.GetPercentile(90));
		vLegendData[10].push_back(pData->m_SummaryData.GetPercentile(95));
		vLegendData[11].push_back(pData->m_SummaryData.GetPercentile(99));
		vLegendData[12].push_back(pData->m_SummaryData.GetSigma());
	}

	int nLegendCount = vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	chartWnd.DrawChart(c2dGraphData);
}

//////summary delay//////////////////////////////////////////////////////////////////////////////////
CSummaryStandDelayChartResult::CSummaryStandDelayChartResult()
{

}

CSummaryStandDelayChartResult::~CSummaryStandDelayChartResult()
{

}

void CSummaryStandDelayChartResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Summary Stand Delays");
	c2dGraphData.m_strYtitle = _T("Time(second)");
	c2dGraphData.m_strXtitle = _T("Stand");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Summary Stand Operations %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	std::vector<CString> vLegends;
	//get the legend
	vLegends.push_back(_T("Min"));
	vLegends.push_back(_T("Average"));
	vLegends.push_back(_T("Max"));
	vLegends.push_back(_T("Q1"));
	vLegends.push_back(_T("Q2"));
	vLegends.push_back(_T("Q3"));
	vLegends.push_back(_T("P1"));
	vLegends.push_back(_T("P5"));
	vLegends.push_back(_T("P10"));
	vLegends.push_back(_T("P90"));
	vLegends.push_back(_T("P95"));
	vLegends.push_back(_T("P99"));
	vLegends.push_back(_T("Sigma"));


	std::vector< std::vector<double> > vLegendData;
	vLegendData.resize(vLegends.size());


	size_t nResultCount = m_vResult.size();
	for (size_t i = 0; i < nResultCount; i++)
	{
		CSummaryStandOperationData* pData = m_vResult.at(i);

		if(pData->m_eDataType != CSummaryStandOperationData::StandDelaySummaryData)
			continue;

		c2dGraphData.m_vrXTickTitle.push_back(pData->m_sName);
		//min
		vLegendData[0].push_back(pData->m_SummaryData.GetMin());
		vLegendData[1].push_back(pData->m_SummaryData.GetAvarage());
		vLegendData[2].push_back(pData->m_SummaryData.GetMax());
		vLegendData[3].push_back(pData->m_SummaryData.GetPercentile(25));
		vLegendData[4].push_back(pData->m_SummaryData.GetPercentile(50));
		vLegendData[5].push_back(pData->m_SummaryData.GetPercentile(75));
		vLegendData[6].push_back(pData->m_SummaryData.GetPercentile(1));
		vLegendData[7].push_back(pData->m_SummaryData.GetPercentile(5));
		vLegendData[8].push_back(pData->m_SummaryData.GetPercentile(10));
		vLegendData[9].push_back(pData->m_SummaryData.GetPercentile(90));
		vLegendData[10].push_back(pData->m_SummaryData.GetPercentile(95));
		vLegendData[11].push_back(pData->m_SummaryData.GetPercentile(99));
		vLegendData[12].push_back(pData->m_SummaryData.GetSigma());
	}

	int nLegendCount = vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	chartWnd.DrawChart(c2dGraphData);
}


////summary plan/////////////////////////////////////////////////////////////////////////////////////
CSummarySchedStandUtilizationChartResult::CSummarySchedStandUtilizationChartResult()
{

}

CSummarySchedStandUtilizationChartResult::~CSummarySchedStandUtilizationChartResult()
{

}

void CSummarySchedStandUtilizationChartResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Summary Schedule Stand Utilization");
	c2dGraphData.m_strYtitle = _T("Time(second)");
	c2dGraphData.m_strXtitle = _T("Stand");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Summary Stand Operations %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	std::vector<CString> vLegends;
	//get the legend
	vLegends.push_back(_T("Min"));
	vLegends.push_back(_T("Average"));
	vLegends.push_back(_T("Max"));
	vLegends.push_back(_T("Q1"));
	vLegends.push_back(_T("Q2"));
	vLegends.push_back(_T("Q3"));
	vLegends.push_back(_T("P1"));
	vLegends.push_back(_T("P5"));
	vLegends.push_back(_T("P10"));
	vLegends.push_back(_T("P90"));
	vLegends.push_back(_T("P95"));
	vLegends.push_back(_T("P99"));
	vLegends.push_back(_T("Sigma"));


	std::vector< std::vector<double> > vLegendData;
	vLegendData.resize(vLegends.size());


	size_t nResultCount = m_vResult.size();
	for (size_t i = 0; i < nResultCount; i++)
	{
		CSummaryStandOperationData* pData = m_vResult.at(i);

		if(pData->m_eDataType != CSummaryStandOperationData::SchedStandUtilizeSummaryData)
			continue;

		c2dGraphData.m_vrXTickTitle.push_back(pData->m_sName);
	
		vLegendData[0].push_back(pData->m_SummaryData.GetMin());
		vLegendData[1].push_back(pData->m_SummaryData.GetAvarage());
		vLegendData[2].push_back(pData->m_SummaryData.GetMax());
		vLegendData[3].push_back(pData->m_SummaryData.GetPercentile(25));
		vLegendData[4].push_back(pData->m_SummaryData.GetPercentile(50));
		vLegendData[5].push_back(pData->m_SummaryData.GetPercentile(75));
		vLegendData[6].push_back(pData->m_SummaryData.GetPercentile(1));
		vLegendData[7].push_back(pData->m_SummaryData.GetPercentile(5));
		vLegendData[8].push_back(pData->m_SummaryData.GetPercentile(10));
		vLegendData[9].push_back(pData->m_SummaryData.GetPercentile(90));
		vLegendData[10].push_back(pData->m_SummaryData.GetPercentile(95));
		vLegendData[11].push_back(pData->m_SummaryData.GetPercentile(99));
		vLegendData[12].push_back(pData->m_SummaryData.GetSigma());
	}

	int nLegendCount = vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	chartWnd.DrawChart(c2dGraphData);
}

////////summary actual////////////////////////////////////////////////////////////////////////////////
CSummaryActualStandUtilizationChartResult::CSummaryActualStandUtilizationChartResult()
{

}

CSummaryActualStandUtilizationChartResult::~CSummaryActualStandUtilizationChartResult()
{

}

void CSummaryActualStandUtilizationChartResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Summary Actual Stand Utilization");
	c2dGraphData.m_strYtitle = _T("Time(second)");
	c2dGraphData.m_strXtitle = _T("Stand");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Summary Stand Operations %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	std::vector<CString> vLegends;
	//get the legend
	vLegends.push_back(_T("Min"));
	vLegends.push_back(_T("Average"));
	vLegends.push_back(_T("Max"));
	vLegends.push_back(_T("Q1"));
	vLegends.push_back(_T("Q2"));
	vLegends.push_back(_T("Q3"));
	vLegends.push_back(_T("P1"));
	vLegends.push_back(_T("P5"));
	vLegends.push_back(_T("P10"));
	vLegends.push_back(_T("P90"));
	vLegends.push_back(_T("P95"));
	vLegends.push_back(_T("P99"));
	vLegends.push_back(_T("Sigma"));


	std::vector< std::vector<double> > vLegendData;
	vLegendData.resize(vLegends.size());


	size_t nResultCount = m_vResult.size();
	for (size_t i = 0; i < nResultCount; i++)
	{
		CSummaryStandOperationData* pData = m_vResult.at(i);

		if(pData->m_eDataType != CSummaryStandOperationData::ActualStandUtilizeSummaryData)
			continue;

		c2dGraphData.m_vrXTickTitle.push_back(pData->m_sName);
		
		vLegendData[0].push_back(pData->m_SummaryData.GetMin());
		vLegendData[1].push_back(pData->m_SummaryData.GetAvarage());
		vLegendData[2].push_back(pData->m_SummaryData.GetMax());
		vLegendData[3].push_back(pData->m_SummaryData.GetPercentile(25));
		vLegendData[4].push_back(pData->m_SummaryData.GetPercentile(50));
		vLegendData[5].push_back(pData->m_SummaryData.GetPercentile(75));
		vLegendData[6].push_back(pData->m_SummaryData.GetPercentile(1));
		vLegendData[7].push_back(pData->m_SummaryData.GetPercentile(5));
		vLegendData[8].push_back(pData->m_SummaryData.GetPercentile(10));
		vLegendData[9].push_back(pData->m_SummaryData.GetPercentile(90));
		vLegendData[10].push_back(pData->m_SummaryData.GetPercentile(95));
		vLegendData[11].push_back(pData->m_SummaryData.GetPercentile(99));
		vLegendData[12].push_back(pData->m_SummaryData.GetSigma());
	}

	int nLegendCount = vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	chartWnd.DrawChart(c2dGraphData);
}

////////////////////////////////////////////////////////////////////////////////////////
CSummarySchedStandIdleChartResult::CSummarySchedStandIdleChartResult()
{

}

CSummarySchedStandIdleChartResult::~CSummarySchedStandIdleChartResult()
{

}

void CSummarySchedStandIdleChartResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Summary Schedule Stand Idle");
	c2dGraphData.m_strYtitle = _T("Time(second)");
	c2dGraphData.m_strXtitle = _T("Stand");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Summary Stand Operations %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	std::vector<CString> vLegends;
	//get the legend
	vLegends.push_back(_T("Min"));
	vLegends.push_back(_T("Average"));
	vLegends.push_back(_T("Max"));
	vLegends.push_back(_T("Q1"));
	vLegends.push_back(_T("Q2"));
	vLegends.push_back(_T("Q3"));
	vLegends.push_back(_T("P1"));
	vLegends.push_back(_T("P5"));
	vLegends.push_back(_T("P10"));
	vLegends.push_back(_T("P90"));
	vLegends.push_back(_T("P95"));
	vLegends.push_back(_T("P99"));
	vLegends.push_back(_T("Sigma"));


	std::vector< std::vector<double> > vLegendData;
	vLegendData.resize(vLegends.size());


	size_t nResultCount = m_vResult.size();
	for (size_t i = 0; i < nResultCount; i++)
	{
		CSummaryStandOperationData* pData = m_vResult.at(i);

		if(pData->m_eDataType != CSummaryStandOperationData::SchedStandIdleSummaryData)
			continue;

		c2dGraphData.m_vrXTickTitle.push_back(pData->m_sName);
		
		vLegendData[0].push_back(pData->m_SummaryData.GetMin());
		vLegendData[1].push_back(pData->m_SummaryData.GetAvarage());
		vLegendData[2].push_back(pData->m_SummaryData.GetMax());
		vLegendData[3].push_back(pData->m_SummaryData.GetPercentile(25));
		vLegendData[4].push_back(pData->m_SummaryData.GetPercentile(50));
		vLegendData[5].push_back(pData->m_SummaryData.GetPercentile(75));
		vLegendData[6].push_back(pData->m_SummaryData.GetPercentile(1));
		vLegendData[7].push_back(pData->m_SummaryData.GetPercentile(5));
		vLegendData[8].push_back(pData->m_SummaryData.GetPercentile(10));
		vLegendData[9].push_back(pData->m_SummaryData.GetPercentile(90));
		vLegendData[10].push_back(pData->m_SummaryData.GetPercentile(95));
		vLegendData[11].push_back(pData->m_SummaryData.GetPercentile(99));
		vLegendData[12].push_back(pData->m_SummaryData.GetSigma());
	}

	int nLegendCount = vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	chartWnd.DrawChart(c2dGraphData);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

CSummaryActualStandIdleChartResult::CSummaryActualStandIdleChartResult()
{

}

CSummaryActualStandIdleChartResult::~CSummaryActualStandIdleChartResult()
{

}

void CSummaryActualStandIdleChartResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Summary Actual Stand Idle");
	c2dGraphData.m_strYtitle = _T("Time(second)");
	c2dGraphData.m_strXtitle = _T("Stand");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Summary Stand Operations %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;

	std::vector<CString> vLegends;
	//get the legend
	vLegends.push_back(_T("Min"));
	vLegends.push_back(_T("Average"));
	vLegends.push_back(_T("Max"));
	vLegends.push_back(_T("Q1"));
	vLegends.push_back(_T("Q2"));
	vLegends.push_back(_T("Q3"));
	vLegends.push_back(_T("P1"));
	vLegends.push_back(_T("P5"));
	vLegends.push_back(_T("P10"));
	vLegends.push_back(_T("P90"));
	vLegends.push_back(_T("P95"));
	vLegends.push_back(_T("P99"));
	vLegends.push_back(_T("Sigma"));


	std::vector< std::vector<double> > vLegendData;
	vLegendData.resize(vLegends.size());


	size_t nResultCount = m_vResult.size();
	for (size_t i = 0; i < nResultCount; i++)
	{
		CSummaryStandOperationData* pData = m_vResult.at(i);

		if(pData->m_eDataType != CSummaryStandOperationData::ActualStandIdleSummaryData)
			continue;

		c2dGraphData.m_vrXTickTitle.push_back(pData->m_sName);
	
		vLegendData[0].push_back(pData->m_SummaryData.GetMin());
		vLegendData[1].push_back(pData->m_SummaryData.GetAvarage());
		vLegendData[2].push_back(pData->m_SummaryData.GetMax());
		vLegendData[3].push_back(pData->m_SummaryData.GetPercentile(25));
		vLegendData[4].push_back(pData->m_SummaryData.GetPercentile(50));
		vLegendData[5].push_back(pData->m_SummaryData.GetPercentile(75));
		vLegendData[6].push_back(pData->m_SummaryData.GetPercentile(1));
		vLegendData[7].push_back(pData->m_SummaryData.GetPercentile(5));
		vLegendData[8].push_back(pData->m_SummaryData.GetPercentile(10));
		vLegendData[9].push_back(pData->m_SummaryData.GetPercentile(90));
		vLegendData[10].push_back(pData->m_SummaryData.GetPercentile(95));
		vLegendData[11].push_back(pData->m_SummaryData.GetPercentile(99));
		vLegendData[12].push_back(pData->m_SummaryData.GetSigma());
	}

	int nLegendCount = vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	chartWnd.DrawChart(c2dGraphData);
}