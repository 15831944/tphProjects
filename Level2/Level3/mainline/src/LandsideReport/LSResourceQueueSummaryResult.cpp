#include "StdAfx.h"
#include ".\lsresourcequeuesummaryresult.h"

#include "../MFCExControl/SortableHeaderCtrl.h"
#include "../MFCExControl/XListCtrl.h"

#include "AirsideReport/CARC3DChart.h"
#include "LandsideBaseParam.h"



LSResourceQueueSummaryResult::LSResourceQueueSummaryResult(void)
{
}

LSResourceQueueSummaryResult::~LSResourceQueueSummaryResult(void)
{
}
void LSResourceQueueSummaryResult::Draw3DChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	//draw chart
	if(pParameter == NULL)
		return;

	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Resource Queue Length Report");
	c2dGraphData.m_strYtitle = _T("Queue Length");
	c2dGraphData.m_strXtitle = _T("Facility");	


	ElapsedTime eStartTime = pParameter->getStartTime();
	ElapsedTime eEndTime = pParameter->getEndTime();
	ElapsedTime eInterval(pParameter->getInterval());

	ASSERT(eEndTime >= eStartTime);
	ASSERT(eInterval > ElapsedTime(0L));

	if(eStartTime > eEndTime || eInterval <= ElapsedTime(0L))
		return;

	c2dGraphData.m_vrLegend.push_back("Min");
	c2dGraphData.m_vrLegend.push_back("Average");
	c2dGraphData.m_vrLegend.push_back("Max");
	c2dGraphData.m_vrLegend.push_back("Q1");
	c2dGraphData.m_vrLegend.push_back("Q2");
	c2dGraphData.m_vrLegend.push_back("Q3");
	c2dGraphData.m_vrLegend.push_back("P1");
	c2dGraphData.m_vrLegend.push_back("P5");
	c2dGraphData.m_vrLegend.push_back("P10");
	c2dGraphData.m_vrLegend.push_back("P90");
	c2dGraphData.m_vrLegend.push_back("P95");
	c2dGraphData.m_vrLegend.push_back("P99");
	c2dGraphData.m_vrLegend.push_back("Sigma");
	//get chart type
	int nChartType = pParameter->getSubType();
	CString strVehicleType = pParameter->GetVehicleTypeName(nChartType).toString();
	std::vector<CString > vXTickTitle;
	c2dGraphData.m_vr2DChartData.resize(13);
	int nDataCount = (int)m_ResultData.size();
	for(int nItem = 0; nItem < nDataCount; nItem++)
	{
		SummaryResQueueItem& dItem = m_ResultData[nItem];
		
		if (dItem.m_strVehicleType != strVehicleType)
			continue;

		vXTickTitle.push_back(dItem.m_strObject);

		c2dGraphData.m_vr2DChartData[0].push_back(dItem.m_estMin);
		c2dGraphData.m_vr2DChartData[1].push_back(dItem.m_estAverage);
		c2dGraphData.m_vr2DChartData[2].push_back(dItem.m_estMax);
		c2dGraphData.m_vr2DChartData[3].push_back(dItem.m_estQ1);
		c2dGraphData.m_vr2DChartData[4].push_back(dItem.m_estQ2);
		c2dGraphData.m_vr2DChartData[5].push_back(dItem.m_estQ3);
		c2dGraphData.m_vr2DChartData[6].push_back(dItem.m_estP1);
		c2dGraphData.m_vr2DChartData[7].push_back(dItem.m_estP5);
		c2dGraphData.m_vr2DChartData[8].push_back(dItem.m_estP10);
		c2dGraphData.m_vr2DChartData[9].push_back(dItem.m_estP90);
		c2dGraphData.m_vr2DChartData[10].push_back(dItem.m_estP95);
		c2dGraphData.m_vr2DChartData[11].push_back(dItem.m_estP99);
		c2dGraphData.m_vr2DChartData[12].push_back(dItem.m_estSigma);

	}
	c2dGraphData.m_vrXTickTitle = vXTickTitle;
	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Landside Resource Queue Report %s"), pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);
}

