// AirsideReportView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "MainFrm.h"
#include "AirsideReportView.h"
#include ".\airsidereportview.h"

// CAirsideReportView

IMPLEMENT_DYNCREATE(CAirsideReportView, CView)

CAirsideReportView::CAirsideReportView()
{
}

CAirsideReportView::~CAirsideReportView()
{
	//CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	//if(pMainFrame)
	//{
	//	pMainFrame->SendMessage(UM_SHOW_CONTROL_BAR,4,0);
	//}
}

BEGIN_MESSAGE_MAP(CAirsideReportView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND_RANGE(ID_GRAPHTYPE_2DBAR, ID_GRAPHTYPE_3DBAR, OnReportGraphType)
	ON_UPDATE_COMMAND_UI_RANGE(ID_GRAPHTYPE_2DBAR, ID_GRAPHTYPE_3DBAR, OnUpdateReportGraphType)
END_MESSAGE_MAP()


// CAirsideReportView drawing

void CAirsideReportView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CAirsideReportView diagnostics

#ifdef _DEBUG
void CAirsideReportView::AssertValid() const
{
	CView::AssertValid();
}

void CAirsideReportView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CAirsideReportView message handlers

int CAirsideReportView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//if (!m_wndARCReport.Create(NULL, WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), this, 10))
	//	return -1;

	//CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	//if(pMainFrame)
	//{
	//	pMainFrame->SendMessage(UM_SHOW_CONTROL_BAR,4,1);
	//}

	return 0;
}

void CAirsideReportView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	//if (::IsWindow(m_wndARCReport.GetReportFrameWnd()->m_hWnd))
	//	m_wndARCReport.GetReportFrameWnd()->MoveWindow(0, 0, cx, cy);
}

BOOL CAirsideReportView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CAirsideReportView::OnReportGraphType(UINT nID)
{
	//AirsideThreeDChartType enmChartType = ASBar_2D;

	//switch (nID)
	//{
	//case ID_GRAPHTYPE_2DBAR:
	//	enmChartType = ASBar_2D;
	//	break;

	//case ID_GRAPHTYPE_2DLINE:
	//	enmChartType = ASLine_2D;
	//	break;

	//case ID_GRAPHTYPE_3DBAR:
	//	enmChartType = ASBar_3D;
	//	break;

	//default:
	//	enmChartType = ASBar_2D;
	//}

	//m_wndARCReport.SetChartType(enmChartType);
}

void CAirsideReportView::OnUpdateReportGraphType(CCmdUI* pCmdUI)
{
	//AirsideThreeDChartType enmChartType = ASBar_2D;

	//switch (pCmdUI->m_nID)
	//{
	//case ID_GRAPHTYPE_2DBAR:
	//	enmChartType = ASBar_2D;
	//	break;

	//case ID_GRAPHTYPE_2DLINE:
	//	enmChartType = ASLine_2D;
	//	break;

	//case ID_GRAPHTYPE_3DBAR:
	//	enmChartType = ASBar_3D;
	//	break;
	//}

	//pCmdUI->SetCheck(m_wndARCReport.GetChartType() == enmChartType);
}
