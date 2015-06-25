// AirsideReportGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "AirsideReportGraphView.h"
#include ".\airsidereportgraphview.h"
#include "TermPlanDoc.h"
#include "../AirsideReport/TypeDefs.h"
#include "../AirsideReport/ReportManager.h"
#include "../AirsideReport/AirsideFlightDelayReport.h"
#include "../AirsideReport/AirsideFlightDelayParam.h"
#include "../AirsideReport/AirsideFlightDelayBaseResult.h"
#include "../AirsideReport/AirsideNodeDelayReportParameter.h"
#include "../AirsideReport/AirsideNodeDelayReport.h"
#include "../AirsideReport/AirsideDistanceTravelParam.h"
#include "../AirsideReport/AirsideDistanceTravelReport.h"
#include "../AirsideReport/AirsideOperationReport.h"
#include "../AirsideReport/AirsideOperationParam.h"
#include "../AirsideReport/AirsideOperationResult.h"
#include "../AirsideReport/FlightActivityReport.h"
#include "../AirsideReport/FlightActivityParam.h"
#include "../AirsideReport/AirsideFlightActivityReportBaseResult.h"
#include "../AirsideReport/FlightOperationalParam.h"
#include "../AirsideReport/FlightOperationalReport.h"
#include "../AirsideReport/AirsideVehicleOperaterReport.h"
#include "../AirsideReport/AirsideVehicleOperParameter.h"
#include "ViewMsg.h"
#include "../AirsideReport/AirsideRunwayOperationReportParam.h"
#include "../AirsideReport/AirsideRunwayOperationsReport.h"
#include "../AirsideReport/AirsideRunwayUtilizationReport.h"
#include "../AirsideReport/AirsideRunwayUtilizationReportParam.h"
#include "../AirsideReport/TaxiwayDelayParameters.h"
#include "../AirsideReport/TaxiwayDelayReport.h"
#include "../AirsideReport/TaxiwayDelayReportResult.h"
#include "../AirsideReport/AirsideFlightConflictPara.h"
#include "../AirsideReport/AirsideFlightConflictReport.h"
#include "../AirsideReport/RunwayExitParameter.h"
#include "../AirsideReport/RunwayExitReport.h"
#include "../AirsideReport/RunwayExitResult.h"
#include "../Engine/Airside/CommonInSim.h"
#include "../AirsideReport/AirsideFlightRunwayDelayReport.h"
#include "../AirsideReport/AirsideFlightRunwayDelayReportPara.h"
#include "../AirsideReport/AirsideFlightBurnParmater.h"
#include "../AirsideReport/AirsideFlightFuelBurnReport.h"
#include "../AirsideReport/AirsideFlightFuelBurnReportResult.h"
#include "../AirsideReport/AirsideRunwayCrossingsReport.h"
#include "../AirsideReport/AirsideRunwayCrossingsPara.h"
#include "../AirsideReport/AirsideRunwayCrossingsBaseResult.h"
#include "../AirsideReport/AirsideFlightStandOperationReport.h"
#include "../AirsideReport/FlightStandOperationParameter.h"
#include "../AirsideReport/AirsideFlightStandOperationBaseResult.h"
#include "../AirsideReport/AirsideIntersectionOperationReport.h"
#include "../AirsideReport/AirsideIntersectionReportParam.h"
#include "../AirsideReport/AirsideIntersectionReportResult.h"
#include "../AirsideReport/AirsideTaxiwayUtilizationResult.h"
#include "../AirsideReport/AirsideTaxiwayUtilizationReport.h"
#include "../AirsideReport/AirsideTaxiwayUtilizationPara.h"
#include "../AirsideReport/TaxiwayUtilizationData.h"
#include "../AirsideReport/TaxiwayItem.h"
#include "../AirsideReport/AirsideControllerWorkloadResult.h"
#include "../AirsideReport/AirsideControllerWorkloadReport.h"
#include "../AirsideReport/AirsideControllerWorkloadParam.h"
#include "../AirsideReport/AirsideTakeoffProcessReport.h"
#include "../AirsideReport/AirsideTakeoffProcessResult.h"
#include "../AirsideReport/AirsideTakeoffProcessParameter.h"
#include "../AirsideReport/AirsideTakeoffProcessDetailResult.h"
#include "../AirsideReport/AirsideTakeoffProcessSummaryResult.h"


// CAirsideReportGraphView

IMPLEMENT_DYNCREATE(CAirsideReportGraphView, CFormView)

CAirsideReportGraphView::CAirsideReportGraphView()
	: CFormView(CAirsideReportGraphView::IDD)
{
}

CAirsideReportGraphView::~CAirsideReportGraphView()
{
}

void CAirsideReportGraphView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDC_CHART_TYPE_COMBO, m_comboChartType);
	DDX_Control(pDX, IDC_CHART_SELECT_COMBO, m_comboChartSelect);
	DDX_Control(pDX, IDC_STATIC_SUBTYPE, m_StaSubText);
	DDX_Control(pDX, IDC_REPORT_SUBTYPE, m_ComBoxSubType);
	DDX_Control(pDX, IDC_REPORT_SUBTYPE1, m_ComBoxSubType1);
	DDX_Control(pDX, IDC_REPORT_SUBTYPE2, m_ComBoxSubType2);
	DDX_Control(pDX, IDC_REPORT_SUBTYPE3, m_ComBoxSubType3);
	DDX_Control(pDX, IDC_REPORT_SUBTYPE4, m_ComBoxSubType4);
}

BEGIN_MESSAGE_MAP(CAirsideReportGraphView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_SEL_COLOR_BTN, OnSelColorBtn)
	ON_BN_CLICKED(IDC_PRINT_BTN, OnPrintBtn)
	ON_CBN_SELCHANGE(IDC_CHART_SELECT_COMBO, OnSelchangeChartSelectCombo)
	//ON_CBN_SELCHANGE(IDC_REPORT_SUBTYPE, OnSelchangeChartSelectCombo)
	ON_CBN_SELENDOK(IDC_REPORT_SUBTYPE,OnSelChangerChartSubType)
	ON_CBN_SELCHANGE(IDC_REPORT_SUBTYPE, OnSelChangerChartSubTypes)
	ON_CBN_SELCHANGE(IDC_REPORT_SUBTYPE1, OnSelChangerChartSubTypes)
	ON_CBN_SELCHANGE(IDC_REPORT_SUBTYPE2, OnSelChangerChartSubTypes)
	ON_CBN_SELCHANGE(IDC_REPORT_SUBTYPE3, OnSelChangerChartSubTypes)
	ON_CBN_SELCHANGE(IDC_REPORT_SUBTYPE4, OnSelChangerChartSubTypes)

END_MESSAGE_MAP()


// CAirsideReportGraphView diagnostics

#ifdef _DEBUG
void CAirsideReportGraphView::AssertValid() const
{
	CFormView::AssertValid();
}

void CAirsideReportGraphView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CAirsideReportGraphView message handlers

void CAirsideReportGraphView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	InitToolbar();
	//*****************Fill Chart Type Combox***********************
	m_comboChartType.AddString("3D Bar");
	m_comboChartType.AddString("2D Bar");
	m_comboChartType.AddString("2D Pie");
	//m_comboChartType.AddString("2D XY");
	m_comboChartType.AddString("3D Line");
	m_comboChartType.AddString("2D Line");
	m_comboChartType.AddString("3D Area");
	m_comboChartType.AddString("2D Area");
	m_comboChartType.AddString("3D Step");
	m_comboChartType.AddString("2D Step");
	m_comboChartType.AddString("3D Combination");
	m_comboChartType.AddString("2D Combination");
	//*****************End of fill chart type combox****************
	//	m_MSChartCtrl.SetAllowSelections(FALSE);
	//m_MSChartCtrl.SetEnabled(FALSE);
	//m_MSChartCtrl.ShowWindow(SW_HIDE);
	m_StaSubText.ShowWindow(SW_HIDE) ;
	m_ComBoxSubType.ShowWindow(SW_HIDE) ;

	GetDlgItem(IDC_STATIC_SUBTYPE1)->ShowWindow(SW_HIDE);
	m_ComBoxSubType1.ShowWindow(SW_HIDE);

	GetDlgItem(IDC_STATIC_SUBTYPE2)->ShowWindow(SW_HIDE);
	m_ComBoxSubType2.ShowWindow(SW_HIDE);

	GetDlgItem(IDC_STATIC_SUBTYPE3)->ShowWindow(SW_HIDE);
	m_ComBoxSubType3.ShowWindow(SW_HIDE);
	
	GetDlgItem(IDC_STATIC_SUBTYPE4)->ShowWindow(SW_HIDE);
	m_ComBoxSubType4.ShowWindow(SW_HIDE);

	m_MSChartCtrl.Create(0,0,WS_CHILD | WS_VISIBLE ,CRect(0,0,100,100),this,1234);
}

int CAirsideReportGraphView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		/*| CBRS_GRIPPER*/ | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_REPORTTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	//if(!m_MSChartCtrl.Create(0,0,WS_CHILD | WS_VISIBLE ,CRect(0,0,100,100),this,1234) )
	//	return -1;

	//C2DChartData c2dGraphData;
	//c2dGraphData.m_strChartTitle = _T("Title");
	//c2dGraphData.m_strYtitle = _T("Number"); 
	//c2dGraphData.m_strXtitle = _T("Count");	
	//c2dGraphData.m_strFooter = _T("footnote");
	//CString strLabel = _T("");

	//std::vector<double> vrData;
	//for (int k = 0;k < 2;++k) {
	//	vrData.clear();
	//	strLabel.Format(_T("%d"),k);
	//	c2dGraphData.m_vrLegend.push_back(strLabel);
	//	for (long i = 0; i < 10; i++)
	//	{	
	//		if(k == 0)
	//		{
	//			strLabel.Format(_T("%d"),i+1);
	//			c2dGraphData.m_vrXTickTitle.push_back(strLabel);
	//		}
	//		vrData.push_back(static_cast<double>(i+1));
	//	}
	//	c2dGraphData.m_vr2DChartData.push_back(vrData);
	//}

	//m_MSChartCtrl.Set3DChartType();
	//m_MSChartCtrl.DrawChart(c2dGraphData);

	return 0;
}