void LSResourceQueueSummaryResult::GenerateResult( CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde )
{
	GenerateChartList(pParameter);
	LSResourceQueueResult::GenerateResult(pFunc, pParameter, pLandisde);

	//generate summary result
	int nDataCount = (int)m_vResDetail.size();
	for (int nItem = 0; nItem < nDataCount; nItem++)
	{
		DetailItem dataItem = m_vResDetail[nItem];
		SummaryResQueueItem summaryItem;
		summaryItem.m_strObject = dataItem.m_strObjName;
		summaryItem.m_strVehicleType = dataItem.m_strVehicleType;
		int nTimeCount = (int)dataItem.m_vTimeItems.size();
		for (int nTime = 0; nTime < nTimeCount; nTime++)
		{	
			DetailTimeItem timeItem = dataItem.m_vTimeItems[nTime];
			summaryItem.m_SummaryData.AddNewData(timeItem.m_nLen);
		}
		summaryItem.m_SummaryData.SortData();
		//Statistical
		{
			summaryItem.m_estMin = static_cast<int>(summaryItem.m_SummaryData.GetMin());
			summaryItem.m_estMax = static_cast<int>(summaryItem.m_SummaryData.GetMax());
			summaryItem.m_estAverage = summaryItem.m_SummaryData.GetAvarage();
			summaryItem.m_estQ1 = summaryItem.m_SummaryData.GetPercentile(25);
			summaryItem.m_estQ2 = summaryItem.m_SummaryData.GetPercentile(50);
			summaryItem.m_estQ3 = summaryItem.m_SummaryData.GetPercentile(75);
			summaryItem.m_estP1 = summaryItem.m_SummaryData.GetPercentile(1);
			summaryItem.m_estP5 = summaryItem.m_SummaryData.GetPercentile(5);
			summaryItem.m_estP10 = summaryItem.m_SummaryData.GetPercentile(10);
			summaryItem.m_estP90 = summaryItem.m_SummaryData.GetPercentile(90);
			summaryItem.m_estP95 = summaryItem.m_SummaryData.GetPercentile(95);
			summaryItem.m_estP99 = summaryItem.m_SummaryData.GetPercentile(99);
			summaryItem.m_estSigma = summaryItem.m_SummaryData.GetSigma();
		}
		m_ResultData.push_back(summaryItem);
	}

}

void LSResourceQueueSummaryResult::RefreshReport( LandsideBaseParam * parameter )
{

}

