// ChildFrameSplit.cpp : implementation file
//
#include "stdafx.h"
#include "resource.h"
#include "TermPlanDoc.h"
#include "ChildFrameSplit.h"
#include "./MFCExControl/ARCBaseTree.h"
#include "NodeView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static UINT indicators[] =
{
	ID_INDICATOR_MODELINGSEQUENCEVIEW,           // status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CChildFrameSplit

IMPLEMENT_DYNCREATE(CChildFrameSplit, CMDIChildWnd)

CChildFrameSplit::CChildFrameSplit()
{
}

CChildFrameSplit::~CChildFrameSplit()
{
}


BEGIN_MESSAGE_MAP(CChildFrameSplit, CMDIChildWnd)
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrameSplit::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrameSplit::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrameSplit message handlers

void CChildFrameSplit::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// update our parent window first
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!

	CDocument* pDocument = GetActiveDocument();
	ASSERT(pDocument != NULL);
	if (bAddToTitle)
	{
		CString strWindowTitle(pDocument->GetTitle());
		if (pDocument == NULL)
			strWindowTitle = m_strTitle;
		else
		{	
			if(GetDocument()->m_systemMode==EnvMode_Terminal)
				strWindowTitle+=_T("        Terminal Modeling Sequence View");
			else if(GetDocument()->m_systemMode==EnvMode_AirSide)
				strWindowTitle += _T("      Airside Modeling Sequence View");
			else if (GetDocument()->m_systemMode==EnvMode_LandSide)
				strWindowTitle += _T("      Landside Modeling Sequence View ");
			else if (GetDocument()->m_systemMode == EnvMode_OnBoard)
				strWindowTitle += _T("      OnBoard Modeling Sequence View ");
			else if (GetDocument()->m_systemMode == EnvMode_Cargo)
				strWindowTitle += _T("      Cargo Modeling Sequence View ");
			else if (GetDocument()->m_systemMode == EnvMode_Environment)
				strWindowTitle += _T("      Environment Modeling Sequence View ");
			else if (GetDocument()->m_systemMode == EnvMode_Financial)
				strWindowTitle += _T("      Financial Modeling Sequence View ");

		}

	   this->SetWindowText(strWindowTitle);

	}
}
int CChildFrameSplit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndStatusBar.Create(this)
		|| !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CRect rect;
	m_wndStatusBar.GetWindowRect(&rect);
	m_wndStatusBar.SetPaneInfo(0, ID_INDICATOR_MODELINGSEQUENCEVIEW, SBPS_NOBORDERS, rect.Width());
	
	return 0;
}

void CChildFrameSplit::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	if (::IsWindow(m_wndStatusBar.GetSafeHwnd()))
	{
		CRect rect;
		m_wndStatusBar.GetWindowRect(&rect);
		m_wndStatusBar.SetPaneInfo(0, ID_INDICATOR_MODELINGSEQUENCEVIEW, SBPS_NOBORDERS, rect.Width());
	}
	
}

void CChildFrameSplit::OnUpdateFrameMenu (BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt)
{
    CMDIChildWnd::OnUpdateFrameMenu (bActivate, pActivateWnd, hMenuAlt);
	GetDocument()->GetMainFrame()->AdjustMenu();
	if(bActivate)
		GetDocument()->UpdateTrackersMenu();
}

CTermPlanDoc* CChildFrameSplit::GetDocument() 
{
	CDocument* pDoc = GetActiveDocument();
	ASSERT(GetActiveDocument()->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	return (CTermPlanDoc*) GetActiveDocument();
}