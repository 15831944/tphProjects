// NodeViewFrame.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "NodeViewFrame.h"
#include ".\nodeviewframe.h"
#include ".\MFCExControl\ARCBaseTree.h"
#include "AirsideMSView.h"
#include "LandsideMSView.h"
#include "NodeView.h"
#include "OnBoardMsview.h"
#include "MainFrm.h"
#include "CargoMSView.h"
#include "EnvironmentMSView.h"
#include "FinancialMSView.h"




// CNodeViewFrame
using namespace MSV;
IMPLEMENT_DYNCREATE(CNodeViewFrame, CChildFrameSplit)

CNodeViewFrame::CNodeViewFrame()
{
	m_pTerminalView = NULL;
	m_pLandsideView = NULL;
	m_pAirsideView = NULL;
	m_pOnBoardView = NULL;
	m_curMode = EnvMode_Terminal;
	m_pCargoView = NULL;
	m_pEnvironmentView = NULL;
	m_pFinancialView = NULL;
}

CNodeViewFrame::~CNodeViewFrame()
{
}
BEGIN_MESSAGE_MAP(CNodeViewFrame, CChildFrameSplit)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CNodeViewFrame message handlers

BOOL MSV::CNodeViewFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle , const RECT& rect , CMDIFrameWnd* pParentWnd , CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	return CChildFrameSplit::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, pContext);
}

int MSV::CNodeViewFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CChildFrameSplit::OnCreate(lpCreateStruct) == -1)
		return -1;

	

	return 0;
}

void CNodeViewFrame::AdjustMSView(EnvironmentMode& mode)
{

	if (mode == m_curMode)
		return;
	CView *pCurView = GetActiveView();

	if (m_curMode == EnvMode_Terminal && m_pTerminalView == NULL)
	{
		m_pTerminalView = pCurView;
	}

	//to airside

	if(mode == EnvMode_AirSide)
	{
		if (m_pAirsideView == NULL)
		{
			CCreateContext context;
			context.m_pNewViewClass = RUNTIME_CLASS(CAirsideMSView);
			context.m_pCurrentDoc = GetActiveDocument();
			m_pAirsideView = STATIC_DOWNCAST(CView, CreateView(&context));	
			m_pAirsideView->OnInitialUpdate();
		}
		if (m_pAirsideView)
		{
			HideViewsExceptThis(m_pAirsideView);

			m_pAirsideView->ShowWindow(SW_SHOW);

			SetActiveView(m_pAirsideView);
			//RecalcLayout();
			::SendMessage(m_hWnd,WM_SIZE,NULL,NULL);
		}
	}
	//to OnBoard
	if(mode == EnvMode_OnBoard)
	{
		if (m_pOnBoardView == NULL)
		{
			CCreateContext context;
			context.m_pNewViewClass = RUNTIME_CLASS(MSV::COnBoardMSView);
			context.m_pCurrentDoc = GetActiveDocument();
			m_pOnBoardView = STATIC_DOWNCAST(CView, CreateView(&context));	
			m_pOnBoardView->OnInitialUpdate();
		}
		if (m_pOnBoardView)
		{
			HideViewsExceptThis(m_pOnBoardView);
			m_pOnBoardView->ShowWindow(SW_SHOW);

			SetActiveView(m_pOnBoardView);
			//RecalcLayout();
			::SendMessage(m_hWnd,WM_SIZE,NULL,NULL);
		}
	}


	// to terminal

	if (mode == EnvMode_Terminal /*|| mode == EnvMode_LandSide*/)
	{
		if (m_pTerminalView == NULL)
		{
			CCreateContext context;
			context.m_pNewViewClass = RUNTIME_CLASS(CNodeView);
			context.m_pCurrentDoc = GetActiveDocument();
			m_pTerminalView = STATIC_DOWNCAST(CView, CreateView(&context));	
			m_pTerminalView->OnInitialUpdate();
		}
		if (m_pTerminalView)
		{
			HideViewsExceptThis(m_pTerminalView);
			m_pTerminalView->ShowWindow(SW_SHOW);

			SetActiveView(m_pTerminalView);
			m_pTerminalView->SetRedraw(TRUE);
			::SendMessage(m_hWnd,WM_SIZE,NULL,NULL);
		}
	}


	if (mode == EnvMode_LandSide)
	{
		if (m_pLandsideView == NULL)
		{
			CCreateContext context;
			context.m_pNewViewClass = RUNTIME_CLASS(CLandsideMSView);
			context.m_pCurrentDoc = GetActiveDocument();
			m_pLandsideView = STATIC_DOWNCAST(CView, CreateView(&context));
			m_pLandsideView->OnInitialUpdate();
		}
		if (m_pLandsideView)
		{
			HideViewsExceptThis(m_pLandsideView);
			m_pLandsideView->ShowWindow(SW_SHOW);

			SetActiveView(m_pLandsideView);
			::SendMessage(m_hWnd,WM_SIZE,NULL,NULL);

		}
	}

	//cargo
	if (mode == EnvMode_Cargo)
	{
		if (m_pCargoView == NULL)
		{
			CCreateContext context;
			context.m_pNewViewClass = RUNTIME_CLASS(CCargoMSView);
			context.m_pCurrentDoc = GetActiveDocument();
			m_pCargoView = STATIC_DOWNCAST(CView, CreateView(&context));
			m_pCargoView->OnInitialUpdate();
		}
		if (m_pCargoView)
		{
			HideViewsExceptThis(m_pCargoView);
			m_pCargoView->ShowWindow(SW_SHOW);

			SetActiveView(m_pCargoView);
			::SendMessage(m_hWnd,WM_SIZE,NULL,NULL);

		}
	}

//environment
	if (mode == EnvMode_Environment)
	{
		if (m_pEnvironmentView == NULL)
		{
			CCreateContext context;
			context.m_pNewViewClass = RUNTIME_CLASS(CEnvironmentMSView);
			context.m_pCurrentDoc = GetActiveDocument();
			m_pEnvironmentView = STATIC_DOWNCAST(CView, CreateView(&context));
			m_pEnvironmentView->OnInitialUpdate();
		}
		if (m_pEnvironmentView)
		{
			HideViewsExceptThis(m_pEnvironmentView);
			m_pEnvironmentView->ShowWindow(SW_SHOW);

			SetActiveView(m_pEnvironmentView);
			::SendMessage(m_hWnd,WM_SIZE,NULL,NULL);

		}
	}

//Financial
	if (mode == EnvMode_Financial)
	{
		if (m_pFinancialView == NULL)
		{
			CCreateContext context;
			context.m_pNewViewClass = RUNTIME_CLASS(CFinancialMSView);
			context.m_pCurrentDoc = GetActiveDocument();
			m_pFinancialView = STATIC_DOWNCAST(CView, CreateView(&context));
			m_pFinancialView->OnInitialUpdate();
		}
		if (m_pFinancialView)
		{
			HideViewsExceptThis(m_pFinancialView);
			m_pFinancialView->ShowWindow(SW_SHOW);

			SetActiveView(m_pFinancialView);
			::SendMessage(m_hWnd,WM_SIZE,NULL,NULL);

		}
	}

	m_curMode = mode;
}




