#include "stdafx.h"
#include "TermPlan.h"
#include "CmpReportGraphView.h"
#include "CompareReportDoc.h"
#include "ComparativePlot.h"
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
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarContent);
}

BEGIN_MESSAGE_MAP(CCmpReportGraphView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_REPORT_LIST_COMBO, OnCbnSelchangeReportListCombo)
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
	const CComparativeReportResultList &resultList = pCompProj->GetCompReportResultList();
	const CmpReportResultVector& vResult = resultList.GetReportResult();

	CString selectedReport;
	m_comboReportList.GetWindowText(selectedReport);
	m_comboReportList.ResetContent();
	for(int i = 0; i < static_cast<int>(vResult.size()); i++)
	{
		m_comboReportList.AddString(vResult[i]->GetCmpReportName());
		int nIndex = m_comboReportList.SelectString( 0, selectedReport);
		if(nIndex == CB_ERR)
		{
			m_comboReportList.SetCurSel(0);
		}
		else
		{
			m_comboReportList.SetCurSel(nIndex);
		}
	}
	m_comboReportList.GetWindowText(selectedReport);
	Draw3DChartByReportName(selectedReport);
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


void CCmpReportGraphView::OnCbnSelchangeReportListCombo()
{
	CString selectedReport;
	m_comboReportList.GetWindowText(selectedReport);
	if(!selectedReport.IsEmpty())
		Draw3DChartByReportName(selectedReport);
}

void CCmpReportGraphView::Draw3DChartByReportName(CString &selectedReport)
{
	CComparativeProject* pCompProj = m_pCmpReport->GetComparativeProject();
	CInputParameter* inputParam = pCompProj->GetInputParam();
	const CComparativeReportResultList &resultList = pCompProj->GetCompReportResultList();
	const CmpReportResultVector& vResult = resultList.GetReportResult();
	for(int i = 0; i < static_cast<int>(vResult.size()); i++)
	{
		CString xxxx = vResult[i]->GetCmpReportName();
		if(selectedReport.CompareNoCase(vResult[i]->GetCmpReportName()) == 0)
		{
			CComparativePlot_new cmpPlot(CMPBar_2D, m_3DChart);
			cmpPlot.Draw3DChart(*vResult[i]);
		}
	}
}
