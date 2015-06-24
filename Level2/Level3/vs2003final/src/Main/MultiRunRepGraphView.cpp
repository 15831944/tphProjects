// MultiRunRepGraphView.cpp : implementation file
//
#include "stdafx.h"
#include "MultiRunRepGraphView.h"
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
#include "ViewMsg.h"

// CMultiRunRepGraphView

IMPLEMENT_DYNCREATE(CMultiRunRepGraphView, CFormView)

CMultiRunRepGraphView::CMultiRunRepGraphView()
	: CFormView(CMultiRunRepGraphView::IDD)
{
}

CMultiRunRepGraphView::~CMultiRunRepGraphView()
{
}

void CMultiRunRepGraphView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHART_TYPE_COMBO, m_comboChartType);
	DDX_Control(pDX, IDC_CHART_SELECT_COMBO, m_comboChartSelect);
	DDX_Control(pDX, IDC_MSCHART1, m_MSChartCtrl);
}

BEGIN_MESSAGE_MAP(CMultiRunRepGraphView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_SEL_COLOR_BTN, OnSelColorBtn)
	ON_CBN_SELCHANGE(IDC_CHART_TYPE_COMBO, OnSelchangeChartTypeCombo)
	ON_CBN_SELCHANGE(IDC_CHART_SELECT_COMBO, OnSelchangeChartSelectCombo)
	ON_BN_CLICKED(IDC_PRINT_BTN, OnPrintBtn)
END_MESSAGE_MAP()

// CMultiRunRepGraphView message handlers
int CMultiRunRepGraphView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC)
		|| !m_wndToolbar.LoadToolBar(IDR_REPORTTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}

	return 0;
}

void CMultiRunRepGraphView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// set the position of the toolbar
	CRect rc;
	CWnd* pToolbarPos = GetDlgItem(IDC_STATIC_TOOLBARCONTENTER);
	pToolbarPos->GetWindowRect(&rc);
	ScreenToClient(&rc);
	pToolbarPos->ShowWindow(SW_HIDE);
	m_wndToolbar.MoveWindow(2,-2,90,rc.Height());
	m_wndToolbar.ShowWindow(SW_SHOW);

	// fill Chart Type Combo box
	m_comboChartType.AddString("3D Bar");
	m_comboChartType.AddString("2D Bar");
	m_comboChartType.AddString("2D Pie");
	m_comboChartType.AddString("3D Line");
	m_comboChartType.AddString("2D Line");
	m_comboChartType.AddString("3D Area");
	m_comboChartType.AddString("2D Area");
	m_comboChartType.AddString("3D Step");
	m_comboChartType.AddString("2D Step");
	m_comboChartType.AddString("3D Combination");
	m_comboChartType.AddString("2D Combination");

	m_MSChartCtrl.ShowWindow(SW_HIDE);
}


void CMultiRunRepGraphView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if (IsWindowVisible())
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		CRect rcChart;
		rcChart.right  = rcClient.right - 10;
		rcChart.left   = rcClient.left+10;
		rcChart.top    = rcClient.top+50;
		rcChart.bottom = rcClient.bottom;

		m_MSChartCtrl.MoveWindow(rcChart);
	}
}

void CMultiRunRepGraphView::OnSelColorBtn() 
{
	USES_CONVERSION;

	CWnd* pChart = GetDlgItem(IDC_MSCHART1);

	// Get Property Page CLSID's
	ISpecifyPropertyPages* pSpecify = NULL;
	IUnknown* pUnk = pChart->GetControlUnknown();
	if (FAILED(pUnk->QueryInterface(IID_ISpecifyPropertyPages,(void**)&pSpecify)))	
		return;

	CAUUID pages;
	pages.cElems = 0;
	pages.pElems = NULL;
	if (FAILED(pSpecify->GetPages(&pages)))
	{
		pSpecify->Release();
		return;
	}
	// display the control's property pages.        
	OleCreatePropertyFrame(m_hWnd, 0, 0, T2OLE("MSChart"), 1, &pUnk, pages.cElems,
							pages.pElems, GetUserDefaultLCID(), 0, NULL);
	// clean up
	pSpecify->Release();
	CoTaskMemFree(pages.pElems);
}

