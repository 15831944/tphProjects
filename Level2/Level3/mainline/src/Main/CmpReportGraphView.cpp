#include "stdafx.h"
#include "CmpReportGraphView.h"
#include "CompareReportDoc.h"
#include ".\compare\ComparativePlot.h"
#include ".\cmpreportgraphview.h"
#include ".\compare\ComparativeQLengthReport.h"
#include "RepGraphViewBaseOperator.h"
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
	DDX_Control(pDX, IDC_REPORT_SUBTYPE_COMBO, m_comboRepSubType);
}

BEGIN_MESSAGE_MAP(CCmpReportGraphView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_SEL_COLOR_BTN, OnSelColorBtn)
	ON_BN_CLICKED(IDC_PRINT_BTN, OnPrintBtn)
	ON_CBN_SELCHANGE(IDC_REPORT_LIST_COMBO, OnCbnSelchangeReportListCombo)
	ON_CBN_SELCHANGE(IDC_CHART_TYPE_COMBO, OnCbnSelchangeChartTypeCombo)
	ON_CBN_SELCHANGE(IDC_REPORT_SUBTYPE_COMBO, OnCbnSelchangeRepSubTypeCombo)
END_MESSAGE_MAP()

void CCmpReportGraphView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	m_ToolBar.MoveWindow(4, 4, 60, 28);
	m_ToolBar.ShowWindow(SW_SHOW);
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
 	CCmpReportParameter* inputParam = pCompProj->GetInputParam();
	CSingleReportsManager* pReportManager = inputParam->GetReportsManager();
	m_comboReportList.ResetContent();
	for(int i = 0; i < pReportManager->getCount(); i++)
	{
		CReportToCompare& report = pReportManager->getReport(i);
		if(report.GetChecked() == TRUE)
		{
			CString strRepName = report.GetName();
			m_comboReportList.AddString(strRepName.MakeUpper());
		}
	}

	UpdateRepSubTypeCombo();
	CString strCurReport = m_pCmpReport->GetFocusReportName();
	int nCurSel = m_comboReportList.SelectString(0, strCurReport);
	if (nCurSel == LB_ERR)
	{
		// no report is selected, hide the chart.
		m_3DChart.m_p3DChart->ShowWindow(SW_HIDE);
		return;
	}

	nCurSel = m_comboRepSubType.GetCurSel();
	if (nCurSel == LB_ERR)
		return;
	
	int nSubType =  m_comboRepSubType.GetItemData(nCurSel);
	Draw3DChartByReportName(strCurReport,nSubType);
}

void CCmpReportGraphView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if(IsWindowVisible())
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		m_3DChart.m_p3DChart->MoveWindow(0, 34, rcClient.Width(), rcClient.Height()-34);
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
	int nCurSel=m_comboReportList.GetCurSel();
	if(nCurSel<0)
	{
		return;
	}
	CString strSelect;
	m_comboReportList.GetLBText(nCurSel,strSelect);

	
	if(!strSelect.IsEmpty())
	{
		m_pCmpReport->SetFocusReportName(strSelect);
		
		UpdateRepSubTypeCombo();
		int nSubCurSel = m_comboRepSubType.GetCurSel();
		if (nSubCurSel == LB_ERR)
		{
			return;
		}

		int nSubType =  m_comboRepSubType.GetItemData(nSubCurSel);

		Draw3DChartByReportName(strSelect,nSubType);
		CCompareReportDoc* pDoc = (CCompareReportDoc*)GetDocument();
		pDoc->UpdateAllViews(this);
	}
}

void CCmpReportGraphView::OnCbnSelchangeRepSubTypeCombo()
{
	int nCurSel=m_comboRepSubType.GetCurSel();
	if(nCurSel<0)
	{
		return;
	}
	int nSubType =  m_comboRepSubType.GetItemData(nCurSel);
	CString strFocusRep = m_pCmpReport->GetFocusReportName();
	Draw3DChartByReportName(strFocusRep,nSubType);
}

