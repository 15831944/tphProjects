// MutiRunRepListView.cpp : implementation file
//
#include "stdafx.h"
#include "MultiRunRepListView.h"
#include "TermPlanDoc.h"
#include "../Reports/ReportParaDB.h"
#include "../Reports/ReportParameter.h"
#include "ViewMsg.h"

// CMultiRunRepListView

IMPLEMENT_DYNCREATE(CMultiRunRepListView, CListView)

CMultiRunRepListView::CMultiRunRepListView()
{
	m_nColumnCount = 0;
}

CMultiRunRepListView::~CMultiRunRepListView()
{
}

BEGIN_MESSAGE_MAP(CMultiRunRepListView, CListView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
END_MESSAGE_MAP()


// CMultiRunRepListView message handlers

int CMultiRunRepListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	DWORD dwStyle = SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE);
	dwStyle |= LVS_SORTASCENDING|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)dwStyle);
	m_ctlHeaderCtrl.SubclassWindow(GetDlgItem(0)->m_hWnd);
//	DWORD dwStyle = GetListCtrl().GetExtendedStyle()| LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
//	GetListCtrl().SetExtendedStyle(dwStyle);

//	m_ctlHeaderCtrl.SubclassWindow(GetDlgItem(0)->m_hWnd);
	return 0;
}

BOOL CMultiRunRepListView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= LVS_SHOWSELALWAYS|LVS_REPORT;
	return CListView::PreCreateWindow(cs);
}

void CMultiRunRepListView::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nTestIndex = pNMListView->iSubItem;
	if (nTestIndex >= 0)
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
			m_ctlHeaderCtrl.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
		else
			m_ctlHeaderCtrl.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
		m_ctlHeaderCtrl.SaveSortList();
	}		
	*pResult = 0;
}

