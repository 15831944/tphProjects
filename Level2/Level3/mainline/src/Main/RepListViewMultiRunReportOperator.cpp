#include "StdAfx.h"
#include "Resource.h"
#include ".\replistviewmultirunreportoperator.h"
#include "../Reports/ReportParameter.h"
#include "TermPlanDoc.h"
#include "../Reports/ReportParaDB.h"

#include "../MFCExControl/SortableHeaderCtrl.h"
#include "RepListView.h"

CRepListViewMultiRunReportOperator::CRepListViewMultiRunReportOperator( CListCtrl *pListCtrl, CARCReportManager* pARCReportManager,CRepListView *pListView )
:CRepListViewBaseOperator(pListCtrl,pARCReportManager,pListView)
{
	m_nColumnCount = 0;
}
CRepListViewMultiRunReportOperator::~CRepListViewMultiRunReportOperator(void)
{
}

void CRepListViewMultiRunReportOperator::LoadReport(LPARAM lHint, CObject* pHint)
{
	CTermPlanDoc* pDoc = GetTermPlanDoc();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();
	std::vector<int>& vSimResults = pDoc->GetARCReportManager().GetmSelectedMultiRunsSimResults();

	ENUM_REPORT_TYPE reportType = pDoc->GetARCReportManager().GetReportType();


	CReportParameter* pReportParam = pDoc->GetTerminal().m_pReportParaDB->GetParameterbyType(reportType);
	if (pReportParam == NULL)
		return;

	//clear list ctrl
	ResetAllContent();

	int iDetailed;
	pReportParam->GetReportType(iDetailed);
	if (iDetailed == 0) //detailed
	{
		switch(reportType)
		{
		case ENUM_DISTANCE_REP:

			{
				MultiRunsReport::Detail::PaxDataList paxDataList;
				SetDetailedPaxDataContents(paxDataList, vSimResults);
			}

			break;
		case ENUM_QUEUETIME_REP:
			{
				MultiRunsReport::Detail::TimeDataList timeDataList;
				SetDetailedTimeDataContents(timeDataList, 0);
				//m_nReportFileType=QueueTimeReportFile;
				m_sExtension = "qtd";
			}
			break;
		case ENUM_SERVICETIME_REP:
			{
				MultiRunsReport::Detail::TimeDataList timeDataList;
				SetDetailedTimeDataContents(timeDataList, 0);
			}
			break;
		case ENUM_DURATION_REP:
			{
				MultiRunsReport::Detail::TimeDataList timeDataList;
				SetDetailedTimeDataContents(timeDataList, 0);
			}

			break;
		case ENUM_ACTIVEBKDOWN_REP:
			break;
		case ENUM_PAXCOUNT_REP:
			{
				MultiRunsReport::Detail::OccupnacyDataList timeDataList;
				SetDetailedOccupancyDataContents(timeDataList, 0);
			}
			break;
		case ENUM_PAXDENS_REP:
			{
				MultiRunsReport::Detail::OccupnacyDataList timeDataList;
				SetDetailOccupancyDataWithNoRanger(timeDataList,0);
			}
			break;
		default:
			break;
		}
	}
	else//summary 
	{
		switch(reportType)
		{

		case ENUM_QUEUETIME_REP:
			{
				CString strPaxType;
				if (pHint == NULL)
				{

					CTermPlanDoc* pDoc = GetTermPlanDoc();
					CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

					const MultiRunsReport::Summary::SummaryQTimeList& dataList = multiReport.GetReportResult_SummaryQueueTime();
					if (dataList.empty())
						return;
					size_t nCount = dataList[0].size();
					if (nCount ==0 )
						return;

					strPaxType = dataList[0][0].strPaxType;
				}
				else
				{
					strPaxType = *(CString *)pHint;
				}
				SetSummaryQueueTimeContents(strPaxType);
			}
			break;
		case ENUM_AVGQUEUELENGTH_REP:
			{
				SetSummaryCriticalQueueContents();
			}
			break;
		case ENUM_SERVICETIME_REP:
			{

				CString strPaxType;
				if (pHint == NULL)
				{

					CTermPlanDoc* pDoc = GetTermPlanDoc();
					CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

					const MultiRunsReport::Summary::SummaryTimeServiceList& dataList = multiReport.GetReportResult_SummaryTimeService();
					if (dataList.empty())
						return;
					size_t nCount = dataList[0].size();
					if (nCount ==0 )
						return;

					strPaxType = dataList[0][0].strPaxType;
				}
				else
				{
					strPaxType = *(CString *)pHint;
				}
				SetSummaryTimeServiceContents(strPaxType);

			}
			break;

		default:
			break;
		}
	}
}