void CMultiRunRepGraphView::OnSelchangeChartTypeCombo() 
{
	int nCurSel = m_comboChartType.GetCurSel();
	if(nCurSel<0)
		return;

	switch(nCurSel)
	{
	case 0:  //3D Bar
		m_MSChartCtrl.SetChartType(0);
		break;
	case 1://2D Bar
		m_MSChartCtrl.SetChartType(1);
		break;
	case 2://2D Pie
		m_MSChartCtrl.SetChartType(14);
		break;
	case 3://3D Line
		m_MSChartCtrl.SetChartType(2);
		break;
	case 4://2D Line
		m_MSChartCtrl.SetChartType(3);
		break;
	case 5://3D Area
		m_MSChartCtrl.SetChartType(4);
		break;
	case 6://2D Area
		m_MSChartCtrl.SetChartType(5);
		break;
	case 7://3D Step
		m_MSChartCtrl.SetChartType(6);
		break;
	case 8://2D Step
		m_MSChartCtrl.SetChartType(7);
		break;
	case 9://3D Combination
		m_MSChartCtrl.SetChartType(8);
		break;
	case 10://2D Combination
		m_MSChartCtrl.SetChartType(9);
		break;
	}

	m_MSChartCtrl.Refresh();
}

void CMultiRunRepGraphView::OnSelchangeChartSelectCombo() 
{
	m_MSChartCtrl.SetShowLegend(FALSE);
	int nCurSel = m_comboChartSelect.GetCurSel();
	if(nCurSel<0)
		return;

	CString strSelect;
	m_comboChartSelect.GetLBText(nCurSel,strSelect);
	if (strSelect.IsEmpty())
		return;

	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();
	std::vector<int>& vSimResults = pDoc->GetARCReportManager().GetmSelectedMultiRunsSimResults();

	ENUM_REPORT_TYPE reportType = pDoc->GetARCReportManager().GetMultiRunsReportType();


	CReportParameter* pReportParam = pDoc->GetTerminal().m_pReportParaDB->GetParameterbyType(reportType);
	if (pReportParam == NULL)
		return;

	//int iDetailed;
	//pReportParam->GetReportType(iDetailed);
	if (reportType == ENUM_QUEUETIME_REP)
	{
		pDoc->UpdateAllViews(NULL,MULTIREPORT_SHOWREPORT,(CObject *)&strSelect);
	}
	if (reportType == ENUM_AVGQUEUELENGTH_REP)
	{
		int nCurSel = m_comboChartSelect.GetCurSel();
		pDoc->UpdateAllViews(NULL,MULTIREPORT_SHOWREPORT,(CObject*)nCurSel);
	}
	else if(reportType == ENUM_SERVICETIME_REP)
	{
		pDoc->UpdateAllViews(NULL,MULTIREPORT_SHOWREPORT,(CObject *)&strSelect);
	}


	m_MSChartCtrl.Refresh();
}

void CMultiRunRepGraphView::OnPrintBtn() 
{
	CDC dc;
	CPrintDialog printDlg(FALSE);
	if(printDlg.DoModal() == IDCANCEL )
		return;

	dc.Attach( printDlg.GetPrinterDC() );
	dc.m_bPrinting = TRUE;
	CString sTitle;
	sTitle = "Flight Schedule";
	DOCINFO di;
	::ZeroMemory( &di, sizeof(DOCINFO) );
	di.cbSize = sizeof( DOCINFO );
	di.lpszDocName = sTitle;

	BOOL bPrintingOK = dc.StartDoc( &di );

	CPrintInfo info;
	OnBeginPrinting( &dc, &info );

	LPDEVMODE lp = (LPDEVMODE) ::GlobalLock(printDlg.m_pd.hDevMode);
	ASSERT(lp);

	m_rclBounds.left = 300;
	m_rclBounds.top = 300;
	m_rclBounds.right = dc.GetDeviceCaps( HORZRES ) -300;
	m_rclBounds.bottom = dc.GetDeviceCaps( VERTRES ) -300;

	::GlobalUnlock(printDlg.m_pd.hDevMode);
	OnDraw(&dc);
	bPrintingOK = TRUE;

	OnEndPrinting( &dc, &info );

	if( bPrintingOK )
		dc.EndDoc();
	else
		dc.AbortDoc();
	dc.Detach();
}