void CMultiRunRepListView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* pHint)
{
	if (lHint == MULTIREPORT_SHOWREPORT) //show report
	{

			CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
			CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();
			std::vector<int>& vSimResults = pDoc->GetARCReportManager().GetmSelectedMultiRunsSimResults();

			ENUM_REPORT_TYPE reportType = pDoc->GetARCReportManager().GetMultiRunsReportType();


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
						MultiRunsReport::Detail::PaxDataList paxDataList;
						SetDetailedPaxDataContents(paxDataList, vSimResults);
					}

					break;
				case ENUM_QUEUETIME_REP:
					{
						MultiRunsReport::Detail::TimeDataList timeDataList;
						SetDetailedTimeDataContents(timeDataList, 0);
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

							CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
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

							CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
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

	//CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	//CSimAndReportManager* pReportManager = pDoc->GetTerminal().GetSimReportManager();
	//ENUM_REPORT_TYPE reportType = pDoc->GetMultiRunsReportType();
	//std::vector<int>& vSimResults = pDoc->GetmSelectedMultiRunsSimResults();

	//CReportParameter* pReportParam = pDoc->GetTerminal().m_pReportParaDB->GetParameterbyType(reportType);
	//if (pReportParam == NULL)
	//	return;

	//int iDetailed;
	//pReportParam->GetReportType(iDetailed);
	//if (iDetailed == 0) //detailed
	//{
	//	switch(reportType)
	//	{
	//	case ENUM_DISTANCE_REP:

	//		{
	//			MultiRunsReport::Detail::PaxDataList paxDataList;

	//			size_t nSimResultCount = vSimResults.size();
	//			for (size_t i = 0; i < nSimResultCount; ++i)
	//			{
	//				pReportManager->SetCurrentSimResult(vSimResults[i]);
	//				pReportManager->SetCurrentReportType(reportType);
	//				CString strReportFileName = pReportManager->GetCurrentReportFileName(pDoc->m_ProjInfo.path);
	//				MultiRunsReport::Detail::DistTravelledDataLoader loader(strReportFileName);
	//				loader.LoadData();
	//				MultiRunsReport::Detail::DistTravelledDataLoader::DataList& dataList = loader.GetData();
	//				paxDataList.push_back(dataList);
	//			}

	//			SetDetailedPaxDataContents(paxDataList, vSimResults);
	//		}

	//		break;
	//	case ENUM_QUEUETIME_REP:
	//		{
	//			MultiRunsReport::Detail::TimeDataList timeDataList;

	//			size_t nSimResultCount = vSimResults.size();
	//			int nRangeCount = 0;
	//			for (size_t i = 0; i < nSimResultCount; ++i)
	//			{
	//				pReportManager->SetCurrentSimResult(vSimResults[i]);
	//				pReportManager->SetCurrentReportType(reportType);
	//				CString strReportFileName = pReportManager->GetCurrentReportFileName(pDoc->m_ProjInfo.path);
	//				MultiRunsReport::Detail::TimeInQueueDataLoader loader(strReportFileName);
	//				loader.LoadData();
	//				MultiRunsReport::Detail::TimeInQueueDataLoader::DataList& dataList = loader.GetData();
	//				timeDataList.push_back(dataList);
	//				nRangeCount = loader.GetRangeCountOnX();
	//			}

	//			SetDetailedTimeDataContents(timeDataList, nRangeCount);
	//		}
	//		break;

	//	case ENUM_SERVICETIME_REP:
	//		{
	//			MultiRunsReport::Detail::TimeDataList timeDataList;

	//			size_t nSimResultCount = vSimResults.size();
	//			int nRangeCount = 0;
	//			for (size_t i = 0; i < nSimResultCount; ++i)
	//			{
	//				pReportManager->SetCurrentSimResult(vSimResults[i]);
	//				pReportManager->SetCurrentReportType(reportType);
	//				CString strReportFileName = pReportManager->GetCurrentReportFileName(pDoc->m_ProjInfo.path);
	//				MultiRunsReport::Detail::TimeInQueueDataLoader loader(strReportFileName);
	//				loader.LoadData();
	//				MultiRunsReport::Detail::TimeInQueueDataLoader::DataList& dataList = loader.GetData();
	//				timeDataList.push_back(dataList);
	//				nRangeCount = loader.GetRangeCountOnX();
	//			}

	//			SetDetailedTimeDataContents(timeDataList, nRangeCount);
	//		}
	//		break;

	//	case ENUM_DURATION_REP:
	//		{
	//			MultiRunsReport::Detail::TimeDataList timeDataList;

	//			size_t nSimResultCount = vSimResults.size();
	//			int nRangeCount = 0;
	//			for (size_t i = 0; i < nSimResultCount; ++i)
	//			{
	//				pReportManager->SetCurrentSimResult(vSimResults[i]);
	//				pReportManager->SetCurrentReportType(reportType);
	//				CString strReportFileName = pReportManager->GetCurrentReportFileName(pDoc->m_ProjInfo.path);
	//				MultiRunsReport::Detail::TimeInTerminalDataLoader loader(strReportFileName);
	//				loader.LoadData();
	//				MultiRunsReport::Detail::TimeInTerminalDataLoader::DataList& dataList = loader.GetData();
	//				timeDataList.push_back(dataList);
	//				nRangeCount = loader.GetRangeCountOnX();
	//			}

	//			SetDetailedTimeDataContents(timeDataList, nRangeCount);
	//		}

	//		break;
	//	case ENUM_ACTIVEBKDOWN_REP:
	//		break;
	//	case ENUM_PAXCOUNT_REP:
	//		{

	//			MultiRunsReport::Detail::OccupnacyDataList timeDataList;

	//			size_t nSimResultCount = vSimResults.size();
	//			int nRangeCount = 0;
	//			for (size_t i = 0; i < nSimResultCount; ++i)
	//			{
	//				pReportManager->SetCurrentSimResult(vSimResults[i]);
	//				pReportManager->SetCurrentReportType(reportType);
	//				CString strReportFileName = pReportManager->GetCurrentReportFileName(pDoc->m_ProjInfo.path);
	//				MultiRunsReport::Detail::OccupancyDataLoader loader(strReportFileName);
	//				loader.LoadData();
	//				MultiRunsReport::Detail::OccupancyDataLoader::DataList & dataList = loader.GetData();
	//				timeDataList.push_back(dataList);
	//				nRangeCount = dataList.size();
	//			}
	//				SetDetailedOccupancyDataContents(timeDataList,0);

	//		}
	//		break;
	//	default:
	//		break;
	//	}
	//}
	//else // summary
	//{
	//	switch(reportType)
	//	{
	//	case ENUM_DISTANCE_REP:
	//	case ENUM_QUEUETIME_REP:
	//	case ENUM_DURATION_REP:
	//	case ENUM_SERVICETIME_REP:
	//		{
	//			MultiRunsReport::Summary::PaxDataList paxDataList;
	//			size_t nSimResultCount = vSimResults.size();
	//			std::vector<MultiRunsReport::Summary::PaxDataValue> tempDataRow(nSimResultCount);

	//			for (size_t i = 0; i < nSimResultCount; ++i)
	//			{
	//				pReportManager->SetCurrentSimResult(vSimResults[i]);
	//				pReportManager->SetCurrentReportType(reportType);
	//				CString strReportFileName = pReportManager->GetCurrentReportFileName(pDoc->m_ProjInfo.path);
	//				MultiRunsReport::Summary::DistTravelledDataLoader loader(strReportFileName);
	//				loader.LoadData();
	//				MultiRunsReport::Summary::DistTravelledDataLoader::DataList& dataList = loader.GetData();
	//				for (size_t n = 0; n < dataList.size(); ++n)
	//				{
	//					if (paxDataList.find(dataList[n].strPaxType) == paxDataList.end())
	//						paxDataList[dataList[n].strPaxType] = tempDataRow;

	//					paxDataList[dataList[n].strPaxType][i] = dataList[n].value;
	//				}
	//			}

	//			SetSummaryPaxDataContents(paxDataList, vSimResults);
	//		}

	//		break;

	//	case ENUM_ACTIVEBKDOWN_REP:
	//		break;
	//	case ENUM_PAXCOUNT_REP:
	//		break;
	//	default:
	//		break;
	//	}
	//}
}
void CMultiRunRepListView::SetSummaryQueueTimeContents(const CString& strPaxType)
{
	CListCtrl& wndListCtrl = GetListCtrl();
	wndListCtrl.DeleteAllItems();

	int nCount = m_ctlHeaderCtrl.GetItemCount();
	for (int i = 0;i < nCount; i++)
		wndListCtrl.DeleteColumn(0);


	//insert column project name and run
	GetListCtrl().InsertColumn( 0, _T("Name"), LVCFMT_CENTER, 180);
	m_ctlHeaderCtrl.SetDataType( 0,  dtSTRING);

	const int m_nColumnCount = 15;
	CString csColumnLabel[m_nColumnCount];
	int nDefaultColumnWidth[m_nColumnCount];
	int nFormat[m_nColumnCount];
	EDataType type[m_nColumnCount];

	csColumnLabel[0] = CString("Passenger Type");
	csColumnLabel[1] = CString("Minimum");
	csColumnLabel[2] = CString("Average");
	csColumnLabel[3] = CString("Maximum");
	csColumnLabel[4] = CString("Count");

	CString str = " (s)";
	csColumnLabel[5] = CString("Q1") + str;
	csColumnLabel[6] = CString("Q2") + str;
	csColumnLabel[7] = CString("Q3") + str;
	csColumnLabel[8] = CString("P1") + str;
	csColumnLabel[9] = CString("P5") + str;
	csColumnLabel[10] = CString("P10") + str;
	csColumnLabel[11] = CString("P90") + str;
	csColumnLabel[12] = CString("P95") + str;
	csColumnLabel[13] = CString("P99") + str;

	csColumnLabel[14] = CString("Sigma") + str;


	nDefaultColumnWidth[0] = 120;
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
	nDefaultColumnWidth[14] = 60;


	for( int i=0; i<15; i++ )
	{
		nFormat[i] = LVCFMT_CENTER; 
	}

	type[0] = dtSTRING;
	type[1] = dtDATETIME;
	type[2] = dtDATETIME;
	type[3] = dtDATETIME;
	type[4] = dtINT;

	type[5] = dtDATETIME;
	type[6] = dtDATETIME;
	type[7] = dtDATETIME;
	type[8] = dtDATETIME;
	type[9] = dtDATETIME;
	type[10] = dtDATETIME;
	type[11] = dtDATETIME;
	type[12] = dtDATETIME;
	type[13] = dtDATETIME;				
	type[14] = dtDATETIME;	


	for( int i=0; i<m_nColumnCount; i++ )
	{
		GetListCtrl().InsertColumn( i+1, csColumnLabel[i], nFormat[i], nDefaultColumnWidth[i] );
		m_ctlHeaderCtrl.SetDataType( i+1, type[i] );
	}

	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Summary::SummaryQTimeList& dataList = multiReport.GetReportResult_SummaryQueueTime();
	if (dataList.empty())
		return;
	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	CString strAverageName;
	
	vSimResultFullName.insert(vSimResultFullName.begin(),_T("Project(s) Average Values"));
	size_t nResultSize = dataList.size();
	
	//insert average

	for (size_t i = 0; i < vSimResultFullName.size(); ++i)
	{	
		size_t nPaxTypeCount = dataList[i].size();
		for (size_t j =0; j < nPaxTypeCount; ++j)
		{
			if(dataList[i][j].strPaxType.CompareNoCase(strPaxType) ==0)
				break;
		}

		if (j == nPaxTypeCount)
			break;

		GetListCtrl().InsertItem(i, vSimResultFullName[i]);
		GetListCtrl().SetItemText(i,1,dataList[i][j].strPaxType);

		GetListCtrl().SetItemText(i,2,dataList[i][j].eMinimum.printTime());
		GetListCtrl().SetItemText(i,3,dataList[i][j].eAverage.printTime());
		GetListCtrl().SetItemText(i,4,dataList[i][j].eMaximum.printTime());
		CString strCount;
		strCount.Format(_T("%d"),dataList[i][j].nCount);
		GetListCtrl().SetItemText(i,5,strCount);
		GetListCtrl().SetItemText(i,6,dataList[i][j].eQ1.printTime());
		GetListCtrl().SetItemText(i,7,dataList[i][j].eQ2.printTime());
		GetListCtrl().SetItemText(i,8,dataList[i][j].eQ3.printTime());
		GetListCtrl().SetItemText(i,9,dataList[i][j].eP1.printTime());
		GetListCtrl().SetItemText(i,10,dataList[i][j].eP5.printTime());
		GetListCtrl().SetItemText(i,11,dataList[i][j].eP10.printTime());
		GetListCtrl().SetItemText(i,12,dataList[i][j].eP90.printTime());
		GetListCtrl().SetItemText(i,13,dataList[i][j].eP95.printTime());
		GetListCtrl().SetItemText(i,14,dataList[i][j].eP99.printTime());
		GetListCtrl().SetItemText(i,15,dataList[i][j].eSigma.printTime());

		GetListCtrl().SetItemData(i,i);
	}


}
void CMultiRunRepListView::SetSummaryTimeServiceContents(const CString& strPaxType)
{
	CListCtrl& wndListCtrl = GetListCtrl();
	wndListCtrl.DeleteAllItems();

	int nCount = m_ctlHeaderCtrl.GetItemCount();
	for (int i = 0;i < nCount; i++)
		wndListCtrl.DeleteColumn(0);


	//insert column project name and run
	GetListCtrl().InsertColumn( 0, _T("Name"), LVCFMT_CENTER, 180);
	m_ctlHeaderCtrl.SetDataType( 0,  dtSTRING);

	const int m_nColumnCount = 15;
	CString csColumnLabel[m_nColumnCount];
	int nDefaultColumnWidth[m_nColumnCount];
	int nFormat[m_nColumnCount];
	EDataType type[m_nColumnCount];

	csColumnLabel[0] = CString("Passenger Type");
	csColumnLabel[1] = CString("Minimum");
	csColumnLabel[2] = CString("Average");
	csColumnLabel[3] = CString("Maximum");
	csColumnLabel[4] = CString("Count");

	CString str = " (s)";
	csColumnLabel[5] = CString("Q1") + str;
	csColumnLabel[6] = CString("Q2") + str;
	csColumnLabel[7] = CString("Q3") + str;
	csColumnLabel[8] = CString("P1") + str;
	csColumnLabel[9] = CString("P5") + str;
	csColumnLabel[10] = CString("P10") + str;
	csColumnLabel[11] = CString("P90") + str;
	csColumnLabel[12] = CString("P95") + str;
	csColumnLabel[13] = CString("P99") + str;

	csColumnLabel[14] = CString("Sigma") + str;


	nDefaultColumnWidth[0] = 120;
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
	nDefaultColumnWidth[14] = 60;


	for( int i=0; i<15; i++ )
	{
		nFormat[i] = LVCFMT_CENTER; 
	}

	type[0] = dtSTRING;
	type[1] = dtDATETIME;
	type[2] = dtDATETIME;
	type[3] = dtDATETIME;
	type[4] = dtINT;

	type[5] = dtDATETIME;
	type[6] = dtDATETIME;
	type[7] = dtDATETIME;
	type[8] = dtDATETIME;
	type[9] = dtDATETIME;
	type[10] = dtDATETIME;
	type[11] = dtDATETIME;
	type[12] = dtDATETIME;
	type[13] = dtDATETIME;				
	type[14] = dtDATETIME;	


	for( int i=0; i<m_nColumnCount; i++ )
	{
		GetListCtrl().InsertColumn( i+1, csColumnLabel[i], nFormat[i], nDefaultColumnWidth[i] );
		m_ctlHeaderCtrl.SetDataType( i+1, type[i] );
	}

	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Summary::SummaryTimeServiceList& dataList = multiReport.GetReportResult_SummaryTimeService();
	if (dataList.empty())
		return;
	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	CString strAverageName;
	
	vSimResultFullName.insert(vSimResultFullName.begin(),_T("Project(s) Average Values"));
	size_t nResultSize = dataList.size();
	
	//insert average

	for (size_t i = 0; i < vSimResultFullName.size(); ++i)
	{	
		size_t nPaxTypeCount = dataList[i].size();
		for (size_t j =0; j < nPaxTypeCount; ++j)
		{
			if(dataList[i][j].strPaxType.CompareNoCase(strPaxType) ==0)
				break;
		}

		if (j == nPaxTypeCount)
			break;

		GetListCtrl().InsertItem(i, vSimResultFullName[i]);
		GetListCtrl().SetItemText(i,1,dataList[i][j].strPaxType);

		GetListCtrl().SetItemText(i,2,dataList[i][j].eMinimum.printTime());
		GetListCtrl().SetItemText(i,3,dataList[i][j].eAverage.printTime());
		GetListCtrl().SetItemText(i,4,dataList[i][j].eMaximum.printTime());
		CString strCount;
		strCount.Format(_T("%d"),dataList[i][j].nCount);
		GetListCtrl().SetItemText(i,5,strCount);
		GetListCtrl().SetItemText(i,6,dataList[i][j].eQ1.printTime());
		GetListCtrl().SetItemText(i,7,dataList[i][j].eQ2.printTime());
		GetListCtrl().SetItemText(i,8,dataList[i][j].eQ3.printTime());
		GetListCtrl().SetItemText(i,9,dataList[i][j].eP1.printTime());
		GetListCtrl().SetItemText(i,10,dataList[i][j].eP5.printTime());
		GetListCtrl().SetItemText(i,11,dataList[i][j].eP10.printTime());
		GetListCtrl().SetItemText(i,12,dataList[i][j].eP90.printTime());
		GetListCtrl().SetItemText(i,13,dataList[i][j].eP95.printTime());
		GetListCtrl().SetItemText(i,14,dataList[i][j].eP99.printTime());
		GetListCtrl().SetItemText(i,15,dataList[i][j].eSigma.printTime());

		GetListCtrl().SetItemData(i,i);
	}


}

void CMultiRunRepListView::SetSummaryCriticalQueueContents()
{
	CListCtrl& wndListCtrl = GetListCtrl();
	wndListCtrl.DeleteAllItems();

	int nCount = m_ctlHeaderCtrl.GetItemCount();
	for (int i = 0;i < nCount; i++)
		wndListCtrl.DeleteColumn(0);

	GetListCtrl().InsertColumn( 0, _T("Name"), LVCFMT_CENTER, 180);
	m_ctlHeaderCtrl.SetDataType( 0,  dtSTRING);

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
		GetListCtrl().InsertColumn( i+1, csColumnLabel[i], nFormat[i], nDefaultColumnWidth[i] );
		m_ctlHeaderCtrl.SetDataType( i+1, type[i] );
	}

	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Summary::SummaryCriticalQueueList& dataList = multiReport.GetReportResult_SummaryCriticalQueue();
	if (dataList.empty())
		return;
	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	vSimResultFullName.insert(vSimResultFullName.begin(),_T("Average Values of Project(s)"));
	size_t nResultSize = dataList.size();
	int nItemIndex = 0;
	for (size_t i = 0; i < vSimResultFullName.size(); ++i)
	{
		for (size_t j =0; j < dataList[i].size(); ++j)
		{
			GetListCtrl().InsertItem(nItemIndex, vSimResultFullName[i]);
			GetListCtrl().SetItemText(nItemIndex,1,dataList[i][j].strProcName);
			CString strGroupSize;
			strGroupSize.Format(_T("%d"),dataList[i][j].nGroupSize);
			GetListCtrl().SetItemText(nItemIndex,2,strGroupSize);

			CString strAvgQueue;
			strAvgQueue.Format(_T("%.2f"),dataList[i][j].fAvgQueue);
			GetListCtrl().SetItemText(nItemIndex,3,strAvgQueue);

			CString strMaxQueue;
			strMaxQueue.Format(_T("%d"),dataList[i][j].nMaxQueue);
			GetListCtrl().SetItemText(nItemIndex,4,strMaxQueue);


			GetListCtrl().SetItemText(nItemIndex,5,dataList[i][j].eMaxQueueTime.printTime());
			
			GetListCtrl().SetItemData(nItemIndex,nItemIndex);
			nItemIndex+=1;
		}

	}


}
void CMultiRunRepListView::SetDetailedPaxDataContents(MultiRunsReport::Detail::PaxDataList& , std::vector<int>& vSimResults)
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Detail::PaxDataList& dataList = multiReport.GetReportResult_PaxDataList();
	if (dataList.empty())
		return;

	CListCtrl& wndListCtrl = GetListCtrl();
	wndListCtrl.DeleteAllItems();

	int nCount = m_ctlHeaderCtrl.GetItemCount();
	for (int i = 0;i < nCount; i++)
		wndListCtrl.DeleteColumn(0);

	wndListCtrl.InsertColumn(0, _T("SimResult"), LVCFMT_LEFT, 80);
	if (dataList.empty())
		return;

	size_t rangeCount = dataList[0].size();
	for (size_t i = 0; i < rangeCount; ++i)
	{
		CString strRange;
		strRange.Format(_T("%d - %d"), dataList[0][i].ValueRange.first, dataList[0][i].ValueRange.second);

		wndListCtrl.InsertColumn(1 + i, strRange, LVCFMT_LEFT, 100);
	}

	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	for (size_t i = 0; i < vSimResultFullName.size(); ++i)
	{
		//CString strSimName;
		//strSimName.Format(_T("SimResult%d"), vSimResults[i]);
		wndListCtrl.InsertItem(i, vSimResultFullName[i]);
		GetListCtrl().SetItemData(i,i);
		for (size_t n = 0; n < rangeCount; ++n)
		{
			CString strCount;
			strCount.Format(_T("%d"), dataList[i][n].nCount);

			wndListCtrl.SetItemText(i, n + 1, strCount);

		}
		

	}
}

