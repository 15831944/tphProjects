#include "StdAfx.h"
#include ".\repgraphviewmultirunreportoperator.h"


#include "TermPlanDoc.h"
#include "../Reports/ReportParaDB.h"
#include "../Reports/ReportParameter.h"

#include "../chart/vcplot.h"
#include "../chart/vctitle.h"
#include "../chart/vcfont.h"
#include "../chart/vcaxis.h"
#include "../chart/vcaxistitle.h"
#include "../chart/vcSeriesCollection.h"
#include "../chart/VcSeries.h"
#include "../chart/VcPen.h"
#include "../chart/vccolor.h"
#include "../chart/vcdatapoints.h"
#include "../chart/vcdatapoint.h"
#include "../chart/vcbrush.h"
#include "../chart/vcfootnote.h"
#include "../chart/vcAxisGrid.h"
#include "mschart.h"

CRepGraphViewMultiRunReportOperator::CRepGraphViewMultiRunReportOperator(CRepGraphView *pGraphView)
:CRepGraphViewBaseOperator(pGraphView)
{
}

CRepGraphViewMultiRunReportOperator::~CRepGraphViewMultiRunReportOperator(void)
{
}
void CRepGraphViewMultiRunReportOperator::SetSummaryQueueTimeContents(const CString& strPaxType)
{
	CTermPlanDoc* pDoc = GetTermPlanDoc();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Summary::SummaryQTimeList& dataList = multiReport.GetReportResult_SummaryQueueTime();
	if (dataList.empty())
		return;

	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	vSimResultFullName.push_back(_T("Average"));
	size_t nResultSize = dataList.size();

	m_MSChartCtrl.ShowWindow(SW_SHOW);

	//column
	m_MSChartCtrl.SetRowCount(nResultSize);
	m_MSChartCtrl.SetColumnCount(1);
	m_MSChartCtrl.SetColumnLabelCount(1);

	MultiRunSummarySubReportType summmarySubType= GetSummarySubReportType();

	//for (size_t nCol = 0; nCol < nResultSize; ++nCol )
	//{
	//	m_MSChartCtrl.SetColumn(nCol+1);
	//	m_MSChartCtrl.SetColumnLabel(vSimResultFullName[nCol]);
	//}
		m_MSChartCtrl.SetColumn(1);
		m_MSChartCtrl.SetColumnLabel(MultiRunSummarySubReportTypeName[summmarySubType]);

	//CString strLabel;
	////m_MSChartCtrl.SetRowCount(nResultSize);
	////m_MSChartCtrl.SetColumnCount(13);
	////m_MSChartCtrl.SetColumnLabelCount(13);
	//m_MSChartCtrl.SetRowCount(13);
	//m_MSChartCtrl.SetRow(1);
	//m_MSChartCtrl.SetRowLabel("Minimum");
	//m_MSChartCtrl.SetRow(2);
	//m_MSChartCtrl.SetRowLabel("Average");
	//m_MSChartCtrl.SetRow(3);
	//m_MSChartCtrl.SetRowLabel("Maximum");

	//m_MSChartCtrl.SetRow(4);
	//m_MSChartCtrl.SetRowLabel("Q1");

	//m_MSChartCtrl.SetRow(5);
	//m_MSChartCtrl.SetRowLabel("Q2");

	//m_MSChartCtrl.SetRow(6);
	//m_MSChartCtrl.SetRowLabel("Q3");

	//m_MSChartCtrl.SetRow(7);
	//m_MSChartCtrl.SetRowLabel("P1");

	//m_MSChartCtrl.SetRow(8);
	//m_MSChartCtrl.SetRowLabel("P5");

	//m_MSChartCtrl.SetRow(9);
	//m_MSChartCtrl.SetRowLabel("P10");

	//m_MSChartCtrl.SetRow(10);
	//m_MSChartCtrl.SetRowLabel("P90");

	//m_MSChartCtrl.SetRow(11);
	//m_MSChartCtrl.SetRowLabel("P95");

	//m_MSChartCtrl.SetRow(12);
	//m_MSChartCtrl.SetRowLabel("P99");

	//m_MSChartCtrl.SetRow(13);
	//m_MSChartCtrl.SetRowLabel("Sigma");
	
	size_t nResultNameSize = vSimResultFullName.size();

	for (int nResult = 0; nResult < (int)nResultNameSize; ++ nResult)
	{
		m_MSChartCtrl.SetRow(nResult+1);
		m_MSChartCtrl.SetRowLabel(vSimResultFullName[nResult]);	
	}



	CString strxValue[100];
	float minvalue[100];
	float avevalue[100];
	float maxvalue[100];

	float Q1value[100];
	float Q2value[100];
	float Q3value[100];

	float P1value[100];
	float P5value[100];
	float P10value[100];

	float P90value[100];
	float P95value[100];
	float P99value[100];

	float Sigmavalue[100];


	for(int k=0;k<100;k++)
	{
		minvalue[k]=avevalue[k]=maxvalue[k]=Q1value[k]=0;
		Q2value[k]=Q3value[k]=P1value[k]=P5value[k]=0;
		Sigmavalue[k] = P10value[k]=P95value[k]=P99value[k]=0;
	}

	size_t i = 0;

	for (i = 0;i < nResultSize; ++i)
	{
		size_t nPaxTypeCount = dataList[i].size();
		for (size_t j =0; j < nPaxTypeCount; ++j)
		{
			if(dataList[i][j].strPaxType.CompareNoCase(strPaxType) ==0)
				break;
		}

		if (j == nPaxTypeCount)
			break;

		avevalue[i]= static_cast<float>(dataList[i][j].eAverage.asMinutes());
		maxvalue[i]= static_cast<float>(dataList[i][j].eMaximum.asMinutes());
		minvalue[i]= static_cast<float>(dataList[i][j].eMinimum.asMinutes());
		P1value[i]= static_cast<float>(dataList[i][j].eP1.asMinutes());
		P5value[i]= static_cast<float>(dataList[i][j].eP5.asMinutes());
		P10value[i]= static_cast<float>(dataList[i][j].eP10.asMinutes());
		P90value[i]= static_cast<float>(dataList[i][j].eP90.asMinutes());
		P95value[i]= static_cast<float>(dataList[i][j].eP95.asMinutes());
		P99value[i]= static_cast<float>(dataList[i][j].eP99.asMinutes());
		Q1value[i]= static_cast<float>(dataList[i][j].eQ1.asMinutes());
		Q2value[i]= static_cast<float>(dataList[i][j].eQ2.asMinutes());
		Q3value[i]= static_cast<float>(dataList[i][j].eQ3.asMinutes());
		//= static_cast<float>(dataList[i].nCount);
		Sigmavalue[i]= static_cast<float>(dataList[i][j].eSigma.asMinutes());		
	}

	i = 0;
	for (i = 1; i <= nResultSize; ++i)
	{
		CString strLabel;
		m_MSChartCtrl.SetRow(i);

		m_MSChartCtrl.SetColumn(1);
		if(summmarySubType == SubType_Minimum)
			strLabel.Format("%f",minvalue[i-1]);
		else if(summmarySubType == SubType_Average)
			strLabel.Format("%f",avevalue[i-1]);
		else if(summmarySubType == SubType_Maximum)
			strLabel.Format("%f",maxvalue[i-1]);
		else if(summmarySubType == SubType_Q1)
			strLabel.Format("%f",Q1value[i-1]);
		else if(summmarySubType == SubType_Q2)
			strLabel.Format("%f",Q2value[i-1]);
		else if(summmarySubType == SubType_Q3)
			strLabel.Format("%f",Q3value[i-1]);
		else if(summmarySubType == SubType_P1)
			strLabel.Format("%f",P1value[i-1]);
		else if(summmarySubType == SubType_P5)
			strLabel.Format("%f",P5value[i-1]);
		else if(summmarySubType == SubType_P10)
			strLabel.Format("%f",P10value[i-1]);
		else if(summmarySubType == SubType_P90)
			strLabel.Format("%f",P90value[i-1]);
		else if(summmarySubType == SubType_P95)
			strLabel.Format("%f",P95value[i-1]);
		else if(summmarySubType == SubType_P99)
			strLabel.Format("%f",P99value[i-1]);
		else if(summmarySubType == SubType_Sigma)
			strLabel.Format("%f",Sigmavalue[i-1]);
		else
		{
			ASSERT(NULL);
			continue;
		}
		m_MSChartCtrl.SetData(strLabel);

		//m_MSChartCtrl.SetRow(2);
		//m_MSChartCtrl.SetColumn(i);
		//strLabel.Format("%f",avevalue[i-1]);
		//m_MSChartCtrl.SetData(strLabel);

		//m_MSChartCtrl.SetRow(3);	
		//m_MSChartCtrl.SetColumn(i);
		//strLabel.Format("%f",maxvalue[i-1]);
		//m_MSChartCtrl.SetData(strLabel);

		//m_MSChartCtrl.SetRow(4);	
		//m_MSChartCtrl.SetColumn(i);
		//strLabel.Format("%f",Q1value[i-1]);
		//m_MSChartCtrl.SetData(strLabel);

		//m_MSChartCtrl.SetRow(5);	
		//m_MSChartCtrl.SetColumn(i);
		//strLabel.Format("%f", Q2value[i-1] );
		//m_MSChartCtrl.SetData(strLabel);

		//m_MSChartCtrl.SetRow(6);	
		//m_MSChartCtrl.SetColumn(i);
		//strLabel.Format("%f",Q3value[i-1]);
		//m_MSChartCtrl.SetData(strLabel);

		//m_MSChartCtrl.SetRow(7);
		//m_MSChartCtrl.SetColumn(i);
		//strLabel.Format("%f",P1value[i-1]);
		//m_MSChartCtrl.SetData(strLabel);

		//m_MSChartCtrl.SetRow(8);
		//m_MSChartCtrl.SetColumn(i);
		//strLabel.Format("%f",P5value[i-1]);
		//m_MSChartCtrl.SetData(strLabel);

		//m_MSChartCtrl.SetRow(9);
		//m_MSChartCtrl.SetColumn(i);
		//strLabel.Format("%f",P10value[i-1]);
		//m_MSChartCtrl.SetData(strLabel);

		//m_MSChartCtrl.SetRow(10);
		//m_MSChartCtrl.SetColumn(i);
		//strLabel.Format("%f",P90value[i-1]);
		//m_MSChartCtrl.SetData(strLabel);

		//m_MSChartCtrl.SetRow(11);
		//m_MSChartCtrl.SetColumn(i);
		//strLabel.Format("%f",P95value[i-1]);
		//m_MSChartCtrl.SetData(strLabel);

		//m_MSChartCtrl.SetRow(12);
		//m_MSChartCtrl.SetColumn(i);
		//strLabel.Format("%f",P99value[i-1]);
		//m_MSChartCtrl.SetData(strLabel);

		//m_MSChartCtrl.SetRow(13);
		//m_MSChartCtrl.SetColumn(i);
		//strLabel.Format("%f",Sigmavalue[i-1]);
		//m_MSChartCtrl.SetData(strLabel);
	}
	m_MSChartCtrl.SetShowLegend(TRUE);
}

