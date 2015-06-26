#include "StdAfx.h"
#include ".\airsidetakeoffprocesssummaryresult.h"
#include "../Common/termfile.h"
#include "Parameters.h"
#include "CARC3DChart.h"
#include "../MFCExControl/XListCtrl.h"
#include "AirsideTakeoffProcessParameter.h"
#include "../Reports/StatisticalTools.h"


CAirsideTakeoffProcessSummaryResult::CAirsideTakeoffProcessSummaryResult(void)
:m_pChartResult(NULL)
{
}

CAirsideTakeoffProcessSummaryResult::~CAirsideTakeoffProcessSummaryResult(void)
{
	if (m_pChartResult)
	{
		delete m_pChartResult;
		m_pChartResult = NULL;
	}
}

void CAirsideTakeoffProcessSummaryResult::AddResultData(const CTakeoffProcessDetailData& detailData )
{
	for (size_t i = 0; i < m_data.size(); i++)
	{
		CTakeoffProcessSummaryData& summaryData = m_data[i];
		if (summaryData.m_sFlightType == detailData.m_sFlightType)
		{
			summaryData.detailDataList.push_back(detailData);
			return;
		}
	}

	CTakeoffProcessSummaryData newSummaryData;
	newSummaryData.m_sFlightType = detailData.m_sFlightType;
	newSummaryData.detailDataList.push_back(detailData);
	m_data.push_back(newSummaryData);
}

void CAirsideTakeoffProcessSummaryResult::BuiltResultData()
{
	//clear old data
	m_data.clear();

	//build new data
	for (size_t i = 0; i < m_vResult.size(); i++)
	{
		const CTakeoffProcessDetailData& detailData = m_vResult[i];
		AddResultData(detailData);
	}
}

void CAirsideTakeoffProcessSummaryResult::GenerateResult( CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter )
{
	CAirsideTakeoffProcessResult::GenerateResult(pCBGetLogFilePath,pParameter);
	//clear summary result old data
	m_data.clear();

	BuiltResultData();

	//generate summary data
	for (size_t i = 0; i < m_data.size(); i++)
	{
		CTakeoffProcessSummaryData& summaryData = m_data[i];
		for (size_t j = 0; j < summaryData.detailDataList.size(); j++)
		{
			CTakeoffProcessDetailData& detailData = summaryData.detailDataList[j];
			summaryData.m_TakeoffQueue.AddNewData(detailData.m_lTakeoffQueueDelay);
			summaryData.m_TimeHoldShortLine.AddNewData(detailData.m_lDelayHoldShortLine);
			summaryData.m_TaixToPosition.AddNewData(detailData.m_lToPostionTime);
			summaryData.m_TimeToPosition.AddNewData(detailData.m_lTimeInPosition);
		}
		summaryData.m_TakeoffQueue.SortData();
		summaryData.m_TimeHoldShortLine.SortData();
		summaryData.m_TaixToPosition.SortData();
		summaryData.m_TimeToPosition.SortData();

	}
	RefreshReport(pParameter);
}

void CAirsideTakeoffProcessSummaryResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	if (m_pChartResult)
	{
		m_pChartResult->Draw3DChart(chartWnd,pParameter);
	}
}

void CAirsideTakeoffProcessSummaryResult::RefreshReport( CParameters * parameter )
{
	CAirsideTakeoffProcessParameter* pParam = (CAirsideTakeoffProcessParameter*)parameter;
	ASSERT(pParam != NULL);

	if (NULL != m_pChartResult)
	{
		delete m_pChartResult;
		m_pChartResult = NULL;
	}

	switch(pParam->getSubType())
	{
	case TakeOffQueueTime:
		{
			m_pChartResult = new CAirsideTakeoffQueueSummaryChart;
		}
		break;
	case TimeAtHoldShortLine:
		{
			m_pChartResult = new CAirsideTimeAtHoldShortLineSummaryChart;
		}
		break;
	case TaxiToPosition:
		{
			m_pChartResult = new CAirsideTaxiToPositionSummaryChart;
		}
		break;
	case TimeInPosition:
		{
			m_pChartResult = new CAirsideTimeInPositionSummaryChart;
		}
		break;
	default:
		return;
	}

	m_pChartResult->GenerateResult(m_data, parameter);
	
}