void CMultiRunRepListView::SetDetailedTimeDataContents(MultiRunsReport::Detail::TimeDataList& , int _nRangeCount )
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Detail::TimeDataList& dataList = multiReport.GetReportResult_TimeDataList();
	if (dataList.empty())
		return;


	CListCtrl& wndListCtrl = GetListCtrl();
	wndListCtrl.DeleteAllItems();

	int nCount = m_ctlHeaderCtrl.GetItemCount();
	for (int i = 0;i < nCount; i++)
		wndListCtrl.DeleteColumn(0);

	wndListCtrl.InsertColumn(0, _T("SimResult"), LVCFMT_LEFT, 80);
	if (dataList.empty())
		return;
	_nRangeCount = static_cast<int>(dataList[0].size());
	for ( int i = 0; i < _nRangeCount; ++i)
	{
		CString strRange;
		bool bInSecond = dataList[0][i].bInSecond;
		strRange = dataList[0][i].TimeValueRange.first.printTime( bInSecond ? 1 : 0 ) + " - " + dataList[0][i].TimeValueRange.second.printTime( bInSecond ? 1 : 0 );

		wndListCtrl.InsertColumn(1 + i, strRange, LVCFMT_LEFT, 100);
	}
	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	for (size_t i = 0; i < vSimResultFullName.size(); ++i)
	{
		//CString strSimName;
		//strSimName.Format(_T("Run%d"), i+1 );
		wndListCtrl.InsertItem(i, vSimResultFullName[i]);
		GetListCtrl().SetItemData(i,i);
		for ( int n = 0; n < _nRangeCount; ++n)
		{
			CString strCount;
			strCount.Format(_T("%d"), dataList[i][n].nCount);

			wndListCtrl.SetItemText(i, n + 1, strCount);
		}
	}
}