void CRepGraphViewMultiRunReportOperator::SetSummaryTimeServiceContents(const CString& strPaxType)
{
	CTermPlanDoc* pDoc = GetTermPlanDoc();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Summary::SummaryTimeServiceList& dataList = multiReport.GetReportResult_SummaryTimeService();
	if (dataList.empty())
		return;

	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	vSimResultFullName.push_back(_T("Average"));
	size_t nResultSize = dataList.size();

	m_MSChartCtrl.ShowWindow(SW_SHOW);

	//column
	m_MSChartCtrl.SetRowCount(nResultSize);
	m_MSChartCtrl.SetColumnCount(1);
	m_MSChartCtrl.SetColumnLabelCount(1);

	MultiRunSummarySubReportType summmarySubType= GetSummarySubReportType();
	m_MSChartCtrl.SetColumn(1);
	m_MSChartCtrl.SetColumnLabel(MultiRunSummarySubReportTypeName[summmarySubType]);


	size_t nResultNameSize = vSimResultFullName.size();

	for (int nResult = 0; nResult < (int)nResultNameSize; ++ nResult)
	{
		m_MSChartCtrl.SetRow(nResult+1);
		m_MSChartCtrl.SetRowLabel(vSimResultFullName[nResult]);	
	}

	CString strxValue[100];
	float minvalue[100];
	float avevalue[100];
	float maxvalue[100];

	float Q1value[100];
	float Q2value[100];
	float Q3value[100];

	float P1value[100];
	float P5value[100];
	float P10value[100];

	float P90value[100];
	float P95value[100];
	float P99value[100];

	float Sigmavalue[100];

	for(int k=0;k<100;k++)
	{
		minvalue[k]=avevalue[k]=maxvalue[k]=Q1value[k]=0;
		Q2value[k]=Q3value[k]=P1value[k]=P5value[k]=0;
		Sigmavalue[k] = P10value[k]=P95value[k]=P99value[k]=0;
	}

	size_t i = 0;

	for (i = 0;i < nResultSize; ++i)
	{
		size_t nPaxTypeCount = dataList[i].size();
		for (size_t j =0; j < nPaxTypeCount; ++j)
		{
			if(dataList[i][j].strPaxType.CompareNoCase(strPaxType) ==0)
				break;
		}

		if (j == nPaxTypeCount)
			break;

		avevalue[i]= static_cast<float>(dataList[i][j].eAverage.asMinutes());
		maxvalue[i]= static_cast<float>(dataList[i][j].eMaximum.asMinutes());
		minvalue[i]= static_cast<float>(dataList[i][j].eMinimum.asMinutes());
		P1value[i]= static_cast<float>(dataList[i][j].eP1.asMinutes());
		P5value[i]= static_cast<float>(dataList[i][j].eP5.asMinutes());
		P10value[i]= static_cast<float>(dataList[i][j].eP10.asMinutes());
		P90value[i]= static_cast<float>(dataList[i][j].eP90.asMinutes());
		P95value[i]= static_cast<float>(dataList[i][j].eP95.asMinutes());
		P99value[i]= static_cast<float>(dataList[i][j].eP99.asMinutes());
		Q1value[i]= static_cast<float>(dataList[i][j].eQ1.asMinutes());
		Q2value[i]= static_cast<float>(dataList[i][j].eQ2.asMinutes());
		Q3value[i]= static_cast<float>(dataList[i][j].eQ3.asMinutes());
		//= static_cast<float>(dataList[i].nCount);
		Sigmavalue[i]= static_cast<float>(dataList[i][j].eSigma.asMinutes());		

	}

	i = 0;
	for (i = 1; i <= nResultSize; ++i)
	{

		CString strLabel;
		m_MSChartCtrl.SetRow(i);

		m_MSChartCtrl.SetColumn(1);
		if(summmarySubType == SubType_Minimum)
			strLabel.Format("%f",minvalue[i-1]);
		else if(summmarySubType == SubType_Average)
			strLabel.Format("%f",avevalue[i-1]);
		else if(summmarySubType == SubType_Maximum)
			strLabel.Format("%f",maxvalue[i-1]);
		else if(summmarySubType == SubType_Q1)
			strLabel.Format("%f",Q1value[i-1]);
		else if(summmarySubType == SubType_Q2)
			strLabel.Format("%f",Q2value[i-1]);
		else if(summmarySubType == SubType_Q3)
			strLabel.Format("%f",Q3value[i-1]);
		else if(summmarySubType == SubType_P1)
			strLabel.Format("%f",P1value[i-1]);
		else if(summmarySubType == SubType_P5)
			strLabel.Format("%f",P5value[i-1]);
		else if(summmarySubType == SubType_P10)
			strLabel.Format("%f",P10value[i-1]);
		else if(summmarySubType == SubType_P90)
			strLabel.Format("%f",P90value[i-1]);
		else if(summmarySubType == SubType_P95)
			strLabel.Format("%f",P95value[i-1]);
		else if(summmarySubType == SubType_P99)
			strLabel.Format("%f",P99value[i-1]);
		else if(summmarySubType == SubType_Sigma)
			strLabel.Format("%f",Sigmavalue[i-1]);
		else
		{
			ASSERT(NULL);
			continue;
		}
		m_MSChartCtrl.SetData(strLabel);
	}
	m_MSChartCtrl.SetShowLegend(TRUE);
}
void CRepGraphViewMultiRunReportOperator::SetSummaryCriticalQueueLenContents()
{
	MultiRunSummarySubReportType summmarySubType= GetSummarySubReportType();

	CString strTitle;
	strTitle.Format(_T("Queue Length(Summary)(%s)"),MultiRunSummarySubReportTypeName[summmarySubType]);

	SetHeadXYTitle(strTitle,"Processor","Average Queue Length");

	CTermPlanDoc* pDoc = GetTermPlanDoc();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Summary::SummaryCriticalQueueList& dataList = multiReport.GetReportResult_SummaryCriticalQueue();
	if (dataList.empty())
		return;

	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	vSimResultFullName.push_back(_T("Average"));
	size_t nResultSize = dataList.size();
	size_t nProcCount = dataList[0].size();


	m_MSChartCtrl.ShowWindow(SW_SHOW);
	m_MSChartCtrl.SetColumnCount(nResultSize);

	m_MSChartCtrl.SetRowCount(nProcCount);
	m_MSChartCtrl.SetRowLabelCount(nProcCount);

	for(size_t nCol =0; nCol < nResultSize; ++nCol)
	{
		m_MSChartCtrl.SetColumn(nCol+1);
		m_MSChartCtrl.SetColumnLabel(vSimResultFullName[nCol]);
	}

	//set data


	for (size_t j =1; j <= nProcCount; ++j)
	{	
		m_MSChartCtrl.SetRow(j);
		m_MSChartCtrl.SetRowLabel(dataList[0][j-1].strProcName);
		for (size_t i =1; i <= nResultSize; ++ i)
		{
			CString strLabel;
			m_MSChartCtrl.SetRow(j);
			m_MSChartCtrl.SetColumn(i);
			//int nMaxQ = dataList[i-1][j-1].nMaxQueue;
			strLabel.Format("%f",dataList[i-1][j-1].fAvgQueue);
			m_MSChartCtrl.SetData(strLabel);

		}

		//strLabel.Format("%s",strProcessor[i]);
		//DealRowLabel(strLabel);
		//m_MSChartCtrl.SetRowLabel(strLabel);


		//m_MSChartCtrl.SetColumn(i);
		//m_MSChartCtrl.SetRow(1);
		//strLabel.Format("%f",dataList[i-1].fAvgQueue);
		//m_MSChartCtrl.SetData(strLabel);
		//m_MSChartCtrl.SetRow(2);


	}

	m_MSChartCtrl.SetShowLegend(TRUE);

}
void CRepGraphViewMultiRunReportOperator::SetSummaryCriticalQueueMaxQueueContents()
{
	SetHeadXYTitle("Time of Max Queue(Summary)","Processor","Time of Max Queue(count)");

	CTermPlanDoc* pDoc = GetTermPlanDoc();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Summary::SummaryCriticalQueueList& dataList = multiReport.GetReportResult_SummaryCriticalQueue();
	if (dataList.empty())
		return;

	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	vSimResultFullName.push_back(_T("Average"));
	size_t nResultSize = dataList.size();

	size_t nProcCount = dataList[0].size();


	m_MSChartCtrl.ShowWindow(SW_SHOW);
	m_MSChartCtrl.SetColumnCount(nResultSize);

	m_MSChartCtrl.SetRowCount(nProcCount);
	m_MSChartCtrl.SetRowLabelCount(nProcCount);

	for(size_t nCol =0; nCol < nResultSize; ++nCol)
	{
		m_MSChartCtrl.SetColumn(nCol+1);
		m_MSChartCtrl.SetColumnLabel(vSimResultFullName[nCol]);
	}

	//set data


	for (size_t j =1; j <= nProcCount; ++j)
	{	
		m_MSChartCtrl.SetRow(j);
		m_MSChartCtrl.SetRowLabel(dataList[0][j-1].strProcName);
		for (size_t i =1; i <= nResultSize; ++ i)
		{
			CString strLabel;
			m_MSChartCtrl.SetRow(j);
			m_MSChartCtrl.SetColumn(i);
			//int nMaxQ = dataList[i-1][j-1].nMaxQueue;
			strLabel.Format("%d",dataList[i-1][j-1].nMaxQueue);
			m_MSChartCtrl.SetData(strLabel);

		}
	}
	m_MSChartCtrl.SetShowLegend(TRUE);


}
void CRepGraphViewMultiRunReportOperator::SetSummaryCriticalQueueMaxQueueTimeContents()
{	
	SetHeadXYTitle("Time of Max Queue(Summary)","Processor","Time of Max Queue(Minutes)");

	CTermPlanDoc* pDoc = GetTermPlanDoc();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Summary::SummaryCriticalQueueList& dataList = multiReport.GetReportResult_SummaryCriticalQueue();
	if (dataList.empty())
		return;

	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	vSimResultFullName.push_back(_T("Average"));
	size_t nResultSize = dataList.size();

	size_t nProcCount = dataList[0].size();


	m_MSChartCtrl.ShowWindow(SW_SHOW);
	m_MSChartCtrl.SetColumnCount(nResultSize);

	m_MSChartCtrl.SetRowCount(nProcCount);
	m_MSChartCtrl.SetRowLabelCount(nProcCount);

	for(size_t nCol =0; nCol < nResultSize; ++nCol)
	{
		m_MSChartCtrl.SetColumn(nCol+1);
		m_MSChartCtrl.SetColumnLabel(vSimResultFullName[nCol]);
	}

	//set data


	for (size_t j =1; j <= nProcCount; ++j)
	{	
		m_MSChartCtrl.SetRow(j);
		m_MSChartCtrl.SetRowLabel(dataList[0][j-1].strProcName);
		for (size_t i =1; i <= nResultSize; ++ i)
		{
			CString strLabel;
			m_MSChartCtrl.SetRow(j);
			m_MSChartCtrl.SetColumn(i);
			//int nMaxQ = dataList[i-1][j-1].nMaxQueue;
			strLabel.Format("%f",static_cast<float>(dataList[i-1][j-1].eMaxQueueTime.asMinutes()));
			m_MSChartCtrl.SetData(strLabel);

		}
	}
	m_MSChartCtrl.SetShowLegend(TRUE);

}
void CRepGraphViewMultiRunReportOperator::SetHeadXYTitle(CString strTitle, CString strXAxisTitle, CString strYAxisTitle)
{
	CVcPlot plot(m_MSChartCtrl.GetPlot());
	_variant_t  Index((long) 0);

	//Spruce up title
	CVcTitle title(m_MSChartCtrl.GetTitle());
	title.SetText(strTitle);

	// Label X Axis
	CVcAxis xaxis(plot.GetAxis(0, Index ));
	CVcAxisTitle xaxistitle(xaxis.GetAxisTitle());
	xaxistitle.SetVisible(true);
	xaxistitle.SetText(strXAxisTitle);
	CVcFont xfont(xaxistitle.GetVtFont());

	// Label Y Axis
	CVcAxis yaxis(plot.GetAxis(1, Index ));
	CVcAxisTitle yaxistitle(yaxis.GetAxisTitle());
	yaxistitle.SetVisible(true);
	yaxistitle.SetText(strYAxisTitle);
}
int CRepGraphViewMultiRunReportOperator::GetValueRangeFromDataList(const MultiRunsReport::Detail::PaxDataList& _datalist)
{
	int MaxValue = 0 ;
	for (MultiRunsReport::Detail::PaxDataList::const_iterator iter = _datalist.begin(); iter != _datalist.end(); ++iter)
	{
		//			m_MSChartCtrl.SetColumnLabel(strSimName);
		MaxValue = max(MaxValue,(int)iter->size()) ;
	}
	return MaxValue ;
}
void CRepGraphViewMultiRunReportOperator::SetDetailedPaxDataContents(MultiRunsReport::Detail::PaxDataList& , std::vector<int>& vSimResults)
{
	CTermPlanDoc* pDoc = GetTermPlanDoc();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Detail::PaxDataList& dataList = multiReport.GetReportResult_PaxDataList();
	if (dataList.empty())
		return;

	m_MSChartCtrl.ShowWindow(SW_SHOW);

	CString strLabel;
	int nRowCount = GetValueRangeFromDataList(dataList);
	m_MSChartCtrl.SetRowCount(nRowCount);
	m_MSChartCtrl.SetColumnCount(multiReport.GetAllSimResultCount());
	m_MSChartCtrl.SetColumnLabelCount(multiReport.GetAllSimResultCount());

	std::vector<CString> vSimResultName = multiReport.GetAllSimResultFullName();
	int nSimNameCount = vSimResultName.size();
	for( int i = 1; i<=nSimNameCount; i++ )
	{
		m_MSChartCtrl.SetColumn( i );			// ea
		m_MSChartCtrl.SetColumnLabel( vSimResultName[i-1]);
	}

	for (int i =1;i <= nRowCount; ++i)
	{	

		int nCol = 1;	
		CString strRange;
		strRange.Format(_T("%d - %d"), dataList[0].at(i-1).ValueRange.first, dataList[0].at(i-1).ValueRange.second);

		m_MSChartCtrl.SetRow(i);
		m_MSChartCtrl.SetRowLabel(strRange);

		for (MultiRunsReport::Detail::PaxDataList::const_iterator iter = dataList.begin(); iter != dataList.end(); ++iter)
		{

			//			m_MSChartCtrl.SetColumnLabel(strSimName);

			m_MSChartCtrl.SetRow( i );					
			m_MSChartCtrl.SetColumn( nCol );			// ea
			CString strValue;
			if(i <= (int)iter->size())
				strValue.Format(_T("%d"), iter->at(i-1).nCount);
			else
				strValue = "0" ;
			m_MSChartCtrl.SetData(strValue);
			nCol++;
		}

	}

}