void CAirsideTakeoffProcessSummaryResult::InitListHead( CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Flight Type"), LVCFMT_LEFT, 80);	

	cxListCtrl.InsertColumn(1, _T("Min(mm:ss)"), LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(2, _T("Avg(mm:ss)"), LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(3, _T("Max(mm:ss)"),LVCFMT_LEFT, 100);

	cxListCtrl.InsertColumn(4, _T("Q1(mm:ss)"),LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(5, _T("Q2(mm:ss)"),LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(6, _T("Q3(mm:ss)"),LVCFMT_LEFT, 100);

	cxListCtrl.InsertColumn(7, _T("P1(mm:ss)"),LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(8, _T("P5(mm:ss)"),LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(9, _T("P10(mm:ss)"),LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(10, _T("P90(mm:ss)"),LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(11, _T("P95(mm:ss)"),LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(12, _T("P99(mm:ss)"),LVCFMT_LEFT, 100);

	cxListCtrl.InsertColumn(13, _T("Std Dev(mm:ss)"),LVCFMT_LEFT, 100);
}

void CAirsideTakeoffProcessSummaryResult::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{
	cxListCtrl.DeleteAllItems();
	int nSize = (int)m_data.size();
	for (int i=0; i <nSize; i++)
	{
		CTakeoffProcessSummaryData& pSummaryData = m_data.at(i);

		
		CString strName(_T(""));

		cxListCtrl.InsertItem(i,pSummaryData.m_sFlightType);
		CStatisticalTools<double>& staDataTools = pSummaryData.GetResultDataByType(parameter->getSubType());
		CString strValue;
		{
			ElapsedTime eTime(long(static_cast<long>(staDataTools.GetMin())/100+0.5));
			strValue.Format("%02d:%02d",eTime.GetMinute(),eTime.GetSecond());		
			cxListCtrl.SetItemText(i,1,strValue);
		}

		{
			ElapsedTime eTime(long(static_cast<long>(staDataTools.GetAvarage())/100+0.5));
			strValue.Format("%02d:%02d",eTime.GetMinute(),eTime.GetSecond());			
			cxListCtrl.SetItemText(i,2,strValue);
		}

		{
			ElapsedTime eTime(long(static_cast<long>(staDataTools.GetMax())/100+0.5));
			strValue.Format("%02d:%02d",eTime.GetMinute(),eTime.GetSecond());			
			cxListCtrl.SetItemText(i,3,strValue);
		}

		{
			ElapsedTime eTime(long(static_cast<long>(staDataTools.GetPercentile(25))/100+0.5));
			strValue.Format("%02d:%02d",eTime.GetMinute(),eTime.GetSecond());		
			cxListCtrl.SetItemText(i,4,strValue);
		}


		{
			ElapsedTime eTime(long(static_cast<long>(staDataTools.GetPercentile(50))/100+0.5));
			strValue.Format("%02d:%02d",eTime.GetMinute(),eTime.GetSecond());		
			cxListCtrl.SetItemText(i,5,strValue);
		}

		{
			ElapsedTime eTime(long(static_cast<long>(staDataTools.GetPercentile(75))/100+0.5));
			strValue.Format("%02d:%02d",eTime.GetMinute(),eTime.GetSecond());	
			cxListCtrl.SetItemText(i,6,strValue);
		}

		{
			ElapsedTime eTime(long(static_cast<long>(staDataTools.GetPercentile(1))/100+0.5));
			strValue.Format("%02d:%02d",eTime.GetMinute(),eTime.GetSecond());			
			cxListCtrl.SetItemText(i,7,strValue);
		}

		{
			ElapsedTime eTime(long(static_cast<long>(staDataTools.GetPercentile(5))/100+0.5));
			strValue.Format("%02d:%02d",eTime.GetMinute(),eTime.GetSecond());			
			cxListCtrl.SetItemText(i,8,strValue);
		}

		{
			ElapsedTime eTime(long(static_cast<long>(staDataTools.GetPercentile(10))/100+0.5));
			strValue.Format("%02d:%02d",eTime.GetMinute(),eTime.GetSecond());			
			cxListCtrl.SetItemText(i,9,strValue);
		}

		{
			ElapsedTime eTime(long(static_cast<long>(staDataTools.GetPercentile(90))/100+0.5));
			strValue.Format("%02d:%02d",eTime.GetMinute(),eTime.GetSecond());		
			cxListCtrl.SetItemText(i,10,strValue);
		}

		{
			ElapsedTime eTime(long(static_cast<long>(staDataTools.GetPercentile(95))/100+0.5));
			strValue.Format("%02d:%02d",eTime.GetMinute(),eTime.GetSecond());	
			cxListCtrl.SetItemText(i,11,strValue);
		}

		{
			ElapsedTime eTime(long(static_cast<long>(staDataTools.GetPercentile(99))/100+0.5));
			strValue.Format("%02d:%02d",eTime.GetMinute(),eTime.GetSecond());			
			cxListCtrl.SetItemText(i,12,strValue);
		}

		{
			ElapsedTime eTime(long(static_cast<long>(staDataTools.GetSigma())/100+0.5));
			strValue.Format("%02d:%02d",eTime.GetMinute(),eTime.GetSecond());			
			cxListCtrl.SetItemText(i,13,strValue);
		}

	}
}

BOOL CAirsideTakeoffProcessSummaryResult::ExportReportData( ArctermFile& _file,CString& Errmsg )
{
	return WriteReportData(_file);
}

BOOL CAirsideTakeoffProcessSummaryResult::ImportReportData( ArctermFile& _file,CString& Errmsg )
{
	return ReadReportData(_file);
}

BOOL CAirsideTakeoffProcessSummaryResult::ExportListData( ArctermFile& _file,CParameters * parameter )
{
	return TRUE;
}

BOOL CAirsideTakeoffProcessSummaryResult::WriteReportData( ArctermFile& _file )
{
	_file.writeField("Airside Summary Takeoff Process Report");
	_file.writeLine();

	_file.writeInt(ASReportType_Summary);
	_file.writeLine();

	int size = (int)m_data.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;
	for (int i = 0 ; i < size ; i++)
	{
		CTakeoffProcessSummaryData& summaryData = m_data[i];
		summaryData.WriteReportData(_file);
		_file.writeLine();
	}
	return TRUE ;
}

BOOL CAirsideTakeoffProcessSummaryResult::ReadReportData( ArctermFile& _file )
{
	m_vResult.clear();
	int size = 0 ;
	if(!_file.getInteger(size))
		return FALSE ;
	for (int i = 0 ; i < size ;i++)
	{
		_file.getLine() ;
		CTakeoffProcessSummaryData  summaryData ;
		summaryData.ReadReportData(_file) ;
		m_data.push_back(summaryData);
	}
	return TRUE ;
}

CAirsideTakeoffProcessSummaryChart::CAirsideTakeoffProcessSummaryChart()
{

}

CAirsideTakeoffProcessSummaryChart::~CAirsideTakeoffProcessSummaryChart()
{

}

void CAirsideTakeoffProcessSummaryChart::GenerateResult( TakeoffSummaryDataList& vResult,CParameters *pParameter )
{
	m_vResult.assign(vResult.begin(),vResult.end());
}

CAirsideTakeoffQueueSummaryChart::CAirsideTakeoffQueueSummaryChart()
{

}

CAirsideTakeoffQueueSummaryChart::~CAirsideTakeoffQueueSummaryChart()
{

}

void CAirsideTakeoffQueueSummaryChart::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Take off Q time Report");
	c2dGraphData.m_strYtitle = _T(" Time(mins)");
	c2dGraphData.m_strXtitle = _T("Flight type");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Summary Airside Take off Q time Report %s "), pParameter->GetParameterString());
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
		CTakeoffProcessSummaryData& pData = m_vResult.at(i);

		c2dGraphData.m_vrXTickTitle.push_back(pData.m_sFlightType);
		vLegendData[0].push_back(pData.m_TakeoffQueue.GetMin()/6000.0);
		vLegendData[1].push_back(pData.m_TakeoffQueue.GetAvarage()/6000.0);
		vLegendData[2].push_back(pData.m_TakeoffQueue.GetMax()/6000.0);
		vLegendData[3].push_back(pData.m_TakeoffQueue.GetPercentile(25)/6000.0);
		vLegendData[4].push_back(pData.m_TakeoffQueue.GetPercentile(50)/6000.0);
		vLegendData[5].push_back(pData.m_TakeoffQueue.GetPercentile(75)/6000.0);
		vLegendData[6].push_back(pData.m_TakeoffQueue.GetPercentile(1)/6000.0);
		vLegendData[7].push_back(pData.m_TakeoffQueue.GetPercentile(5)/6000.0);
		vLegendData[8].push_back(pData.m_TakeoffQueue.GetPercentile(10)/6000.0);
		vLegendData[9].push_back(pData.m_TakeoffQueue.GetPercentile(90)/6000.0);
		vLegendData[10].push_back(pData.m_TakeoffQueue.GetPercentile(95)/6000.0);
		vLegendData[11].push_back(pData.m_TakeoffQueue.GetPercentile(99)/6000.0);
		vLegendData[12].push_back(pData.m_TakeoffQueue.GetSigma()/6000.0);
	}

	int nLegendCount = (int)vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	chartWnd.DrawChart(c2dGraphData);
}

CAirsideTimeAtHoldShortLineSummaryChart::CAirsideTimeAtHoldShortLineSummaryChart()
{

}

CAirsideTimeAtHoldShortLineSummaryChart::~CAirsideTimeAtHoldShortLineSummaryChart()
{

}

void CAirsideTimeAtHoldShortLineSummaryChart::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Time at Hold Short Line Report");
	c2dGraphData.m_strYtitle = _T(" Time(mins)");
	c2dGraphData.m_strXtitle = _T("Flight type");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Summary Airside Time at Hold Short Line Report %s "), pParameter->GetParameterString());
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
		CTakeoffProcessSummaryData& pData = m_vResult.at(i);

		c2dGraphData.m_vrXTickTitle.push_back(pData.m_sFlightType);
		vLegendData[0].push_back(pData.m_TimeHoldShortLine.GetMin()/6000.0);
		vLegendData[1].push_back(pData.m_TimeHoldShortLine.GetAvarage()/6000.0);
		vLegendData[2].push_back(pData.m_TimeHoldShortLine.GetMax()/6000.0);
		vLegendData[3].push_back(pData.m_TimeHoldShortLine.GetPercentile(25)/6000.0);
		vLegendData[4].push_back(pData.m_TimeHoldShortLine.GetPercentile(50)/6000.0);
		vLegendData[5].push_back(pData.m_TimeHoldShortLine.GetPercentile(75)/6000.0);
		vLegendData[6].push_back(pData.m_TimeHoldShortLine.GetPercentile(1)/6000.0);
		vLegendData[7].push_back(pData.m_TimeHoldShortLine.GetPercentile(5)/6000.0);
		vLegendData[8].push_back(pData.m_TimeHoldShortLine.GetPercentile(10)/6000.0);
		vLegendData[9].push_back(pData.m_TimeHoldShortLine.GetPercentile(90)/6000.0);
		vLegendData[10].push_back(pData.m_TimeHoldShortLine.GetPercentile(95)/6000.0);
		vLegendData[11].push_back(pData.m_TimeHoldShortLine.GetPercentile(99)/6000.0);
		vLegendData[12].push_back(pData.m_TimeHoldShortLine.GetSigma()/6000.0);
	}

	int nLegendCount = (int)vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	chartWnd.DrawChart(c2dGraphData);
}

CAirsideTaxiToPositionSummaryChart::CAirsideTaxiToPositionSummaryChart()
{

}

CAirsideTaxiToPositionSummaryChart::~CAirsideTaxiToPositionSummaryChart()
{

}

void CAirsideTaxiToPositionSummaryChart::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Taxi to Position Report");
	c2dGraphData.m_strYtitle = _T(" Time(mins)");
	c2dGraphData.m_strXtitle = _T("Flight type");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Summary Airside Taxi to Position Report %s "), pParameter->GetParameterString());
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
		CTakeoffProcessSummaryData& pData = m_vResult.at(i);

		c2dGraphData.m_vrXTickTitle.push_back(pData.m_sFlightType);
		vLegendData[0].push_back(pData.m_TaixToPosition.GetMin()/6000.0);
		vLegendData[1].push_back(pData.m_TaixToPosition.GetAvarage()/6000.0);
		vLegendData[2].push_back(pData.m_TaixToPosition.GetMax()/6000.0);
		vLegendData[3].push_back(pData.m_TaixToPosition.GetPercentile(25)/6000.0);
		vLegendData[4].push_back(pData.m_TaixToPosition.GetPercentile(50)/6000.0);
		vLegendData[5].push_back(pData.m_TaixToPosition.GetPercentile(75)/6000.0);
		vLegendData[6].push_back(pData.m_TaixToPosition.GetPercentile(1)/6000.0);
		vLegendData[7].push_back(pData.m_TaixToPosition.GetPercentile(5)/6000.0);
		vLegendData[8].push_back(pData.m_TaixToPosition.GetPercentile(10)/6000.0);
		vLegendData[9].push_back(pData.m_TaixToPosition.GetPercentile(90)/6000.0);
		vLegendData[10].push_back(pData.m_TaixToPosition.GetPercentile(95)/6000.0);
		vLegendData[11].push_back(pData.m_TaixToPosition.GetPercentile(99)/6000.0);
		vLegendData[12].push_back(pData.m_TaixToPosition.GetSigma()/6000.0);
	}

	int nLegendCount = (int)vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	chartWnd.DrawChart(c2dGraphData);
}

CAirsideTimeInPositionSummaryChart::CAirsideTimeInPositionSummaryChart()
{

}

CAirsideTimeInPositionSummaryChart::~CAirsideTimeInPositionSummaryChart()
{

}

void CAirsideTimeInPositionSummaryChart::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Time in Position Report");
	c2dGraphData.m_strYtitle = _T(" Time(mins)");
	c2dGraphData.m_strXtitle = _T("Flight type");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Summary Onboard Time in Position Report %s "), pParameter->GetParameterString());
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
		CTakeoffProcessSummaryData& pData = m_vResult.at(i);

		c2dGraphData.m_vrXTickTitle.push_back(pData.m_sFlightType);
		vLegendData[0].push_back(pData.m_TimeToPosition.GetMin()/6000.0);
		vLegendData[1].push_back(pData.m_TimeToPosition.GetAvarage()/6000.0);
		vLegendData[2].push_back(pData.m_TimeToPosition.GetMax()/6000.0);
		vLegendData[3].push_back(pData.m_TimeToPosition.GetPercentile(25)/6000.0);
		vLegendData[4].push_back(pData.m_TimeToPosition.GetPercentile(50)/6000.0);
		vLegendData[5].push_back(pData.m_TimeToPosition.GetPercentile(75)/6000.0);
		vLegendData[6].push_back(pData.m_TimeToPosition.GetPercentile(1)/6000.0);
		vLegendData[7].push_back(pData.m_TimeToPosition.GetPercentile(5)/6000.0);
		vLegendData[8].push_back(pData.m_TimeToPosition.GetPercentile(10)/6000.0);
		vLegendData[9].push_back(pData.m_TimeToPosition.GetPercentile(90)/6000.0);
		vLegendData[10].push_back(pData.m_TimeToPosition.GetPercentile(95)/6000.0);
		vLegendData[11].push_back(pData.m_TimeToPosition.GetPercentile(99)/6000.0);
		vLegendData[12].push_back(pData.m_TimeToPosition.GetSigma()/6000.0);
	}

	int nLegendCount = (int)vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	chartWnd.DrawChart(c2dGraphData);
}