//OnSize
//{
//	get client rect
//	set active view POSITION:
//	set status bar position
//}
void MSV::CNodeViewFrame::OnSize(UINT nType, int cx, int cy)
{
	CChildFrameSplit::OnSize(nType, cx, cy);
	
	if (GetActiveView() != NULL)
	{	
		CRect rectStaBar;
		m_wndStatusBar.GetWindowRect(&rectStaBar);
		CRect rcClient;
		GetClientRect(&rcClient);
		GetActiveView()->SetWindowPos(NULL,rcClient.left,rcClient.top,rcClient.Width(),rcClient.Height()- rectStaBar.Height(),SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
	
	}
	// TODO: Add your message handler code here		
	if (::IsWindow(m_wndStatusBar.GetSafeHwnd()))
	{	
		CRect rectStaBar;
		m_wndStatusBar.GetWindowRect(&rectStaBar);
/*		CRect rcClient;
		GetClientRect(&rcClient);*/			
		//m_wndStatusBar.MoveWindow(rcClient.left,rcClient.bottom - 30,rcClient.Width(),30);
		m_wndStatusBar.SetPaneInfo(0, ID_INDICATOR_MODELINGSEQUENCEVIEW, SBPS_NOBORDERS, rectStaBar.Width());
	}
}

void MSV::CNodeViewFrame::OnUpdateFrameMenu(BOOL bActive, CWnd* pActiveWnd, HMENU hMenuAlt)
{

	CChildFrameSplit::OnUpdateFrameMenu(bActive, pActiveWnd, hMenuAlt);

	GetDocument()->GetMainFrame()->AdjustMenu();

}

void MSV::CNodeViewFrame::HideViewsExceptThis( CView * pViewExcept )
{
	//Terminal
	if (m_pTerminalView && m_pTerminalView != pViewExcept)
		m_pTerminalView->ShowWindow(SW_HIDE);
	
	//Landside
	if (m_pLandsideView && m_pLandsideView != pViewExcept)
		m_pLandsideView->ShowWindow(SW_HIDE);

	//Airside
	if (m_pAirsideView && m_pAirsideView != pViewExcept)
		m_pAirsideView->ShowWindow(SW_HIDE);
	
	//Onboard
	if (m_pOnBoardView && m_pOnBoardView != pViewExcept)
		m_pOnBoardView->ShowWindow(SW_HIDE);

	//Cargo
	if (m_pCargoView && m_pCargoView != pViewExcept)
		m_pCargoView->ShowWindow(SW_HIDE);

	//Environment
	if (m_pEnvironmentView && m_pEnvironmentView != pViewExcept)
		m_pEnvironmentView->ShowWindow(SW_HIDE);

	//Environment
	if (m_pFinancialView && m_pFinancialView != pViewExcept)
		m_pFinancialView->ShowWindow(SW_HIDE);

}

void MSV::CNodeViewFrame::ActiveViewOfMode( EnvironmentMode& envMode )
{


}




