void CRepListViewMultiRunReportOperator::SetSummaryQueueTimeContents(const CString& strPaxType)
{
	CListCtrl& wndListCtrl = GetListCtrl();
	//wndListCtrl.DeleteAllItems();

	int nCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nCount; i++)
		GetListCtrl().DeleteColumn(0);
	//ResetAllContent();
	GetListCtrl().DeleteAllItems();



	//insert column project name and run
	DWORD headStyle = LVCFMT_CENTER;
	headStyle &= ~HDF_OWNERDRAW;

	GetListCtrl().InsertColumn(0,"",headStyle,30);
	GetListHeaderCtrl()->SetDataType( 0,  dtINT);
	GetListCtrl().InsertColumn( 1, _T("RUN"), headStyle, 60);
	GetListHeaderCtrl()->SetDataType( 1,  dtSTRING);

	const int m_nColumnCount = 15;
	CString csColumnLabel[m_nColumnCount];
	int nDefaultColumnWidth[m_nColumnCount];
	int nFormat[m_nColumnCount];
	EDataType type[m_nColumnCount];

	//csColumnLabel[0] = CString("Passenger Type");
	//csColumnLabel[1] = CString("Minimum");
	//csColumnLabel[2] = CString("Average");
	//csColumnLabel[3] = CString("Maximum");
	//csColumnLabel[4] = CString("Count");

	//CString str = " (s)";
	//csColumnLabel[5] = CString("Q1") + str;
	//csColumnLabel[6] = CString("Q2") + str;
	//csColumnLabel[7] = CString("Q3") + str;
	//csColumnLabel[8] = CString("P1") + str;
	//csColumnLabel[9] = CString("P5") + str;
	//csColumnLabel[10] = CString("P10") + str;
	//csColumnLabel[11] = CString("P90") + str;
	//csColumnLabel[12] = CString("P95") + str;
	//csColumnLabel[13] = CString("P99") + str;

	//csColumnLabel[14] = CString("Sigma") + str;


	csColumnLabel[0] = CString("Min");
	csColumnLabel[1] = CString("Avg");
	csColumnLabel[2] = CString("Max");

	CString str = " (s)";
	csColumnLabel[3] = CString("Q1");
	csColumnLabel[4] = CString("Q2");
	csColumnLabel[5] = CString("Q3");
	csColumnLabel[6] = CString("P1");
	csColumnLabel[7] = CString("P5");
	csColumnLabel[8] = CString("P10");
	csColumnLabel[9] = CString("P90");
	csColumnLabel[10] = CString("P95");
	csColumnLabel[11] = CString("P99");
	csColumnLabel[12] = CString("Sigma");

	csColumnLabel[13] = CString("Passenger Type");
	csColumnLabel[14] = CString("Count");



	nDefaultColumnWidth[0] = 60;
	nDefaultColumnWidth[1] = 60;
	nDefaultColumnWidth[2] = 60;
	nDefaultColumnWidth[3] = 60;
	nDefaultColumnWidth[4] = 60;
	nDefaultColumnWidth[5] = 60;
	nDefaultColumnWidth[6] = 60;
	nDefaultColumnWidth[7] = 60;
	nDefaultColumnWidth[8] = 60;
	nDefaultColumnWidth[9] = 60;
	nDefaultColumnWidth[10] = 60;
	nDefaultColumnWidth[11] = 60;
	nDefaultColumnWidth[12] = 60;
	nDefaultColumnWidth[13] = 120;
	nDefaultColumnWidth[14] = 60;


	for( int i=0; i<15; i++ )
	{
		nFormat[i] = LVCFMT_CENTER; 
	}

	type[0] = dtDATETIME;	
	type[1] = dtDATETIME;
	type[2] = dtDATETIME;
	type[3] = dtDATETIME;
	type[4] = dtDATETIME;
	type[5] = dtDATETIME;
	type[6] = dtDATETIME;
	type[7] = dtDATETIME;
	type[8] = dtDATETIME;
	type[9] = dtDATETIME;
	type[10] = dtDATETIME;
	type[11] = dtDATETIME;
	type[12] = dtDATETIME;
	type[13] = dtSTRING;
	type[14] = dtINT;

	for( int i=0; i<m_nColumnCount; i++ )
	{
		DWORD dwStyle = nFormat[i];
		dwStyle &= ~HDF_OWNERDRAW;

		GetListCtrl().InsertColumn( i+2, csColumnLabel[i], /*nFormat[i]*/dwStyle, nDefaultColumnWidth[i] );
		GetListHeaderCtrl()->SetDataType( i+2, type[i] );
	}

	CTermPlanDoc* pDoc = GetTermPlanDoc();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Summary::SummaryQTimeList& dataList = multiReport.GetReportResult_SummaryQueueTime();
	if (dataList.empty())
		return;
	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	CString strAverageName;
	//remove RUN
	for (int nName =0; nName <static_cast<int>(vSimResultFullName.size()); ++nName )
	{
		CString strResultName = vSimResultFullName[nName];
		CString strValue = _T("");
		strValue.Format(_T("%d"),atoi(strResultName.TrimLeft(_T("RUN")))+1);
		vSimResultFullName[nName] = strValue;
	}

	vSimResultFullName.push_back(_T("Average"));
	size_t nResultSize = dataList.size();

	//insert average
	int nItemIndex = 0;
	for (size_t i = 0; i < vSimResultFullName.size(); ++i)
	{	
		size_t nPaxTypeCount = dataList[i].size();
		
		size_t j =0;
		for (; j < nPaxTypeCount; ++j)
		{
			if(dataList[i][j].strPaxType.CompareNoCase(strPaxType) ==0)
				break;
		}

		if (j == nPaxTypeCount)
			break;

		CString strIndex;
		strIndex.Format(_T("%d"),i+1);
		GetListCtrl().InsertItem(i,strIndex);
		GetListCtrl().SetItemText(i,1,vSimResultFullName[i]);


		GetListCtrl().SetItemText(i,2,dataList[i][j].eMinimum.printTime());
		GetListCtrl().SetItemText(i,3,dataList[i][j].eAverage.printTime());
		GetListCtrl().SetItemText(i,4,dataList[i][j].eMaximum.printTime());

		GetListCtrl().SetItemText(i,5,dataList[i][j].eQ1.printTime());
		GetListCtrl().SetItemText(i,6,dataList[i][j].eQ2.printTime());
		GetListCtrl().SetItemText(i,7,dataList[i][j].eQ3.printTime());
		GetListCtrl().SetItemText(i,8,dataList[i][j].eP1.printTime());
		GetListCtrl().SetItemText(i,9,dataList[i][j].eP5.printTime());
		GetListCtrl().SetItemText(i,10,dataList[i][j].eP10.printTime());
		GetListCtrl().SetItemText(i,11,dataList[i][j].eP90.printTime());
		GetListCtrl().SetItemText(i,12,dataList[i][j].eP95.printTime());
		GetListCtrl().SetItemText(i,13,dataList[i][j].eP99.printTime());
		GetListCtrl().SetItemText(i,14,dataList[i][j].eSigma.printTime());
		GetListCtrl().SetItemText(i,15,dataList[i][j].strPaxType);

		CString strCount;
		strCount.Format(_T("%d"),dataList[i][j].nCount);
		GetListCtrl().SetItemText(i,16,strCount);


		GetListCtrl().SetItemData(i,i);
		nItemIndex++;
	}

}
void CRepListViewMultiRunReportOperator::SetSummaryTimeServiceContents(const CString& strPaxType)
{
	CListCtrl& wndListCtrl = GetListCtrl();
	//wndListCtrl.DeleteAllItems();

	int nCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nCount; i++)
		GetListCtrl().DeleteColumn(0);