void CRepGraphViewMultiRunReportOperator::SetDetailedTimeDataContents(MultiRunsReport::Detail::TimeDataList&, std::vector<int>& vSimResults )
{
	CTermPlanDoc* pDoc = GetTermPlanDoc();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Detail::TimeDataList& dataList = multiReport.GetReportResult_TimeDataList();
	if (dataList.empty())
		return;

	m_MSChartCtrl.ShowWindow(SW_SHOW);


	int nSimResultCount = dataList.size();
	MultiRunsReport::Detail::TimeDataList::const_iterator it = dataList.begin();
	CString strLabel;
	
	int MaxValue = 0 ;
	for (MultiRunsReport::Detail::TimeDataList::const_iterator iter = dataList.begin(); iter != dataList.end(); ++iter)
	{
		//			m_MSChartCtrl.SetColumnLabel(strSimName);
		MaxValue = max(MaxValue,(int)iter->size()) ;
	}

	int nRangeCount =  MaxValue;
	m_MSChartCtrl.SetRowCount( nRangeCount );	// time range
	m_MSChartCtrl.SetColumnCount( nSimResultCount );	// within each time range
	m_MSChartCtrl.SetColumnLabelCount( nSimResultCount );

	std::vector<CString> vSimResultName = multiReport.GetAllSimResultFullName();
	int nSimNameCount = vSimResultName.size();
	for( int i = 1; i<=nSimNameCount; i++ )
	{
		m_MSChartCtrl.SetColumn( i );			// ea
		m_MSChartCtrl.SetColumnLabel( vSimResultName[i-1]);
	}

	for ( int i = 1; i <= nRangeCount; ++i)
	{
		// for each time range
		int nCol = 1;
		bool bInSecond = it->at( i-1 ).bInSecond;
		CString csRowLabel = it->at( i-1 ).TimeValueRange.first.printTime( bInSecond ? 1 : 0 ) + " - " + it->at( i-1 ).TimeValueRange.second.printTime( bInSecond ? 1 : 0 );
		m_MSChartCtrl.SetRow( i );					
		m_MSChartCtrl.SetRowLabel( csRowLabel );
		for(MultiRunsReport::Detail::TimeDataList::const_iterator iter = dataList.begin(); iter != dataList.end(); ++iter)
		{
			// for each bar
			m_MSChartCtrl.SetRow( i );					
			m_MSChartCtrl.SetColumn( nCol );			// ea
			CString strValue;
			if(i <= (int)iter->size())
				strValue.Format(_T("%d"), iter->at(i-1).nCount);
			else
				strValue = "0" ;
			m_MSChartCtrl.SetData(strValue);
			nCol++;
		}

	}
}

