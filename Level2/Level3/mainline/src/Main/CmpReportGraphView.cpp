#include "stdafx.h"
#include "CmpReportGraphView.h"
#include "CompareReportDoc.h"
#include ".\compare\ComparativePlot.h"
#include ".\cmpreportgraphview.h"

#define COMPARE_REPORT_GRAPH_BASE 1000
#define COMPARE_REPORT_GRAPH_CHART_CTRL	COMPARE_REPORT_GRAPH_BASE+1
#define COMPARE_REPORT_GRAPH_COMBOBOX1	COMPARE_REPORT_GRAPH_BASE+2

IMPLEMENT_DYNCREATE(CCmpReportGraphView, CFormView)

CCmpReportGraphView::CCmpReportGraphView() : CFormView(CCmpReportGraphView::IDD)
{
	m_pCmpReport = NULL;
	m_categoryType = -1;
}

CCmpReportGraphView::~CCmpReportGraphView()
{
}

void CCmpReportGraphView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHART_TYPE_COMBO, m_comboChartType);
	DDX_Control(pDX, IDC_REPORT_LIST_COMBO, m_comboReportList);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarContainer);
}

BEGIN_MESSAGE_MAP(CCmpReportGraphView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_SEL_COLOR_BTN, OnSelColorBtn)
	ON_BN_CLICKED(IDC_PRINT_BTN, OnPrintBtn)
	ON_CBN_SELCHANGE(IDC_REPORT_LIST_COMBO, OnCbnSelchangeReportListCombo)
	ON_CBN_SELCHANGE(IDC_CHART_TYPE_COMBO, OnCbnSelchangeChartTypeCombo)
END_MESSAGE_MAP()

void CCmpReportGraphView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	InitToolbar();
	//*****************Fill Chart Type Combox***********************
	m_comboChartType.AddString("3D Bar");
	m_comboChartType.AddString("2D Bar(Default)");
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
	m_comboChartType.SetCurSel(1);// Set "2D Bar(Default)" as default.
	if(!m_3DChart.Create(0,0,WS_CHILD | WS_VISIBLE ,CRect(0,0,100,100),this,COMPARE_REPORT_GRAPH_CHART_CTRL) )
		return;
	m_pCmpReport = ((CCompareReportDoc*)GetDocument())->GetCmpReport();
}

void CCmpReportGraphView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/)
{
	if(!IsWindowVisible())
		return;
	CComparativeProject* pCompProj = m_pCmpReport->GetComparativeProject();
 	CInputParameter* inputParam = pCompProj->GetInputParam();
	const CComparativeReportResultList &resultList = pCompProj->GetCompReportResultList();
	const CmpReportResultVector& vResult = resultList.GetReportResult();

	CString selectedReport;
	m_comboReportList.GetWindowText(selectedReport);
	m_comboReportList.ResetContent();
	for(int i = 0; i < static_cast<int>(vResult.size()); i++)
	{
		m_comboReportList.AddString(vResult[i]->GetCmpReportName());
	}
	int nIndex = m_comboReportList.SelectString( 0, selectedReport);
	if(nIndex == CB_ERR)
	{
		m_comboReportList.SetCurSel(0);
	}
	else
	{
		m_comboReportList.SetCurSel(nIndex);
	}
	m_comboReportList.GetWindowText(selectedReport);
	if(!selectedReport.IsEmpty())
	{
		m_pCmpReport->SetCurReport(selectedReport);
		Draw3DChartByReportName(selectedReport);
		CCompareReportDoc* pDoc = (CCompareReportDoc*)GetDocument();
		pDoc->UpdateAllViews(this);
	}
}

void CCmpReportGraphView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if(IsWindowVisible())
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		CRect rcChart;
		rcChart.right  = rcClient.right - 10;
		rcChart.left   = rcClient.left+10;
		rcChart.top    = rcClient.top+50;
		rcChart.bottom = rcClient.bottom;
		m_3DChart.m_p3DChart->MoveWindow(rcChart);
	}
}

int CCmpReportGraphView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		/*| CBRS_GRIPPER*/ | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_REPORTTOOLBAR))
		return -1;
	return 0;
}


#ifdef _DEBUG
void CCmpReportGraphView::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmpReportGraphView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


void CCmpReportGraphView::OnCbnSelchangeReportListCombo()
{
	CString selectedReport;
	m_comboReportList.GetWindowText(selectedReport);
	if(!selectedReport.IsEmpty())
	{
		m_pCmpReport->SetCurReport(selectedReport);
		Draw3DChartByReportName(selectedReport);
		CCompareReportDoc* pDoc = (CCompareReportDoc*)GetDocument();
		pDoc->UpdateAllViews(this);
	}
}

void CCmpReportGraphView::Draw3DChartByReportName(CString &selectedReport)
{
	CComparativeProject* pCompProj = m_pCmpReport->GetComparativeProject();
	const CComparativeReportResultList &resultList = pCompProj->GetCompReportResultList();
	const CmpReportResultVector& vResult = resultList.GetReportResult();
	for(int i = 0; i < static_cast<int>(vResult.size()); i++)
	{
		if(selectedReport.CompareNoCase(vResult[i]->GetCmpReportName()) == 0)
		{
			CComparativePlot cmpPlot(CMPBar_2D, m_3DChart);
			cmpPlot.Draw3DChart(*vResult[i]);
			break;
		}
	}
}

void CCmpReportGraphView::InitToolbar()
{
	CRect rc;
	m_toolbarContainer.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(2, -2, 90, rc.Height());
	m_toolbarContainer.ShowWindow(SW_HIDE);
	m_ToolBar.ShowWindow(SW_SHOW);

}

void CCmpReportGraphView::OnSelColorBtn() 
{
	m_3DChart.PropertyEdit();
}
void CCmpReportGraphView::OnPrintBtn() 
{
	m_3DChart.Print3DChart();
}

void CCmpReportGraphView::OnCbnSelchangeChartTypeCombo()
{
	int nSelGraphType = m_comboChartType.GetCurSel();
	switch(nSelGraphType) 
	{
	case 0://3D Bar
		m_3DChart.Set3DChartType(Arc3DChartType_3D_Bar);
		break;
	case 1://2D Bar
		m_3DChart.Set3DChartType(Arc3DChartType_2D_Bar);
		break;
	case 2://2D Pie
		m_3DChart.Set3DChartType(Arc3DChartType(14));
		break;
	case 3://3D Line
		m_3DChart.Set3DChartType(Arc3DChartType_3D_Line);
		break;
	case 4://2D Line
		m_3DChart.Set3DChartType(Arc3DChartType_2D_Line);
		break;
	case 5://3D Area
		m_3DChart.Set3DChartType(Arc3DChartType_3D_Area);
		break;
	case 6://2D Area
		m_3DChart.Set3DChartType(Arc3DChartType_2D_Area);
		break;
	case 7://3D Step
		m_3DChart.Set3DChartType(Arc3DChartType(6));
		break;
	case 8://2D Step
		m_3DChart.Set3DChartType(Arc3DChartType(7));
		break;
	case 9://3D Combination
		m_3DChart.Set3DChartType(Arc3DChartType(8));
		break;
	case 10://2D Combination
		m_3DChart.Set3DChartType(Arc3DChartType(9));
		break;
	default:
		break;
	}
}