//	ResetAllContent();
	GetListCtrl().DeleteAllItems();


	//insert column project name and run
	//GetListCtrl().InsertColumn( 0, _T("RUN"), LVCFMT_CENTER, 180);
	//GetListHeaderCtrl()->SetDataType( 0,  dtSTRING);
	DWORD headStyle = LVCFMT_CENTER;
	headStyle &= ~HDF_OWNERDRAW;

	GetListCtrl().InsertColumn(0,"",headStyle,20);
	GetListHeaderCtrl()->SetDataType( 0,  dtINT);
	GetListCtrl().InsertColumn( 1, _T("RUN"), headStyle, 60);
	GetListHeaderCtrl()->SetDataType( 1,  dtSTRING);

	const int m_nColumnCount = 15;
	CString csColumnLabel[m_nColumnCount];
	int nDefaultColumnWidth[m_nColumnCount];
	int nFormat[m_nColumnCount];
	EDataType type[m_nColumnCount];


	csColumnLabel[0] = CString("Min");
	csColumnLabel[1] = CString("Avg");
	csColumnLabel[2] = CString("Max");

	CString str = " (s)";
	csColumnLabel[3] = CString("Q1");
	csColumnLabel[4] = CString("Q2");
	csColumnLabel[5] = CString("Q3");
	csColumnLabel[6] = CString("P1");
	csColumnLabel[7] = CString("P5");
	csColumnLabel[8] = CString("P10");
	csColumnLabel[9] = CString("P90");
	csColumnLabel[10] = CString("P95");
	csColumnLabel[11] = CString("P99");
	csColumnLabel[12] = CString("Sigma");

	csColumnLabel[13] = CString("Passenger Type");
	csColumnLabel[14] = CString("Count");


	nDefaultColumnWidth[0] = 60;
	nDefaultColumnWidth[1] = 60;
	nDefaultColumnWidth[2] = 60;
	nDefaultColumnWidth[3] = 60;
	nDefaultColumnWidth[4] = 60;
	nDefaultColumnWidth[5] = 60;
	nDefaultColumnWidth[6] = 60;
	nDefaultColumnWidth[7] = 60;
	nDefaultColumnWidth[8] = 60;
	nDefaultColumnWidth[9] = 60;
	nDefaultColumnWidth[10] = 60;
	nDefaultColumnWidth[11] = 60;
	nDefaultColumnWidth[12] = 60;
	nDefaultColumnWidth[13] = 60;
	nDefaultColumnWidth[14] = 120;




	for( int i=0; i<15; i++ )
	{
		nFormat[i] = LVCFMT_CENTER; 
	}

	type[0] = dtDATETIME;
	type[1] = dtDATETIME;
	type[2] = dtDATETIME;
	type[3] = dtDATETIME;
	type[4] = dtDATETIME;

	type[5] = dtDATETIME;
	type[6] = dtDATETIME;
	type[7] = dtDATETIME;
	type[8] = dtDATETIME;
	type[9] = dtDATETIME;
	type[10] = dtDATETIME;
	type[11] = dtDATETIME;
	type[12] = dtDATETIME;
	type[13] = dtSTRING;				
	type[14] = dtINT;	


	for( int i=0; i<m_nColumnCount; i++ )
	{
		DWORD dwStyle = nFormat[i];
		dwStyle &= ~HDF_OWNERDRAW;

		GetListCtrl().InsertColumn( i+2, csColumnLabel[i],/* nFormat[i]*/dwStyle, nDefaultColumnWidth[i] );
		GetListHeaderCtrl()->SetDataType( i+2, type[i] );
	}

	CTermPlanDoc* pDoc = GetTermPlanDoc();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Summary::SummaryTimeServiceList& dataList = multiReport.GetReportResult_SummaryTimeService();
	if (dataList.empty())
		return;
	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	
	//remove RUN
	for (int nName =0; nName <static_cast<int>(vSimResultFullName.size()); ++nName )
	{
		CString strResultName = vSimResultFullName[nName];
		CString strValue = _T("");
		strValue.Format(_T("%d"),atoi(strResultName.TrimLeft(_T("RUN")))+1);
		vSimResultFullName[nName] = strValue;
	}

	CString strAverageName;
	vSimResultFullName.push_back(_T("Average"));
	size_t nResultSize = dataList.size();

	//insert average
	int nItemIndex = 0;
	for (size_t i = 0; i < vSimResultFullName.size(); ++i)
	{	
		size_t nPaxTypeCount = dataList[i].size();
		size_t j =0; 
		for (;j < nPaxTypeCount; ++j)
		{
			if(dataList[i][j].strPaxType.CompareNoCase(strPaxType) ==0)
				break;
		}

		if (j == nPaxTypeCount)
			break;

		CString strIndex;
		strIndex.Format(_T("%d"),nItemIndex+1);
		GetListCtrl().InsertItem(i,strIndex);
		GetListCtrl().SetItemText(i,1,vSimResultFullName[i]);

		GetListCtrl().SetItemText(i,2,dataList[i][j].eMinimum.printTime());
		GetListCtrl().SetItemText(i,3,dataList[i][j].eAverage.printTime());
		GetListCtrl().SetItemText(i,4,dataList[i][j].eMaximum.printTime());

		GetListCtrl().SetItemText(i,5,dataList[i][j].eQ1.printTime());
		GetListCtrl().SetItemText(i,6,dataList[i][j].eQ2.printTime());
		GetListCtrl().SetItemText(i,7,dataList[i][j].eQ3.printTime());
		GetListCtrl().SetItemText(i,8,dataList[i][j].eP1.printTime());
		GetListCtrl().SetItemText(i,9,dataList[i][j].eP5.printTime());
		GetListCtrl().SetItemText(i,10,dataList[i][j].eP10.printTime());
		GetListCtrl().SetItemText(i,11,dataList[i][j].eP90.printTime());
		GetListCtrl().SetItemText(i,12,dataList[i][j].eP95.printTime());
		GetListCtrl().SetItemText(i,13,dataList[i][j].eP99.printTime());
		GetListCtrl().SetItemText(i,14,dataList[i][j].eSigma.printTime());
		GetListCtrl().SetItemText(i,15,dataList[i][j].strPaxType);

		CString strCount;
		strCount.Format(_T("%d"),dataList[i][j].nCount);
		GetListCtrl().SetItemText(i,16,strCount);


		GetListCtrl().SetItemData(i,i);
		nItemIndex++;
	}

}