void CRepGraphViewMultiRunReportOperator::SetSummaryPaxDataContents(MultiRunsReport::Summary::PaxDataList& dataList, std::vector<int>& vSimResults)
{
	m_MSChartCtrl.ShowWindow(SW_SHOW);

	CString strLabel;
	m_MSChartCtrl.SetRowCount(dataList.size());
	m_MSChartCtrl.SetColumnCount(vSimResults.size());
	m_MSChartCtrl.SetColumnLabelCount(vSimResults.size());

	int nRow = 1;
	for (MultiRunsReport::Summary::PaxDataList::iterator iter = dataList.begin(); iter != dataList.end(); ++iter)
	{
		m_MSChartCtrl.SetRow(nRow);
		m_MSChartCtrl.SetRowLabel(iter->first);

		for (size_t i = 0; i < vSimResults.size(); ++i)
		{
			CString strValue;
			strValue.Format(_T("%.2f"), iter->second[i].fAveValue);

			CString strSimName;
			strSimName.Format(_T("SimResult%d"), vSimResults[i]);

			m_MSChartCtrl.SetColumn(i + 1);
			m_MSChartCtrl.SetColumnLabel(strSimName);
			m_MSChartCtrl.SetData(strValue);
		}

		nRow++;
	}
}
void CRepGraphViewMultiRunReportOperator::SetDetailOccupancyTimeWithNoRanger(MultiRunsReport::Detail::OccupnacyDataList& , std::vector<int>& vSimResults)
{
	CTermPlanDoc* pDoc = GetTermPlanDoc();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Detail::OccupnacyDataList& dataList = multiReport.GetReportResult_OccupnacyDataList();
	if (dataList.empty())
		return;

	m_MSChartCtrl.ShowWindow(SW_SHOW);


	int nSimResultCount = dataList.size();
	MultiRunsReport::Detail::OccupnacyDataList::const_iterator it = dataList.begin();
	CString strLabel;

	int MaxValue = 0 ;
	for (MultiRunsReport::Detail::OccupnacyDataList::const_iterator iter = dataList.begin(); iter != dataList.end(); ++iter)
	{
		//			m_MSChartCtrl.SetColumnLabel(strSimName);
		MaxValue = max(MaxValue,(int)iter->size()) ;
	}

	int nRangeCount = MaxValue;
	m_MSChartCtrl.SetRowCount( nRangeCount );	// time range
	m_MSChartCtrl.SetColumnCount( nSimResultCount );	// within each time range
	m_MSChartCtrl.SetColumnLabelCount( nSimResultCount );

	std::vector<CString> vSimResultName = multiReport.GetAllSimResultFullName();
	int nSimNameCount = vSimResultName.size();
	for( int i = 1; i<=nSimNameCount; i++ )
	{
		m_MSChartCtrl.SetColumn( i );			// ea
		m_MSChartCtrl.SetColumnLabel( vSimResultName[i-1]);
	}

	for ( int i = 1; i <= nRangeCount; ++i)
	{
		// for each time range
		int nCol = 1;
		bool bInSecond = false;//it->at( i-1 ).bInSecond;
		CString csRowLabel = it->at( i-1 ).startTime.printTime( bInSecond ? 1 : 0 ) ;
		m_MSChartCtrl.SetRow( i );					
		m_MSChartCtrl.SetRowLabel( csRowLabel );
		for(MultiRunsReport::Detail::OccupnacyDataList::const_iterator iter = dataList.begin(); iter != dataList.end(); ++iter)
		{
			// for each bar
			m_MSChartCtrl.SetRow( i );					
			m_MSChartCtrl.SetColumn( nCol );			// ea
			CString strValue;
			if(i <= (int)iter->size())
				strValue.Format(_T("%d"), iter->at(i-1).nPaxCount);
			else
				strValue = "0" ;
			m_MSChartCtrl.SetData(strValue);
			nCol++;
		}

	}
}
void CRepGraphViewMultiRunReportOperator::SetDetailOccupancyTimeDataContents(MultiRunsReport::Detail::OccupnacyDataList& , std::vector<int>& vSimResults)
{
	CTermPlanDoc* pDoc = GetTermPlanDoc();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Detail::OccupnacyDataList& dataList = multiReport.GetReportResult_OccupnacyDataList();
	if (dataList.empty())
		return;

	m_MSChartCtrl.ShowWindow(SW_SHOW);


	int nSimResultCount = dataList.size();
	MultiRunsReport::Detail::OccupnacyDataList::const_iterator it = dataList.begin();
	CString strLabel;

	int MaxValue = 0 ;
	for (MultiRunsReport::Detail::OccupnacyDataList::const_iterator iter = dataList.begin(); iter != dataList.end(); ++iter)
	{
		//			m_MSChartCtrl.SetColumnLabel(strSimName);
		MaxValue = max(MaxValue,(int)iter->size()) ;
	}

	int nRangeCount =MaxValue;
	m_MSChartCtrl.SetRowCount( nRangeCount-1 );	// time range
	m_MSChartCtrl.SetColumnCount( nSimResultCount );	// within each time range
	m_MSChartCtrl.SetColumnLabelCount( nSimResultCount );

	std::vector<CString> vSimResultName = multiReport.GetAllSimResultFullName();
	int nSimNameCount = vSimResultName.size();
	for( int i = 1; i<=nSimNameCount; i++ )
	{
		m_MSChartCtrl.SetColumn( i );			// ea
		m_MSChartCtrl.SetColumnLabel( vSimResultName[i-1]);
	}

	for ( int i = 1; i < nRangeCount; ++i)
	{
		// for each time range
		int nCol = 1;
		bool bInSecond = false;//it->at( i-1 ).bInSecond;
		CString csRowLabel = it->at( i-1 ).startTime.printTime( bInSecond ? 1 : 0 ) + " - " + it->at( i ).startTime.printTime( bInSecond ? 1 : 0 );
		m_MSChartCtrl.SetRow( i );					
		m_MSChartCtrl.SetRowLabel( csRowLabel );
		for(MultiRunsReport::Detail::OccupnacyDataList::const_iterator iter = dataList.begin(); iter != dataList.end(); ++iter)
		{
			// for each bar
			m_MSChartCtrl.SetRow( i );					
			m_MSChartCtrl.SetColumn( nCol );			// ea
			CString strValue;
			if(i <= (int)iter->size())
				strValue.Format(_T("%d"), iter->at(i-1).nPaxCount);
			else
				strValue = "0" ;
			m_MSChartCtrl.SetData(strValue);
			nCol++;
		}

	}
}

