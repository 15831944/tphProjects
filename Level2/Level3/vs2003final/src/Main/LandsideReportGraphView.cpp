// LandsideReportGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "LandsideReportGraphView.h"
#include "TermPlanDoc.h"
#include "../Common/ViewMsg.h"
#include "../LandsideReport/LandsideBaseReport.h"
#include "../LandsideReport/LandsideBaseParam.h"

// LandsideReportGraphView

IMPLEMENT_DYNCREATE(LandsideReportGraphView, CFormView)

LandsideReportGraphView::LandsideReportGraphView()
	: CFormView(LandsideReportGraphView::IDD)
{
}

LandsideReportGraphView::~LandsideReportGraphView()
{
}

void LandsideReportGraphView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDC_CHART_TYPE_COMBO, m_comboChartType);
	DDX_Control(pDX, IDC_CHART_SELECT_COMBO, m_comboChartSelect);
	DDX_Control(pDX, IDC_REPORT_SUBTYPE, m_comSubType);


}

BEGIN_MESSAGE_MAP(LandsideReportGraphView, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_SEL_COLOR_BTN, OnSelColorBtn)
	ON_CBN_SELCHANGE(IDC_CHART_TYPE_COMBO, OnCbnSelendokChartTypeCombo)
	ON_CBN_SELCHANGE(IDC_CHART_SELECT_COMBO, OnSelchangeChartSelectCombo)
	ON_BN_CLICKED(IDC_PRINT_BTN, OnPrintBtn)
	ON_COMMAND(ID_GRAPHVIEW_MAXIMAX, OnGraphviewMaximax)
	ON_COMMAND(ID_GRAPHVIEW_NORMAL, OnGraphviewNormal)
END_MESSAGE_MAP()


// LandsideReportGraphView diagnostics

#ifdef _DEBUG
void LandsideReportGraphView::AssertValid() const
{
	CFormView::AssertValid();
}

void LandsideReportGraphView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// LandsideReportGraphView message handlers

void LandsideReportGraphView::OnInitialUpdate()
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
	m_comSubType.ShowWindow(SW_HIDE);
}

void LandsideReportGraphView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class
	if (lHint != AIRSIDEREPORT_SHOWREPORT)
		return;

	LandsideBaseReport *pCurReport =  GetDocument()->GetARCReportManager().GetLandsideReportManager()->GetReport();
	if(pCurReport == NULL)
		return;
	LandsideBaseParam* pParameter = pCurReport->getParam();
	if(pParameter == NULL)
		return;

	LandsideReportType rpType = (LandsideReportType)pCurReport->GetReportType();

	switch (rpType)
	{
	case LANDSIDE_VEHICLE_ACTIVITY://
	case LANDSIDE_VEHICLE_DELAY:
	case LANDSIDE_VEHICLE_TOTALDELAY:
	case LANDSIDE_RESOURCE_QUEUELEN:
	case LANDSIDE_RESOURCE_THROUGHPUT:
		{
			m_comboChartSelect.ResetContent();

			LSGraphChartTypeList lstChart = pCurReport->GetChartList();
			for (int nChart = 0; nChart < lstChart.GetCount(); ++nChart)
			{
				LSGraphChartType chart = lstChart.GetItem(nChart);

				int nIndex = m_comboChartSelect.AddString(chart.m_strTitle);
				m_comboChartSelect.SetItemData(nIndex, chart.m_nEnum);
			}
		
			m_comboChartSelect.SetCurSel(0);

			int nCursel =  m_comboChartSelect.GetCurSel();
			if (nCursel == LB_ERR)
				return;

			int nSubType =  m_comboChartSelect.GetItemData(nCursel);

			pParameter->setSubType(nSubType);
			pCurReport->Draw3DChart(m_MSChartCtrl);


		}
		break;

	default:
		{
			ASSERT(0);
		}

	}

}

void LandsideReportGraphView::OnSize(UINT nType, int cx, int cy)
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

int LandsideReportGraphView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		/*| CBRS_GRIPPER*/ | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_REPORTTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if(!m_MSChartCtrl.Create(0,0,WS_CHILD | WS_VISIBLE ,CRect(0,0,100,100),this,1234) )
		return -1;
	return 0;
}
void LandsideReportGraphView::InitToolbar()
{
	CRect rc;
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow( 2,-2,90,rc.Height() );
	m_toolbarcontenter.ShowWindow(SW_HIDE);
	m_ToolBar.ShowWindow( SW_SHOW );

}
void LandsideReportGraphView::OnSelColorBtn() 
{
	m_MSChartCtrl.PropertyEdit();
}
void LandsideReportGraphView::OnPrintBtn() 
{
	m_MSChartCtrl.Print3DChart();
}

void LandsideReportGraphView::OnSelchangeChartSelectCombo()
{
	int nCursel =  m_comboChartSelect.GetCurSel();
	if (nCursel == LB_ERR)
		return;

	LandsideBaseReport *pCurReport =  GetDocument()->GetARCReportManager().GetLandsideReportManager()->GetReport();
	if(pCurReport == NULL)
		return;
	LandsideBaseParam* pParameter = pCurReport->getParam();
	if(pParameter == NULL)
		return;

	LandsideReportType rpType = (LandsideReportType)pCurReport->GetReportType();

	int nSubType =  m_comboChartSelect.GetItemData(nCursel);
	switch(rpType) 
	{
	case LANDSIDE_VEHICLE_ACTIVITY:
	case LANDSIDE_VEHICLE_DELAY:
	case LANDSIDE_VEHICLE_TOTALDELAY:
	case LANDSIDE_RESOURCE_THROUGHPUT:
	case LANDSIDE_RESOURCE_QUEUELEN:
		{
			pParameter->setSubType(nSubType);
			pCurReport->Draw3DChart(m_MSChartCtrl);
		}
		break;
	default:
		{
			ASSERT(0);
		}
		break;
	}
	GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_DISLISTVIEW,NULL);
}
void LandsideReportGraphView::ResetAllContent()
{
	m_comboChartSelect.ResetContent();
	m_comboChartType.SetCurSel(1);
	m_MSChartCtrl.m_p3DChart->SetEnabled(FALSE);
	m_MSChartCtrl.m_p3DChart->ShowWindow(SW_HIDE);
}
void LandsideReportGraphView::OnCbnSelendokChartTypeCombo()
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
void LandsideReportGraphView::OnGraphviewMaximax() 
{
	//// TODO: Add your command handler code here
	//CRect rectMax;
	//GetParentFrame()->GetClientRect(&rectMax);
	//rectMax.top+=2;
	//MoveWindow(&rectMax);
}

void LandsideReportGraphView::OnGraphviewNormal() 
{
	//// TODO: Add your command handler code here
	//CRect rectFrame,rectListView,rectGraphView;
	//GetParentFrame()->GetClientRect(&rectFrame);

	//CTermPlanDoc* pDoc = (CTermPlanDoc*)(GetDocument());
	//CView * pTempView;
	//POSITION pos = pDoc->GetFirstViewPosition();
	//while (pos != NULL)
	//{
	//	pTempView = pDoc->GetNextView(pos);
	//	if(pTempView->IsKindOf(RUNTIME_CLASS(CRepListView)))
	//	{
	//		pTempView->GetWindowRect(&rectListView);
	//		rectGraphView.left=rectListView.right+5;
	//		rectGraphView.right=rectFrame.right;
	//		rectGraphView.top=rectFrame.top+2;
	//		rectGraphView.bottom=rectFrame.bottom;
	//	}
	//}
	//MoveWindow(&rectGraphView);
}













