void CRepListViewMultiRunReportOperator::SetSummaryCriticalQueueContents()
{
	CListCtrl& wndListCtrl = GetListCtrl();
	//wndListCtrl.DeleteAllItems();

	int nCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nCount; i++)
		GetListCtrl().DeleteColumn(0);
	//ResetAllContent();
	GetListCtrl().DeleteAllItems();

	//GetListCtrl().InsertColumn( 0, _T("RUN"), LVCFMT_CENTER, 180);
	//GetListHeaderCtrl()->SetDataType( 0,  dtSTRING);
	DWORD headStyle = LVCFMT_CENTER;
	headStyle &= ~HDF_OWNERDRAW;
	GetListCtrl().InsertColumn(0,"",headStyle,20);
	GetListHeaderCtrl()->SetDataType( 0,  dtINT);
	GetListCtrl().InsertColumn( 1, _T("RUN"), headStyle, 60);
	GetListHeaderCtrl()->SetDataType( 1,  dtSTRING);

	const int  m_nColumnCount = 5;
	CString csColumnLabel[m_nColumnCount];
	int nDefaultColumnWidth[m_nColumnCount];
	int nFormat[m_nColumnCount];
	EDataType type[m_nColumnCount];

	csColumnLabel[0] = CString("Processor");
	csColumnLabel[1] = CString("Group Size");
	csColumnLabel[2] = CString("Avg Queue");
	csColumnLabel[3] = CString("Max Queue");
	csColumnLabel[4] = CString("Max Queue Time");

	nDefaultColumnWidth[0] = 120;
	nDefaultColumnWidth[1] = 70;
	nDefaultColumnWidth[2] = 70;
	nDefaultColumnWidth[3] = 70;
	nDefaultColumnWidth[4] = 70;

	for( int i=0; i<5; i++ )
	{
		nFormat[i] = LVCFMT_CENTER; 
	}

	type[0] = dtSTRING;
	type[1] = dtINT;
	type[2] = dtDEC;
	type[3] = dtINT;
	type[4] = dtDATETIME;

	for( int i=0; i<m_nColumnCount; i++ )
	{
		DWORD dwStyle = nFormat[i];
		dwStyle &= ~HDF_OWNERDRAW;

		GetListCtrl().InsertColumn( i+2, csColumnLabel[i],/* nFormat[i]*/dwStyle, nDefaultColumnWidth[i] );
		GetListHeaderCtrl()->SetDataType( i+2, type[i] );
	}

	CTermPlanDoc* pDoc = GetTermPlanDoc() ;
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Summary::SummaryCriticalQueueList& dataList = multiReport.GetReportResult_SummaryCriticalQueue();
	if (dataList.empty())
		return;
	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();

	//remove RUN
	for (int nName =0; nName <static_cast<int>(vSimResultFullName.size()); ++nName )
	{
		CString strResultName = vSimResultFullName[nName];
		CString strValue = _T("");
		strValue.Format(_T("%d"),atoi(strResultName.TrimLeft(_T("RUN")))+1);
		vSimResultFullName[nName] = strValue;
		
	}

	vSimResultFullName.push_back(_T("Average"));

	size_t nResultSize = dataList.size();
	int nItemIndex = 0;
	for (size_t i = 0; i < vSimResultFullName.size(); ++i)
	{
		for (size_t j =0; j < dataList[i].size(); ++j)
		{
			CString strIndex;
			strIndex.Format(_T("%d"),nItemIndex+1);
			GetListCtrl().InsertItem(nItemIndex,strIndex);
			GetListCtrl().SetItemText(nItemIndex,1,vSimResultFullName[i]);
			GetListCtrl().SetItemText(nItemIndex,2,dataList[i][j].strProcName);
			CString strGroupSize;
			strGroupSize.Format(_T("%d"),dataList[i][j].nGroupSize);
			GetListCtrl().SetItemText(nItemIndex,3,strGroupSize);

			CString strAvgQueue;
			strAvgQueue.Format(_T("%.2f"),dataList[i][j].fAvgQueue);
			GetListCtrl().SetItemText(nItemIndex,4,strAvgQueue);

			CString strMaxQueue;
			strMaxQueue.Format(_T("%d"),dataList[i][j].nMaxQueue);
			GetListCtrl().SetItemText(nItemIndex,5,strMaxQueue);


			GetListCtrl().SetItemText(nItemIndex,6,dataList[i][j].eMaxQueueTime.printTime());

			GetListCtrl().SetItemData(nItemIndex,nItemIndex);
			nItemIndex+=1;
		}

	}

}
void CRepListViewMultiRunReportOperator::SetDetailedPaxDataContents(MultiRunsReport::Detail::PaxDataList& , std::vector<int>& vSimResults)
{
	CTermPlanDoc* pDoc = GetTermPlanDoc();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Detail::PaxDataList& dataList = multiReport.GetReportResult_PaxDataList();
	if (dataList.empty())
		return;

	CListCtrl& wndListCtrl = GetListCtrl();
	//wndListCtrl.DeleteAllItems();

	int nCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nCount; i++)
		GetListCtrl().DeleteColumn(0);