void CRepGraphViewMultiRunReportOperator::LoadReport( LPARAM lHint, CObject* pHint )
{
	//if (lHint == MULTIREPORT_SHOWREPORT) //show report
	{
		CTermPlanDoc* pDoc = GetTermPlanDoc();
		CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();
		std::vector<int>& vSimResults = pDoc->GetARCReportManager().GetmSelectedMultiRunsSimResults();

		ENUM_REPORT_TYPE reportType = pDoc->GetARCReportManager().GetReportType();


		CReportParameter* pReportParam = pDoc->GetTerminal().m_pReportParaDB->GetParameterbyType(reportType);
		if (pReportParam == NULL)
			return;

		int iDetailed;
		pReportParam->GetReportType(iDetailed);
		if (iDetailed == 0) //detailed
		{
			switch(reportType)
			{
			case ENUM_DISTANCE_REP:

				{
					SetHeadXYTitle( "Distance Traveled(Detail)", "Distance(m)", "Number of Passenger" );
					MultiRunsReport::Detail::PaxDataList paxDataList;

					SetDetailedPaxDataContents(paxDataList, vSimResults);
					CVcFootnote footnote(m_MSChartCtrl.GetFootnote());
					CString strFootText = "Distance Traveled(Detail);" + GetParameterFootString();

					footnote.SetText(strFootText.MakeUpper());
				}

				break;
			case ENUM_QUEUETIME_REP:
				{
					SetHeadXYTitle( "Passenger Time in Queue(Detail)", "Queue Time(hh:mm:ss)", "Number of Passenger" );
					MultiRunsReport::Detail::TimeDataList timeDataList;


					SetDetailedTimeDataContents(timeDataList, vSimResults);
					CVcFootnote footnote(m_MSChartCtrl.GetFootnote());

					CString strFootText = "Passenger Time in Queue(Detail);" + GetParameterFootString();
					footnote.SetText(strFootText.MakeUpper());
				}
				break;
			case ENUM_SERVICETIME_REP:
				{
					SetHeadXYTitle( "Passenger Time in Service(Detail)", "Service Time(hh:mm:ss)", "Number of Passenger" );
					MultiRunsReport::Detail::TimeDataList timeDataList;


					SetDetailedTimeDataContents(timeDataList, vSimResults);
					CVcFootnote footnote(m_MSChartCtrl.GetFootnote());

					CString strFootText = "Passenger Time in Service(Detail);" + GetParameterFootString();
					footnote.SetText(strFootText.MakeUpper());
				}
				break;
			case ENUM_DURATION_REP:
				{
					SetHeadXYTitle( "Passenger Time in Terminal(Detail)", "Duration(hh:mm)", "Number of Passenger" );
					MultiRunsReport::Detail::TimeDataList timeDataList;

					SetDetailedTimeDataContents(timeDataList, vSimResults);	
						
					CVcFootnote footnote(m_MSChartCtrl.GetFootnote());

					CString strFootText = "Passenger Time in Terminal(Detail);" + GetParameterFootString();

					footnote.SetText(strFootText.MakeUpper());
				}

				break;
			case ENUM_ACTIVEBKDOWN_REP:
				break;
			case ENUM_PAXCOUNT_REP:
				{
					SetHeadXYTitle( "Passenger Count in Terminal (Detail)", "Time(hh:mm)", "Number of Passenger" );
					MultiRunsReport::Detail::OccupnacyDataList timeDataList;

					SetDetailOccupancyTimeDataContents(timeDataList, vSimResults);
					CVcFootnote footnote(m_MSChartCtrl.GetFootnote());

					CString strFootText = "Passenger Count in Terminal (Detail);" + GetParameterFootString();
					footnote.SetText(strFootText.MakeUpper());
				}
				break;
			case ENUM_PAXDENS_REP:
				{
					SetHeadXYTitle("Space Density (Detail)","Time(hh:mm)","Passengers in Area");
					MultiRunsReport::Detail::OccupnacyDataList timeDataList;


					SetDetailOccupancyTimeWithNoRanger(timeDataList, vSimResults);

					CVcFootnote footnote(m_MSChartCtrl.GetFootnote());
					CString strFootText = "Space Density (Detail);" + GetParameterFootString();

					footnote.SetText(strFootText.MakeUpper());

				}
				break;
			default:
				break;
			}
		}
		else // summary
		{

			MultiRunSummarySubReportType subReportType = GetSummarySubReportType();
			
			CString strSubReportType = MultiRunSummarySubReportTypeName[subReportType];
			
			switch(reportType)
			{
			case ENUM_QUEUETIME_REP:
				{
					CString strTitle ;
					strTitle.Format(_T("Queue Time (Summary)(%s)"),strSubReportType);
					SetHeadXYTitle(strTitle,"Passenger Type","Minutes in Queues");
					CString strPaxType;
					if (pHint == NULL)
					{

						CTermPlanDoc* pDoc = GetTermPlanDoc();
						CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

						const MultiRunsReport::Summary::SummaryQTimeList& dataList = multiReport.GetReportResult_SummaryQueueTime();
						if (dataList.empty())
							return;

						size_t nPaxType = dataList[0].size();
						if (nPaxType == 0)
							return;

						m_comboChartSelect.ResetContent();
						for (size_t i =0; i < nPaxType; ++i)
						{
							m_comboChartSelect.AddString(dataList[0][i].strPaxType);
						}

						m_comboChartSelect.SetCurSel(0);
						strPaxType = dataList[0][0].strPaxType;
					}
					else
					{
						strPaxType = *(CString *)pHint;

					}
					if (strPaxType.IsEmpty())
						return;

					CVcFootnote footnote(m_MSChartCtrl.GetFootnote());
					CString strFootText;
					strFootText.Format(_T("%s;%s"),strTitle,GetParameterFootString(strPaxType));

					footnote.SetText(strFootText.MakeUpper());
					SetSummaryQueueTimeContents(strPaxType);
				}
				break;
			case ENUM_AVGQUEUELENGTH_REP:
				{
					if (pHint == NULL)
					{
						m_comboChartSelect.ResetContent();

						m_comboChartSelect.AddString("Queue Length(Summary)");
						m_comboChartSelect.AddString("Time of Max Queue(Summary)");
						m_comboChartSelect.AddString("Max Queue Time Histogram(Summary)");
						m_comboChartSelect.SetCurSel(0);
					}

					int nCursel = m_comboChartSelect.GetCurSel();
					if (nCursel == 0)
					{
						SetSummaryCriticalQueueLenContents();
						CVcFootnote footnote(m_MSChartCtrl.GetFootnote());
						
						CString strFootText = "Queue Length(Summary);"+ GetParameterFootString();

						footnote.SetText(strFootText.MakeUpper());
					}
					else if (nCursel == 1)
					{
						SetSummaryCriticalQueueMaxQueueContents();
						CVcFootnote footnote(m_MSChartCtrl.GetFootnote());
						
						CString strFootText = "Time of Max Queue(Summary);"+ GetParameterFootString();
                        footnote.SetText(strFootText.MakeUpper());

					}
					else if (nCursel == 2)
					{
						SetSummaryCriticalQueueMaxQueueTimeContents();
						CVcFootnote footnote(m_MSChartCtrl.GetFootnote());

						CString strFootText = "Max Queue Time Histogram(Summary);" + GetParameterFootString();
						footnote.SetText(strFootText.MakeUpper());
					}

				}
				break;
			case ENUM_SERVICETIME_REP:
				{
					CString strTitle ;
					strTitle.Format(_T("Time in Service(Summary)(%s)"),strSubReportType);

					SetHeadXYTitle(strTitle,"Passenger Type","Minutes in Service");
					CString strPaxType;
					if (pHint == NULL)
					{

						CTermPlanDoc* pDoc = GetTermPlanDoc();
						CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

						const MultiRunsReport::Summary::SummaryTimeServiceList& dataList = multiReport.GetReportResult_SummaryTimeService();
						if (dataList.empty())
							return;

						size_t nPaxType = dataList[0].size();
						if (nPaxType == 0)
							return;

						m_comboChartSelect.ResetContent();
						for (size_t i =0; i < nPaxType; ++i)
						{
							m_comboChartSelect.AddString(dataList[0][i].strPaxType);
						}

						m_comboChartSelect.SetCurSel(0);
						strPaxType = dataList[0][0].strPaxType;
					}
					else
					{
						strPaxType = *(CString *)pHint;

					}
					if (strPaxType.IsEmpty())
						return;

					CVcFootnote footnote(m_MSChartCtrl.GetFootnote());
					CString strFootText;
					strFootText.Format(_T("%s;%s"),strTitle,GetParameterFootString(strPaxType));
					footnote.SetText(strFootText.MakeUpper());
					SetSummaryTimeServiceContents(strPaxType);
				}
				break;
				//case ENUM_DISTANCE_REP:
				//case ENUM_QUEUETIME_REP:
				//case ENUM_DURATION_REP:
				//case ENUM_SERVICETIME_REP:
				//	{
				//		MultiRunsReport::Summary::PaxDataList paxDataList;
				//		size_t nSimResultCount = vSimResults.size();
				//		std::vector<MultiRunsReport::Summary::PaxDataValue> tempDataRow(nSimResultCount);

				//		for (size_t i = 0; i < nSimResultCount; ++i)
				//		{
				//			pReportManager->SetCurrentSimResult(vSimResults[i]);
				//			pReportManager->SetCurrentReportType(reportType);
				//			CString strReportFileName = pReportManager->GetCurrentReportFileName(pDoc->m_ProjInfo.path);
				//			MultiRunsReport::Summary::DistTravelledDataLoader loader(strReportFileName);
				//			loader.LoadData();
				//			MultiRunsReport::Summary::DistTravelledDataLoader::DataList& dataList = loader.GetData();
				//			for (size_t n = 0; n < dataList.size(); ++n)
				//			{
				//				if (paxDataList.find(dataList[n].strPaxType) == paxDataList.end())
				//					paxDataList[dataList[n].strPaxType] = tempDataRow;

				//				paxDataList[dataList[n].strPaxType][i] = dataList[n].value;
				//			}

				//			//SetHeadXYTitle(loader.GetTitle(), loader.GetXAxisTitle(), loader.GetYAxisTitle(), loader.GetFootnote());
				//		}

				//		SetSummaryPaxDataContents(paxDataList, vSimResults);
				//	}

				//	break;

				//case ENUM_ACTIVEBKDOWN_REP:
				//	break;
				//case ENUM_PAXCOUNT_REP:
				//	break;
			default:
				break;
			}
		}


	}

}