void CMultiRunRepListView::SetSummaryPaxDataContents(MultiRunsReport::Summary::PaxDataList& dataList, std::vector<int>& vSimResults)
{
	CListCtrl& wndListCtrl = GetListCtrl();
	wndListCtrl.DeleteAllItems();

	int nCount = m_ctlHeaderCtrl.GetItemCount();
	for (int i = 0;i < nCount; i++)
		wndListCtrl.DeleteColumn(0);

	wndListCtrl.InsertColumn(0, _T("Passenger Type"), LVCFMT_LEFT, 80);
	for (size_t i = 0; i < vSimResults.size(); ++i)
	{
		CString strSimName;
		strSimName.Format(_T("SimResult%d"), vSimResults[i]);

		wndListCtrl.InsertColumn(1 + i * 3, strSimName + _T(" Min Value"), LVCFMT_LEFT, 100);
		wndListCtrl.InsertColumn(2 + i * 3, strSimName + _T(" Average Value"), LVCFMT_LEFT, 100);
		wndListCtrl.InsertColumn(3 + i * 3, strSimName + _T(" Max Value"), LVCFMT_LEFT, 100);
	}

	int nRow = 0;
	for (MultiRunsReport::Summary::PaxDataList::iterator iter = dataList.begin(); iter != dataList.end(); ++iter)
	{
		wndListCtrl.InsertItem(nRow, iter->first);
		GetListCtrl().SetItemData(nRow,nRow);
		for (size_t i = 0; i < vSimResults.size(); ++i)
		{
			CString strValue;

			strValue.Format(_T("%.2f"), iter->second[i].fMinValue);
			wndListCtrl.SetItemText(nRow, 1 + i * 3, strValue);

			strValue.Format(_T("%.2f"), iter->second[i].fAveValue);
			wndListCtrl.SetItemText(nRow, 2 + i * 3, strValue);

			strValue.Format(_T("%.2f"), iter->second[i].fMaxValue);
			wndListCtrl.SetItemText(nRow, 3 + i * 3, strValue);
		}

		nRow++;
	}
}
void CMultiRunRepListView::SetDetailOccupancyDataWithNoRanger(MultiRunsReport::Detail::OccupnacyDataList& , int _nRangeCount)
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Detail::OccupnacyDataList& dataList = multiReport.GetReportResult_OccupnacyDataList();
	if (dataList.empty())
		return;

	CListCtrl& wndListCtrl = GetListCtrl();
	wndListCtrl.DeleteAllItems();

	int nCount = m_ctlHeaderCtrl.GetItemCount();
	for (int i = 0;i < nCount; i++)
		wndListCtrl.DeleteColumn(0);

	wndListCtrl.InsertColumn(0, _T("SimResult"), LVCFMT_LEFT, 80);
	if (dataList.empty())
		return;
	_nRangeCount = static_cast<int>(dataList[0].size());

	for ( int i = 0; i < _nRangeCount; ++i)
	{
		CString strRange;
		bool bInSecond = false;//dataList[0][i].bInSecond;
		strRange = dataList[0][i].startTime.printTime( bInSecond ? 1 : 0 ) ;

		wndListCtrl.InsertColumn(1 + i, strRange, LVCFMT_LEFT, 100);
	}
	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	for (size_t i = 0; i < vSimResultFullName.size(); ++i)
	{
		//CString strSimName;
		//strSimName.Format(_T("Run%d"), i+1 );
		wndListCtrl.InsertItem(i, vSimResultFullName[i]);
		GetListCtrl().SetItemData(i,i);
		for ( int n = 0; n < _nRangeCount-1; ++n)
		{
			CString strCount;
			strCount.Format(_T("%d"), dataList[i][n].nPaxCount);

			wndListCtrl.SetItemText(i, n + 1, strCount);
		}
	}
}
void CMultiRunRepListView::SetDetailedOccupancyDataContents(MultiRunsReport::Detail::OccupnacyDataList& , int _nRangeCount )
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Detail::OccupnacyDataList& dataList = multiReport.GetReportResult_OccupnacyDataList();
	if (dataList.empty())
		return;

	CListCtrl& wndListCtrl = GetListCtrl();
	wndListCtrl.DeleteAllItems();

	int nCount = m_ctlHeaderCtrl.GetItemCount();
	for (int i = 0;i < nCount; i++)
		wndListCtrl.DeleteColumn(0);

	wndListCtrl.InsertColumn(0, _T("SimResult"), LVCFMT_LEFT, 80);
	if (dataList.empty())
		return;
	_nRangeCount = static_cast<int>(dataList[0].size());

	for ( int i = 0; i < _nRangeCount-1; ++i)
	{
		CString strRange;
		bool bInSecond = false;//dataList[0][i].bInSecond;
		strRange = dataList[0][i].startTime.printTime( bInSecond ? 1 : 0 ) + " - " + dataList[0][i+1].startTime.printTime( bInSecond ? 1 : 0 );

		wndListCtrl.InsertColumn(1 + i, strRange, LVCFMT_LEFT, 100);
	}
	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	for (size_t i = 0; i < vSimResultFullName.size(); ++i)
	{
		//CString strSimName;
		//strSimName.Format(_T("Run%d"), i+1 );
		wndListCtrl.InsertItem(i, vSimResultFullName[i]);
		GetListCtrl().SetItemData(i,i);
		for ( int n = 0; n < _nRangeCount-1; ++n)
		{
			CString strCount;
			strCount.Format(_T("%d"), dataList[i][n].nPaxCount);

			wndListCtrl.SetItemText(i, n + 1, strCount);
		}
	}
}