//	ResetAllContent();

	GetListCtrl().DeleteAllItems();
	DWORD headStyle = LVCFMT_CENTER;
	headStyle &= ~HDF_OWNERDRAW;
	wndListCtrl.InsertColumn(0,"",headStyle,20);

	headStyle = LVCFMT_LEFT;
	headStyle &= ~HDF_OWNERDRAW;
	wndListCtrl.InsertColumn(1, _T("SimResult"), headStyle, 80);
	
	if (dataList.empty())
		return;

	
	int maxValue = 0 ;
	int MaxIndex = 0 ;
	for (int i = 0 ; i < (int)dataList.size() ;i++)
	{
		if((int)dataList[i].size() > maxValue)
		{
			maxValue = (int)dataList[i].size()  ;
			MaxIndex  = i ;
		}

	}
	size_t rangeCount = maxValue;
	for (size_t i = 0; i < rangeCount; ++i)
	{
		CString strRange;
		strRange.Format(_T("%d - %d"), dataList[MaxIndex][i].ValueRange.first, dataList[MaxIndex][i].ValueRange.second);

		DWORD dwStyle = LVCFMT_LEFT;
		dwStyle &= ~HDF_OWNERDRAW;

		wndListCtrl.InsertColumn(2+i, strRange, /*LVCFMT_LEFT*/dwStyle, 100);
	}

	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	for (size_t i = 0; i < vSimResultFullName.size(); ++i)
	{
		//CString strSimName;
		//strSimName.Format(_T("SimResult%d"), vSimResults[i]);
		CString strIndex;
		strIndex.Format(_T("%d"),i+1);
		wndListCtrl.InsertItem(i,strIndex);

		int nCurSimResult = atoi(vSimResultFullName[i].Mid(3,vSimResultFullName[i].GetLength()));
		CString strRun = _T("");
		strRun.Format(_T("%s%d"),vSimResultFullName[i].Left(3),nCurSimResult+1);
		
	//	wndListCtrl.InsertItem(i, strRun);
		wndListCtrl.SetItemText(i,1,strRun);
		GetListCtrl().SetItemData(i,i);
		for (size_t n = 0; n < rangeCount; ++n)
		{
			CString strCount;
			if(n <  dataList[i].size())
				strCount.Format(_T("%d"), dataList[i][n].nCount);
			else
				strCount.Format(_T("%d"),0) ;

			wndListCtrl.SetItemText(i, n + 2, strCount);

		}
	}
}