void LSResourceQueueSummaryResult::InitListHead( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ )
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	ASSERT(piSHC != NULL);
	if (piSHC == NULL)
		return;


	int colIndex = 0;

	cxListCtrl.InsertColumn(colIndex, "Facility", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtSTRING );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex,"Vehicle Type",LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex,dtSTRING);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, _T("Min"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(colIndex,dtINT);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, _T("Max"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(colIndex,dtINT);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, _T("Average"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(colIndex,dtDEC);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, _T("Q1"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(colIndex,dtDEC);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, _T("Q2"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(colIndex,dtDEC);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, _T("Q3"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(colIndex,dtDEC);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, _T("P1"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(colIndex,dtDEC);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, _T("P5"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(colIndex,dtDEC);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, _T("P10"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(colIndex,dtDEC);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, _T("P90"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(colIndex,dtDEC);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, _T("P95"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(colIndex,dtDEC);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, _T("P99"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(colIndex,dtDEC);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, _T("sigma"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(colIndex,dtDEC);


}

void LSResourceQueueSummaryResult::FillListContent( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter )
{	
	int nDataCount = (int)m_ResultData.size();
	int nRowIndex = 0;
	int nColIndex = 1;
	for (int nItem = 0; nItem < nDataCount; nItem++)
	{
		SummaryResQueueItem queueItem = m_ResultData[nItem];
		cxListCtrl.InsertItem(nRowIndex, queueItem.m_strObject);

		//vehicle Type
		nColIndex = 1;
		cxListCtrl.SetItemText(nRowIndex, nColIndex,queueItem.m_strVehicleType);

		nColIndex += 1;
		//Min queue
		CString strValue;
		strValue.Format(_T("%d"),queueItem.m_estMin);
		cxListCtrl.SetItemText(nRowIndex, nColIndex,strValue);

		//Max queue
		nColIndex += 1;
		strValue.Format(_T("%d"),queueItem.m_estMax);
		cxListCtrl.SetItemText(nRowIndex, nColIndex,strValue);

		//Mean queue
		nColIndex += 1;
		strValue.Format(_T("%.2f"),queueItem.m_estAverage);
		cxListCtrl.SetItemText(nRowIndex, nColIndex,strValue);

		//Q1
		nColIndex += 1;
		strValue.Format(_T("%.2f"),queueItem.m_estQ1);
		cxListCtrl.SetItemText(nRowIndex, nColIndex,strValue);

		//Q2
		nColIndex += 1;
		strValue.Format(_T("%.2f"),queueItem.m_estQ2);
		cxListCtrl.SetItemText(nRowIndex, nColIndex,strValue);

		//Q3
		nColIndex += 1;
		strValue.Format(_T("%.2f"),queueItem.m_estQ3);
		cxListCtrl.SetItemText(nRowIndex, nColIndex,strValue);

		//P1
		nColIndex += 1;
		strValue.Format(_T("%.2f"),queueItem.m_estP1);
		cxListCtrl.SetItemText(nRowIndex, nColIndex,strValue);

		//P5
		nColIndex += 1;
		strValue.Format(_T("%.2f"),queueItem.m_estP5);
		cxListCtrl.SetItemText(nRowIndex, nColIndex,strValue);

		//P10
		nColIndex += 1;
		strValue.Format(_T("%.2f"),queueItem.m_estP10);
		cxListCtrl.SetItemText(nRowIndex, nColIndex,strValue);

		//P90
		nColIndex += 1;
		strValue.Format(_T("%.2f"),queueItem.m_estP90);
		cxListCtrl.SetItemText(nRowIndex, nColIndex,strValue);

		//P95
		nColIndex += 1;
		strValue.Format(_T("%.2f"),queueItem.m_estP95);
		cxListCtrl.SetItemText(nRowIndex, nColIndex,strValue);

		//P99
		nColIndex += 1;
		strValue.Format(_T("%.2f"),queueItem.m_estP99);
		cxListCtrl.SetItemText(nRowIndex, nColIndex,strValue);

		//Std dev
		nColIndex += 1;
		strValue.Format(_T("%.2f"),queueItem.m_estSigma);
		cxListCtrl.SetItemText(nRowIndex, nColIndex,strValue);

		nRowIndex++;
	}
}

LSGraphChartTypeList LSResourceQueueSummaryResult::GetChartList() const
{	
	return m_chartList;
}

BOOL LSResourceQueueSummaryResult::ReadReportData( ArctermFile& _file )
{
	int nDataCount = 0;
	_file.getInteger(nDataCount);
	_file.getLine();

	for (int i = 0; i < nDataCount; i++)
	{
		SummaryResQueueItem dataItem;

		_file.getField(dataItem.m_strObject.GetBuffer(1024),1024);
		dataItem.m_strObject.ReleaseBuffer();
		_file.getField(dataItem.m_strVehicleType.GetBuffer(1024),1024);
		dataItem.m_strVehicleType.ReleaseBuffer();

		_file.getInteger(dataItem.m_estMin);
		_file.getFloat(dataItem.m_estAverage);
		_file.getInteger(dataItem.m_estMax);
		_file.getFloat(dataItem.m_estQ1);
		_file.getFloat(dataItem.m_estQ2);
		_file.getFloat(dataItem.m_estQ3);
		_file.getFloat(dataItem.m_estP1);
		_file.getFloat(dataItem.m_estP5);
		_file.getFloat(dataItem.m_estP10);
		_file.getFloat(dataItem.m_estP90);
		_file.getFloat(dataItem.m_estP95);
		_file.getFloat(dataItem.m_estP99);
		_file.getFloat(dataItem.m_estSigma);
		m_ResultData.push_back(dataItem);
	
		_file.getLine();
	}
	return TRUE;
}

BOOL LSResourceQueueSummaryResult::WriteReportData( ArctermFile& _file )
{
	int nDataCount = (int)m_ResultData.size();
	_file.writeInt(nDataCount);
	_file.writeLine();


	for(int nItem = 0; nItem < nDataCount; nItem++)
	{
		SummaryResQueueItem dataItem = m_ResultData[nItem];
		
		_file.writeField(dataItem.m_strObject);
		_file.writeField(dataItem.m_strVehicleType);
		_file.writeInt(dataItem.m_estMin);
		_file.writeDouble(dataItem.m_estAverage);
		_file.writeInt(dataItem.m_estMax);
		_file.writeDouble(dataItem.m_estQ1);
		_file.writeDouble(dataItem.m_estQ2);
		_file.writeDouble(dataItem.m_estQ3);
		_file.writeDouble(dataItem.m_estP1);
		_file.writeDouble(dataItem.m_estP5);
		_file.writeDouble(dataItem.m_estP10);
		_file.writeDouble(dataItem.m_estP90);
		_file.writeDouble(dataItem.m_estP95);
		_file.writeDouble(dataItem.m_estP99);
		_file.writeDouble(dataItem.m_estSigma);
		_file.writeLine();
	}
	return TRUE;
}

void LSResourceQueueSummaryResult::GenerateChartList( LandsideBaseParam * parameter )
{
	m_chartList.Clean();
	for (int i = 0; i < parameter->GetVehicleTypeNameCount(); i++)
	{
		CString strVehicleType = parameter->GetVehicleTypeName(i).toString();
		CString strRepTitle;
		strRepTitle.Format(_T("Queue length[%s]"),strVehicleType);
		m_chartList.AddItem(i,strRepTitle);
	}
}