void CRepGraphViewMultiRunReportOperator::OnCbnSelChangeReportSubType()
{
	m_MSChartCtrl.SetShowLegend(FALSE);
	int nCurSel = m_comboChartSelect.GetCurSel();
	if(nCurSel<0)
		return;

	CString strSelect;
	m_comboChartSelect.GetLBText(nCurSel,strSelect);
	if (strSelect.IsEmpty())
		return;

	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetTermPlanDoc();

	ENUM_REPORT_TYPE reportType = pDoc->GetARCReportManager().GetReportType();

	if (reportType == ENUM_QUEUETIME_REP)
	{
		LoadReport(0,(CObject *)&strSelect);
	}
	if (reportType == ENUM_AVGQUEUELENGTH_REP)
	{
		int nCurSel = m_comboChartSelect.GetCurSel();
		LoadReport(0,(CObject*)nCurSel);
	}
	else if(reportType == ENUM_SERVICETIME_REP)
	{
		LoadReport(0,(CObject *)&strSelect);
	}


	m_MSChartCtrl.Refresh();
}

MultiRunSummarySubReportType CRepGraphViewMultiRunReportOperator::GetSummarySubReportType()
{
	MultiRunSummarySubReportType subReportType;
	int nCurSubType = m_comboSubType.GetCurSel();
	if(nCurSubType == -1)
	{
		subReportType = SubType_Average;
	}
	else
	{
		subReportType = (MultiRunSummarySubReportType)nCurSubType;
	}

	return subReportType;
}
void CRepGraphViewMultiRunReportOperator::OnSelchangeChartSelectCombo() 
{
	m_MSChartCtrl.SetShowLegend(FALSE);
	int nCurSel = m_comboChartSelect.GetCurSel();
	if(nCurSel<0)
		return;

	CString strSelect;
	m_comboChartSelect.GetLBText(nCurSel,strSelect);
	if (strSelect.IsEmpty())
		return;

	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetTermPlanDoc();


	ENUM_REPORT_TYPE reportType = pDoc->GetARCReportManager().GetReportType();


	//int iDetailed;
	//pReportParam->GetReportType(iDetailed);
	if (reportType == ENUM_QUEUETIME_REP)
	{
		pDoc->UpdateAllViews(NULL,0,(CObject *)&strSelect);
	}
	if (reportType == ENUM_AVGQUEUELENGTH_REP)
	{
		int nCurSel = m_comboChartSelect.GetCurSel();
		pDoc->UpdateAllViews(NULL,0,(CObject*)nCurSel);
	}
	else if(reportType == ENUM_SERVICETIME_REP)
	{
		pDoc->UpdateAllViews(NULL,0,(CObject *)&strSelect);
	}


	m_MSChartCtrl.Refresh();
}