void CRepListViewMultiRunReportOperator::SetDetailedTimeDataContents(MultiRunsReport::Detail::TimeDataList& , int _nRangeCount )
{
	CTermPlanDoc* pDoc = GetTermPlanDoc();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Detail::TimeDataList& dataList = multiReport.GetReportResult_TimeDataList();
	if (dataList.empty())
		return;


	CListCtrl& wndListCtrl = GetListCtrl();
	//wndListCtrl.DeleteAllItems();

	int nCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nCount; i++)
		GetListCtrl().DeleteColumn(0);
	//ResetAllContent();
	GetListCtrl().DeleteAllItems();

	DWORD headStyle = LVCFMT_CENTER;
	headStyle &= ~HDF_OWNERDRAW;
//	wndListCtrl.InsertColumn(0, _T("SimResult"), LVCFMT_LEFT, 80);
	wndListCtrl.InsertColumn(0,"",headStyle,20);

	headStyle = LVCFMT_LEFT;
	headStyle &= ~HDF_OWNERDRAW;
	wndListCtrl.InsertColumn(1, _T("SimResult"), headStyle, 80);
	if (dataList.empty())
		return;

	int maxValue = 0 ;
	int MaxIndex = 0 ;
	for (int i = 0 ; i < (int)dataList.size() ;i++)
	{
		if((int)dataList[i].size() > maxValue)
		{
			maxValue = (int)dataList[i].size()  ;
			MaxIndex  = i ;
		}

	}

	_nRangeCount = maxValue;
	for ( int i = 0; i < _nRangeCount; ++i)
	{
		CString strRange;
		bool bInSecond = dataList[MaxIndex][i].bInSecond;
		strRange = dataList[MaxIndex][i].TimeValueRange.first.printTime( bInSecond ? 1 : 0 ) + " - " + dataList[MaxIndex][i].TimeValueRange.second.printTime( bInSecond ? 1 : 0 );

		DWORD dwStyle = LVCFMT_LEFT;
		dwStyle &= ~HDF_OWNERDRAW;

		wndListCtrl.InsertColumn(2 + i, strRange, dwStyle, 100);
	}
	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	for (size_t i = 0; i < vSimResultFullName.size(); ++i)
	{
		//CString strSimName;
		//strSimName.Format(_T("Run%d"), i+1 );
		CString strIndex;
		strIndex.Format(_T("%d"),i+1);
		wndListCtrl.InsertItem(i,strIndex);

		int nCurSimResult = atoi(vSimResultFullName[i].Mid(3,vSimResultFullName[i].GetLength()));
		CString strRun = _T("");
		strRun.Format(_T("%s%d"),vSimResultFullName[i].Left(3),nCurSimResult+1);
		wndListCtrl.SetItemText(i,1,strRun);
	//	wndListCtrl.InsertItem(i, strRun);
		GetListCtrl().SetItemData(i,i);
		for ( int n = 0; n < _nRangeCount; ++n)
		{
			CString strCount;
			if(n <  (int)dataList[i].size())
				strCount.Format(_T("%d"), dataList[i][n].nCount);
			else
				strCount.Format(_T("%d"),0) ;

			wndListCtrl.SetItemText(i, n + 2, strCount);
		}
	}
}