void CCmpReportGraphView::Draw3DChartByReportName(CString &selectedReport,int nSubType)
{
	CComparativeProject* pCompProj = m_pCmpReport->GetComparativeProject();
	const CCmpReportManager &resultList = pCompProj->GetCompReportResultList();
	const CmpReportResultVector& vResult = resultList.GetReportResult();
	for(int i = 0; i < static_cast<int>(vResult.size()); i++)
	{
		if(selectedReport.CompareNoCase(vResult[i]->GetCmpReportName()) == 0)
		{
			CComparativePlot cmpPlot(CMPBar_2D, m_3DChart);
			cmpPlot.Draw3DChart(*vResult[i],nSubType);
			return;
		}
	}
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

void CCmpReportGraphView::UpdateRepSubTypeCombo()
{
	m_comboRepSubType.ResetContent();
	CString strFocusRep = m_pCmpReport->GetFocusReportName();
	if(strFocusRep.IsEmpty())
	{
		return;
	}
	CCmpReportParameter* param = m_pCmpReport->GetComparativeProject()->GetInputParam();
	CReportToCompare* pReport = param->GetReportsManager()->GetReportByName(strFocusRep);

	CString strCombo;
	switch(pReport->GetCategory())
	{
	case ENUM_QUEUELENGTH_REP:
		{
			if(pReport->GetParameter().GetReportDetail() == REPORT_TYPE_DETAIL)
			{
				int nIdx = m_comboRepSubType.AddString("Queue Length(\"Detailed\")");
				m_comboRepSubType.SetItemData(nIdx,CComparativeQLengthReport::QUEUELENGTH_TYPE);

				m_comboRepSubType.SetCurSel(0);
			}
			else
			{
				int nIdx = m_comboRepSubType.AddString("Minimum Queue Length");
				m_comboRepSubType.SetItemData(nIdx,CComparativeQLengthReport::MIN_QLENGTH);
				nIdx = m_comboRepSubType.AddString("Average Queue Length");
				m_comboRepSubType.SetItemData(nIdx,CComparativeQLengthReport::AVA_QLENGTH);
				nIdx = m_comboRepSubType.AddString("Maximum Queue Length");
				m_comboRepSubType.SetItemData(nIdx,CComparativeQLengthReport::MAX_QLENGTH);
				nIdx = m_comboRepSubType.AddString("Combined Queue Length");
				m_comboRepSubType.SetItemData(nIdx,CComparativeQLengthReport::TOTAL_QLENGTH);
				m_comboRepSubType.SetCurSel(0);
			}
		}
		break;
	case ENUM_QUEUETIME_REP:
		{
			if(pReport->GetParameter().GetReportDetail() == REPORT_TYPE_DETAIL)
			{
				m_comboRepSubType.AddString("Queue Time(Detailed)");
				m_comboRepSubType.SetCurSel(0);
			}
			else
			{
				for(int i= SubType_Average; i<= SubType_All;i++)
				{
					int nIdx = m_comboRepSubType.AddString(MultiRunSummarySubReportTypeName[i]);
					m_comboRepSubType.SetItemData(nIdx, i);
				}
				m_comboRepSubType.SetCurSel(0);
			}
		}
		break;
	case ENUM_THROUGHPUT_REP:
		{
			if(pReport->GetParameter().GetReportDetail() == REPORT_TYPE_DETAIL)
			{
				m_comboRepSubType.AddString("Pax Served");
				m_comboRepSubType.SetCurSel(0);
			}
			else
			{
				m_comboRepSubType.AddString("Total Pax by Group(Summary)");
				m_comboRepSubType.AddString("Pax per Processor(Summary)");
				m_comboRepSubType.AddString("Group Throughput per Hour(Summary)");
				m_comboRepSubType.AddString("Processor Throughput per Hour(Summary)");
				m_comboRepSubType.SetCurSel(0);			
			}
		}
		break;
	case ENUM_PAXDENS_REP:
		{
			m_comboRepSubType.AddString("Count");
			m_comboRepSubType.AddString("Pax/m2");
			m_comboRepSubType.AddString("m2/Pax");
			m_comboRepSubType.SetCurSel(0);
		}
		break;
	case ENUM_PAXCOUNT_REP:
		{
			if(pReport->GetParameter().GetReportDetail() == REPORT_TYPE_DETAIL)//Detailed
			{
				m_comboRepSubType.AddString("Space Occupancy(Detailed)");
				m_comboRepSubType.SetCurSel(0);
			}
			else//Summary
			{
				m_comboRepSubType.AddString("Passenger/Period");
				m_comboRepSubType.AddString("Maximum Passenger Count");
				m_comboRepSubType.AddString("Total Passenger Count");
				m_comboRepSubType.SetCurSel(0);	
			}
		}
		break;
	case ENUM_ACOPERATION_REP:
		{
			m_comboRepSubType.AddString("Total");
			m_comboRepSubType.SetCurSel(0);
		}
		break;
	case ENUM_DURATION_REP:
		{
			if(pReport->GetParameter().GetReportDetail() == REPORT_TYPE_DETAIL)
			{
				m_comboRepSubType.AddString("Duration(Detailed)");
				m_comboRepSubType.SetCurSel(0);
			}
			else
			{
				m_comboRepSubType.AddString("Duration(Summary)");
				m_comboRepSubType.SetCurSel(0);		
			}
		}
		break;
	case ENUM_DISTANCE_REP:
		{
			if(pReport->GetParameter().GetReportDetail() == REPORT_TYPE_DETAIL)
			{
				m_comboRepSubType.AddString("Distance Traveled(Detailed)");
				m_comboRepSubType.SetCurSel(0);
			}
			else
			{
				m_comboRepSubType.AddString("Distance Traveled(Summary)");
				m_comboRepSubType.SetCurSel(0);			
			}
		}
		break;
	}
}