void CMultiRunRepGraphView::OnDraw(CDC* pDC) 
{
	if (!pDC->IsPrinting())
		return;

	IViewObject* pViewObj = NULL;
	if (SUCCEEDED(m_MSChartCtrl.GetControlUnknown()->QueryInterface(IID_IViewObject,(void**)&pViewObj)))
	{
		RECT rect;
		m_MSChartCtrl.GetClientRect(&rect);

		RECTL rclWBounds;
		CPoint ptOrg = pDC->GetWindowOrg();
		CSize sizeExt(pDC->GetDeviceCaps( HORZRES ),pDC->GetDeviceCaps( VERTRES )); //= pDC->GetWindowExt();
		rclWBounds.left = 100;//ptOrg.x;
		rclWBounds.top = 100;//ptOrg.y;
		rclWBounds.right = sizeExt.cx;//100;
		rclWBounds.bottom = sizeExt.cy;//100;

		pViewObj->Draw(DVASPECT_TRANSPARENT,-1, NULL, NULL, pDC->m_hAttribDC, pDC->m_hDC, &m_rclBounds,&rclWBounds,NULL,NULL);
		pViewObj->Release();
	}
}

void CMultiRunRepGraphView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* pHint)
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
					SetHeadXYTitle( "Distance Traveled", "Distance(m)", "Number of Passenger" );
					MultiRunsReport::Detail::PaxDataList paxDataList;

					SetDetailedPaxDataContents(paxDataList, vSimResults);
				}

				break;
			case ENUM_QUEUETIME_REP:
				{
					SetHeadXYTitle( "Passenger Time in Queue", "Queue Time(hh:mm:ss)", "Number of Passenger" );
					MultiRunsReport::Detail::TimeDataList timeDataList;


					SetDetailedTimeDataContents(timeDataList, vSimResults);
				}
				break;
			case ENUM_SERVICETIME_REP:
				{
					SetHeadXYTitle( "Passenger Time in Service", "Service Time(hh:mm:ss)", "Number of Passenger" );
					MultiRunsReport::Detail::TimeDataList timeDataList;


					SetDetailedTimeDataContents(timeDataList, vSimResults);
				}
				break;
			case ENUM_DURATION_REP:
				{
					SetHeadXYTitle( "Passenger Time in Terminal", "Duration(hh:mm)", "Number of Passenger" );
					MultiRunsReport::Detail::TimeDataList timeDataList;



					SetDetailedTimeDataContents(timeDataList, vSimResults);
				}

				break;
			case ENUM_ACTIVEBKDOWN_REP:
				break;
			case ENUM_PAXCOUNT_REP:
				{
					SetHeadXYTitle( "Passenger Count in Terminal", "Time(hh:mm)", "Number of Passenger" );
					MultiRunsReport::Detail::OccupnacyDataList timeDataList;

					SetDetailOccupancyTimeDataContents(timeDataList, vSimResults);
				}
				break;
			case ENUM_PAXDENS_REP:
				{
					SetHeadXYTitle("Space Density","Time(hh:mm)","Passengers in Area");
					MultiRunsReport::Detail::TimeDataList timeDataList;


					SetDetailedTimeDataContents(timeDataList, vSimResults);
				}
				break;
			default:
				break;
			}
		}
		else // summary
		{
			switch(reportType)
			{
			case ENUM_QUEUETIME_REP:
				{
					SetHeadXYTitle("Queue Time (Summary)","Passenger Type","Minutes in Queues");
					CString strPaxType;
					if (pHint == NULL)
					{
					
						CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
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
					footnote.SetText(strPaxType);
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
						footnote.SetText("Queue Length(Summary)");
					}
					else if (nCursel == 1)
					{
						SetSummaryCriticalQueueMaxQueueContents();
						CVcFootnote footnote(m_MSChartCtrl.GetFootnote());
						footnote.SetText("Time of Max Queue(Summary)");

					}
					else if (nCursel == 2)
					{
						SetSummaryCriticalQueueMaxQueueTimeContents();
						CVcFootnote footnote(m_MSChartCtrl.GetFootnote());
						footnote.SetText("Max Queue Time Histogram(Summary)");
					}

				}
				break;
			case ENUM_SERVICETIME_REP:
				{
					SetHeadXYTitle("Time in Service (Summary)","Passenger Type","Minutes in Service");
					CString strPaxType;
					if (pHint == NULL)
					{

						CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
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
					footnote.SetText(strPaxType);
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
	//		case ENUM_DISTANCE_REP:

	//			{
	//				SetHeadXYTitle( "Distance Traveled", "Distance(m)", "Number of Passenger" );
	//				MultiRunsReport::Detail::PaxDataList paxDataList;

	//				size_t nSimResultCount = vSimResults.size();
	//				int nRangeCount = 0;
	//				for (size_t i = 0; i < nSimResultCount; ++i)
	//				{
	//					pReportManager->SetCurrentSimResult(vSimResults[i]);
	//					pReportManager->SetCurrentReportType(reportType);
	//					CString strReportFileName = pReportManager->GetCurrentReportFileName(pDoc->m_ProjInfo.path);
	//					MultiRunsReport::Detail::DistTravelledDataLoader loader(strReportFileName);
	//					loader.LoadData();
	//					MultiRunsReport::Detail::DistTravelledDataLoader::DataList& dataList = loader.GetData();
	//					paxDataList.push_back(dataList);
	//				}

	//				SetDetailedPaxDataContents(paxDataList, vSimResults);
	//			}

	//			break;
	//		case ENUM_QUEUETIME_REP:
	//			{
	//				SetHeadXYTitle( "Passenger Time in Queue", "Queue Time(hh:mm:ss)", "Number of Passenger" );
	//				MultiRunsReport::Detail::TimeDataList timeDataList;

	//				size_t nSimResultCount = vSimResults.size();
	//				int nRangeCount = 0;
	//				for (size_t i = 0; i < nSimResultCount; ++i)
	//				{
	//					pReportManager->SetCurrentSimResult(vSimResults[i]);
	//					pReportManager->SetCurrentReportType(reportType);
	//					CString strReportFileName = pReportManager->GetCurrentReportFileName(pDoc->m_ProjInfo.path);
	//					MultiRunsReport::Detail::TimeInTerminalDataLoader loader(strReportFileName);
	//					loader.LoadData();
	//					MultiRunsReport::Detail::TimeInTerminalDataLoader::DataList& dataList = loader.GetData();
	//					timeDataList.push_back(dataList);
	//					nRangeCount = dataList.size();
	//				}

	//				SetDetailedTimeDataContents(timeDataList, vSimResults);
	//			}
	//			break;
	//		case ENUM_SERVICETIME_REP:
	//			{
	//				SetHeadXYTitle( "Passenger Time in Service", "Service Time(hh:mm:ss)", "Number of Passenger" );
	//				MultiRunsReport::Detail::TimeDataList timeDataList;

	//				size_t nSimResultCount = vSimResults.size();
	//				int nRangeCount = 0;
	//				for (size_t i = 0; i < nSimResultCount; ++i)
	//				{
	//					pReportManager->SetCurrentSimResult(vSimResults[i]);
	//					pReportManager->SetCurrentReportType(reportType);
	//					CString strReportFileName = pReportManager->GetCurrentReportFileName(pDoc->m_ProjInfo.path);
	//					MultiRunsReport::Detail::TimeInTerminalDataLoader loader(strReportFileName);
	//					loader.LoadData();
	//					MultiRunsReport::Detail::TimeInTerminalDataLoader::DataList& dataList = loader.GetData();
	//					timeDataList.push_back(dataList);
	//					nRangeCount = dataList.size();
	//				}

	//				SetDetailedTimeDataContents(timeDataList, vSimResults);
	//			}
	//			break;
	//		case ENUM_DURATION_REP:
	//			{
	//				SetHeadXYTitle( "Passenger Time in Terminal", "Duration(hh:mm)", "Number of Passenger" );
	//				MultiRunsReport::Detail::TimeDataList timeDataList;

	//				size_t nSimResultCount = vSimResults.size();
	//				int nRangeCount = 0;
	//				for (size_t i = 0; i < nSimResultCount; ++i)
	//				{
	//					pReportManager->SetCurrentSimResult(vSimResults[i]);
	//					pReportManager->SetCurrentReportType(reportType);
	//					CString strReportFileName = pReportManager->GetCurrentReportFileName(pDoc->m_ProjInfo.path);
	//					MultiRunsReport::Detail::TimeInTerminalDataLoader loader(strReportFileName);
	//					loader.LoadData();
	//					MultiRunsReport::Detail::TimeInTerminalDataLoader::DataList& dataList = loader.GetData();
	//					timeDataList.push_back(dataList);
	//					nRangeCount = dataList.size();
	//				}

	//				SetDetailedTimeDataContents(timeDataList, vSimResults);
	//			}

	//			break;
	//		case ENUM_ACTIVEBKDOWN_REP:
	//			break;
	//		case ENUM_PAXCOUNT_REP:
	//			{
	//				SetHeadXYTitle( "Passenger Count in Terminal", "Time(hh:mm)", "Number of Passenger" );
	//				MultiRunsReport::Detail::OccupnacyDataList timeDataList;

	//				size_t nSimResultCount = vSimResults.size();
	//				int nRangeCount = 0;
	//				for (size_t i = 0; i < nSimResultCount; ++i)
	//				{
	//					pReportManager->SetCurrentSimResult(vSimResults[i]);
	//					pReportManager->SetCurrentReportType(reportType);
	//					CString strReportFileName = pReportManager->GetCurrentReportFileName(pDoc->m_ProjInfo.path);
	//					MultiRunsReport::Detail::OccupancyDataLoader loader(strReportFileName);
	//					loader.LoadData();
	//					MultiRunsReport::Detail::OccupancyDataLoader::DataList & dataList = loader.GetData();
	//					timeDataList.push_back(dataList);
	//					nRangeCount = dataList.size();
	//				}

	//				SetDetailOccupancyTimeDataContents(timeDataList, vSimResults);
	//			}
	//			break;
	//		default:
	//			break;
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

	//				//SetHeadXYTitle(loader.GetTitle(), loader.GetXAxisTitle(), loader.GetYAxisTitle(), loader.GetFootnote());
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
void CMultiRunRepGraphView::SetSummaryQueueTimeContents(const CString& strPaxType)
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Summary::SummaryQTimeList& dataList = multiReport.GetReportResult_SummaryQueueTime();
	if (dataList.empty())
		return;

	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	vSimResultFullName.insert(vSimResultFullName.begin(),_T("Average Values of Project(s)"));
	size_t nResultSize = dataList.size();

	m_MSChartCtrl.ShowWindow(SW_SHOW);

	//column
	m_MSChartCtrl.SetRowCount(13);
	m_MSChartCtrl.SetColumnCount(nResultSize);
	m_MSChartCtrl.SetColumnLabelCount(nResultSize);
	for (size_t nCol = 0; nCol < nResultSize; ++nCol )
	{
		m_MSChartCtrl.SetColumn(nCol+1);
		m_MSChartCtrl.SetColumnLabel(vSimResultFullName[nCol]);
	}


	CString strLabel;
	//m_MSChartCtrl.SetRowCount(nResultSize);
	//m_MSChartCtrl.SetColumnCount(13);
	//m_MSChartCtrl.SetColumnLabelCount(13);
	m_MSChartCtrl.SetRowCount(13);
	m_MSChartCtrl.SetRow(1);
	m_MSChartCtrl.SetRowLabel("Minimum");
	m_MSChartCtrl.SetRow(2);
	m_MSChartCtrl.SetRowLabel("Average");
	m_MSChartCtrl.SetRow(3);
	m_MSChartCtrl.SetRowLabel("Maximum");

	m_MSChartCtrl.SetRow(4);
	m_MSChartCtrl.SetRowLabel("Q1");

	m_MSChartCtrl.SetRow(5);
	m_MSChartCtrl.SetRowLabel("Q2");

	m_MSChartCtrl.SetRow(6);
	m_MSChartCtrl.SetRowLabel("Q3");

	m_MSChartCtrl.SetRow(7);
	m_MSChartCtrl.SetRowLabel("P1");

	m_MSChartCtrl.SetRow(8);
	m_MSChartCtrl.SetRowLabel("P5");

	m_MSChartCtrl.SetRow(9);
	m_MSChartCtrl.SetRowLabel("P10");

	m_MSChartCtrl.SetRow(10);
	m_MSChartCtrl.SetRowLabel("P90");

	m_MSChartCtrl.SetRow(11);
	m_MSChartCtrl.SetRowLabel("P95");

	m_MSChartCtrl.SetRow(12);
	m_MSChartCtrl.SetRowLabel("P99");

	m_MSChartCtrl.SetRow(13);
	m_MSChartCtrl.SetRowLabel("Sigma");


	
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
		m_MSChartCtrl.SetRow(1);
		//DealRowLabel(strxValue[i]);
		m_MSChartCtrl.SetColumn(i);
		//m_MSChartCtrl.SetRowLabel(vSimResultFullName[i-1]);
		strLabel.Format("%f",minvalue[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(2);
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",avevalue[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(3);	
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",maxvalue[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(4);	
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",Q1value[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(5);	
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f", Q2value[i-1] );
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(6);	
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",Q3value[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(7);
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",P1value[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(8);
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",P5value[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(9);
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",P10value[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(10);
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",P90value[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(11);
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",P95value[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(12);
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",P99value[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(13);
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",Sigmavalue[i-1]);
		m_MSChartCtrl.SetData(strLabel);
	}
	m_MSChartCtrl.SetShowLegend(TRUE);
}

void CMultiRunRepGraphView::SetSummaryTimeServiceContents(const CString& strPaxType)
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Summary::SummaryTimeServiceList& dataList = multiReport.GetReportResult_SummaryTimeService();
	if (dataList.empty())
		return;

	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	vSimResultFullName.insert(vSimResultFullName.begin(),_T("Average Values of Project(s)"));
	size_t nResultSize = dataList.size();

	m_MSChartCtrl.ShowWindow(SW_SHOW);

	//column
	m_MSChartCtrl.SetRowCount(13);
	m_MSChartCtrl.SetColumnCount(nResultSize);
	m_MSChartCtrl.SetColumnLabelCount(nResultSize);
	for (size_t nCol = 0; nCol < nResultSize; ++nCol )
	{
		m_MSChartCtrl.SetColumn(nCol+1);
		m_MSChartCtrl.SetColumnLabel(vSimResultFullName[nCol]);
	}


	CString strLabel;
	//m_MSChartCtrl.SetRowCount(nResultSize);
	//m_MSChartCtrl.SetColumnCount(13);
	//m_MSChartCtrl.SetColumnLabelCount(13);
	m_MSChartCtrl.SetRowCount(13);
	m_MSChartCtrl.SetRow(1);
	m_MSChartCtrl.SetRowLabel("Minimum");
	m_MSChartCtrl.SetRow(2);
	m_MSChartCtrl.SetRowLabel("Average");
	m_MSChartCtrl.SetRow(3);
	m_MSChartCtrl.SetRowLabel("Maximum");

	m_MSChartCtrl.SetRow(4);
	m_MSChartCtrl.SetRowLabel("Q1");

	m_MSChartCtrl.SetRow(5);
	m_MSChartCtrl.SetRowLabel("Q2");

	m_MSChartCtrl.SetRow(6);
	m_MSChartCtrl.SetRowLabel("Q3");

	m_MSChartCtrl.SetRow(7);
	m_MSChartCtrl.SetRowLabel("P1");

	m_MSChartCtrl.SetRow(8);
	m_MSChartCtrl.SetRowLabel("P5");

	m_MSChartCtrl.SetRow(9);
	m_MSChartCtrl.SetRowLabel("P10");

	m_MSChartCtrl.SetRow(10);
	m_MSChartCtrl.SetRowLabel("P90");

	m_MSChartCtrl.SetRow(11);
	m_MSChartCtrl.SetRowLabel("P95");

	m_MSChartCtrl.SetRow(12);
	m_MSChartCtrl.SetRowLabel("P99");

	m_MSChartCtrl.SetRow(13);
	m_MSChartCtrl.SetRowLabel("Sigma");


	
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
		m_MSChartCtrl.SetRow(1);
		//DealRowLabel(strxValue[i]);
		m_MSChartCtrl.SetColumn(i);
		//m_MSChartCtrl.SetRowLabel(vSimResultFullName[i-1]);
		strLabel.Format("%f",minvalue[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(2);
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",avevalue[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(3);	
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",maxvalue[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(4);	
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",Q1value[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(5);	
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f", Q2value[i-1] );
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(6);	
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",Q3value[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(7);
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",P1value[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(8);
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",P5value[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(9);
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",P10value[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(10);
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",P90value[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(11);
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",P95value[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(12);
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",P99value[i-1]);
		m_MSChartCtrl.SetData(strLabel);

		m_MSChartCtrl.SetRow(13);
		m_MSChartCtrl.SetColumn(i);
		strLabel.Format("%f",Sigmavalue[i-1]);
		m_MSChartCtrl.SetData(strLabel);
	}
	m_MSChartCtrl.SetShowLegend(TRUE);
}
void CMultiRunRepGraphView::SetSummaryCriticalQueueLenContents()
{
	SetHeadXYTitle("Queue Length(Summary)","Processor","Average Queue Length");
	
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Summary::SummaryCriticalQueueList& dataList = multiReport.GetReportResult_SummaryCriticalQueue();
	if (dataList.empty())
		return;

	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	vSimResultFullName.insert(vSimResultFullName.begin(),_T("Average Values of Project(s)"));
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
void CMultiRunRepGraphView::SetSummaryCriticalQueueMaxQueueContents()
{
	SetHeadXYTitle("Time of Max Queue(Summary)","Processor","Time of Max Queue(count)");

	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Summary::SummaryCriticalQueueList& dataList = multiReport.GetReportResult_SummaryCriticalQueue();
	if (dataList.empty())
		return;

	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	vSimResultFullName.insert(vSimResultFullName.begin(),_T("Average Values of Project(s)"));
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
void CMultiRunRepGraphView::SetSummaryCriticalQueueMaxQueueTimeContents()
{	
	SetHeadXYTitle("Time of Max Queue(Summary)","Processor","Time of Max Queue(Minutes)");

	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Summary::SummaryCriticalQueueList& dataList = multiReport.GetReportResult_SummaryCriticalQueue();
	if (dataList.empty())
	return;

	std::vector<CString> vSimResultFullName = multiReport.GetAllSimResultFullName();
	vSimResultFullName.insert(vSimResultFullName.begin(),_T("Average Values of Project(s)"));
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
void CMultiRunRepGraphView::SetHeadXYTitle(CString strTitle, CString strXAxisTitle, CString strYAxisTitle)
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

void CMultiRunRepGraphView::SetDetailedPaxDataContents(MultiRunsReport::Detail::PaxDataList& , std::vector<int>& vSimResults)
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Detail::PaxDataList& dataList = multiReport.GetReportResult_PaxDataList();
	if (dataList.empty())
		return;

	m_MSChartCtrl.ShowWindow(SW_SHOW);

	CString strLabel;
	int nRowCount = dataList[0].size();
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

	for (size_t i =1;i <= dataList[0].size(); ++i)
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
			strValue.Format(_T("%d"), iter->at(i-1).nCount);
			m_MSChartCtrl.SetData(strValue);
			nCol++;
		}

	}

}

void CMultiRunRepGraphView::SetDetailedTimeDataContents(MultiRunsReport::Detail::TimeDataList&, std::vector<int>& vSimResults )
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Detail::TimeDataList& dataList = multiReport.GetReportResult_TimeDataList();
	if (dataList.empty())
		return;

	m_MSChartCtrl.ShowWindow(SW_SHOW);


	int nSimResultCount = dataList.size();
	MultiRunsReport::Detail::TimeDataList::const_iterator it = dataList.begin();
	CString strLabel;
	int nRangeCount = it->size();
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
			strValue.Format(_T("%d"), iter->at(i-1).nCount);
			m_MSChartCtrl.SetData(strValue);
			nCol++;
		}

	}
}

void CMultiRunRepGraphView::SetSummaryPaxDataContents(MultiRunsReport::Summary::PaxDataList& dataList, std::vector<int>& vSimResults)
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

void CMultiRunRepGraphView::SetDetailOccupancyTimeDataContents(MultiRunsReport::Detail::OccupnacyDataList& , std::vector<int>& vSimResults)
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

	const MultiRunsReport::Detail::OccupnacyDataList& dataList = multiReport.GetReportResult_OccupnacyDataList();
	if (dataList.empty())
		return;

	m_MSChartCtrl.ShowWindow(SW_SHOW);


	int nSimResultCount = dataList.size();
	MultiRunsReport::Detail::OccupnacyDataList::const_iterator it = dataList.begin();
	CString strLabel;
	int nRangeCount = it->size();
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
			strValue.Format(_T("%d"), iter->at(i-1).nPaxCount);
			m_MSChartCtrl.SetData(strValue);
			nCol++;
		}

	}
}