void CAirsideReportGraphView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if(IsWindowVisible())
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		CRect rcChart;
		rcChart.right  = rcClient.right - 10;
		rcChart.left   = rcClient.left+10;
		rcChart.top    = rcClient.top+70;
		rcChart.bottom = rcClient.bottom;

		m_MSChartCtrl.m_p3DChart->MoveWindow(rcChart);
	}
}
void CAirsideReportGraphView::InitToolbar()
{
	CRect rc;
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow( 2,-2,90,rc.Height() );
	m_toolbarcontenter.ShowWindow(SW_HIDE);
	m_ToolBar.ShowWindow( SW_SHOW );
	//	m_toolbarcontenter.ShowWindow(SW_HIDE);
	//	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );
	//	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE );
	//	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );

}

void CAirsideReportGraphView::OnSelColorBtn() 
{
	m_MSChartCtrl.PropertyEdit();
}
void CAirsideReportGraphView::OnPrintBtn() 
{
	m_MSChartCtrl.Print3DChart();
}
/************************************************************************
UnknownType				= 0,
Airside_Legacy			= 1,
Airside_DistanceTravel	= 2,
Airside_Duration		= 3,
Airside_FlightDelay		= 4,
Airside_FlightActivity		= 5,
Airside_AircraftOperation = 6,
Airside_NodeDelay = 7,
/************************************************************************/
void CAirsideReportGraphView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/)
{
	if (lHint != AIRSIDEREPORT_SHOWREPORT)
		return;
	//hide the vehicle type select 
	m_StaSubText.ShowWindow(SW_HIDE) ;
	m_ComBoxSubType.ShowWindow(SW_HIDE)  ;

	reportType rpType = GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReportType();

	if (rpType != Airside_FlightConflict ||( rpType == Airside_FlightConflict
		&& GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters()->getReportType() != ASReportType_Summary))
	{
		m_StaSubText.ShowWindow(SW_HIDE) ;
		m_ComBoxSubType.ShowWindow(SW_HIDE)  ;

		GetDlgItem(IDC_STATIC_SUBTYPE1)->ShowWindow(SW_HIDE);
		m_ComBoxSubType1.ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_SUBTYPE2)->ShowWindow(SW_HIDE);
		m_ComBoxSubType2.ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_SUBTYPE3)->ShowWindow(SW_HIDE);
		m_ComBoxSubType3.ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_SUBTYPE4)->ShowWindow(SW_HIDE);
		m_ComBoxSubType4.ShowWindow(SW_HIDE);
	}

	switch(rpType)
	{
	case Airside_FlightDelay:
		{
			char* subReportName[] = 
			{
					_T("Schedule delay"),
					_T("Flight Total Delay"),
					_T("Segment Delay"),
					_T("Component Delay"),
					_T("Air Delay"),
					_T("Taxi Delay"),
					_T("Stand Delay"),
					_T("Service Delay"),
					_T("Take Off Delay"),
					_T("Detail Delay Count"),

					_T("Schedule delay"),
					_T("Flight Total Delay"),
					_T("Segment Delay"),
					_T("Component Delay"),
					_T("Air Delay"),
					_T("Taxi Delay"),
					_T("Stand Delay"),
					_T("Service Delay"),
					_T("Take Off Delay")

			};
			m_comboChartSelect.ResetContent();

            if(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters()->getReportType() == ASReportType_Detail)
			{
				for (int nType = CAirsideFlightDelayReport::subReportType::SRT_DETAIL_SCHEDULEDELAY; nType < CAirsideFlightDelayReport::subReportType::SRT_DETAIL_COUNT; ++ nType)
				{
					int nIndex = m_comboChartSelect.AddString(subReportName[nType]);
					m_comboChartSelect.SetItemData(nIndex ,nType);
				}
				m_comboChartSelect.SetCurSel(0);
			}
			else
			{
				for (int nType = CAirsideFlightDelayReport::subReportType::SRT_SUMMARY_SCHEDULEDELAY; nType < CAirsideFlightDelayReport::subReportType::SRT_COUNT; ++ nType)
				{
					int nIndex = m_comboChartSelect.AddString(subReportName[nType]);
					m_comboChartSelect.SetItemData(nIndex ,nType);				
				}
			}
			m_comboChartSelect.SetCurSel(0);
			// generate default report

			int nCursel =  m_comboChartSelect.GetCurSel();
			if (nCursel == LB_ERR)
				return;

			int nSubType =  m_comboChartSelect.GetItemData(nCursel);

			CAirsideFlightDelayParam *pParam = reinterpret_cast<CAirsideFlightDelayParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			CAirsideFlightDelayReport *pPreport = reinterpret_cast< CAirsideFlightDelayReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			std::vector<int> vReportRun;
			if(pParam->GetReportRuns(vReportRun) && pParam->GetEnableMultiRun())
			{
				if (vReportRun.size() > 1)
				{
					GetDocument()->GetARCReportManager().GetAirsideReportManager()->updateMultiRun3Dchart(m_MSChartCtrl);
					return;
				}
			}
			pPreport->RefreshReport(pParam);
			pPreport->GetReportResult()->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_DistanceTravel:
		{
			m_comboChartSelect.ResetContent();

			if(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters()->getReportType() == ASReportType_Detail)
			{
				int nIndex = m_comboChartSelect.AddString(_T("Distance Travel(Detailed)"));
//				m_comboChartSelect.SetItemData(nIndex ,CAirsideFlightDelayReport::SRT_DETAIL_NODEDELAY);			
			}
			else
			{
				int nIndex = m_comboChartSelect.AddString(_T("Distance Travel(Summary)"));
//				m_comboChartSelect.SetItemData(nIndex ,CAirsideFlightDelayReport::SRT_SUMMARY_NODEDELAY);	
			}

			m_comboChartSelect.SetCurSel(0);

			int nCursel =  m_comboChartSelect.GetCurSel();
			if (nCursel == LB_ERR)
				return;

			int nSubType =  m_comboChartSelect.GetItemData(nCursel);

			CAirsideDistanceTravelParam *pParam = reinterpret_cast<CAirsideDistanceTravelParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			
			CAirsideDistanceTravelReport *pPreport = reinterpret_cast< CAirsideDistanceTravelReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);

			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_Duration:
		{

		}
		break;
	case Airside_FlightActivity: //only detail
		{
			m_comboChartSelect.ResetContent();
			enumASReportType_Detail_Summary emReportType = GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters()->getReportType();
			if(emReportType == ASReportType_Detail)
			{
				int nIndex = -1;
				nIndex = m_comboChartSelect.AddString(_T("Entry Distribution"));
				m_comboChartSelect.SetItemData(nIndex,CFlightActivityBaseResult::FAR_SysEntryDist);

				nIndex = m_comboChartSelect.AddString(_T("Exit Distribution"));
				m_comboChartSelect.SetItemData(nIndex,CFlightActivityBaseResult::FAR_SysExitDist);

				m_comboChartSelect.SetCurSel(0);

				int nCursel =  m_comboChartSelect.GetCurSel();
				if (nCursel == LB_ERR)
					return;

				int nSubType =  m_comboChartSelect.GetItemData(nCursel);
				CFlightActivityParam *pParam = reinterpret_cast<CFlightActivityParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
				pParam->setSubType(nSubType);


				CFlightActivityReport *pPreport = reinterpret_cast<CFlightActivityReport *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
				pPreport->RefreshReport(pParam);

				CFlightActivityBaseResult *pResult =  pPreport->m_pResult;
				if (pResult)
				 	pResult->Draw3DChart(m_MSChartCtrl, pParam);
			} 
			else
			{
				int nIndex = -1;
				nIndex = m_comboChartSelect.AddString(_T("VT"));
				m_comboChartSelect.SetItemData(nIndex,CFlightActivityBaseResult::ACTIVITY_RESULT_VT);

				nIndex  = m_comboChartSelect.AddString(_T("DT"));
				m_comboChartSelect.SetItemData(nIndex,CFlightActivityBaseResult::ACTIVITY_RESULT_DT);
				
				nIndex  = m_comboChartSelect.AddString(_T("AT"));
				m_comboChartSelect.SetItemData(nIndex,CFlightActivityBaseResult::ACTIVITY_RESULT_AT);

				m_comboChartSelect.SetCurSel(0);

				int nCursel =  m_comboChartSelect.GetCurSel();
				if (nCursel == LB_ERR)
					return;

				int nSubType =  m_comboChartSelect.GetItemData(nCursel);
				CFlightActivityParam *pParam = reinterpret_cast<CFlightActivityParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
				pParam->setSubType(nSubType);

				m_comboChartType.SetCurSel(4);
				m_MSChartCtrl.Set3DChartType(Arc3DChartType_2D_Line);
				
				CFlightActivityReport *pPreport = reinterpret_cast<CFlightActivityReport *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
				pPreport->RefreshReport(pParam);

				CFlightActivityBaseResult *pResult =  pPreport->m_pResult;
				if (pResult)
					pResult->Draw3DChart(m_MSChartCtrl, pParam);
			}
		}
		break;
	case Airside_NodeDelay:
		{
			m_comboChartSelect.ResetContent();

			if(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters()->getReportType() == ASReportType_Detail)
			{
				int nIndex = m_comboChartSelect.AddString(_T("Node Delay(Detailed)"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideFlightDelayReport::SRT_DETAIL_NODEDELAY);			
			}
			else
			{
				int nIndex = m_comboChartSelect.AddString(_T("Node Delay(Summary)"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideFlightDelayReport::SRT_SUMMARY_NODEDELAY);	
			}

			m_comboChartSelect.SetCurSel(0);

			int nCursel =  m_comboChartSelect.GetCurSel();
			if (nCursel == LB_ERR)
				return;

			int nSubType =  m_comboChartSelect.GetItemData(nCursel);

			CAirsideNodeDelayReportParameter *pParam = reinterpret_cast<CAirsideNodeDelayReportParameter *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			CAirsideNodeDelayReport *pPreport = reinterpret_cast< CAirsideNodeDelayReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);

			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_AircraftOperation:
		{
			m_comboChartSelect.ResetContent();

			int nIndex = m_comboChartSelect.AddString(_T("Total"));

			m_comboChartSelect.SetCurSel(0);

			int nCursel =  m_comboChartSelect.GetCurSel();
			if (nCursel == LB_ERR)
				return;

			int nSubType =  m_comboChartSelect.GetItemData(nCursel);

			CAirsideOperationParam *pParam = reinterpret_cast<CAirsideOperationParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			//pParam->setSubType(nSubType);
			CAirsideOperationReport *pPreport = reinterpret_cast< CAirsideOperationReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);

			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_AircraftOperational:
		{
			m_comboChartSelect.ResetContent();

			int nIndex = -1;
			nIndex = m_comboChartSelect.AddString(_T("Flight Type VS Air Distance"));
			m_comboChartSelect.SetItemData(nIndex,CFlightOperationalReport::OPERATIONAL_FLIGHTTYPEVSAIRDISTANCE);

			nIndex = m_comboChartSelect.AddString(_T("Flight Type VS Ground Distance"));
			m_comboChartSelect.SetItemData(nIndex,CFlightOperationalReport::OPERATIONAL_FLIGHTTYPEVSGROUNDDISTANCE);

			m_comboChartSelect.SetCurSel(0);

			int nCursel =  m_comboChartSelect.GetCurSel();
			if (nCursel == LB_ERR)
				return;

			int nSubType =  m_comboChartSelect.GetItemData(nCursel);
			CFlightOperationalParam *pParam = reinterpret_cast<CFlightOperationalParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);

			CFlightOperationalReport *pPreport = reinterpret_cast<CFlightOperationalReport *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);

			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_RunwayOperaitons:
		{
			m_comboChartSelect.ResetContent();


			if(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters()->getReportType() == ASReportType_Detail)
			{
				//CString TypeName[] = 
				//{
				//	_T("Landings by Runway"),
				//		_T("TakeOff by Runway"),
				//		_T("Lead-Trail Operation by Runway"),
				//		_T("Movements per Interval")
				//};


				//ChartType_Detail = 0,
				//	ChartType_Detail_LandingsByRunway,
				//	ChartType_Detail_TakeOffByRunway,
				//	ChartType_Detail_LeadTrailByRunway,
				//	ChartType_Detail_MovementsPerRunway,


				//	ChartType_Detail_Count,


				//	ChartType_Summary = 100,
				//	ChartType_Summary_RunwayOperationalStatistic,

				//	ChartType_Summary_Count,

				int nIndex = m_comboChartSelect.AddString(_T("Landings by Runway"));
				m_comboChartSelect.SetItemData(nIndex ,AirsideRunwayOperationsReport::ChartType_Detail_LandingsByRunway);
		
				nIndex = m_comboChartSelect.AddString(_T("TakeOff by Runway"));
				m_comboChartSelect.SetItemData(nIndex ,AirsideRunwayOperationsReport::ChartType_Detail_TakeOffByRunway);

				nIndex = m_comboChartSelect.AddString(_T("Lead-Trail Operation by Runway"));
				m_comboChartSelect.SetItemData(nIndex ,AirsideRunwayOperationsReport::ChartType_Detail_LeadTrailByRunway);

				nIndex = m_comboChartSelect.AddString(_T("Movements per Interval"));
				m_comboChartSelect.SetItemData(nIndex ,AirsideRunwayOperationsReport::ChartType_Detail_MovementsPerRunway);
			}
			else
			{
				int nIndex = m_comboChartSelect.AddString(_T("Runway Movements Operational Statistics"));
				m_comboChartSelect.SetItemData(nIndex ,AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Operations);

				nIndex = m_comboChartSelect.AddString(_T("Runway Landing Operational Statistics"));
				m_comboChartSelect.SetItemData(nIndex ,AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Landing);

				nIndex = m_comboChartSelect.AddString(_T("Runway Take off Operational Statistics "));
				m_comboChartSelect.SetItemData(nIndex ,AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_TakeOff);

			}

			m_comboChartSelect.SetCurSel(0);

			int nCursel =  m_comboChartSelect.GetCurSel();
			if (nCursel == LB_ERR)
				return;

			int nSubType =  m_comboChartSelect.GetItemData(nCursel);

			AirsideRunwayOperationReportParam *pParam = reinterpret_cast<AirsideRunwayOperationReportParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			AirsideRunwayOperationsReport *pPreport = reinterpret_cast< AirsideRunwayOperationsReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);

			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);

		}
		break;
	case Airside_TakeoffProcess:
		{
			m_comboChartSelect.ResetContent();
			if(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters()->getReportType() == ASReportType_Detail)
			{
				int nIndex = m_comboChartSelect.AddString(_T("Take off Q Delay"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideTakeoffProcessDetailResult::TakeoffQueueDelay);

				nIndex = m_comboChartSelect.AddString(_T("Time to Position"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideTakeoffProcessDetailResult::TimeToPosition);

				nIndex = m_comboChartSelect.AddString(_T("Take off Delay"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideTakeoffProcessDetailResult::TakeOffDelay);
			}
			else
			{
				int nIndex = m_comboChartSelect.AddString(_T("Take off Q time"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideTakeoffProcessSummaryResult::TakeOffQueueTime);

				nIndex = m_comboChartSelect.AddString(_T("Time at hold short line"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideTakeoffProcessSummaryResult::TimeAtHoldShortLine);

				nIndex = m_comboChartSelect.AddString(_T("Taxi to position"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideTakeoffProcessSummaryResult::TaxiToPosition);

				nIndex = m_comboChartSelect.AddString(_T("Time in position"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideTakeoffProcessSummaryResult::TimeInPosition);
			}

			m_comboChartSelect.SetCurSel(0);

			int nCursel =  m_comboChartSelect.GetCurSel();
			if (nCursel == LB_ERR)
				return;

			int nSubType =  m_comboChartSelect.GetItemData(nCursel);

			CAirsideTakeoffProcessParameter *pParam = reinterpret_cast<CAirsideTakeoffProcessParameter *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			CAirsideTakeoffProcessReport *pPreport = reinterpret_cast< CAirsideTakeoffProcessReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);

			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case  Airside_VehicleOperation: 
		{
			m_comboChartSelect.ResetContent(); 
			if(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters()->getReportType() == ASReportType_Detail)
			{
				int nIndex = m_comboChartSelect.AddString(_T("Time On Road"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::DETAIL_ONROAD_TIME);	

				nIndex = m_comboChartSelect.AddString(_T("Time In Server")) ;
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::DETAIL_TOTALTIME_INSERVER);	

				nIndex = m_comboChartSelect.AddString(_T("Time Idel")) ;
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::DETAIL_IDEL_TIME);

				nIndex = m_comboChartSelect.AddString(_T("Number Of Server")) ;
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::DETAIL_NUMBER_SERVER);

				nIndex = m_comboChartSelect.AddString(_T("Number To Pool")) ;
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::DETAIL_NUMBER_TOPOOL);

				nIndex = m_comboChartSelect.AddString(_T("Number for Gas")) ;
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::DETAIL_NUMBER_FORGAS);

				nIndex = m_comboChartSelect.AddString(_T("Time In Lots")) ;
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::DETAIL_TIME_INLOTS);

				nIndex = m_comboChartSelect.AddString(_T("Fuel Consumed")) ;
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::DETAIL_FUEL_CONSUMED);

				nIndex = m_comboChartSelect.AddString(_T("Avg Speed")) ;
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::DETAIL_AVG_SPEED);

				nIndex = m_comboChartSelect.AddString(_T("Max Speed")) ;
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::DETAIL_MAX_SPEED);

				nIndex = m_comboChartSelect.AddString(_T("Number of vehicle servers"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::DETAIL_VEHICLE_SERVICE);
			}
			else
			{
				int nIndex = m_comboChartSelect.AddString(_T("Time On Road(Summary)"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::SUMMARY_ON_ROAD);	

				 nIndex = m_comboChartSelect.AddString(_T("Speed(Summary)"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::SUMMARY_SPEED);	

				 nIndex = m_comboChartSelect.AddString(_T("Time In Server(Summary)"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::SUMMARY_IN_SERVER);

				nIndex = m_comboChartSelect.AddString(_T("Fuel Consumed(Summary)"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::SUMMARY_FUEL);

				nIndex = m_comboChartSelect.AddString(_T("Distance Covered(Summary)"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::SUMMARY_DISTANCE);

				nIndex = m_comboChartSelect.AddString(_T("Utilization(Summary)"));
				m_comboChartSelect.SetItemData(nIndex ,CAirsideVehicleOperaterReport::SUMMARY_UTILIZATION);

				m_StaSubText.ShowWindow(SW_SHOW) ;
				m_ComBoxSubType.ShowWindow(SW_SHOW)  ;
				m_ComBoxSubType.ResetContent() ;
				CAirsideVehicleOperParameter *pParam = reinterpret_cast<CAirsideVehicleOperParameter *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
				std::vector<CString>* vehicleTys = pParam->GetVehicleType() ;
				if(vehicleTys->empty())
				{
					std::vector<int> m_data = pParam->GetExistVehicleType() ;
                    for (int i = 0 ;i < (int)m_data.size() ;i++)
                    {
						nIndex = m_ComBoxSubType.AddString(CAirsideVehicleOperaterReport::GetVehicleTypeNameById(m_data[i])) ;
						m_ComBoxSubType.SetItemData(nIndex ,m_data[i]);
                    }
				}else
				{
					for (int i = 0 ; i < (int)vehicleTys->size() ;i++)
					{
						nIndex = m_ComBoxSubType.AddString((*vehicleTys)[i]) ;
						m_ComBoxSubType.SetItemData(nIndex ,CAirsideVehicleOperaterReport::GetVehicleIDByName((*vehicleTys)[i]));
					}
				}
				m_ComBoxSubType.SetCurSel(0) ;

				int Vehiclesel = m_ComBoxSubType.GetCurSel() ;
				if(Vehiclesel == LB_ERR)
					return ;
				 int vehicleType = m_ComBoxSubType.GetItemData(Vehiclesel) ;
				 pParam->SetVehicleType(vehicleType) ;
			}
			m_comboChartSelect.SetCurSel(0);
            
			int nCursel =  m_comboChartSelect.GetCurSel();
			if (nCursel == LB_ERR)
				return;
 
			int nSubType =  m_comboChartSelect.GetItemData(nCursel);
    
			CAirsideVehicleOperParameter *pParam = reinterpret_cast<CAirsideVehicleOperParameter *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			CAirsideVehicleOperaterReport *pPreport = reinterpret_cast< CAirsideVehicleOperaterReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			
			pPreport->RefreshReport(pParam);

			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
        break ;
	case Airside_RunwayUtilization:
		{
			InitRunwayUtilizationReport();
		}
		break;
	case Airside_TaxiwayDelay:
		{
			InitTaxiwayDelayReport();
		}
		break ;
	case Airside_FlightConflict:
		{
			AirsideFlightConflictPara *pParam = reinterpret_cast<AirsideFlightConflictPara *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			m_comboChartSelect.ResetContent();

			if(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters()->getReportType() == ASReportType_Detail)
			{

				int nIndex = m_comboChartSelect.AddString(_T("Distribution Of Per AC"));
				m_comboChartSelect.SetItemData(nIndex ,AirsideFlightConflictReport::ChartType_Detail_DistributionOfPerAC);

				nIndex = m_comboChartSelect.AddString(_T("By Time Of Day"));
				m_comboChartSelect.SetItemData(nIndex ,AirsideFlightConflictReport::ChartType_Detail_ByTimeOfDay);

				nIndex = m_comboChartSelect.AddString(_T("By Area And Time Of Day"));
				m_comboChartSelect.SetItemData(nIndex ,AirsideFlightConflictReport::ChartType_Detail_ByAreaAndTimeOfDay);

				nIndex = m_comboChartSelect.AddString(_T("By Action And Time Of Day"));
				m_comboChartSelect.SetItemData(nIndex ,AirsideFlightConflictReport::ChartType_Detail_ByActionAndTimeOfDay);

				nIndex = m_comboChartSelect.AddString(_T("By Operation And Time Of Day"));
				m_comboChartSelect.SetItemData(nIndex ,AirsideFlightConflictReport::ChartType_Detail_ByOperationAndTimeOfDay);

				nIndex = m_comboChartSelect.AddString(_T("By Location And Time Of Day"));
				m_comboChartSelect.SetItemData(nIndex ,AirsideFlightConflictReport::ChartType_Detail_ByLocationAndTimeOfDay);

			}
			else
			{
				int nIndex = m_comboChartSelect.AddString(_T("Summary report"));
				m_comboChartSelect.SetItemData(nIndex ,AirsideFlightConflictReport::ChartType_Summary);

				InitFlightConflictSummaryReportFilter(pParam);

				int nConType = m_ComBoxSubType.GetItemData(0);
				int nAreaIdx = m_ComBoxSubType1.GetItemData(0);
				int nLocType = m_ComBoxSubType2.GetItemData(0);
				int nOpType = m_ComBoxSubType3.GetItemData(0);
				int nActType = m_ComBoxSubType4.GetItemData(0);

				pParam->setSummaryFilterPara(nConType,nAreaIdx,nLocType,nOpType,nActType);

			}

			m_comboChartSelect.SetCurSel(0);
			int nCursel =  m_comboChartSelect.GetCurSel();

			if (nCursel == LB_ERR)
				return;

			int nSubType =  m_comboChartSelect.GetItemData(nCursel);
			pParam->setSubType(nSubType);

			AirsideFlightConflictReport *pPreport = reinterpret_cast< AirsideFlightConflictReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);

			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_RunwayExit:
		{
			CRunwayExitParameter *pParam = reinterpret_cast<CRunwayExitParameter *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			CRunwayExitReport *pPreport = reinterpret_cast< CRunwayExitReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);

			CAirsideReportBaseResult *pResult =  pPreport->GetBaseDelayReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
			break ;
		}
	case Airside_RunwayDelay:
		{
			AirsideFlightRunwayDelayReportPara *pParam = reinterpret_cast<AirsideFlightRunwayDelayReportPara *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			m_comboChartSelect.ResetContent();

			if(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters()->getReportType() == ASReportType_Detail)
			{

				int nIndex = m_comboChartSelect.AddString(_T("Total Runway Delay"));
				m_comboChartSelect.SetItemData(nIndex ,AirsideFlightRunwayDelayReport::ChartType_Detail_Total);

				if (pParam->getFltTypeMode() =='B'|| pParam->getFltTypeMode() =='A')
				{
					nIndex = m_comboChartSelect.AddString(_T("Landing Roll"));
					m_comboChartSelect.SetItemData(nIndex ,AirsideFlightRunwayDelayReport::ChartType_Detail_ByLandingRoll);

					nIndex = m_comboChartSelect.AddString(_T("Exiting Runway"));
					m_comboChartSelect.SetItemData(nIndex ,AirsideFlightRunwayDelayReport::ChartType_Detail_ByExisting);
				}

				if (pParam->getFltTypeMode() =='B'|| pParam->getFltTypeMode() =='D')
				{
					nIndex = m_comboChartSelect.AddString(_T("In Takeoff Queue"));
					m_comboChartSelect.SetItemData(nIndex ,AirsideFlightRunwayDelayReport::ChartType_Detail_ByInTakeoffQueue);

					nIndex = m_comboChartSelect.AddString(_T("At Hold Short Line"));
					m_comboChartSelect.SetItemData(nIndex ,AirsideFlightRunwayDelayReport::ChartType_Detail_ByHoldShort);

					nIndex = m_comboChartSelect.AddString(_T("In Position"));
					m_comboChartSelect.SetItemData(nIndex ,AirsideFlightRunwayDelayReport::ChartType_Detail_ByInPosition);
				}
			}
			else
			{
				int nIndex = m_comboChartSelect.AddString(_T("Total Runway Delay"));
				m_comboChartSelect.SetItemData(nIndex ,AirsideFlightRunwayDelayReport::ChartType_Summary_Total);

				if (pParam->getFltTypeMode() =='B'|| pParam->getFltTypeMode() =='A')
				{
					nIndex = m_comboChartSelect.AddString(_T("Landing Roll"));
					m_comboChartSelect.SetItemData(nIndex ,AirsideFlightRunwayDelayReport::ChartType_Summary_ByLandingRoll);

					nIndex = m_comboChartSelect.AddString(_T("Exiting Runway"));
					m_comboChartSelect.SetItemData(nIndex ,AirsideFlightRunwayDelayReport::ChartType_Summary_ByExisting);
				}

				if (pParam->getFltTypeMode() =='B'|| pParam->getFltTypeMode() =='D')
				{
					nIndex = m_comboChartSelect.AddString(_T("In Takeoff Queue"));
					m_comboChartSelect.SetItemData(nIndex ,AirsideFlightRunwayDelayReport::ChartType_Summary_ByInTakeoffQueue);

					nIndex = m_comboChartSelect.AddString(_T("At Hold Short Line"));
					m_comboChartSelect.SetItemData(nIndex ,AirsideFlightRunwayDelayReport::ChartType_Summary_ByHoldShort);

					nIndex = m_comboChartSelect.AddString(_T("In Position"));
					m_comboChartSelect.SetItemData(nIndex ,AirsideFlightRunwayDelayReport::ChartType_Summary_ByInPosition);
				}
			}

			m_comboChartSelect.SetCurSel(0);
			int nCursel =  m_comboChartSelect.GetCurSel();

			if (nCursel == LB_ERR)
				return;

			int nSubType =  m_comboChartSelect.GetItemData(nCursel);
			pParam->setSubType(nSubType);

			AirsideFlightRunwayDelayReport *pPreport = reinterpret_cast< AirsideFlightRunwayDelayReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);

			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_FlightFuelBurning:
		{
			CAirsideFlightFuelBurnParmater *pParam = reinterpret_cast<CAirsideFlightFuelBurnParmater *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			CAirsideFlightFuelBurnReport *pPreport = reinterpret_cast< CAirsideFlightFuelBurnReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);

			CAirsideFlightFuelBurnReportResult *pResult =  pPreport->GetAirsideReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
			break ;
		}
	case Airside_RunwayCrossings:
		{
			m_comboChartSelect.ResetContent();
			enumASReportType_Detail_Summary emReportType = GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters()->getReportType();
			if(emReportType == ASReportType_Detail)
			{
				int nIndex = -1;
				nIndex = m_comboChartSelect.AddString(_T("Detailed Runway Crossings"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideRunwayCrossingsBaseResult::DT_COUNT);

				nIndex = m_comboChartSelect.AddString(_T("Runway Crossings Wait Time"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideRunwayCrossingsBaseResult::DT_WAITTIME);

				nIndex = m_comboChartSelect.AddString(_T("Runway Crossings by Taxiway"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideRunwayCrossingsBaseResult::DT_TAXINODE);

				m_comboChartSelect.SetCurSel(0);

				int nCursel =  m_comboChartSelect.GetCurSel();
				if (nCursel == LB_ERR)
					return;

				int nSubType =  m_comboChartSelect.GetItemData(nCursel);
				AirsideRunwayCrossingsParameter *pParam = reinterpret_cast<AirsideRunwayCrossingsParameter *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
				pParam->setSubType(nSubType);


				CAirsideRunwayCrossingsReport *pPreport = reinterpret_cast<CAirsideRunwayCrossingsReport *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
				pPreport->RefreshReport(pParam);

				CAirsideRunwayCrossingsBaseResult *pResult =  pPreport->m_pResult;
				if (pResult)
					pResult->Draw3DChart(m_MSChartCtrl, pParam);
			} 
			else
			{
				int nIndex = -1;
				nIndex = m_comboChartSelect.AddString(_T("Summary Runway Crossings"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideRunwayCrossingsBaseResult::SM_COUNT);

				nIndex  = m_comboChartSelect.AddString(_T("Summary Runway Wait Times"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideRunwayCrossingsBaseResult::SM_WAITTIME);

				m_comboChartSelect.SetCurSel(0);

				int nCursel =  m_comboChartSelect.GetCurSel();
				if (nCursel == LB_ERR)
					return;

				int nSubType =  m_comboChartSelect.GetItemData(nCursel);
				AirsideRunwayCrossingsParameter *pParam = reinterpret_cast<AirsideRunwayCrossingsParameter *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
				pParam->setSubType(nSubType);

				CAirsideRunwayCrossingsReport *pPreport = reinterpret_cast<CAirsideRunwayCrossingsReport *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
				pPreport->RefreshReport(pParam);

				CAirsideRunwayCrossingsBaseResult *pResult =  pPreport->m_pResult;
				if (pResult)
					pResult->Draw3DChart(m_MSChartCtrl, pParam);
			}
		}
		break;
	case Airside_StandOperations:
		{
			m_comboChartSelect.ResetContent();
			enumASReportType_Detail_Summary emReportType = GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters()->getReportType();
			if(emReportType == ASReportType_Detail)
			{
				int nIndex = -1;
				nIndex = m_comboChartSelect.AddString(_T("Detailed Stand Utilization(Occupancy time)"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideFlightStandOperationReport::ChartType_Detail_Occupancy);

				nIndex = m_comboChartSelect.AddString(_T("Detailed Stand Utilization(Idle time)"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideFlightStandOperationReport::ChartType_Detail_IdleTime);

				nIndex = m_comboChartSelect.AddString(_T("Detailed Stand Utilization(Ratio)"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideFlightStandOperationReport::ChartType_Detail_Percentage);

				nIndex = m_comboChartSelect.AddString(_T("Stand Delay"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideFlightStandOperationReport::ChartType_Detail_Delay);

				nIndex = m_comboChartSelect.AddString(_T("Stand conflicts"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideFlightStandOperationReport::ChartType_Detail_Conflict);

				m_comboChartSelect.SetCurSel(0);

				int nCursel =  m_comboChartSelect.GetCurSel();
				if (nCursel == LB_ERR)
					return;

				int nSubType =  m_comboChartSelect.GetItemData(nCursel);
				CFlightStandOperationParameter *pParam = reinterpret_cast<CFlightStandOperationParameter *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
				pParam->setSubType(nSubType);


				CAirsideFlightStandOperationReport *pPreport = reinterpret_cast<CAirsideFlightStandOperationReport *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
				pPreport->RefreshReport(pParam);

				CAirsideFlightStandOperationBaseResult *pResult =  pPreport->m_pBaseResult;
				if (pResult)
					pResult->Draw3DChart(m_MSChartCtrl, pParam);
			} 
			else
			{
				int nIndex = -1;
				nIndex = m_comboChartSelect.AddString(_T("Summary Schedule Stand Utilization"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideFlightStandOperationReport::ChartType_Summary_SchedUtilization);

				nIndex  = m_comboChartSelect.AddString(_T("Summary Schedule Stand Idle"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideFlightStandOperationReport::ChartType_Summary_SchedIdle);

				nIndex = m_comboChartSelect.AddString(_T("Summary Actual Stand Utilization"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideFlightStandOperationReport::ChartType_Summary_ActualUtilization);

				nIndex  = m_comboChartSelect.AddString(_T("Summary Actual Stand Idle"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideFlightStandOperationReport::ChartType_Summary_ActualIdle);

				nIndex = m_comboChartSelect.AddString(_T("Summary Stand Delay"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideFlightStandOperationReport::ChartType_Summary_Delay);

				nIndex  = m_comboChartSelect.AddString(_T("Summary Stand Conflicts"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideFlightStandOperationReport::ChartType_Summary_Conflict);

				m_comboChartSelect.SetCurSel(0);

				int nCursel =  m_comboChartSelect.GetCurSel();
				if (nCursel == LB_ERR)
					return;

				int nSubType =  m_comboChartSelect.GetItemData(nCursel);
				CFlightStandOperationParameter *pParam = reinterpret_cast<CFlightStandOperationParameter *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
				pParam->setSubType(nSubType);

				CAirsideFlightStandOperationReport *pPreport = reinterpret_cast<CAirsideFlightStandOperationReport *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
				pPreport->RefreshReport(pParam);

				CAirsideFlightStandOperationBaseResult *pResult =  pPreport->m_pBaseResult;
				if (pResult)
					pResult->Draw3DChart(m_MSChartCtrl, pParam);
			}
		}
		break;
	case Airside_IntersectionOperation:
		{
			m_comboChartSelect.ResetContent();
			enumASReportType_Detail_Summary emReportType = GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters()->getReportType();
			if(emReportType == ASReportType_Detail)
			{
				int nIndex = -1;
				nIndex = m_comboChartSelect.AddString(_T("Crossings Count"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideIntersectionReportBaseResult::DT_CROSSING);

				//DT_CROSSING = 0,
				//	DT_BUSYFACTOR,
				//	DT_DELAY,
				//	DT_CONFLICT,

				//	SM_CROSSING,
				//	SM_DELAY
				nIndex = m_comboChartSelect.AddString(_T("Busy Factor Time"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideIntersectionReportBaseResult::DT_BUSYFACTOR);

				nIndex = m_comboChartSelect.AddString(_T("Delay Time"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideIntersectionReportBaseResult::DT_DELAY);

				nIndex = m_comboChartSelect.AddString(_T("Conflict Count"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideIntersectionReportBaseResult::DT_CONFLICT);

				m_comboChartSelect.SetCurSel(0);

				int nCursel =  m_comboChartSelect.GetCurSel();
				if (nCursel == LB_ERR)
					return;

				int nSubType =  m_comboChartSelect.GetItemData(nCursel);
				CAirsideIntersectionReportParam *pParam = reinterpret_cast<CAirsideIntersectionReportParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
				pParam->setSubType(nSubType);


				CAirsideIntersectionOperationReport *pPreport = reinterpret_cast<CAirsideIntersectionOperationReport *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
				pPreport->RefreshReport(pParam);

				CAirsideIntersectionReportBaseResult  *pResult =  pPreport->m_pResult;
				if (pResult)
					pResult->Draw3DChart(m_MSChartCtrl, pParam);
			}
			else
			{
				int nIndex = -1;
				nIndex = m_comboChartSelect.AddString(_T("Crossings"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideIntersectionReportBaseResult::SM_CROSSING);


				nIndex = m_comboChartSelect.AddString(_T("Delay Time"));
				m_comboChartSelect.SetItemData(nIndex,CAirsideIntersectionReportBaseResult::SM_DELAY);

				m_comboChartSelect.SetCurSel(0);

				int nCursel =  m_comboChartSelect.GetCurSel();
				if (nCursel == LB_ERR)
					return;


				int nSubType =  m_comboChartSelect.GetItemData(nCursel);
				CAirsideIntersectionReportParam *pParam = reinterpret_cast<CAirsideIntersectionReportParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
				pParam->setSubType(nSubType);

				m_StaSubText.ShowWindow(SW_SHOW) ;
				m_StaSubText.SetWindowText(_T("Node"));
				m_ComBoxSubType.ShowWindow(SW_SHOW)  ;
				m_ComBoxSubType.ResetContent();
				std::vector<CTaxiwayItem::CTaxiwayNodeItem > vAllNodes = pParam->getAllNodes();
				int nNodeCount = static_cast<int>(vAllNodes.size());

				int nWidth = 0;
				for (int nNode = 0; nNode < nNodeCount; ++nNode)
				{
					int nNodeIndex = m_ComBoxSubType.AddString(vAllNodes[nNode].m_strName);
					m_ComBoxSubType.SetItemData(nNodeIndex,vAllNodes[nNode].m_nNodeID);
					int nWidth2 = m_ComBoxSubType.GetDC()->GetTextExtent(vAllNodes[nNode].m_strName).cx;
					if (nWidth2>nWidth)
						nWidth = nWidth2;
				}
				m_ComBoxSubType.SetDroppedWidth(nWidth);

				m_ComBoxSubType.SetCurSel(0);
				int nNodeCursel =  m_ComBoxSubType.GetCurSel();
				if (nNodeCursel == LB_ERR)
					return;
				int nSummaryNode = m_ComBoxSubType.GetItemData(nNodeCursel);
				pParam->setSummaryNodeID(nSummaryNode);

				CAirsideIntersectionOperationReport *pPreport = reinterpret_cast<CAirsideIntersectionOperationReport *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
				pPreport->RefreshReport(pParam);

				CAirsideIntersectionReportBaseResult  *pResult =  pPreport->m_pResult;
				if (pResult)
					pResult->Draw3DChart(m_MSChartCtrl, pParam);

			}
		}
		break;
	case Airside_TaxiwayUtilization:
		{
			m_comboChartSelect.ResetContent();
			enumASReportType_Detail_Summary emReportType = GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters()->getReportType();
			
			int nIndex = -1;
			nIndex = m_comboChartSelect.AddString(_T("Movements"));
			m_comboChartSelect.SetItemData(nIndex,CAirsideTaxiwayUtilizationBaseResult::TAXIWAY_UTILIZATION_MOVEMENT);
			
			nIndex = m_comboChartSelect.AddString(_T("Occupancy"));
			m_comboChartSelect.SetItemData(nIndex,CAirsideTaxiwayUtilizationBaseResult::TAXIWAY_UTILIZATION_OCCUPANCYTIME);

			nIndex = m_comboChartSelect.AddString(_T("Speed"));
			m_comboChartSelect.SetItemData(nIndex,CAirsideTaxiwayUtilizationBaseResult::TAXIWAY_UTILIZATION_SPEED);


			m_comboChartSelect.SetCurSel(0);

			int nCursel =  m_comboChartSelect.GetCurSel();
			if (nCursel == LB_ERR)
				return;

			int nSubType =  m_comboChartSelect.GetItemData(nCursel);
			CAirsideTaxiwayUtilizationPara *pParam = reinterpret_cast<CAirsideTaxiwayUtilizationPara *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);

			m_StaSubText.ShowWindow(SW_SHOW) ;
			m_StaSubText.SetWindowText(_T("Taxiway: "));
			m_ComBoxSubType.ShowWindow(SW_SHOW)  ;
			m_ComBoxSubType.ResetContent();

			int nAllIndex = m_ComBoxSubType.AddString(_T("All taxiways"));
			m_ComBoxSubType.SetItemData(nAllIndex,(DWORD_PTR)NULL);

			
			CAirsideTaxiwayUtilizationReport *pPreport = reinterpret_cast<CAirsideTaxiwayUtilizationReport *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);

			CAirsideTaxiwayUtilizationBaseResult  *pResult =  pPreport->m_pResult;

			if (pResult)
			{
				int nWidth = 0;
				std::vector<CTaxiwayUtilizationData*> vResult = pResult->GetResult();
				std::vector<CTaxiwayUtilizationData*> vTempResult;
		
				int nResultCount = (int)vResult.size();
				std::vector<std::pair<CString,std::pair<CString,CString> > > vTaxiway;
				for (int nResult = 0; nResult < nResultCount; nResult++)
				{
					CTaxiwayUtilizationData* data = vResult[nResult];

					std::pair<CString,std::pair<CString,CString> >& item = std::make_pair(data->m_sTaxiway,std::make_pair(data->m_sStartNode,data->m_sEndNode));
					if (std::find(vTaxiway.begin(),vTaxiway.end(),item) == vTaxiway.end())
					{
						vTaxiway.push_back(item);
						vTempResult.push_back(data);
					}
				}

				int nTaxiwayCount = (int)vTempResult.size();
				for (int nTaxiway = 0; nTaxiway < nTaxiwayCount; ++nTaxiway)
				{
					CTaxiwayUtilizationData* pItem = vTempResult.at(nTaxiway);
				
					CString strText(_T(""));
					strText.Format(_T("%s intersection %s to intersection %s"),pItem->m_sTaxiway,pItem->m_sStartNode,\
						pItem->m_sEndNode);
					int nTaxiwayIndex = m_ComBoxSubType.AddString(strText);
					m_ComBoxSubType.SetItemData(nTaxiwayIndex,(DWORD_PTR)pItem);
					int nWidth2 = m_ComBoxSubType.GetDC()->GetTextExtent(strText).cx;
					if (nWidth2>nWidth)
						nWidth = nWidth2;
				}
				m_ComBoxSubType.SetDroppedWidth(nWidth);

				m_ComBoxSubType.SetCurSel(0);

				pResult->Draw3DChart(m_MSChartCtrl, pParam);
			}
			
		}
		break;
	case Airside_ControllerWorkload:
		{
			//ASSERT(0);

			m_comboChartSelect.ResetContent();
			enumASReportType_Detail_Summary emReportType = GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters()->getReportType();

			int nIndex = -1;
			nIndex = m_comboChartSelect.AddString(_T("Total Index"));
			m_comboChartSelect.SetItemData(nIndex, -1 );
			CAirsideControllerWorkloadParam *pParam = reinterpret_cast<CAirsideControllerWorkloadParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());

			for (int weight = CAirsideControllerWorkloadParam::weight_PushBacks;
				weight < CAirsideControllerWorkloadParam::weight_count; ++weight)
			{
				//default weight is 1
				CString strWeightName = pParam->getWeightName((CAirsideControllerWorkloadParam::enumWeight)weight);

				nIndex = m_comboChartSelect.AddString(strWeightName);
				m_comboChartSelect.SetItemData(nIndex, weight );			

			}

			m_comboChartSelect.SetCurSel(0);

		CAirsideControllerWorkloadReport *pPreport = reinterpret_cast< CAirsideControllerWorkloadReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			//-1, total index
			pParam->setSubType( -1);
			pPreport->RefreshReport(pParam);

			AirsideControllerWorkload::CReportResult  *pResult =  pPreport->m_pResult;
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	default:
		{
			ASSERT(0);
		}
		break;
	}
}
void CAirsideReportGraphView::InitTaxiwayDelayReport()
{
	m_comboChartSelect.ResetContent(); 
	CParameters* pParm = GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters() ;
	if(pParm->getReportType() == ASReportType_Detail)
	{
		int ndx = m_comboChartSelect.AddString(_T("ARRIVAL")) ;
		m_comboChartSelect.SetItemData(ndx,(DWORD_PTR)CTaxiwayDelayParameters::ARRIVAL) ;

		ndx = m_comboChartSelect.AddString(_T("Departure")) ;
		m_comboChartSelect.SetItemData(ndx,(DWORD_PTR)CTaxiwayDelayParameters::DEPARTURE) ;

		ndx = m_comboChartSelect.AddString(_T("Total")) ;
		m_comboChartSelect.SetItemData(ndx ,( DWORD_PTR)CTaxiwayDelayParameters::TOTAL) ;
	}
	m_comboChartSelect.SetCurSel(0) ;
	pParm->setSubType(CTaxiwayDelayParameters::ARRIVAL) ;
	CTaxiwayDelayReport *pPreport = reinterpret_cast< CTaxiwayDelayReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());

	pPreport->RefreshReport(pParm);
	CTaxiwayDelayReportResult *pResult = pPreport->GetBaseDelayReportResult();
	if (pResult)
		pResult->Draw3DChart(m_MSChartCtrl, pParm);
}

void CAirsideReportGraphView::InitFlightConflictSummaryReportFilter(AirsideFlightConflictPara *pParam )
{
	//Conflict type
	m_StaSubText.SetWindowText("Conflict Type");
	m_StaSubText.ShowWindow(SW_SHOW) ;
	m_ComBoxSubType.ShowWindow(SW_SHOW) ;

	m_ComBoxSubType.ResetContent();
	int nIndex = m_ComBoxSubType.AddString(_T("All"));
	m_ComBoxSubType.SetItemData(nIndex ,-1);

	nIndex = m_ComBoxSubType.AddString(_T("AC-AC"));
	m_ComBoxSubType.SetItemData(nIndex ,(int)FlightConflict::AC_AC);

	nIndex = m_ComBoxSubType.AddString(_T("AC-GSE"));
	m_ComBoxSubType.SetItemData(nIndex ,(int)FlightConflict::AC_GSE);
	m_ComBoxSubType.SetCurSel(0);

	//Area/Sector
	GetDlgItem(IDC_STATIC_SUBTYPE1)->ShowWindow(SW_SHOW);
	m_ComBoxSubType1.ShowWindow(SW_SHOW);

	m_ComBoxSubType1.ResetContent();
	nIndex = m_ComBoxSubType1.AddString(_T("All"));
	m_ComBoxSubType1.SetItemData(nIndex, -1);
	std::vector<ALTObjectID> vAreas = pParam->GetAreaList();

	int nCount = vAreas.size();
	for (int i = 0; i < nCount; i++)
	{
		ALTObjectID objName = vAreas.at(i);
		CString strName = objName.GetIDString();
		if (strName.IsEmpty())
			continue;

		nIndex = m_ComBoxSubType1.AddString(strName);
		m_ComBoxSubType1.SetItemData(nIndex, i);
	}
	m_ComBoxSubType1.SetCurSel(0);

	//Location
	GetDlgItem(IDC_STATIC_SUBTYPE2)->ShowWindow(SW_SHOW);
	m_ComBoxSubType2.ShowWindow(SW_SHOW);

	m_ComBoxSubType2.ResetContent();
	nIndex = m_ComBoxSubType2.AddString(_T("All"));
	m_ComBoxSubType2.SetItemData(nIndex ,-1);
	for (int i = 0; i <  FlightConflict::LOCATION_FIELD; i++)
	{
		nIndex = m_ComBoxSubType2.AddString( FlightConflict::LOCATIONTYPE[i]);
		m_ComBoxSubType2.SetItemData(nIndex ,i);	
	}
	m_ComBoxSubType2.SetCurSel(0);

	//Operation
	GetDlgItem(IDC_STATIC_SUBTYPE3)->ShowWindow(SW_SHOW);
	m_ComBoxSubType3.ShowWindow(SW_SHOW);

	m_ComBoxSubType3.ResetContent();
	nIndex = m_ComBoxSubType3.AddString(_T("All"));
	m_ComBoxSubType3.SetItemData(nIndex ,-1);
	for (int i = 0; i <  FlightConflict::OPERATION_FIELD; i++)
	{
		nIndex = m_ComBoxSubType3.AddString( FlightConflict::OPERATIONTYPE[i]);
		m_ComBoxSubType3.SetItemData(nIndex ,i);	
	}
	m_ComBoxSubType3.SetCurSel(0);

	//Action
	GetDlgItem(IDC_STATIC_SUBTYPE4)->ShowWindow(SW_SHOW);
	m_ComBoxSubType4.ShowWindow(SW_SHOW);

	m_ComBoxSubType4.ResetContent();
	nIndex = m_ComBoxSubType4.AddString(_T("All"));
	m_ComBoxSubType4.SetItemData(nIndex ,-1);
	for (int i = 0; i <  FlightConflict::ACTION_FIELD; i++)
	{
		nIndex = m_ComBoxSubType4.AddString( FlightConflict::ACTIONTYPE[i]);
		m_ComBoxSubType4.SetItemData(nIndex ,i);	
	}
	m_ComBoxSubType4.SetCurSel(0);
}

void CAirsideReportGraphView::OnSelchangeChartSelectCombo()
{
	int nCursel =  m_comboChartSelect.GetCurSel();
	if (nCursel == LB_ERR)
		return;

	int nSubType =  m_comboChartSelect.GetItemData(nCursel);

	int iSubValue = 0;
	reportType rpType =  GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReportType();
	switch(rpType) {
	case Airside_FlightDelay:
		{
            GetDlgItem(IDC_STATIC_SUBTYPE)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_STATIC_SUBTYPE)->SetWindowText(_T("Sub Type"));
            m_ComBoxSubType.ShowWindow(FALSE);
			CAirsideFlightDelayParam *pParam = reinterpret_cast<CAirsideFlightDelayParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			std::vector<int> vReportRun;
			bool bMultiple = false;
			if(pParam->GetReportRuns(vReportRun) && pParam->GetEnableMultiRun())
			{
				if (vReportRun.size() > 1)
				{
                    if(nSubType == CAirsideFlightDelayReport::subReportType::SRT_DETAIL_SEGMENTDELAY || 
                        nSubType == CAirsideFlightDelayReport::subReportType::SRT_SUMMARY_SEGMENTDELAY)
                    {
                        GetDlgItem(IDC_STATIC_SUBTYPE)->ShowWindow(SW_SHOW);
                        m_ComBoxSubType.ShowWindow(TRUE);
                        m_ComBoxSubType.ResetContent();
                        int nIndex = m_ComBoxSubType.AddString("Air");
                        m_ComBoxSubType.SetItemData(nIndex,(DWORD)CAirsideFlightDelayReport::FltDelaySegment_Air);
                        nIndex = m_ComBoxSubType.AddString("Taxi");
                        m_ComBoxSubType.SetItemData(nIndex,(DWORD)CAirsideFlightDelayReport::FltDelaySegment_Taxi);
                        nIndex = m_ComBoxSubType.AddString("Stand");
                        m_ComBoxSubType.SetItemData(nIndex,(DWORD)CAirsideFlightDelayReport::FltDelaySegment_Stand);
                        nIndex = m_ComBoxSubType.AddString("Service");
                        m_ComBoxSubType.SetItemData(nIndex,(DWORD)CAirsideFlightDelayReport::FltDelaySegment_Service);
                        nIndex = m_ComBoxSubType.AddString("Takeoff");
                        m_ComBoxSubType.SetItemData(nIndex,(DWORD)CAirsideFlightDelayReport::FltDelaySegment_TakeOff);
                        m_ComBoxSubType.SetCurSel(0);
                        GetDocument()->GetARCReportManager().GetAirsideReportManager()->updateMultiRun3Dchart(m_MSChartCtrl, CAirsideFlightDelayReport::FltDelaySegment_Air);

						iSubValue = CAirsideFlightDelayReport::FltDelaySegment_Air;
                    }
                    else if(nSubType == CAirsideFlightDelayReport::subReportType::SRT_DETAIL_COMPONENTDELAY || 
                        nSubType == CAirsideFlightDelayReport::subReportType::SRT_SUMMARY_COMPONENTDELAY)
                    {
                        GetDlgItem(IDC_STATIC_SUBTYPE)->ShowWindow(SW_SHOW);
                        m_ComBoxSubType.ShowWindow(TRUE);
                        m_ComBoxSubType.ResetContent();
                        int nIndex = m_ComBoxSubType.AddString("Slowed");
                        m_ComBoxSubType.SetItemData(nIndex,(DWORD)FltDelayReason_Slowed);
                        nIndex = m_ComBoxSubType.AddString("Vectored");
                        m_ComBoxSubType.SetItemData(nIndex,(DWORD)FltDelayReason_Vectored);
                        nIndex = m_ComBoxSubType.AddString("Hold");
                        m_ComBoxSubType.SetItemData(nIndex,(DWORD)FltDelayReason_AirHold);
                        nIndex = m_ComBoxSubType.AddString("Alt change");
                        m_ComBoxSubType.SetItemData(nIndex,(DWORD)FltDelayReason_AltitudeChanged);
                        nIndex = m_ComBoxSubType.AddString("Side step");
                        m_ComBoxSubType.SetItemData(nIndex,(DWORD)FltDelayReason_SideStep);
                        nIndex = m_ComBoxSubType.AddString("Stop");
                        m_ComBoxSubType.SetItemData(nIndex,(DWORD)FltDelayReason_Stop);
                        nIndex = m_ComBoxSubType.AddString("Service");
                        m_ComBoxSubType.SetItemData(nIndex,(DWORD)FltDelayReason_Service);
                        m_ComBoxSubType.SetCurSel(0);
                        GetDocument()->GetARCReportManager().GetAirsideReportManager()->updateMultiRun3Dchart(m_MSChartCtrl, FltDelayReason_Slowed);

						iSubValue = FltDelayReason_Slowed;
                    }
                    else
                    {
                        GetDocument()->GetARCReportManager().GetAirsideReportManager()->updateMultiRun3Dchart(m_MSChartCtrl);
                    }
					bMultiple = true;
				}
			}

			if (bMultiple == false)
			{
				CAirsideFlightDelayReport *pPreport = reinterpret_cast< CAirsideFlightDelayReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());

				pPreport->RefreshReport(pParam);
				CAirsideFlightDelayBaseResult *pResult =  pPreport->GetReportResult();
				if (pResult)
					pResult->Draw3DChart(m_MSChartCtrl, pParam);
			}
		}
		break;
	case Airside_NodeDelay:
		{
			CAirsideNodeDelayReportParameter *pParam = reinterpret_cast<CAirsideNodeDelayReportParameter *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			CAirsideNodeDelayReport *pPreport = reinterpret_cast< CAirsideNodeDelayReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());

			pPreport->RefreshReport(pParam);
			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;

	case Airside_AircraftOperation:
		{
			CAirsideOperationParam *pParam = reinterpret_cast<CAirsideOperationParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			//pParam->setSubType(nSubType);
			CAirsideOperationReport *pPreport = reinterpret_cast< CAirsideOperationReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());

			pPreport->RefreshReport(pParam);
			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;

	case Airside_FlightActivity:
		{
			CFlightActivityParam *pParam = reinterpret_cast<CFlightActivityParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			CFlightActivityReport *pPreport = reinterpret_cast< CFlightActivityReport*> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());

			pPreport->RefreshReport(pParam);
 			CFlightActivityBaseResult *pResult =  pPreport->m_pResult;
		
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_AircraftOperational:
		{
			CFlightOperationalParam *pParam = reinterpret_cast<CFlightOperationalParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			CFlightOperationalReport *pPreport = reinterpret_cast< CFlightOperationalReport*> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());

			pPreport->RefreshReport(pParam);
			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;

	case Airside_RunwayOperaitons:
		{

			AirsideRunwayOperationReportParam *pParam = reinterpret_cast<AirsideRunwayOperationReportParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			AirsideRunwayOperationsReport *pPreport = reinterpret_cast< AirsideRunwayOperationsReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);

			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_VehicleOperation:
		{
			CAirsideVehicleOperParameter *pParam = reinterpret_cast<CAirsideVehicleOperParameter *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			CAirsideVehicleOperaterReport *pPreport = reinterpret_cast< CAirsideVehicleOperaterReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);
			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_TakeoffProcess:
		{
			CAirsideTakeoffProcessParameter *pParam = reinterpret_cast<CAirsideTakeoffProcessParameter *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			CAirsideTakeoffProcessReport *pPreport = reinterpret_cast< CAirsideTakeoffProcessReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);
			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_RunwayUtilization:
		{

			CAirsideRunwayUtilizationReportParam *pParam = reinterpret_cast<CAirsideRunwayUtilizationReportParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			CAirsideRunwayUtilizationReport *pPreport = reinterpret_cast< CAirsideRunwayUtilizationReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);

			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_TaxiwayDelay:
		{

			CTaxiwayDelayParameters *pParam = reinterpret_cast<CTaxiwayDelayParameters *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			CTaxiwayDelayReport *pPreport = reinterpret_cast< CTaxiwayDelayReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);

			CTaxiwayDelayReportResult *pResult =  pPreport->GetBaseDelayReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_FlightConflict:
		{

			AirsideFlightConflictPara *pParam = reinterpret_cast<AirsideFlightConflictPara *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			AirsideFlightConflictReport *pPreport = reinterpret_cast< AirsideFlightConflictReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);

			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_RunwayDelay:
		{

			AirsideFlightRunwayDelayReportPara *pParam = reinterpret_cast<AirsideFlightRunwayDelayReportPara *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			AirsideFlightRunwayDelayReport *pPreport = reinterpret_cast< AirsideFlightRunwayDelayReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			pPreport->RefreshReport(pParam);

			CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_RunwayCrossings:
		{
			AirsideRunwayCrossingsParameter *pParam = reinterpret_cast<AirsideRunwayCrossingsParameter *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			CAirsideRunwayCrossingsReport *pPreport = reinterpret_cast< CAirsideRunwayCrossingsReport*> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());

			pPreport->RefreshReport(pParam);
			CAirsideRunwayCrossingsBaseResult *pResult =  pPreport->m_pResult;

			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_StandOperations:
		{
			CFlightStandOperationParameter *pParam = reinterpret_cast<CFlightStandOperationParameter *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			CAirsideFlightStandOperationReport *pPreport = reinterpret_cast< CAirsideFlightStandOperationReport*> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());

			pPreport->RefreshReport(pParam);
			CAirsideFlightStandOperationBaseResult *pResult =  pPreport->m_pBaseResult;

			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_IntersectionOperation:
		{
			CAirsideIntersectionReportParam *pParam = reinterpret_cast<CAirsideIntersectionReportParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			CAirsideIntersectionOperationReport *pPreport = reinterpret_cast< CAirsideIntersectionOperationReport*> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());

			pPreport->RefreshReport(pParam);
			CAirsideIntersectionReportBaseResult   *pResult =  pPreport->m_pResult;

			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_TaxiwayUtilization:
		{
			CAirsideTaxiwayUtilizationPara *pParam = reinterpret_cast<CAirsideTaxiwayUtilizationPara *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			pParam->setSubType(nSubType);
			CAirsideTaxiwayUtilizationReport *pPreport = reinterpret_cast< CAirsideTaxiwayUtilizationReport*> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());

			pPreport->RefreshReport(pParam);
			CAirsideTaxiwayUtilizationBaseResult   *pResult =  pPreport->m_pResult;

			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	case Airside_ControllerWorkload:
		{
			//ASSERT(0);
			CAirsideControllerWorkloadParam *pParam = reinterpret_cast<CAirsideControllerWorkloadParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
			CAirsideControllerWorkloadReport *pPreport = reinterpret_cast< CAirsideControllerWorkloadReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
			//-1, total index
			pParam->setSubType(nSubType);
			pPreport->RefreshReport(pParam);

			AirsideControllerWorkload::CReportResult  *pResult =  pPreport->m_pResult;
			if (pResult)
				pResult->Draw3DChart(m_MSChartCtrl, pParam);
		}
		break;
	default:
		{
			ASSERT(0);
		}
		break;
	}

	GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_DISLISTVIEW,/*NULL*/(CObject*)iSubValue);
}

void CAirsideReportGraphView::ResetAllContent()
{

	m_comboChartType.SetCurSel(1);
}

void CAirsideReportGraphView::OnCbnSelendokChartTypeCombo()
{
	int nSelGraphType = m_comboChartType.GetCurSel();
	switch(nSelGraphType) 
	{
	case 0://3D Bar
		m_MSChartCtrl.Set3DChartType(Arc3DChartType_3D_Bar);
		break;
	case 1://2D Bar
		m_MSChartCtrl.Set3DChartType(Arc3DChartType_2D_Bar);
		break;
	case 2://2D Pie
		m_MSChartCtrl.Set3DChartType(Arc3DChartType(14));
		break;
	case 3://3D Line
		m_MSChartCtrl.Set3DChartType(Arc3DChartType_3D_Line);
		break;
	case 4://2D Line
		m_MSChartCtrl.Set3DChartType(Arc3DChartType_2D_Line);
		break;
	case 5://3D Area
		m_MSChartCtrl.Set3DChartType(Arc3DChartType_3D_Area);
		break;
	case 6://2D Area
		m_MSChartCtrl.Set3DChartType(Arc3DChartType_2D_Area);
		break;
	case 7://3D Step
		m_MSChartCtrl.Set3DChartType(Arc3DChartType(6));
		break;
	case 8://2D Step
		m_MSChartCtrl.Set3DChartType(Arc3DChartType(7));
		break;
	case 9://3D Combination
		m_MSChartCtrl.Set3DChartType(Arc3DChartType(8));
		break;
	case 10://2D Combination
		m_MSChartCtrl.Set3DChartType(Arc3DChartType(9));
		break;
	default:
		break;
	}
}

void CAirsideReportGraphView::InitRunwayUtilizationReport()
{
	m_comboChartSelect.ResetContent();

	if(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters()->getReportType() == ASReportType_Detail)
	{
		//CString TypeName[] = 
		//{
		//	_T("Landings by Runway"),
		//		_T("TakeOff by Runway"),
		//		_T("Lead-Trail Operation by Runway"),
		//		_T("Movements per Interval")
		//};


		//ChartType_Detail = 0,
		//	ChartType_Detail_LandingsByRunway,
		//	ChartType_Detail_TakeOffByRunway,
		//	ChartType_Detail_LeadTrailByRunway,
		//	ChartType_Detail_MovementsPerRunway,


		//	ChartType_Detail_Count,


		//	ChartType_Summary = 100,
		//	ChartType_Summary_RunwayOperationalStatistic,

		//	ChartType_Summary_Count,

		int nIndex = m_comboChartSelect.AddString(_T("Runway Occupancy"));
		m_comboChartSelect.SetItemData(nIndex ,CAirsideRunwayUtilizationReport::ChartType_Detail_RunwayOccupancy);

		nIndex = m_comboChartSelect.AddString(_T("Runway Utilization"));
		m_comboChartSelect.SetItemData(nIndex ,CAirsideRunwayUtilizationReport::ChartType_Detail_RunwayUtilization);

	}
	else
	{
		int nIndex = m_comboChartSelect.AddString(_T("Summary occupancy per interval"));
		m_comboChartSelect.SetItemData(nIndex ,CAirsideRunwayUtilizationReport::ChartType_Summary_RunwayOccupancy);

		nIndex = m_comboChartSelect.AddString(_T("Summary utilization per interval"));
		m_comboChartSelect.SetItemData(nIndex ,CAirsideRunwayUtilizationReport::ChartType_Summary_RunwayUtilization);

	}

	m_comboChartSelect.SetCurSel(0);

	int nCursel =  m_comboChartSelect.GetCurSel();
	if (nCursel == LB_ERR)
		return;

	int nSubType =  m_comboChartSelect.GetItemData(nCursel);

	CAirsideRunwayUtilizationReportParam *pParam = reinterpret_cast<CAirsideRunwayUtilizationReportParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
	pParam->setSubType(nSubType);
	CAirsideRunwayUtilizationReport *pPreport = reinterpret_cast< CAirsideRunwayUtilizationReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
	pPreport->RefreshReport(pParam);

	CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
	if (pResult)
		pResult->Draw3DChart(m_MSChartCtrl, pParam);
}

void CAirsideReportGraphView::OnSelChangerChartSubType()
{
	if (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReportType() == Airside_FlightConflict)
	{
		int nCursel = m_ComBoxSubType.GetCurSel() ;
		if(nCursel == LB_ERR)
			return ;
		int type = m_ComBoxSubType.GetItemData(nCursel) ;

		CAirsideVehicleOperParameter *pParam = reinterpret_cast<CAirsideVehicleOperParameter *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
		CAirsideVehicleOperaterReport *pPreport = reinterpret_cast< CAirsideVehicleOperaterReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
		pParam->SetVehicleType(type) ;
		pPreport->RefreshReport(pParam);

		CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
		if (pResult)
			pResult->Draw3DChart(m_MSChartCtrl, pParam);
	}
	else if(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReportType() == Airside_IntersectionOperation)
	{
		int nCursel = m_ComBoxSubType.GetCurSel() ;
		if(nCursel == LB_ERR)
			return ;
		int nSummaryNode = m_ComBoxSubType.GetItemData(nCursel) ;

		CAirsideIntersectionReportParam *pParam = reinterpret_cast<CAirsideIntersectionReportParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
		CAirsideIntersectionOperationReport *pPreport = reinterpret_cast< CAirsideIntersectionOperationReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
		pParam->setSummaryNodeID(nSummaryNode) ;
		pPreport->RefreshReport(pParam);

		CAirsideIntersectionReportBaseResult  *pResult =  pPreport->m_pResult;
		if (pResult)
			pResult->Draw3DChart(m_MSChartCtrl, pParam);
	}
	else if(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReportType() == Airside_TaxiwayUtilization)
	{
		int nCursel = m_ComBoxSubType.GetCurSel() ;
		if(nCursel == LB_ERR)
			return ;
		CTaxiwayUtilizationData* pSelectItem = (CTaxiwayUtilizationData*)m_ComBoxSubType.GetItemData(nCursel);

		CAirsideTaxiwayUtilizationPara *pParam = reinterpret_cast<CAirsideTaxiwayUtilizationPara *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
		CAirsideTaxiwayUtilizationReport *pPreport = reinterpret_cast< CAirsideTaxiwayUtilizationReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
		pParam->SetCurrentSelect(pSelectItem);
		pPreport->RefreshReport(pParam);

		CAirsideTaxiwayUtilizationBaseResult  *pResult =  pPreport->m_pResult;
		if (pResult)
			pResult->Draw3DChart(m_MSChartCtrl, pParam);
	}
	else if(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReportType() == Airside_ControllerWorkload)
	{
		int nCursel = m_ComBoxSubType.GetCurSel() ;
		if(nCursel == LB_ERR)
			return ;

		CAirsideControllerWorkloadParam *pParam = reinterpret_cast<CAirsideControllerWorkloadParam *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
		CAirsideControllerWorkloadReport *pPreport = reinterpret_cast< CAirsideControllerWorkloadReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
		//-1, total index
		pParam->setSubType(nCursel - 1);
		pPreport->RefreshReport(pParam);

		AirsideControllerWorkload::CReportResult  *pResult =  pPreport->m_pResult;
		if (pResult)
			pResult->Draw3DChart(m_MSChartCtrl, pParam);
	}
    else if(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReportType() == Airside_FlightDelay)
    {
        //int nCursel =  m_comboChartSelect.GetCurSel();
        //if (nCursel == LB_ERR)
        //    return;
        //int nSubType =  m_comboChartSelect.GetItemData(nCursel);

        //if(nSubType == CAirsideFlightDelayReport::subReportType::SRT_DETAIL_SEGMENTDELAY || 
        //    nSubType == CAirsideFlightDelayReport::subReportType::SRT_SUMMARY_SEGMENTDELAY)
        //{
        //    nCursel = m_ComBoxSubType.GetCurSel() ;
        //    if(nCursel == LB_ERR)
        //        return ;
        //    CAirsideFlightDelayReport::FltDelaySegment subType = (CAirsideFlightDelayReport::FltDelaySegment)m_ComBoxSubType.GetItemData(nCursel);
        //    GetDocument()->GetARCReportManager().GetAirsideReportManager()->updateMultiRun3Dchart(m_MSChartCtrl, subType);
        //    GetDocument()->UpdateAllViews(this, AIRSIDEREPORT_DISLISTVIEW, (CObject*)subType);
        //}
        //else if(nSubType == CAirsideFlightDelayReport::subReportType::SRT_DETAIL_COMPONENTDELAY || 
        //    nSubType == CAirsideFlightDelayReport::subReportType::SRT_SUMMARY_COMPONENTDELAY)
        //{
        //    nCursel = m_ComBoxSubType.GetCurSel() ;
        //    if(nCursel == LB_ERR)
        //        return ;
        //    FltDelayReason subType = (FltDelayReason)m_ComBoxSubType.GetItemData(nCursel);
        //    GetDocument()->GetARCReportManager().GetAirsideReportManager()->updateMultiRun3Dchart(m_MSChartCtrl, subType);
        //    GetDocument()->UpdateAllViews(this, AIRSIDEREPORT_DISLISTVIEW, (CObject*)subType);
        //}
        //else
        //{
        //    GetDocument()->GetARCReportManager().GetAirsideReportManager()->updateMultiRun3Dchart(m_MSChartCtrl);
        //}

		int nCursel = m_ComBoxSubType.GetCurSel();
		if (nCursel == LB_ERR)
			return;

		int iType = (int)m_ComBoxSubType.GetItemData(nCursel);
		GetDocument()->GetARCReportManager().GetAirsideReportManager()->updateMultiRun3Dchart(m_MSChartCtrl, iType);
		GetDocument()->UpdateAllViews(this, AIRSIDEREPORT_DISLISTVIEW, (CObject*)iType);
    }
}

void CAirsideReportGraphView::OnSelChangerChartSubTypes()
{
	if (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReportType() == Airside_FlightConflict)
	{
		int nConType = m_ComBoxSubType.GetItemData(m_ComBoxSubType.GetCurSel());
		int nAreaIdx = m_ComBoxSubType1.GetItemData(m_ComBoxSubType1.GetCurSel());
		int nLocType = m_ComBoxSubType2.GetItemData(m_ComBoxSubType2.GetCurSel());
		int nOpType = m_ComBoxSubType3.GetItemData(m_ComBoxSubType3.GetCurSel());
		int nActType = m_ComBoxSubType4.GetItemData(m_ComBoxSubType4.GetCurSel());

		AirsideFlightConflictPara *pParam = reinterpret_cast<AirsideFlightConflictPara *>(GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters());
		pParam->setSummaryFilterPara(nConType, nAreaIdx, nLocType, nOpType, nActType);

		AirsideFlightConflictReport *pPreport = reinterpret_cast< AirsideFlightConflictReport *> (GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport());
		pPreport->RefreshReport(pParam);

		CAirsideReportBaseResult *pResult =  pPreport->GetReportResult();
		if (pResult)
			pResult->Draw3DChart(m_MSChartCtrl, pParam);

	}
}
