#include "stdafx.h"
#include "TermPlan.h"
#include "CmpReportGraphView.h"
#include "CompareReportDoc.h"
#include "ComparativePlot.h"

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
	DDX_Control(pDX, IDC_CHART_SELECT_COMBO, m_comboChartSelect);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarContent);
}

BEGIN_MESSAGE_MAP(CCmpReportGraphView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CCmpReportGraphView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	m_pCmpReport = ((CCompareReportDoc*)GetDocument())->GetCmpReport();
}

void CCmpReportGraphView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/)
{
	if(!IsWindowVisible())
		return;
	CComparativeProject* pCompProj = m_pCmpReport->GetComparativeProject();
	CInputParameter* inputParam = pCompProj->GetInputParam();
	CReportsManager* reportsManager = inputParam->GetReportsManagerPtr();
	std::vector<CReportToCompare>& vReportList = reportsManager->GetReportsList();

	CModelsManager* modelsManager = inputParam->GetModelsManagerPtr();
	std::vector<CModelToCompare *>& vModelList = modelsManager->GetModelsList();
	const CComparativeReportResultList &crrList = pCompProj->GetCompReportResultList();
	const CmpReportResultVector& vReport = crrList.GetReportResult();

	for(int i = 0; i < static_cast<int>(vReport.size()); i++)
	{
		CComparativePlot_new cmpPlot(CMPBar_2D, m_3DChart, vModelList);
		cmpPlot.Draw3DChart(*vReport[i]);
		break;
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
	if(!m_3DChart.Create(0,0,WS_CHILD | WS_VISIBLE ,CRect(0,0,100,100),this,COMPARE_REPORT_GRAPH_CHART_CTRL) )
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