CString CRepGraphViewMultiRunReportOperator::GetParameterFootString(CString strReplacePaxType)
{
	CReportParameter *pReportParam = GetTermPlanDoc()->GetARCReportManager().GetReportPara();
	if(pReportParam == NULL)
		return "";



	CString strFootText;

	CString strStartTime;

	ElapsedTime time;
	// Start time
	if (pReportParam->GetStartTime(time))
	{
		strStartTime = time.printTime();
	}
	CString strEndTime;
	// End time
	if (pReportParam->GetEndTime(time))
	{
		strEndTime = time.printTime();
	}
	strFootText += strStartTime;
	strFootText += _T("-");
	strFootText += strEndTime;

	std::vector <int> vRuns;
	pReportParam->GetReportRuns(vRuns);

	strFootText+=_T(";");
	for (int nRun = 0; nRun < static_cast<int>(vRuns.size()); ++nRun)
	{
		CString strRun;
		strRun.Format(_T("RUN%d"),vRuns.at(nRun));
		strFootText += strRun;
		strFootText +=_T(",");
	}
	strFootText.TrimRight(_T(","));

	CString strPaxType;
	std::vector<CMobileElemConstraint> vPaxType;
	if (pReportParam->GetPaxType(vPaxType))
	{
		if(!strReplacePaxType.IsEmpty())
		{
			strFootText += _T("; ");
			strFootText += strReplacePaxType;
		}
		else if (!vPaxType.empty())
		{
			for (size_t i = 0; i < vPaxType.size(); i++)
			{
				CString strTemp;
				vPaxType[i].screenPrint(strTemp);
				strPaxType += strTemp;
				strPaxType += _T(",");
			}

			strPaxType.TrimRight(_T(","));
			
			strFootText += _T("; ");
			strFootText += strPaxType;
		}

	}

	// Processor group
	CString strProcID;
	std::vector<ProcessorID> vProcGroup;
	if (pReportParam->GetProcessorGroup(vProcGroup))
	{
		if (!vProcGroup.empty())
		{
			for (size_t i = 0; i < vProcGroup.size(); i++)
			{
				strProcID += vProcGroup[i].GetIDString();
				strProcID += _T(",");
			}

			strProcID.TrimRight(_T(","));
		}
		else
			strProcID = _T("All Processor");

		strFootText += _T(";");
		strFootText += strProcID;
	}

	CString strFromToProc ;
	// From to Processors
	CReportParameter::FROM_TO_PROCS fromToProcs;
	if (pReportParam->GetFromToProcs(fromToProcs))
	{
		if (!fromToProcs.m_vFromProcs.empty())
		{

			for (size_t i = 0; i < fromToProcs.m_vFromProcs.size(); i++)
			{

				strFromToProc += fromToProcs.m_vFromProcs[i].GetIDString();
				strFromToProc += _T(",");
			}

		}
		else
		{
			strFromToProc +=  _T("All Processor, ");
		}

		if (!fromToProcs.m_vToProcs.empty())
		{
			for (size_t i = 0; i < fromToProcs.m_vToProcs.size(); i++)
			{

				strFromToProc += fromToProcs.m_vToProcs[i].GetIDString();
				strFromToProc += _T(",");
			}
		}
		else
		{
			strFromToProc +=  _T("All Processor,");
		}

		strFromToProc.Trim(_T(","));

		strFootText += _T(";");
		strFootText += strFromToProc;
	}


	return strFootText;
}



