void CRepListViewMultiRunReportOperator::SetSummaryPaxDataContents(MultiRunsReport::Summary::PaxDataList& dataList, std::vector<int>& vSimResults)
{
	CListCtrl& wndListCtrl = GetListCtrl();
	//wndListCtrl.DeleteAllItems();

	int nCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nCount; i++)
		GetListCtrl().DeleteColumn(0);
//	ResetAllContent();
	GetListCtrl().DeleteAllItems();

//	wndListCtrl.InsertColumn(0, _T("Passenger Type"), LVCFMT_LEFT, 80);
	DWORD headStyle = LVCFMT_CENTER;
	headStyle &= ~HDF_OWNERDRAW;
	wndListCtrl.InsertColumn(0,"",headStyle,20);

	headStyle = LVCFMT_LEFT;
	headStyle &= ~HDF_OWNERDRAW;
	wndListCtrl.InsertColumn(1, _T("SimResult"), headStyle, 80);

	for (size_t i = 0; i < vSimResults.size(); ++i)
	{
		CString strSimName;
		strSimName.Format(_T("SimResult%d"), vSimResults[i]);
		DWORD dwStyle = LVCFMT_LEFT;
		dwStyle &= ~HDF_OWNERDRAW;

		wndListCtrl.InsertColumn(2 + i * 3, strSimName + _T(" Min Value"), dwStyle, 100);
		wndListCtrl.InsertColumn(3 + i * 3, strSimName + _T(" Average Value"), dwStyle, 100);
		wndListCtrl.InsertColumn(4 + i * 3, strSimName + _T(" Max Value"), dwStyle, 100);
	}

	int nRow = 0;
	for (MultiRunsReport::Summary::PaxDataList::iterator iter = dataList.begin(); iter != dataList.end(); ++iter)
	{
		CString strIndex;
		strIndex.Format(_T("%d"),nRow+1);
		wndListCtrl.InsertItem(nRow,strIndex);
		wndListCtrl.SetItemText(nRow,1,iter->first);
		//wndListCtrl.InsertItem(nRow, iter->first);
		GetListCtrl().SetItemData(nRow,nRow);
		for (size_t i = 0; i < vSimResults.size(); ++i)
		{
			CString strValue;

			strValue.Format(_T("%.2f"), iter->second[i].fMinValue);
			wndListCtrl.SetItemText(nRow, 1 + i * 3 + 1, strValue);

			strValue.Format(_T("%.2f"), iter->second[i].fAveValue);
			wndListCtrl.SetItemText(nRow, 2 + i * 3 + 1, strValue);

			strValue.Format(_T("%.2f"), iter->second[i].fMaxValue);
			wndListCtrl.SetItemText(nRow, 3 + i * 3 + 1, strValue);
		}

		nRow++;
	}
}
void CRepListViewMultiRunReportOperator::SetDetailOccupancyDataWithNoRanger(MultiRunsReport::Detail::OccupnacyDataList& , int _nRangeCount)
{
	CTermPlanDoc* pDoc = GetTermPlanDoc();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Detail::OccupnacyDataList& dataList = multiReport.GetReportResult_OccupnacyDataList();
	if (dataList.empty())
		return;

	CListCtrl& wndListCtrl = GetListCtrl();
	//wndListCtrl.DeleteAllItems();

	int nCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nCount; i++)
		GetListCtrl().DeleteColumn(0);
	//	ResetAllContent();
	GetListCtrl().DeleteAllItems();

	/*wndListCtrl.InsertColumn(0, _T("SimResult"), LVCFMT_LEFT, 80);*/
	DWORD headStyle = LVCFMT_CENTER;
	headStyle &= ~HDF_OWNERDRAW;
	wndListCtrl.InsertColumn(0,"",headStyle,20);

	headStyle = LVCFMT_LEFT;
	headStyle &= ~HDF_OWNERDRAW;
	wndListCtrl.InsertColumn(1, _T("SimResult"), headStyle, 80);
	if (dataList.empty())
		return;
	_nRangeCount = static_cast<int>(dataList[0].size());

	for ( int i = 0; i < _nRangeCount; ++i)
	{
		CString strRange;
		bool bInSecond = false;//dataList[0][i].bInSecond;
		strRange = dataList[0][i].startTime.printTime( bInSecond ? 1 : 0 );

		DWORD dwStyle = LVCFMT_LEFT;
		dwStyle &= ~HDF_OWNERDRAW;
		wndListCtrl.InsertColumn(1 + i, strRange, dwStyle, 100);
	}
	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	for (size_t i = 0; i < vSimResultFullName.size(); ++i)
	{
		//CString strSimName;
		//strSimName.Format(_T("Run%d"), i+1 );
		CString strIndex;
		strIndex.Format(_T("%d"),i+1);
		wndListCtrl.InsertItem(i,strIndex);

	//	wndListCtrl.InsertItem(i, vSimResultFullName[i]);
		wndListCtrl.SetItemText(i,1,vSimResultFullName[i]);
		GetListCtrl().SetItemData(i,i);
		for ( int n = 0; n < _nRangeCount; ++n)
		{
			CString strCount;
			strCount.Format(_T("%d"), dataList[i][n].nPaxCount);

			wndListCtrl.SetItemText(i, n + 2, strCount);
		}
	}
}
void CRepListViewMultiRunReportOperator::SetDetailedOccupancyDataContents(MultiRunsReport::Detail::OccupnacyDataList& , int _nRangeCount )
{
	CTermPlanDoc* pDoc = GetTermPlanDoc();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Detail::OccupnacyDataList& dataList = multiReport.GetReportResult_OccupnacyDataList();
	if (dataList.empty())
		return;

	CListCtrl& wndListCtrl = GetListCtrl();
	//wndListCtrl.DeleteAllItems();

	int nCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nCount; i++)
		GetListCtrl().DeleteColumn(0);
