// CCompareReportView2.cpp : implementation file
//
#include "stdafx.h"
#include "TermPlan.h"
#include "CompareReportView.h"

// CCompareReportView2

IMPLEMENT_DYNCREATE(CCompareReportView, CView)

CCompareReportView::CCompareReportView()
{
}

CCompareReportView::~CCompareReportView()
{
}

BEGIN_MESSAGE_MAP(CCompareReportView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND_RANGE(ID_REPORTGRAPHTYPE_2DBAR, ID_REPORTGRAPHTYPE_3DBAR, OnReportGraphType)
	ON_UPDATE_COMMAND_UI_RANGE(ID_REPORTGRAPHTYPE_2DBAR, ID_REPORTGRAPHTYPE_3DBAR, OnUpdateReportGraphType)
END_MESSAGE_MAP()


// CCompareReportView2 drawing

void CCompareReportView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CCompareReportView2 diagnostics

#ifdef _DEBUG
void CCompareReportView::AssertValid() const
{
	CView::AssertValid();
}

void CCompareReportView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CCompareReportView2 message handlers

int CCompareReportView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndCmpReport.Create(NULL, WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, 10))
		return -1;

	return 0;
}

void CCompareReportView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndCmpReport.GetReportFrameWnd()->m_hWnd))
		m_wndCmpReport.GetReportFrameWnd()->MoveWindow(0, 0, cx, cy);
}

BOOL CCompareReportView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CCompareReportView::OnReportGraphType(UINT nID)
{
	ThreeDChartType enmChartType = CMPBar_2D;

	switch (nID)
	{
	case ID_REPORTGRAPHTYPE_2DBAR:
		enmChartType = CMPBar_2D;
		break;

	case ID_REPORTGRAPHTYPE_2DLINE:
		enmChartType = CMPLine_2D;
		break;

	case ID_REPORTGRAPHTYPE_3DBAR:
		enmChartType = CMPBar_3D;
		break;

	default:
		enmChartType = CMPBar_2D;
	}

	m_wndCmpReport.SetChartType(enmChartType);
}

void CCompareReportView::OnUpdateReportGraphType(CCmdUI* pCmdUI)
{
	ThreeDChartType enmChartType = CMPBar_2D;

	switch (pCmdUI->m_nID)
	{
	case ID_REPORTGRAPHTYPE_2DBAR:
		enmChartType = CMPBar_2D;
		break;

	case ID_REPORTGRAPHTYPE_2DLINE:
		enmChartType = CMPLine_2D;
		break;

	case ID_REPORTGRAPHTYPE_3DBAR:
		enmChartType = CMPBar_3D;
		break;
	}

	pCmdUI->SetCheck(m_wndCmpReport.GetChartType() == enmChartType);
}