//	ResetAllContent();
	GetListCtrl().DeleteAllItems();

//	wndListCtrl.InsertColumn(0, _T("SimResult"), LVCFMT_LEFT, 80);
	DWORD headStyle = LVCFMT_CENTER;
	headStyle &= ~HDF_OWNERDRAW;
	wndListCtrl.InsertColumn(0,"",headStyle,20);

	headStyle = LVCFMT_LEFT;
	headStyle &= ~HDF_OWNERDRAW;
	wndListCtrl.InsertColumn(1, _T("SimResult"), headStyle, 80);

	if (dataList.empty())
		return;
	_nRangeCount = static_cast<int>(dataList[0].size());

	for ( int i = 0; i < _nRangeCount-1; ++i)
	{
		CString strRange;
		bool bInSecond = false;//dataList[0][i].bInSecond;
		strRange = dataList[0][i].startTime.printTime( bInSecond ? 1 : 0 ) + " - " + dataList[0][i+1].startTime.printTime( bInSecond ? 1 : 0 );

		DWORD dwStyle = LVCFMT_LEFT;
		dwStyle &= ~HDF_OWNERDRAW;
		wndListCtrl.InsertColumn(2 + i, strRange, dwStyle, 100);
	}
	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	for (size_t i = 0; i < vSimResultFullName.size(); ++i)
	{
		//CString strSimName;
		//strSimName.Format(_T("Run%d"), i+1 );
		CString strIndex;
		strIndex.Format(_T("%d"),i+1);
		wndListCtrl.InsertItem(i,strIndex);

		int nCurSimResult = atoi(vSimResultFullName[i].Mid(3,vSimResultFullName[i].GetLength()));
		CString strRun = _T("");
		strRun.Format(_T("%s%d"),vSimResultFullName[i].Left(3),nCurSimResult+1);
		wndListCtrl.SetItemText(i,1,strRun);
		//wndListCtrl.InsertItem(i, strRun);
		GetListCtrl().SetItemData(i,i);
		for ( int n = 0; n < _nRangeCount-1; ++n)
		{
			CString strCount;
			strCount.Format(_T("%d"), dataList[i][n].nPaxCount);

			wndListCtrl.SetItemText(i, n + 2, strCount);
		}
	}
}

CSortableHeaderCtrl* CRepListViewMultiRunReportOperator::GetListHeaderCtrl()
{

	if(getListView())
		return &(getListView()->GetSortableHeaderCtrl());
	ASSERT(NULL);
	return NULL;
}

void CRepListViewMultiRunReportOperator::OnListviewExport()
{
	if(m_sExtension.IsEmpty())		//If no items listed, then return;
		return;
	CString upExtension = m_sExtension;
	upExtension.MakeUpper();
	CFileDialog filedlg( FALSE,m_sExtension, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Report files (*." + m_sExtension + ") | *." + m_sExtension + ";*." + upExtension + "|All type (*.*)|*.*|", NULL, 0, FALSE );
	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();

	//CString sFileName;
	//if( m_pReportManager->GetLoadReportType() == load_by_user )	
	//{
	//	sFileName = m_pReportManager->GetUserLoadReportFile();
	//}
	//else				  // load_by_system
	//{
	//	sFileName = GetTermPlanDoc()->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( GetTermPlanDoc()->m_ProjInfo.path );
	//}

	//CFileFind fileFind;
	//if(fileFind.FindFile(csFileName))
	//{
	//	if(MessageBox(NULL,"File already exists, are you sure to overwrite it?", "Warning!", MB_YESNO) == IDNO)
	//		return;
	//}
	//CopyFile(sFileName, csFileName, TRUE);
}

void CRepListViewMultiRunReportOperator::GetFileReportTypeAndExtension()
{

}

CRepListView * CRepListViewMultiRunReportOperator::getListView() const
{
	return (CRepListView *)m_pListView;
}